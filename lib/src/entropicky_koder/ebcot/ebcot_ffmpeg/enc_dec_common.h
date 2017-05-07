#ifndef ENC_DEC_COMMON_HEADER_INCLUDED
#define ENC_DEC_COMMON_HEADER_INCLUDED

#include <cstdint>
#include <cstring>
#include <cassert>
#include <iostream>

#include "mqc.h"










#define FFSWAP(type,a,b) do{type SWAP_tmp= b; b= a; a= SWAP_tmp;}while(0)
#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFABS(a) ((a) >= 0 ? (a) : (-(a)))

/* tier-1 routines */
#define NMSEDEC_BITS 7
//#define NMSEDEC_BITS 3   // moje uprava
#define NMSEDEC_FRACBITS (NMSEDEC_BITS-1)

#define JPEG2000_MAX_PASSES   100

// T1 flags
// flags determining significance of neighbor coefficients
#define JPEG2000_T1_SIG_N  0x0001
#define JPEG2000_T1_SIG_E  0x0002
#define JPEG2000_T1_SIG_W  0x0004
#define JPEG2000_T1_SIG_S  0x0008
#define JPEG2000_T1_SIG_NE 0x0010
#define JPEG2000_T1_SIG_NW 0x0020
#define JPEG2000_T1_SIG_SE 0x0040
#define JPEG2000_T1_SIG_SW 0x0080
#define JPEG2000_T1_SIG_NB (JPEG2000_T1_SIG_N  | JPEG2000_T1_SIG_E  |   \
                            JPEG2000_T1_SIG_S  | JPEG2000_T1_SIG_W  |   \
                            JPEG2000_T1_SIG_NE | JPEG2000_T1_SIG_NW |   \
                            JPEG2000_T1_SIG_SE | JPEG2000_T1_SIG_SW)
// flags determining sign bit of neighbor coefficients
#define JPEG2000_T1_SGN_N  0x0100
#define JPEG2000_T1_SGN_S  0x0200
#define JPEG2000_T1_SGN_W  0x0400
#define JPEG2000_T1_SGN_E  0x0800

#define JPEG2000_T1_VIS    0x1000
#define JPEG2000_T1_SIG    0x2000
#define JPEG2000_T1_REF    0x4000

#define JPEG2000_T1_SGN    0x8000

// Codeblock coding styles
#define JPEG2000_CBLK_BYPASS    0x01 // Selective arithmetic coding bypass
#define JPEG2000_CBLK_RESET     0x02 // Reset context probabilities
#define JPEG2000_CBLK_TERMALL   0x04 // Terminate after each coding pass
#define JPEG2000_CBLK_VSC       0x08 // Vertical stripe causal context formation
#define JPEG2000_CBLK_PREDTERM  0x10 // Predictable termination
#define JPEG2000_CBLK_SEGSYM    0x20 // Segmentation symbols present

/********************* STRUKTURY ************************/

typedef struct Jpeg2000T1Context {
    // enc: mq prepisuje
    int data[6144 + 20]; // +20 je kvuli 1 extra flush
    // enc: nulovani
    uint16_t flags[6156];
    // enc: nastavuje
//    MqcState mqc;          //m  19 kontextu + mq koder
    int stride;            //m  pohyb v obrazku ==> celkove y - sizey cblk
} Jpeg2000T1Context;


typedef struct Jpeg2000Pass {
    uint16_t rate;
    int64_t disto;
    uint8_t flushed[4];
    int flushed_len;
} Jpeg2000Pass;

typedef struct Jpeg2000Cblk {
    uint8_t npasses;
    uint8_t ninclpasses; // number coding of passes included in codestream
    uint8_t nonzerobits;
    uint16_t length;
    uint8_t data[8192];
    int data_start[JPEG2000_MAX_PASSES];
    Jpeg2000Pass passes[JPEG2000_MAX_PASSES];
} Jpeg2000Cblk; // code block


/************************* LUTS ***************************/

extern uint8_t ff_jpeg2000_sigctxno_lut[256][4];
extern uint8_t ff_jpeg2000_sgnctxno_lut[16][16],
               ff_jpeg2000_xorbit_lut[16][16];
extern int lut_nmsedec_ref [1<<NMSEDEC_BITS],
           lut_nmsedec_ref0[1<<NMSEDEC_BITS],
           lut_nmsedec_sig [1<<NMSEDEC_BITS],
           lut_nmsedec_sig0[1<<NMSEDEC_BITS];


/* Get context label (number in range[0..8]) of a coefficient for significance
 * propagation and cleanup coding passes. */
static inline int ff_jpeg2000_getsigctxno(int flag, int bandno){
    return ff_jpeg2000_sigctxno_lut[flag & 255][bandno];
}
/* Get context label (number in range[9..13]) for sign decoding. */
static inline int ff_jpeg2000_getsgnctxno(int flag, int *xorbit)
{
    *xorbit = ff_jpeg2000_xorbit_lut[flag & 15][(flag >> 8) & 15];
    return ff_jpeg2000_sgnctxno_lut[flag & 15][(flag >> 8) & 15];
}
/* Get context label (number in range[14..16]) of a coefficient for magnitude
 * refinement pass. */
static inline int ff_jpeg2000_getrefctxno(int flag)
{
    static const uint8_t refctxno_lut[2][2] = { { 14, 15 }, { 16, 16 } };
    return refctxno_lut[(flag >> 14) & 1][(flag & 255) != 0];
}

void ff_jpeg2000_set_significance(Jpeg2000T1Context *t1, int x, int y, int negative);

int av_log2(unsigned v);


void init_luts(void);
void ff_jpeg2000_init_tier1_luts(void);

#endif // ENC_DEC_COMMON_HEADER_INCLUDED
