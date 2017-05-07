#include "ebcot_enc.h"






static int getnmsedec_sig(int x, int bpno)
{
    if (bpno > NMSEDEC_FRACBITS)
        return lut_nmsedec_sig[(x >> (bpno - NMSEDEC_FRACBITS)) & ((1 << NMSEDEC_BITS) - 1)];
    return lut_nmsedec_sig0[x & ((1 << NMSEDEC_BITS) - 1)];
}

static void encode_sigpass(
		Jpeg2000T1Context *t1, MqcState *mqc,
		int width, int height, int bandno, int *nmsedec, int bpno)
{
    int y0, x, y, mask = 1 << (bpno + NMSEDEC_FRACBITS);
    for (y0 = 0; y0 < height; y0 += 4)
        for (x = 0; x < width; x++)
            for (y = y0; y < height && y < y0+4; y++){
                if (!(t1->flags[(y+1) * t1->stride + x+1] & JPEG2000_T1_SIG) && (t1->flags[(y+1) * t1->stride + x+1] & JPEG2000_T1_SIG_NB)){
                    int ctxno = ff_jpeg2000_getsigctxno(t1->flags[(y+1) * t1->stride + x+1], bandno),
                        bit = t1->data[(y) * t1->stride + x] & mask ? 1 : 0;
                    ff_mqc_encode(mqc, mqc->cx_states + ctxno, bit);
                    if (bit){
                        int xorbit;
                        int ctxno = ff_jpeg2000_getsgnctxno(t1->flags[(y+1) * t1->stride + x+1], &xorbit);
                        ff_mqc_encode(mqc, mqc->cx_states + ctxno, (t1->flags[(y+1) * t1->stride + x+1] >> 15) ^ xorbit);
                        *nmsedec += getnmsedec_sig(t1->data[(y) * t1->stride + x], bpno + NMSEDEC_FRACBITS);
                        ff_jpeg2000_set_significance(t1, x, y, t1->flags[(y+1) * t1->stride + x+1] >> 15);
                    }
                    t1->flags[(y+1) * t1->stride + x+1] |= JPEG2000_T1_VIS;
                }
            }
}

static int getnmsedec_ref(int x, int bpno)
{
    if (bpno > NMSEDEC_FRACBITS)
        return lut_nmsedec_ref[(x >> (bpno - NMSEDEC_FRACBITS)) & ((1 << NMSEDEC_BITS) - 1)];
    return lut_nmsedec_ref0[x & ((1 << NMSEDEC_BITS) - 1)];
}

static void encode_refpass(
		Jpeg2000T1Context *t1, MqcState *mqc,
		int width, int height, int *nmsedec, int bpno)
{
    int y0, x, y, mask = 1 << (bpno + NMSEDEC_FRACBITS);
    for (y0 = 0; y0 < height; y0 += 4)
        for (x = 0; x < width; x++)
            for (y = y0; y < height && y < y0+4; y++)
                if ((t1->flags[(y+1) * t1->stride + x+1] & (JPEG2000_T1_SIG | JPEG2000_T1_VIS)) == JPEG2000_T1_SIG){
                    int ctxno = ff_jpeg2000_getrefctxno(t1->flags[(y+1) * t1->stride + x+1]);
                    *nmsedec += getnmsedec_ref(t1->data[(y) * t1->stride + x], bpno + NMSEDEC_FRACBITS);
                    ff_mqc_encode(mqc, mqc->cx_states + ctxno, t1->data[(y) * t1->stride + x] & mask ? 1:0);
                    t1->flags[(y+1) * t1->stride + x+1] |= JPEG2000_T1_REF;
                }
}

static void encode_clnpass(
		Jpeg2000T1Context *t1, 
		MqcState *mqc,
		int width, int height, int bandno, int *nmsedec, int bpno)
{
    int y0, x, y, mask = 1 << (bpno + NMSEDEC_FRACBITS);
    for (y0 = 0; y0 < height; y0 += 4)
        for (x = 0; x < width; x++){
            if (y0 + 3 < height && !(
            (t1->flags[(y0+1) * t1->stride + x+1] & (JPEG2000_T1_SIG_NB | JPEG2000_T1_VIS | JPEG2000_T1_SIG)) ||
            (t1->flags[(y0+2) * t1->stride + x+1] & (JPEG2000_T1_SIG_NB | JPEG2000_T1_VIS | JPEG2000_T1_SIG)) ||
            (t1->flags[(y0+3) * t1->stride + x+1] & (JPEG2000_T1_SIG_NB | JPEG2000_T1_VIS | JPEG2000_T1_SIG)) ||
            (t1->flags[(y0+4) * t1->stride + x+1] & (JPEG2000_T1_SIG_NB | JPEG2000_T1_VIS | JPEG2000_T1_SIG))))
            {
                // aggregation mode
                int rlen;
                for (rlen = 0; rlen < 4; rlen++)
                    if (t1->data[(y0+rlen) * t1->stride + x] & mask)
                        break;
                ff_mqc_encode(mqc, mqc->cx_states + MQC_CX_RL, rlen != 4);
                if (rlen == 4)
                    continue;
                ff_mqc_encode(mqc, mqc->cx_states + MQC_CX_UNI, rlen >> 1);
                ff_mqc_encode(mqc, mqc->cx_states + MQC_CX_UNI, rlen & 1);
                for (y = y0 + rlen; y < y0 + 4; y++){
                    if (!(t1->flags[(y+1) * t1->stride + x+1] & (JPEG2000_T1_SIG | JPEG2000_T1_VIS))){
                        int ctxno = ff_jpeg2000_getsigctxno(t1->flags[(y+1) * t1->stride + x+1], bandno);
                        if (y > y0 + rlen)
                            ff_mqc_encode(mqc, mqc->cx_states + ctxno, t1->data[(y) * t1->stride + x] & mask ? 1:0);
                        if (t1->data[(y) * t1->stride + x] & mask){ // newly significant
                            int xorbit;
                            int ctxno = ff_jpeg2000_getsgnctxno(t1->flags[(y+1) * t1->stride + x+1], &xorbit);
                            *nmsedec += getnmsedec_sig(t1->data[(y) * t1->stride + x], bpno + NMSEDEC_FRACBITS);
                            ff_mqc_encode(mqc, mqc->cx_states + ctxno, (t1->flags[(y+1) * t1->stride + x+1] >> 15) ^ xorbit);
                            ff_jpeg2000_set_significance(t1, x, y, t1->flags[(y+1) * t1->stride + x+1] >> 15);
                        }
                    }
                    t1->flags[(y+1) * t1->stride + x+1] &= ~JPEG2000_T1_VIS;
                }
            } else{
                for (y = y0; y < y0 + 4 && y < height; y++){
                    if (!(t1->flags[(y+1) * t1->stride + x+1] & (JPEG2000_T1_SIG | JPEG2000_T1_VIS))){
                        int ctxno = ff_jpeg2000_getsigctxno(t1->flags[(y+1) * t1->stride + x+1], bandno);
                        ff_mqc_encode(mqc, mqc->cx_states + ctxno, t1->data[(y) * t1->stride + x] & mask ? 1:0);
                        if (t1->data[(y) * t1->stride + x] & mask){ // newly significant
                            int xorbit;
                            int ctxno = ff_jpeg2000_getsgnctxno(t1->flags[(y+1) * t1->stride + x+1], &xorbit);
                            *nmsedec += getnmsedec_sig(t1->data[(y) * t1->stride + x], bpno + NMSEDEC_FRACBITS);
                            ff_mqc_encode(mqc, mqc->cx_states + ctxno, (t1->flags[(y+1) * t1->stride + x+1] >> 15) ^ xorbit);
                            ff_jpeg2000_set_significance(t1, x, y, t1->flags[(y+1) * t1->stride + x+1] >> 15);
                        }
                    }
                    t1->flags[(y+1) * t1->stride + x+1] &= ~JPEG2000_T1_VIS;
                }
            }
        }
}



void encode_cblk(
		Jpeg2000T1Context *t1, MqcState *mqc, 
		Jpeg2000Cblk *cblk,
        int width, int height, int bandpos )
{
    int pass_t = 2, passno, x, y, max=0, nmsedec, bpno;
    int64_t wmsedec = 0;

    memset(t1->flags, 0, t1->stride * (height + 2) * sizeof(*t1->flags));

    for (y = 0; y < height; y++){
        for (x = 0; x < width; x++){
            if (t1->data[(y) * t1->stride + x] < 0){
                t1->flags[(y+1) * t1->stride + x+1] |= JPEG2000_T1_SGN;
                t1->data[(y) * t1->stride + x] = -t1->data[(y) * t1->stride + x];
            }
            max = FFMAX(max, t1->data[(y) * t1->stride + x]);
        }
    }

    if (max == 0){
        cblk->nonzerobits = 0;
        bpno = 0;
    } else{
        cblk->nonzerobits = av_log2(max) + 1 - NMSEDEC_FRACBITS;
        bpno = cblk->nonzerobits - 1;
    }

    ff_mqc_initenc(mqc, cblk->data);

    for (passno = 0; bpno >= 0; passno++){
        nmsedec=0;

        switch(pass_t){
            case 0: encode_sigpass(t1, mqc, width, height, bandpos, &nmsedec, bpno);
                    break;
            case 1: encode_refpass(t1, mqc, width, height, &nmsedec, bpno);
                    break;
            case 2: encode_clnpass(t1, mqc, width, height, bandpos, &nmsedec, bpno);
                    break;
        }

        cblk->passes[passno].rate = ff_mqc_flush_to(mqc, cblk->passes[passno].flushed, &cblk->passes[passno].flushed_len);
        wmsedec += (int64_t)nmsedec << (2*bpno);
        cblk->passes[passno].disto = wmsedec;

        if (++pass_t == 3){
            pass_t = 0;
            bpno--;
        }
    }
    cblk->npasses = passno;
    cblk->ninclpasses = passno;

    cblk->passes[passno-1].rate = ff_mqc_flush_to(mqc, cblk->passes[passno-1].flushed, &cblk->passes[passno-1].flushed_len);
}

