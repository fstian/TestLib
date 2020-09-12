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
/*
**
** File:            "dec_cng.c"
**
** Description:     Comfort noise generation
**                  performed at the decoder part
**
*/
/**** Fixed point version ***/
#include <stdio.h>
#include <stdlib.h>

#include "basicop.h"
#include "ld8a.h"
#include "tab_ld8a.h"
#include "vad.h"
#include "dtx.h"
#include "sid.h"
#include "tab_dtx.h"



/**
** Function:        Init_Dec_cng()
**
** Description:     Initialize dec_cng static variables
**
**
*/
void Init_Dec_cng(TG729BDECODER *pG729DecH)
{
  
  Copy( freq_prev_reset, pG729DecH->lspSid, M);
  pG729DecH->sid_gain = tab_Sidgain[0];

  return;
}

/*-----------------------------------------------------------*
 * procedure Dec_cng:                                        *
 *           ~~~~~~~~                                        *
 *                     Receives frame type                   *
 *                     0  :  for untransmitted frames        *
 *                     2  :  for SID frames                  *
 *                     Decodes SID frames                    *
 *                     Computes current frame excitation     *
 *                     Computes current frame LSPs
 *-----------------------------------------------------------*/

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
)
{
  s16 temp, ind;
  s16 dif;

  dif = sub(past_ftyp, 1);
  
  /* SID Frame */
  /*************/
  if(parm[0] != 0) {

    pG729DecH->sid_gain = tab_Sidgain[(l32)parm[4]];           
    
    /* Inverse quantization of the LSP */
    sid_lsfq_decode(&parm[1], pG729DecH->lspSid, freq_prev, pG729DecH);
    
  }

  /* non SID Frame */
  /*****************/
  else {
    
    /* Case of 1st SID frame erased : quantize-decode   */
    /* energy estimate stored in sid_gain         */
    if(dif == 0) {
      Qua_Sidgain(&sid_sav, &sh_sid_sav, 0, &temp, &ind);
      pG729DecH->sid_gain = tab_Sidgain[(l32)ind];
    }
    
  }
  
  if(dif == 0) {
    pG729DecH->cur_gain = pG729DecH->sid_gain;
  }
  else {
    pG729DecH->cur_gain = mult_r(pG729DecH->cur_gain, A_GAIN0);
    pG729DecH->cur_gain = add(pG729DecH->cur_gain, mult_r(pG729DecH->sid_gain, A_GAIN1));
  }
 
  Calc_exc_rand(pG729DecH->cur_gain, exc, seed, FLAG_DEC, pG729DecH);

  /* Interpolate the Lsp vectors */
  Int_qlpc(lsp_old, pG729DecH->lspSid, A_t);
  Copy(pG729DecH->lspSid, lsp_old, M);
  
  return;
}



/*---------------------------------------------------------------------------*
 * Function  Init_lsfq_noise                                                 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~                                                 *
 *                                                                           *
 * -> Initialization of variables for the lsf quantization in the SID        *
 *                                                                           *
 *---------------------------------------------------------------------------*/
void Init_dec_lsfq_noise(TG729BDECODER *pG729DecH)
{
  s16 i, j;
  l32 acc0;

  // initialize the noise_fg 
  for (i=0; i<4; i++)
    Copy(fg[0][i], pG729DecH->noise_fg[0][i], M);
  
  for (i=0; i<4; i++)
    for (j=0; j<M; j++){
      acc0 = L_mult(fg[0][i][j], 19660);
      acc0 = L_mac(acc0, fg[1][i][j], 13107);
      pG729DecH->noise_fg[1][i][j] = extract_h(acc0);
    }
}


void sid_lsfq_decode(s16 *index,             /* (i) : quantized indices    */
                     s16 *lspq,              /* (o) : quantized lsp vector */
                     s16 freq_prev[MA_NP][M], /* (i) : memory of predictor  */
					 TG729BDECODER *pG729DecH
                     )
{
  l32 acc0;
  s16 i, j, k, lsfq[M], tmpbuf[M];

  /* get the lsf error vector */
  Copy(lspcb1[PtrTab_1[index[1]]], tmpbuf, M);
  for (i=0; i<M/2; i++)
    tmpbuf[i] = add(tmpbuf[i], lspcb2[PtrTab_2[0][index[2]]][i]);
  for (i=M/2; i<M; i++)
    tmpbuf[i] = add(tmpbuf[i], lspcb2[PtrTab_2[1][index[2]]][i]);

  /* guarantee minimum distance of 0.0012 (~10 in Q13) between tmpbuf[j] 
     and tmpbuf[j+1] */
  for (j=1; j<M; j++){
    acc0 = L_mult(tmpbuf[j-1], 16384);
    acc0 = L_mac(acc0, tmpbuf[j], -16384);
    acc0 = L_mac(acc0, 10, 16384);
    k = extract_h(acc0);

    if (k > 0){
      tmpbuf[j-1] = sub(tmpbuf[j-1], k);
      tmpbuf[j] = add(tmpbuf[j], k);
    }
  }
  
  /* compute the quantized lsf vector */
  Lsp_prev_compose(tmpbuf, lsfq, pG729DecH->noise_fg[index[0]], freq_prev, 
                   noise_fg_sum[index[0]]);
  
  /* update the prediction memory */
  Lsp_prev_update(tmpbuf, freq_prev);
  
  /* lsf stability check */
  Lsp_stability(lsfq);

  /* convert lsf to lsp */
  Lsf_lsp2(lsfq, lspq, M);

}




