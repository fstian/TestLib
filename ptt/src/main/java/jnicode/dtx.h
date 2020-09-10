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

#ifndef DTX_H
#define DTX_H

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*
 * Prototypes for DTX/CNG                                                   *
 *--------------------------------------------------------------------------*/

/* Encoder DTX/CNG functions */
void Init_Cod_cng(TG729BENCODER *pG729EncH);
void Cod_cng(
  s16 *exc,          /* (i/o) : excitation array                     */
  s16 pastVad,       /* (i)   : previous VAD decision                */
  s16 *lsp_old_q,    /* (i/o) : previous quantized lsp               */
  s16 *Aq,           /* (o)   : set of interpolated LPC coefficients */
  s16 *ana,          /* (o)   : coded SID parameters                 */
  s16 freq_prev[MA_NP][M],
                        /* (i/o) : previous LPS for quantization        */
  s16 *seed,          /* (i/o) : random generator seed                */
  TG729BENCODER *pG729EncH
);

void Update_cng(
  s16 *r_h,      /* (i) :   MSB of frame autocorrelation        */
  s16 exp_r,     /* (i) :   scaling factor associated           */
  s16 Vad,        /* (i) :   current Vad decision                */
  TG729BENCODER *pG729EncH
);

/* SID gain Quantization */
void Qua_Sidgain(
  s16 *ener,     /* (i)   array of energies                   */
  s16 *sh_ener,  /* (i)   corresponding scaling factors       */
  s16 nb_ener,   /* (i)   number of energies or               */
  s16 *enerq,    /* (o)   decoded energies in dB              */
  s16 *idx       /* (o)   SID gain quantization index         */
);

/* CNG excitation generation */
void Calc_exc_rand(
  s16 cur_gain,      /* (i)   :   target sample gain                 */
  s16 *exc,          /* (i/o) :   excitation array                   */
  s16 *seed,         /* (i)   :   current Vad decision               */
  s16 flag_cod,         /* (i)   :   encoder/decoder flag               */
  TG729BDECODER *pG729DecH
);

void Enc_Calc_exc_rand(
					   s16 cur_gain,
					   s16 *exc,
					   s16 *seed,
					   l32 flag_cod,
					   TG729BENCODER *pG729EncH
					   );

/* SID LSP Quantization */
void Get_freq_prev(s16 x[MA_NP][M],TG729BENCODER *pG729EncH);
void Update_freq_prev(s16 x[MA_NP][M], TG729BENCODER *pG729EncH);
void Get_decfreq_prev(s16 x[MA_NP][M], TG729BDECODER *pG729DecH);
void Update_decfreq_prev(s16 x[MA_NP][M], TG729BDECODER *pG729DecH);

/* Decoder CNG generation */
void Init_Dec_cng(TG729BDECODER *pG729DecH);
void Dec_cng(
  s16 past_ftyp,     /* (i)   : past frame type                      */
  s16 sid_sav,       /* (i)   : energy to recover SID gain           */
  s16 sh_sid_sav,    /* (i)   : corresponding scaling factor         */
  s16 *parm,         /* (i)   : coded SID parameters                 */
  s16 *exc,          /* (i/o) : excitation array                     */
  s16 *lsp_old,      /* (i/o) : previous lsp                         */
  s16 *A_t,          /* (o)   : set of interpolated LPC coefficients */
  s16 *seed,         /* (i/o) : random generator seed                */
  s16 freq_prev[MA_NP][M],
                        /* (i/o) : previous LPS for quantization        */
  TG729BDECODER *pG729DecH
);

s16 read_frame(FILE *f_serial, s16 *parm);


#ifdef __cplusplus
}
#endif

#endif   //end of  DTX_H




