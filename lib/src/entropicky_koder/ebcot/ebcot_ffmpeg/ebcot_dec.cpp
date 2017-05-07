#include "enc_dec_common.h"

static inline int needs_termination(int style, int passno) {
    if (style & JPEG2000_CBLK_BYPASS) {
        int type = passno % 3;
        passno /= 3;
        if (type == 0 && passno > 2)
            return 2;
        if (type == 2 && passno > 2)
            return 1;
        if (style & JPEG2000_CBLK_TERMALL) {
            return passno > 2 ? 2 : 1;
        }
    }
    if (style & JPEG2000_CBLK_TERMALL)
        return 1;
    return 0;
}


/* TIER-1 routines */


static void decode_sigpass(Jpeg2000T1Context *t1, MqcState *mqc, int width, int height,
                           int bpno, int bandno,
                           int vert_causal_ctx_csty_symbol)
{
    int mask = 3 << (bpno - 1), y0, x, y;

    for (y0 = 0; y0 < height; y0 += 4)
        for (x = 0; x < width; x++)
            for (y = y0; y < height && y < y0 + 4; y++) {
                int flags_mask = -1;
                if (vert_causal_ctx_csty_symbol && y == y0 + 3)
                    flags_mask &= ~(JPEG2000_T1_SIG_S | JPEG2000_T1_SIG_SW | JPEG2000_T1_SIG_SE | JPEG2000_T1_SGN_S);
                if ((t1->flags[(y+1) * t1->stride + x+1] & JPEG2000_T1_SIG_NB & flags_mask)
                && !(t1->flags[(y+1) * t1->stride + x+1] & (JPEG2000_T1_SIG | JPEG2000_T1_VIS))) {
                    if (ff_mqc_decode(mqc, mqc->cx_states + ff_jpeg2000_getsigctxno(t1->flags[(y+1) * t1->stride + x+1] & flags_mask, bandno))) {
                        int xorbit, ctxno = ff_jpeg2000_getsgnctxno(t1->flags[(y+1) * t1->stride + x+1] & flags_mask, &xorbit);
                        if (mqc->raw)
                             t1->data[(y) * t1->stride + x] = ff_mqc_decode(mqc, mqc->cx_states + ctxno) ? -mask : mask;
                        else
                             t1->data[(y) * t1->stride + x] = (ff_mqc_decode(mqc, mqc->cx_states + ctxno) ^ xorbit) ?
                                               -mask : mask;

                        ff_jpeg2000_set_significance(t1, x, y,
                                                     t1->data[(y) * t1->stride + x] < 0);
                    }
                    t1->flags[(y + 1) * t1->stride + x + 1] |= JPEG2000_T1_VIS;
                }
            }
}



static void decode_refpass(Jpeg2000T1Context *t1, MqcState *mqc, int width, int height,
                           int bpno, int vert_causal_ctx_csty_symbol)
{
    int phalf, nhalf;
    int y0, x, y;

    phalf = 1 << (bpno - 1);
    nhalf = -phalf;

    for (y0 = 0; y0 < height; y0 += 4)
        for (x = 0; x < width; x++)
            for (y = y0; y < height && y < y0 + 4; y++)
                if ((t1->flags[(y + 1) * t1->stride + x + 1] & (JPEG2000_T1_SIG | JPEG2000_T1_VIS)) == JPEG2000_T1_SIG) {
                    int flags_mask = (vert_causal_ctx_csty_symbol && y == y0 + 3) ?
                        ~(JPEG2000_T1_SIG_S | JPEG2000_T1_SIG_SW | JPEG2000_T1_SIG_SE | JPEG2000_T1_SGN_S) : -1;
                    int ctxno = ff_jpeg2000_getrefctxno(t1->flags[(y + 1) * t1->stride + x + 1] & flags_mask);
                    int r     = ff_mqc_decode(mqc,
                                              mqc->cx_states + ctxno)
                                ? phalf : nhalf;
                    t1->data[(y) * t1->stride + x]          += t1->data[(y) * t1->stride + x] < 0 ? -r : r;
                    t1->flags[(y + 1) * t1->stride + x + 1] |= JPEG2000_T1_REF;
                }
}



static void decode_clnpass( Jpeg2000T1Context *t1, MqcState *mqc,
                           int width, int height, int bpno, int bandno,
                           int seg_symbols, int vert_causal_ctx_csty_symbol)
{
    int mask = 3 << (bpno - 1), y0, x, y, runlen, dec;

    for (y0 = 0; y0 < height; y0 += 4) {
        for (x = 0; x < width; x++) {
            int flags_mask = -1;
            if (vert_causal_ctx_csty_symbol)
                flags_mask &= ~(JPEG2000_T1_SIG_S | JPEG2000_T1_SIG_SW | JPEG2000_T1_SIG_SE | JPEG2000_T1_SGN_S);
            if (y0 + 3 < height &&
                !((t1->flags[(y0 + 1) * t1->stride + x + 1] & (JPEG2000_T1_SIG_NB | JPEG2000_T1_VIS | JPEG2000_T1_SIG)) ||
                  (t1->flags[(y0 + 2) * t1->stride + x + 1] & (JPEG2000_T1_SIG_NB | JPEG2000_T1_VIS | JPEG2000_T1_SIG)) ||
                  (t1->flags[(y0 + 3) * t1->stride + x + 1] & (JPEG2000_T1_SIG_NB | JPEG2000_T1_VIS | JPEG2000_T1_SIG)) ||
                  (t1->flags[(y0 + 4) * t1->stride + x + 1] & (JPEG2000_T1_SIG_NB | JPEG2000_T1_VIS | JPEG2000_T1_SIG) & flags_mask))) {
                if (!ff_mqc_decode(mqc, mqc->cx_states + MQC_CX_RL))
                    continue;
                runlen = ff_mqc_decode(mqc,
                                       mqc->cx_states + MQC_CX_UNI);
                runlen = (runlen << 1) | ff_mqc_decode(mqc,
                                                       mqc->cx_states +
                                                       MQC_CX_UNI);
                dec = 1;
            } else {
                runlen = 0;
                dec    = 0;
            }

            for (y = y0 + runlen; y < y0 + 4 && y < height; y++) {
                int flags_mask = -1;
                if (vert_causal_ctx_csty_symbol && y == y0 + 3)
                    flags_mask &= ~(JPEG2000_T1_SIG_S | JPEG2000_T1_SIG_SW | JPEG2000_T1_SIG_SE | JPEG2000_T1_SGN_S);
                if (!dec) {
                    if (!(t1->flags[(y+1) * t1->stride + x+1] & (JPEG2000_T1_SIG | JPEG2000_T1_VIS))) {
                        dec = ff_mqc_decode(mqc, mqc->cx_states + ff_jpeg2000_getsigctxno(t1->flags[(y+1) * t1->stride + x+1] & flags_mask,
                                                                                             bandno));
                    }
                }
                if (dec) {
                    int xorbit;
                    int ctxno = ff_jpeg2000_getsgnctxno(t1->flags[(y + 1) * t1->stride + x + 1] & flags_mask,
                                                        &xorbit);
                    t1->data[(y) * t1->stride + x] = (ff_mqc_decode(mqc,
                                                    mqc->cx_states + ctxno) ^
                                      xorbit)
                                     ? -mask : mask;
                    ff_jpeg2000_set_significance(t1, x, y, t1->data[(y) * t1->stride + x] < 0);
                }
                dec = 0;
                t1->flags[(y + 1) * t1->stride + x + 1] &= ~JPEG2000_T1_VIS;
            }
        }
    }
    if (seg_symbols) {
        int val;
        val = ff_mqc_decode(mqc, mqc->cx_states + MQC_CX_UNI);
        val = (val << 1) + ff_mqc_decode(mqc, mqc->cx_states + MQC_CX_UNI);
        val = (val << 1) + ff_mqc_decode(mqc, mqc->cx_states + MQC_CX_UNI);
        val = (val << 1) + ff_mqc_decode(mqc, mqc->cx_states + MQC_CX_UNI);
        if (val != 0xa)
           std::cerr << "Segmentation symbol value incorrect" << std::endl;
    }
}


int decode_cblk(  Jpeg2000T1Context *t1, MqcState *mqc, Jpeg2000Cblk *cblk,
                  int width, int height, int bandpos)
{
    int passno = cblk->npasses, pass_t = 2, bpno = cblk->nonzerobits - 1;
    int pass_cnt = 0;
    int vert_causal_ctx_csty_symbol = 0 & JPEG2000_CBLK_VSC;
    int term_cnt = 0;
    int coder_type;

    assert(width <= 1024 && height <= 1024);
    assert(width*height <= 4096);

    memset(t1->data, 0, t1->stride * height * sizeof(*t1->data));

    /* If code-block contains no compressed data: nothing to do. */
    if (!cblk->length)
        return 0;

    memset(t1->flags, 0, t1->stride * (height + 2) * sizeof(*t1->flags));

    cblk->data[cblk->length] = 0xff;
    cblk->data[cblk->length+1] = 0xff;
    ff_mqc_initdec(mqc, cblk->data, 0, 1);

    while (passno--) {
        if (bpno < 0) {
            //av_log(s->avctx, AV_LOG_ERROR, "bpno became negative\n");
            return 1;
        }
        switch(pass_t) {
        case 0:
            decode_sigpass(t1, mqc, width, height, bpno + 1, bandpos,
                           vert_causal_ctx_csty_symbol);
            break;
        case 1:
            decode_refpass(t1, mqc, width, height, bpno + 1, vert_causal_ctx_csty_symbol);
            break;
        case 2:
            assert(!mqc->raw);
            decode_clnpass(/*s,*/ t1, mqc, width, height, bpno + 1, bandpos,
                           0 & JPEG2000_CBLK_SEGSYM,
                           vert_causal_ctx_csty_symbol);
            break;
        }
        if (0 & JPEG2000_CBLK_RESET) // XXX no testcase for just this
            ff_mqc_init_contexts(mqc);

        if (passno && (coder_type = needs_termination(0, pass_cnt))) {
            //if (term_cnt >= cblk->nb_terminations) {
                //av_log(s->avctx, AV_LOG_ERROR, "Missing needed termination \n");
                //return 1; //return AVERROR_INVALIDDATA;
            //}
            if (FFABS(cblk->data + cblk->data_start[term_cnt + 1] - 2 - mqc->bp) > 0) {
                //av_log(s->avctx, AV_LOG_WARNING, "Mid mismatch %"PTRDIFF_SPECIFIER" in pass %d of %d\n",
                //    cblk->data + cblk->data_start[term_cnt + 1] - 2 - mqc->bp,
                //    pass_cnt, cblk->npasses);
            }

            ff_mqc_initdec(mqc, cblk->data + cblk->data_start[++term_cnt], coder_type == 2, 0);
        }

        pass_t++;
        if (pass_t == 3) {
            bpno--;
            pass_t = 0;
        }
        pass_cnt ++;
    }

    if (cblk->data + cblk->length - 2*(term_cnt < 0) != mqc->bp) {
       std::cerr << "End mismatch ... " << std::endl;
       }
        //av_log(s->avctx, AV_LOG_WARNING, "End mismatch %"PTRDIFF_SPECIFIER"\n",
            //   cblk->data + cblk->length - 2*(term_cnt < cblk->nb_terminations) - t1->mqc.bp);
   // }

    return 0;
}

