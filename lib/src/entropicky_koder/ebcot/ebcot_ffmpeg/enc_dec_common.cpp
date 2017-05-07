#include "enc_dec_common.h"

// look up tables
uint8_t ff_jpeg2000_sigctxno_lut[256][4];
uint8_t ff_jpeg2000_sgnctxno_lut[16][16], ff_jpeg2000_xorbit_lut[16][16];

int lut_nmsedec_ref [1<<NMSEDEC_BITS],
    lut_nmsedec_ref0[1<<NMSEDEC_BITS],
    lut_nmsedec_sig [1<<NMSEDEC_BITS],
    lut_nmsedec_sig0[1<<NMSEDEC_BITS];

// soucast luts
static int getsigctxno(int flag, int bandno)
{
    int h, v, d;

    h = ((flag & JPEG2000_T1_SIG_E)  ? 1 : 0) +
        ((flag & JPEG2000_T1_SIG_W)  ? 1 : 0);
    v = ((flag & JPEG2000_T1_SIG_N)  ? 1 : 0) +
        ((flag & JPEG2000_T1_SIG_S)  ? 1 : 0);
    d = ((flag & JPEG2000_T1_SIG_NE) ? 1 : 0) +
        ((flag & JPEG2000_T1_SIG_NW) ? 1 : 0) +
        ((flag & JPEG2000_T1_SIG_SE) ? 1 : 0) +
        ((flag & JPEG2000_T1_SIG_SW) ? 1 : 0);

    if (bandno < 3) {
        if (bandno == 1)
            FFSWAP(int, h, v);
        if (h == 2) return 8;
        if (h == 1) {
            if (v >= 1) return 7;
            if (d >= 1) return 6;
            return 5;
        }
        if (v == 2) return 4;
        if (v == 1) return 3;
        if (d >= 2) return 2;
        if (d == 1) return 1;
    } else {
        if (d >= 3) return 8;
        if (d == 2) {
            if (h+v >= 1) return 7;
            return 6;
        }
        if (d == 1) {
            if (h+v >= 2) return 5;
            if (h+v == 1) return 4;
            return 3;
        }
        if (h+v >= 2) return 2;
        if (h+v == 1) return 1;
    }
    return 0;
}

static const int contribtab[3][3] = { {  0, -1,  1 }, { -1, -1,  0 }, {  1,  0,  1 } };
static const int  ctxlbltab[3][3] = { { 13, 12, 11 }, { 10,  9, 10 }, { 11, 12, 13 } };
static const int  xorbittab[3][3] = { {  1,  1,  1 }, {  1,  0,  0 }, {  0,  0,  0 } };

static int getsgnctxno(int flag, uint8_t *xorbit)
{
    int vcontrib, hcontrib;

    hcontrib = contribtab[flag & JPEG2000_T1_SIG_E ? flag & JPEG2000_T1_SGN_E ? 1 : 2 : 0]
                         [flag & JPEG2000_T1_SIG_W ? flag & JPEG2000_T1_SGN_W ? 1 : 2 : 0] + 1;
    vcontrib = contribtab[flag & JPEG2000_T1_SIG_S ? flag & JPEG2000_T1_SGN_S ? 1 : 2 : 0]
                         [flag & JPEG2000_T1_SIG_N ? flag & JPEG2000_T1_SGN_N ? 1 : 2 : 0] + 1;
    *xorbit = xorbittab[hcontrib][vcontrib];

    return ctxlbltab[hcontrib][vcontrib];
}

void ff_jpeg2000_init_tier1_luts(void)
{
    int i, j;
    for (i = 0; i < 256; i++)
        for (j = 0; j < 4; j++)
            ff_jpeg2000_sigctxno_lut[i][j] = getsigctxno(i, j);
    for (i = 0; i < 16; i++)
        for (j = 0; j < 16; j++)
            ff_jpeg2000_sgnctxno_lut[i][j] =
                getsgnctxno(i + (j << 8), &ff_jpeg2000_xorbit_lut[i][j]);
}


//void ff_jpeg2000_init_tier1_luts(void)
void init_luts(void)
{
    int i, a,
        mask = ~((1<<NMSEDEC_FRACBITS)-1);

    for (i = 0; i < (1 << NMSEDEC_BITS); i++){
        lut_nmsedec_sig[i]  = FFMAX(6*i - (9<<NMSEDEC_FRACBITS-1) << 12-NMSEDEC_FRACBITS, 0);
        lut_nmsedec_sig0[i] = FFMAX((i*i + (1<<NMSEDEC_FRACBITS-1) & mask) << 1, 0);

        a = (i >> (NMSEDEC_BITS-2)&2) + 1;
        lut_nmsedec_ref[i]  = FFMAX((-2*i + (1<<NMSEDEC_FRACBITS) + a*i - (a*a<<NMSEDEC_FRACBITS-2))
                                    << 13-NMSEDEC_FRACBITS, 0);
        lut_nmsedec_ref0[i] = FFMAX(((i*i + (1-4*i << NMSEDEC_FRACBITS-1) + (1<<2*NMSEDEC_FRACBITS)) & mask)
                                    << 1, 0);
    }
}



//******************************************** DALSI

/* Update significance of a coefficient at current position (x,y) and
 * for neighbors. */
void ff_jpeg2000_set_significance(Jpeg2000T1Context *t1, int x, int y,
                                  int negative)
{
    x++;
    y++;
    t1->flags[(y) * t1->stride + x] |= JPEG2000_T1_SIG;
    if (negative) {
        t1->flags[(y) * t1->stride + x + 1] |= JPEG2000_T1_SIG_W | JPEG2000_T1_SGN_W;
        t1->flags[(y) * t1->stride + x - 1] |= JPEG2000_T1_SIG_E | JPEG2000_T1_SGN_E;
        t1->flags[(y + 1) * t1->stride + x] |= JPEG2000_T1_SIG_N | JPEG2000_T1_SGN_N;
        t1->flags[(y - 1) * t1->stride + x] |= JPEG2000_T1_SIG_S | JPEG2000_T1_SGN_S;
    } else {
        t1->flags[(y) * t1->stride + x + 1] |= JPEG2000_T1_SIG_W;
        t1->flags[(y) * t1->stride + x - 1] |= JPEG2000_T1_SIG_E;
        t1->flags[(y + 1) * t1->stride + x] |= JPEG2000_T1_SIG_N;
        t1->flags[(y - 1) * t1->stride + x] |= JPEG2000_T1_SIG_S;
    }
    t1->flags[(y + 1) * t1->stride + x + 1] |= JPEG2000_T1_SIG_NW;
    t1->flags[(y + 1) * t1->stride + x - 1] |= JPEG2000_T1_SIG_NE;
    t1->flags[(y - 1) * t1->stride + x + 1] |= JPEG2000_T1_SIG_SW;
    t1->flags[(y - 1) * t1->stride + x - 1] |= JPEG2000_T1_SIG_SE;
}


static const uint8_t ff_log2_tab[256]={
      0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
      5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
      6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
      6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
   };

int av_log2(unsigned v){
   int n = 0;

   if (v & 0xffff0000) {
      v >>= 16;
      n += 16;
   }
   if (v & 0xff00) {
      v >>= 8;
      n += 8;
   }
   n += ff_log2_tab[v];

   return n;
}
