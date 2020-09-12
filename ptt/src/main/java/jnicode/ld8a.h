/*****************************************************************************
模块名      : g729ab编解码器
文件名      : 
相关文件    : 
文件实现功能: g729ab编解码功能句柄式接口实现
版本        : V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
作者        : 徐超
-----------------------------------------------------------------------------
修改记录:
日    期      版本        修改人      修改内容
2006/02/16    1.0         徐  超      创    建
******************************************************************************/

#ifndef LD8A_H
#define LD8A_H

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------*
 * LD8A.H                                                        *
 * ~~~~~~                                                        *
 * Function prototypes and constants use for G.729A 8kb/s coder. *
 *                                                               *
 *---------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 *       Codec constant parameters (coder, decoder, and postfilter)         *
 *--------------------------------------------------------------------------*/
#include "algorithmtype_xc.h"
#include "g729ab.h"


/*-------------------------------*
 * Mathematic functions.         *
 *-------------------------------*/

l32 Inv_sqrt(   /* (o) Q30 : output value   (range: 0<=val<1)           */
  l32 L_x       /* (i) Q0  : input value    (range: 0<=val<=7fffffff)   */
);

void Log2(
  l32 L_x,       /* (i) Q0 : input value                                 */
  s16 *exponent, /* (o) Q0 : Integer part of Log2.   (range: 0<=val<=30) */
  s16 *fraction  /* (o) Q15: Fractionnal part of Log2. (range: 0<=val<1) */
);

l32 Pow2(        /* (o) Q0  : result       (range: 0<=val<=0x7fffffff) */
  s16 exponent,  /* (i) Q0  : Integer part.      (range: 0<=val<=30)   */
  s16 fraction   /* (i) Q15 : Fractionnal part.  (range: 0.0<=val<1.0) */
);

/*-------------------------------*
 * Pre and post-process.         *
 *-------------------------------*/

void Init_Pre_Process(TG729BENCODER *pG729EncH);

void Init_Post_Process(TG729BDECODER *pG729DecH);

void Pre_Process(
  s16 signal[],   /* Input/output signal */
  s16 lg,          /* Length of signal    */
  TG729BENCODER *pG729EncH
);

void Post_Process(
 s16 signal[],   /* Input/output signal */
 s16 lg,          /* Length of signal    */
 TG729BDECODER *pG729DecH
);

/*----------------------------------*
 * Main coder and decoder functions *
 *----------------------------------*/

//void Init_Coder_ld8a(void);
 void Init_Coder_ld8a(TG729BENCODER *pG729EncH);

void Coder_ld8a(
 s16 ana[],       /* output  : Analysis parameters */
 s16 frame,
 s16 vad_enable,
 TG729BENCODER *pG729EncH
);

void Init_Decod_ld8a(TG729BDECODER *pG729DecH);

void Decod_ld8a(
  s16  parm[],      /* (i)   : vector of synthesis parameters
                                  parm[0] = bad frame indicator (bfi)  */
  s16  synth[],     /* (o)   : synthesis speech                     */
  s16  A_t[],       /* (o)   : decoded LP filter in 2 subframes     */
  s16  *T2,         /* (o)   : decoded pitch lag in 2 subframes     */
  s16  *Vad,         /* (o)   : VAD                                  */
  TG729BDECODER *pG729DecH
);

/*-------------------------------*
 * LPC analysis and filtering.   *
 *-------------------------------*/

void Autocorr(
  s16 x[],      /* (i)    : Input signal                      */
  s16 m,        /* (i)    : LPC order                         */
  s16 r_h[],    /* (o)    : Autocorrelations  (msb)           */
  s16 r_l[],    /* (o)    : Autocorrelations  (lsb)           */
  s16 *exp_R0
);

void Lag_window(
  s16 m,         /* (i)     : LPC order                        */
  s16 r_h[],     /* (i/o)   : Autocorrelations  (msb)          */
  s16 r_l[]      /* (i/o)   : Autocorrelations  (lsb)          */
);

void Levinson(
  s16 Rh[],      /* (i)     : Rh[m+1] Vector of autocorrelations (msb) */
  s16 Rl[],      /* (i)     : Rl[m+1] Vector of autocorrelations (lsb) */
  s16 A[],       /* (o) Q12 : A[m]    LPC coefficients  (m = 10)       */
  s16 rc[],      /* (o) Q15 : rc[M]   Relection coefficients.          */
  s16 *Err       /* (o)     : Residual energy                          */
);

void Az_lsp(
  s16 a[],        /* (i) Q12 : predictor coefficients              */
  s16 lsp[],      /* (o) Q15 : line spectral pairs                 */
  s16 old_lsp[]   /* (i)     : old lsp[] (in case not found 10 roots) */
);

void Lsp_Az(
  s16 lsp[],    /* (i) Q15 : line spectral frequencies            */
  s16 a[]       /* (o) Q12 : predictor coefficients (order = 10)  */
);

void Lsf_lsp(
  s16 lsf[],    /* (i) Q15 : lsf[m] normalized (range: 0.0<=val<=0.5) */
  s16 lsp[],    /* (o) Q15 : lsp[m] (range: -1<=val<1)                */
  s16 m         /* (i)     : LPC order                                */
);

void Lsp_lsf(
  s16 lsp[],    /* (i) Q15 : lsp[m] (range: -1<=val<1)                */
  s16 lsf[],    /* (o) Q15 : lsf[m] normalized (range: 0.0<=val<=0.5) */
  s16 m         /* (i)     : LPC order                                */
);

void Int_qlpc(
 s16 lsp_old[], /* input : LSP vector of past frame              */
 s16 lsp_new[], /* input : LSP vector of present frame           */
 s16 Az[]       /* output: interpolated Az() for the 2 subframes */
);

void Weight_Az(
  s16 a[],      /* (i) Q12 : a[m+1]  LPC coefficients             */
  s16 gamma,    /* (i) Q15 : Spectral expansion factor.           */
  s16 m,        /* (i)     : LPC order.                           */
  s16 ap[]      /* (o) Q12 : Spectral expanded LPC coefficients   */
);

void Residu(
  s16 a[],    /* (i) Q12 : prediction coefficients                     */
  s16 x[],    /* (i)     : speech (values x[-m..-1] are needed (m=10)  */
  s16 y[],    /* (o)     : residual signal                             */
  s16 lg      /* (i)     : size of filtering                           */
);

void Syn_filt(
  s16 a[],     /* (i) Q12 : a[m+1] prediction coefficients   (m=10)  */
  s16 x[],     /* (i)     : input signal                             */
  s16 y[],     /* (o)     : output signal                            */
  s16 lg,      /* (i)     : size of filtering                        */
  s16 mem[],   /* (i/o)   : memory associated with this filtering.   */
  s16 update   /* (i)     : 0=no update, 1=update of memory.         */
);

void Convolve(
  s16 x[],      /* (i)     : input vector                           */
  s16 h[],      /* (i) Q12 : impulse response                       */
  s16 y[],      /* (o)     : output vector                          */
  s16 L         /* (i)     : vector size                            */
);


/*-----------------------*
 * Pitch functions.      *
 *-----------------------*/

s16 Pitch_ol_fast(  /* output: open loop pitch lag                        */
   s16 signal[],    /* input : signal used to compute the open loop pitch */
                       /*     signal[-pit_max] to signal[-1] should be known */
   s16   pit_max,   /* input : maximum pitch lag                          */
   s16   L_frame    /* input : length of frame to compute pitch           */
);

s16 Pitch_fr3_fast(/* (o)     : pitch period.                          */
  s16 exc[],       /* (i)     : excitation buffer                      */
  s16 xn[],        /* (i)     : target vector                          */
  s16 h[],         /* (i) Q12 : impulse response of filters.           */
  s16 L_subfr,     /* (i)     : Length of subframe                     */
  s16 t0_min,      /* (i)     : minimum value in the searched range.   */
  s16 t0_max,      /* (i)     : maximum value in the searched range.   */
  s16 i_subfr,     /* (i)     : indicator for first subframe.          */
  s16 *pit_frac    /* (o)     : chosen fraction.                       */
);

s16 G_pitch(      /* (o) Q14 : Gain of pitch lag saturated to 1.2       */
  s16 xn[],       /* (i)     : Pitch target.                            */
  s16 y1[],       /* (i)     : Filtered adaptive codebook.              */
  s16 g_coeff[],  /* (i)     : Correlations need for gain quantization. */
  s16 L_subfr     /* (i)     : Length of subframe.                      */
);

s16 Enc_lag3(     /* output: Return index of encoding */
  s16 T0,         /* input : Pitch delay              */
  s16 T0_frac,    /* input : Fractional pitch delay   */
  s16 *T0_min,    /* in/out: Minimum search delay     */
  s16 *T0_max,    /* in/out: Maximum search delay     */
  s16 pit_min,    /* input : Minimum pitch delay      */
  s16 pit_max,    /* input : Maximum pitch delay      */
  s16 pit_flag    /* input : s16 for 1st subframe    */
);

void Dec_lag3(        /* output: return integer pitch lag       */
  s16 index,       /* input : received pitch index           */
  s16 pit_min,     /* input : minimum pitch lag              */
  s16 pit_max,     /* input : maximum pitch lag              */
  s16 i_subfr,     /* input : subframe flag                  */
  s16 *T0,         /* output: integer part of pitch lag      */
  s16 *T0_frac     /* output: fractional part of pitch lag   */
);

s16 Interpol_3(      /* (o)  : interpolated value  */
  s16 *x,            /* (i)  : input vector        */
  s16 frac           /* (i)  : fraction            */
);

void Pred_lt_3(
  s16   exc[],       /* in/out: excitation buffer */
  s16   T0,          /* input : integer pitch lag */
  s16   frac,        /* input : fraction of lag   */
  s16   L_subfr      /* input : subframe size     */
);

s16 Parity_Pitch(    /* output: parity bit (XOR of 6 MSB bits)    */
   s16 pitch_index   /* input : index for which parity to compute */
);

s16  Check_Parity_Pitch( /* output: 0 = no error, 1= error */
  s16 pitch_index,       /* input : index of parameter     */
  s16 parity             /* input : parity bit             */
);

void Cor_h_X(
     s16 h[],        /* (i) Q12 :Impulse response of filters      */
     s16 X[],        /* (i)     :Target vector                    */
     s16 D[]         /* (o)     :Correlations between h[] and D[] */
                        /*          Normalized to 13 bits            */
);

/*-----------------------*
 * Innovative codebook.  *
 *-----------------------*/

#define DIM_RR  616 /* size of correlation matrix                            */
#define NB_POS  8   /* Number of positions for each pulse                    */
#define STEP    5   /* Step betweem position of the same pulse.              */
#define MSIZE   64  /* Size of vectors for cross-correlation between 2 pulses*/

/* The following constants are Q15 fractions.
   These fractions is used to keep maximum precision on "alp" sum */

#define _1_2    (s16)(16384)
#define _1_4    (s16)( 8192)
#define _1_8    (s16)( 4096)
#define _1_16   (s16)( 2048)

s16  ACELP_Code_A(    /* (o)     :index of pulses positions    */
  s16 x[],            /* (i)     :Target vector                */
  s16 h[],            /* (i) Q12 :Inpulse response of filters  */
  s16 T0,             /* (i)     :Pitch lag                    */
  s16 pitch_sharp,    /* (i) Q14 :Last quantized pitch gain    */
  s16 code[],         /* (o) Q13 :Innovative codebook          */
  s16 y[],            /* (o) Q12 :Filtered innovative codebook */
  s16 *sign           /* (o)     :Signs of 4 pulses            */
);

void Decod_ACELP(
  s16 sign,      /* (i)     : signs of 4 pulses.                       */
  s16 index,     /* (i)     : Positions of the 4 pulses.               */
  s16 cod[]      /* (o) Q13 : algebraic (fixed) codebook excitation    */
);
/*--------------------------------------------------------------------------*
 *       LSP constant parameters                                            *
 *--------------------------------------------------------------------------*/

/*-------------------------------*
 * LSP VQ functions.             *
 *-------------------------------*/

void Lsf_lsp2(
  s16 lsf[],    /* (i) Q13 : lsf[m] (range: 0.0<=val<PI) */
  s16 lsp[],    /* (o) Q15 : lsp[m] (range: -1<=val<1)   */
  s16 m         /* (i)     : LPC order                   */
);

void Lsp_lsf2(
  s16 lsp[],    /* (i) Q15 : lsp[m] (range: -1<=val<1)   */
  s16 lsf[],    /* (o) Q13 : lsf[m] (range: 0.0<=val<PI) */
  s16 m         /* (i)     : LPC order                   */
);

void Qua_lsp(
  s16 lsp[],       /* (i) Q15 : Unquantized LSP            */
  s16 lsp_q[],     /* (o) Q15 : Quantized LSP              */
  s16 ana[],        /* (o)     : indexes                    */
  TG729BENCODER *pG729EncH
);

void Get_wegt(
  s16 flsp[],    /* Q13 */
  s16 wegt[]     /* Q11 -> normalized */
);

//void Lsp_encw_reset(  void);
void Lsp_encw_reset(TG729BENCODER *pG729EncH);

void Lsp_qua_cs(
  s16 flsp_in[M],    /* Q13 */
  s16 lspq_out[M],   /* Q13 */
  s16 *code,
  TG729BENCODER *pG729EncH
);

void Lsp_expand_1(
  s16 buf[],          /* Q13 */
  s16 gap             /* Q13 */
);

void Lsp_expand_2(
  s16 buf[],         /* Q13 */
  s16 gap            /* Q13 */
);

void Lsp_expand_1_2(
  s16 buf[],         /* Q13 */
  s16 gap            /* Q13 */
);

void Lsp_get_quant(
  s16 lspcb1[][M],      /* Q13 */
  s16 lspcb2[][M],      /* Q13 */
  s16 code0,
  s16 code1,
  s16 code2,
  s16 fg[][M],            /* Q15 */
  s16 freq_prev[][M],     /* Q13 */
  s16 lspq[],                /* Q13 */
  s16 fg_sum[]               /* Q15 */
);

void Lsp_get_tdist(
  s16 wegt[],        /* normalized */
  s16 buf[],         /* Q13 */
  l32 *L_tdist,      /* Q27 */
  s16 rbuf[],        /* Q13 */
  s16 fg_sum[]       /* Q15 */
);

void Lsp_last_select(
  l32 L_tdist[],     /* Q27 */
  s16 *mode_index
);

void Lsp_pre_select(
  s16 rbuf[],              /* Q13 */
  s16 lspcb1[][M],      /* Q13 */
  s16 *cand
);

void Lsp_select_1(
  s16 rbuf[],              /* Q13 */
  s16 lspcb1[],            /* Q13 */
  s16 wegt[],              /* normalized */
  s16 lspcb2[][M],      /* Q13 */
  s16 *index
);

void Lsp_select_2(
  s16 rbuf[],              /* Q13 */
  s16 lspcb1[],            /* Q13 */
  s16 wegt[],              /* normalized */
  s16 lspcb2[][M],      /* Q13 */
  s16 *index
);

void Lsp_stability(
  s16 buf[]     /* Q13 */
);

void Relspwed(
  s16 lsp[],                          /* Q13 */
  s16 wegt[],                         /* normalized */
  s16 lspq[],                         /* Q13 */
  s16 lspcb1[][M],                 /* Q13 */
  s16 lspcb2[][M],                 /* Q13 */
  s16 fg[MODE][MA_NP][M],          /* Q15 */
  s16 freq_prev[MA_NP][M],         /* Q13 */
  s16 fg_sum[MODE][M],             /* Q15 */
  s16 fg_sum_inv[MODE][M],         /* Q12 */
  s16 code_ana[]
);

void D_lsp(
  s16 prm[],          /* (i)     : indexes of the selected LSP */
  s16 lsp_q[],        /* (o) Q15 : Quantized LSP parameters    */
  s16 erase,           /* (i)     : frame erase information     */
  TG729BDECODER *pG729DecH
);


void Lsp_decw_reset(TG729BDECODER *pG729DecH);


void Lsp_iqua_cs(
 s16 prm[],          /* input : codes of the selected LSP*/
 s16 lsp_q[],        /* output: Quantized LSP parameters*/
 s16 erase,           /* input : frame erase information */
 TG729BDECODER *pG729DecH
);

void Lsp_prev_compose(
  s16 lsp_ele[],             /* Q13 */
  s16 lsp[],                 /* Q13 */
  s16 fg[][M],            /* Q15 */
  s16 freq_prev[][M],     /* Q13 */
  s16 fg_sum[]               /* Q15 */
);

void Lsp_prev_extract(
  s16 lsp[M],                 /* Q13 */
  s16 lsp_ele[M],             /* Q13 */
  s16 fg[MA_NP][M],           /* Q15 */
  s16 freq_prev[MA_NP][M],    /* Q13 */
  s16 fg_sum_inv[M]           /* Q12 */
);

void Lsp_prev_update(
  s16 lsp_ele[M],             /* Q13 */
  s16 freq_prev[MA_NP][M]     /* Q13 */
);


/*--------------------------------------------------------------------------*
 * gain VQ functions.                                                       *
 *--------------------------------------------------------------------------*/

s16 Qua_gain(
  s16 code[],    /* (i) Q13 : Innovative vector.                         */
  s16 g_coeff[], /* (i)     : Correlations <xn y1> -2<y1 y1>             */
                    /*            <y2,y2>, -2<xn,y2>, 2<y1,y2>              */
  s16 exp_coeff[],/* (i)    : Q-Format g_coeff[]                         */
  s16 L_subfr,   /* (i)     : Subframe length.                           */
  s16 *gain_pit, /* (o) Q14 : Pitch gain.                                */
  s16 *gain_cod, /* (o) Q1  : Code gain.                                 */
  s16 tameflag,   /* (i)     : flag set to 1 if taming is needed          */
  TG729BENCODER *pG729EncH
);

void Dec_gain(
  s16 index,     /* (i)     : Index of quantization.                     */
  s16 code[],    /* (i) Q13 : Innovative vector.                         */
  s16 L_subfr,   /* (i)     : Subframe length.                           */
  s16 bfi,       /* (i)     : Bad frame indicator                        */
  s16 *gain_pit, /* (o) Q14 : Pitch gain.                                */
  s16 *gain_cod,  /* (o) Q1  : Code gain.                                 */
  TG729BDECODER *pG729DecH
);

void Gain_predict(
  s16 past_qua_en[],/* (i) Q10 :Past quantized energies                  */
  s16 code[],    /* (i) Q13 : Innovative vector.                         */
  s16 L_subfr,   /* (i)     : Subframe length.                           */
  s16 *gcode0,   /* (o) Qxx : Predicted codebook gain                    */
  s16 *exp_gcode0 /* (o)    : Q-Format(gcode0)                           */
);

void Gain_update(
  s16 past_qua_en[],/* (i) Q10 :Past quantized energies                  */
  l32 L_gbk12    /* (i) Q13 : gbk1[indice1][1]+gbk2[indice2][1]          */
);

void Gain_update_erasure(
  s16 past_qua_en[]/* (i) Q10 :Past quantized energies                   */
);

void Corr_xy2(
      s16 xn[],           /* (i) Q0  :Target vector.                  */
      s16 y1[],           /* (i) Q0  :Adaptive codebook.              */
      s16 y2[],           /* (i) Q12 :Filtered innovative vector.     */
      s16 g_coeff[],      /* (o) Q[exp]:Correlations between xn,y1,y2 */
      s16 exp_g_coeff[]   /* (o)       :Q-format of g_coeff[]         */
);

/*-----------------------*
 * Bitstream function    *
 *-----------------------*/

void  prm2bits_ld8k(s16 prm[], s16 bits[]);
void  bits2prm_ld8k(s16 bits[], s16 prm[]);
void  bits2prm_ld8k_org(s16 bits[], s16 prm[]);

/*-----------------------------------*
 * Post-filter functions.            *
 *-----------------------------------*/

void Init_Post_Filter(TG729BDECODER *pG729DecH);

void Post_Filter(
  s16 *syn,       /* in/out: synthesis speech (postfiltered is output)    */
  s16 *Az_4,       /* input : interpolated LPC parameters in all subframes */
  s16 *T,           /* input : decoded pitch lags in all subframes          */
  s16 Vad,
  TG729BDECODER *pG729DecH
);

void pit_pst_filt(
  s16 *signal,      /* (i)     : input signal                        */
  s16 *scal_sig,    /* (i)     : input signal (scaled, divided by 4) */
  s16 t0_min,       /* (i)     : minimum value in the searched range */
  s16 t0_max,       /* (i)     : maximum value in the searched range */
  s16 L_subfr,      /* (i)     : size of filtering                   */
  s16 *signal_pst   /* (o)     : harmonically postfiltered signal    */
);

void preemphasis(
  s16 *signal,  /* (i/o)   : input signal overwritten by the output */
  s16 g,        /* (i) Q15 : preemphasis coefficient                */
  s16 L,         /* (i)     : size of filtering                      */
  TG729BDECODER *pG729DecH
);

void agc(
  s16 *sig_in,   /* (i)     : postfilter input signal  */
  s16 *sig_out,  /* (i/o)   : postfilter output signal */
  s16 l_trm,      /* (i)     : subframe size            */
  TG729BDECODER *pG729DecH
);

/*--------------------------------------------------------------------------*
 * Constants and prototypes for taming procedure.                           *
 *--------------------------------------------------------------------------*/

#define GPCLIP      15564      /* Maximum pitch gain if taming is needed Q14*/
#define GPCLIP2     481        /* Maximum pitch gain if taming is needed Q9 */
#define GP0999      16383      /* Maximum pitch gain if taming is needed    */
#define L_THRESH_ERR 983040000L /* Error threshold taming 16384. * 60000.   */

//void   Init_exc_err(void);
void Init_exc_err(TG729BENCODER *pG729EncH);

void   update_exc_err(s16 gain_pit, s16 t0, TG729BDECODER *pG729DecH);
void   enc_update_exc_err(s16 gain_pit, s16 t0, TG729BENCODER *pG729EncH);

s16 test_err(s16 t0, s16 t0_frac, TG729BENCODER *pG729EncH);


void enc_update_exc_err(
 s16 gain_pit,      /* (i) pitch gain */
 s16 T0,             /* (i) integer part of pitch delay */
 TG729BENCODER *pG729EncH
);

/*--------------------------------------------------------------------------*
 * Prototypes for auxiliary functions.                                      *
 *--------------------------------------------------------------------------*/

void Copy(
  s16 x[],      /* (i)   : input vector   */
  s16 y[],      /* (o)   : output vector  */
  s16 L         /* (i)   : vector length  */
);

void Set_zero(
  s16 x[],       /* (o)    : vector to clear     */
  s16 L          /* (i)    : length of vector    */
);

s16 Random(s16 *seed);

void Init_enc_lsfq_noise(TG729BENCODER  *pG729EncH);

#ifdef __cplusplus
}
#endif

#endif //end of LD8A_H



