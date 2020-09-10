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
#include <stdio.h>

#include "basicop.h"
#include "ld8a.h"
#include "tab_ld8a.h"
#include "dtx.h"
#include "tab_dtx.h"

/* local function */
static s16 MakeDec(
               s16 dSLE,    /* (i)  : differential low band energy */
               s16 dSE,     /* (i)  : differential full band energy */
               s16 SD,      /* (i)  : differential spectral distortion */
               s16 dSZC     /* (i)  : differential zero crossing rate */
);
#if 0
/* static variables */
static s16 pG729EncH->MeanLSF[M];
static s16 pG729EncH->Min_buffer[16];
static s16 pG729EncH->Prev_Min, pG729EncH->Next_Min, pG729EncH->Min;
static s16 pG729EncH->MeanE, pG729EncH->MeanSE, pG729EncH->MeanSLE, pG729EncH->MeanSZC;
static s16 pG729EncH->vad_prev_energy;
static s16 pG729EncH->count_sil, pG729EncH->count_update, pG729EncH->count_ext;
static s16 pG729EncH->flag, pG729EncH->v_flag, pG729EncH->less_count;

#endif

/*---------------------------------------------------------------------------*
 * Function  vad_init                                                        *
 * ~~~~~~~~~~~~~~~~~~                                                        *
 *                                                                           *
 * -> Initialization of variables for voice activity detection               *
 *                                                                           *
 *---------------------------------------------------------------------------*/
void vad_init(TG729BENCODER *pG729EncH)
{
	/* Static vectors to zero */
	Set_zero(pG729EncH->MeanLSF, M);
	
	/* Initialize VAD parameters */
	pG729EncH->MeanSE = 0;
	pG729EncH->MeanSLE = 0;
	pG729EncH->MeanE = 0;
	pG729EncH->MeanSZC = 0;
	pG729EncH->count_sil = 0;
	pG729EncH->count_update = 0;
	pG729EncH->count_ext = 0;
	pG729EncH->less_count = 0;
	pG729EncH->flag = 1;
	pG729EncH->Min = MAX_16;
}

#if 1
/*-----------------------------------------------------------------*
 * Functions vad                                                   *
 *           ~~~                                                   *
 * Input:                                                          *
 *   rc            : reflection coefficient                        *
 *   lsf[]         : unquantized lsf vector                        *
 *   r_h[]         : upper 16-bits of the autocorrelation vector   *
 *   r_l[]         : lower 16-bits of the autocorrelation vector   *
 *   exp_R0        : exponent of the autocorrelation vector        *
 *   sigpp[]       : preprocessed input signal                     *
 *   frm_count     : frame counter                                 *
 *   prev_marker   : VAD decision of the last frame                *
 *   pprev_marker  : VAD decision of the frame before last frame   *
 *                                                                 *
 * Output:                                                         *
 *                                                                 *
 *   marker        : VAD decision of the current frame             * 
 *                                                                 *
 *-----------------------------------------------------------------*/
void vad(
         s16 rc,
         s16 *lsf, 
         s16 *r_h,
         s16 *r_l, 
         s16 exp_R0,
         s16 *sigpp,
         s16 frm_count,
         s16 prev_marker,
         s16 pprev_marker,
         s16 *marker,
		 TG729BENCODER *pG729EncH)
{
 /* scalar */
  l32 acc0;
  s16 i, j, exp, frac;
  s16 ENERGY, ENERGY_low, SD, ZC, dSE, dSLE, dSZC;
  s16 COEF, C_COEF, COEFZC, C_COEFZC, COEFSD, C_COEFSD;

  /* compute the frame energy */
  acc0 = L_Comp(r_h[0], r_l[0]);
  Log2(acc0, &exp, &frac);
  acc0 = Mpy_32_16(exp, frac, 9864);
  i = sub(exp_R0, 1);  
  i = sub(i, 1);
  acc0 = L_mac(acc0, 9864, i);
  acc0 = L_shl(acc0, 11);
  ENERGY = extract_h(acc0);
  ENERGY = sub(ENERGY, 4875);

  /* compute the low band energy */
  acc0 = 0;
  for (i=1; i<=NP; i++)
    acc0 = L_mac(acc0, r_h[i], lbf_corr[i]);
  acc0 = L_shl(acc0, 1);
  acc0 = L_mac(acc0, r_h[0], lbf_corr[0]);
  Log2(acc0, &exp, &frac);
  acc0 = Mpy_32_16(exp, frac, 9864);
  i = sub(exp_R0, 1);  
  i = sub(i, 1);
  acc0 = L_mac(acc0, 9864, i);
  acc0 = L_shl(acc0, 11);
  ENERGY_low = extract_h(acc0);
  ENERGY_low = sub(ENERGY_low, 4875);
  
  /* compute SD */
  acc0 = 0;
  for (i=0; i<M; i++){
    j = sub(lsf[i], pG729EncH->MeanLSF[i]);
    acc0 = L_mac(acc0, j, j);
  }
  SD = extract_h(acc0);      /* Q15 */
  
  /* compute # zero crossing */
  ZC = 0;
  for (i=ZC_START+1; i<=ZC_END; i++)
    if (mult(sigpp[i-1], sigpp[i]) < 0)
      ZC = add(ZC, 410);     /* Q15 */

  /* Initialize and update Mins */
  if(sub(frm_count, 129) < 0){
    if (sub(ENERGY, pG729EncH->Min) < 0){
      pG729EncH->Min = ENERGY;
      pG729EncH->Prev_Min = ENERGY;
    }
    
    if((frm_count & 0x0007) == 0){
      i = sub(shr(frm_count,3),1);
      pG729EncH->Min_buffer[i] = pG729EncH->Min;
      pG729EncH->Min = MAX_16;
    }
  }

  if((frm_count & 0x0007) == 0){
    pG729EncH->Prev_Min = pG729EncH->Min_buffer[0];
    for (i=1; i<16; i++){
      if (sub(pG729EncH->Min_buffer[i], pG729EncH->Prev_Min) < 0)
        pG729EncH->Prev_Min = pG729EncH->Min_buffer[i];
    }
  }
  
  if(sub(frm_count, 129) >= 0){
    if(((frm_count & 0x0007) ^ (0x0001)) == 0){
      pG729EncH->Min = pG729EncH->Prev_Min;
      pG729EncH->Next_Min = MAX_16;
    }
    if (sub(ENERGY, pG729EncH->Min) < 0)
      pG729EncH->Min = ENERGY;
    if (sub(ENERGY, pG729EncH->Next_Min) < 0)
      pG729EncH->Next_Min = ENERGY;
    
    if((frm_count & 0x0007) == 0){
      for (i=0; i<15; i++)
        pG729EncH->Min_buffer[i] = pG729EncH->Min_buffer[i+1]; 
      pG729EncH->Min_buffer[15] = pG729EncH->Next_Min; 
      pG729EncH->Prev_Min = pG729EncH->Min_buffer[0];
      for (i=1; i<16; i++) 
        if (sub(pG729EncH->Min_buffer[i], pG729EncH->Prev_Min) < 0)
          pG729EncH->Prev_Min = pG729EncH->Min_buffer[i];
    }
    
  }

  if (sub(frm_count, INIT_FRAME) <= 0)
    if(sub(ENERGY, 3072) < 0){
      *marker = NOISE;
      pG729EncH->less_count++;
    }
    else{
      *marker = VOICE;
      acc0 = L_deposit_h(pG729EncH->MeanE);
      acc0 = L_mac(acc0, ENERGY, 1024);
      pG729EncH->MeanE = extract_h(acc0);
      acc0 = L_deposit_h(pG729EncH->MeanSZC);
      acc0 = L_mac(acc0, ZC, 1024);
      pG729EncH->MeanSZC = extract_h(acc0);
      for (i=0; i<M; i++){
        acc0 = L_deposit_h(pG729EncH->MeanLSF[i]);
        acc0 = L_mac(acc0, lsf[i], 1024);
        pG729EncH->MeanLSF[i] = extract_h(acc0);
      }
    }
  
  if (sub(frm_count, INIT_FRAME) >= 0){
    if (sub(frm_count, INIT_FRAME) == 0){
      acc0 = L_mult(pG729EncH->MeanE, factor_fx[pG729EncH->less_count]);
      acc0 = L_shl(acc0, shift_fx[pG729EncH->less_count]);
      pG729EncH->MeanE = extract_h(acc0);

      acc0 = L_mult(pG729EncH->MeanSZC, factor_fx[pG729EncH->less_count]);
      acc0 = L_shl(acc0, shift_fx[pG729EncH->less_count]);
      pG729EncH->MeanSZC = extract_h(acc0);

      for (i=0; i<M; i++){
        acc0 = L_mult(pG729EncH->MeanLSF[i], factor_fx[pG729EncH->less_count]);
        acc0 = L_shl(acc0, shift_fx[pG729EncH->less_count]);
        pG729EncH->MeanLSF[i] = extract_h(acc0);
      }

      pG729EncH->MeanSE = sub(pG729EncH->MeanE, 2048);   /* Q11 */
      pG729EncH->MeanSLE = sub(pG729EncH->MeanE, 2458);  /* Q11 */
    }

    dSE = sub(pG729EncH->MeanSE, ENERGY);
    dSLE = sub(pG729EncH->MeanSLE, ENERGY_low);
    dSZC = sub(pG729EncH->MeanSZC, ZC);

    if(sub(ENERGY, 3072) < 0)
      *marker = NOISE;
    else 
      *marker = MakeDec(dSLE, dSE, SD, dSZC);

    pG729EncH->v_flag = 0;
    if((prev_marker==VOICE) && (*marker==NOISE) && (add(dSE,410) < 0) 
       && (sub(ENERGY, 3072)>0)){
      *marker = VOICE;
      pG729EncH->v_flag = 1;
    }

    if(pG729EncH->flag == 1){
      if((pprev_marker == VOICE) && 
         (prev_marker == VOICE) && 
         (*marker == NOISE) && 
         (sub(abs_s(sub(pG729EncH->vad_prev_energy,ENERGY)), 614) <= 0)){
        pG729EncH->count_ext++;
        *marker = VOICE;
        pG729EncH->v_flag = 1;
        if(sub(pG729EncH->count_ext, 4) <= 0)
          pG729EncH->flag=1;
        else{
          pG729EncH->count_ext=0;
          pG729EncH->flag=0;
        }
      }
    }
    else
      pG729EncH->flag=1;
    
    if(*marker == NOISE)
      pG729EncH->count_sil++;

    if((*marker == VOICE) && (sub(pG729EncH->count_sil, 10) > 0) && 
       (sub(sub(ENERGY,pG729EncH->vad_prev_energy), 614) <= 0)){
      *marker = NOISE;
      pG729EncH->count_sil=0;
    }

    if(*marker == VOICE)
      pG729EncH->count_sil=0;

    if ((sub(sub(ENERGY, 614), pG729EncH->MeanSE) < 0) && (sub(frm_count, 128) > 0)
        && (!pG729EncH->v_flag) && (sub(rc, 19661) < 0))
      *marker = NOISE;

    if ((sub(sub(ENERGY,614),pG729EncH->MeanSE) < 0) && (sub(rc, 24576) < 0)
        && (sub(SD, 83) < 0)){ 
      pG729EncH->count_update++;
      if (sub(pG729EncH->count_update, INIT_COUNT) < 0){
        COEF = 24576;
        C_COEF = 8192;
        COEFZC = 26214;
        C_COEFZC = 6554;
        COEFSD = 19661;
        C_COEFSD = 13017;
      } 
      else
        if (sub(pG729EncH->count_update, INIT_COUNT+10) < 0){
          COEF = 31130;
          C_COEF = 1638;
          COEFZC = 30147;
          C_COEFZC = 2621;
          COEFSD = 21299;
          C_COEFSD = 11469;
        }
        else
          if (sub(pG729EncH->count_update, INIT_COUNT+20) < 0){
            COEF = 31785;
            C_COEF = 983;
            COEFZC = 30802;
            C_COEFZC = 1966;
            COEFSD = 22938;
            C_COEFSD = 9830;
          }
          else
            if (sub(pG729EncH->count_update, INIT_COUNT+30) < 0){
              COEF = 32440;
              C_COEF = 328;
              COEFZC = 31457;
              C_COEFZC = 1311;
              COEFSD = 24576;
              C_COEFSD = 8192;
            }
            else
              if (sub(pG729EncH->count_update, INIT_COUNT+40) < 0){
                COEF = 32604;
                C_COEF = 164;
                COEFZC = 32440;
                C_COEFZC = 328;
                COEFSD = 24576;
                C_COEFSD = 8192;
              }
              else{
                COEF = 32604;
                C_COEF = 164;
                COEFZC = 32702;
                C_COEFZC = 66;
                COEFSD = 24576;
                C_COEFSD = 8192;
              }
      

      /* compute pG729EncH->MeanSE */
      acc0 = L_mult(COEF, pG729EncH->MeanSE);
      acc0 = L_mac(acc0, C_COEF, ENERGY);
      pG729EncH->MeanSE = extract_h(acc0);

      /* compute pG729EncH->MeanSLE */
      acc0 = L_mult(COEF, pG729EncH->MeanSLE);
      acc0 = L_mac(acc0, C_COEF, ENERGY_low);
      pG729EncH->MeanSLE = extract_h(acc0);

      /* compute pG729EncH->MeanSZC */
      acc0 = L_mult(COEFZC, pG729EncH->MeanSZC);
      acc0 = L_mac(acc0, C_COEFZC, ZC);
      pG729EncH->MeanSZC = extract_h(acc0);
      
      /* compute pG729EncH->MeanLSF */
      for (i=0; i<M; i++){
        acc0 = L_mult(COEFSD, pG729EncH->MeanLSF[i]);
        acc0 = L_mac(acc0, C_COEFSD, lsf[i]);
        pG729EncH->MeanLSF[i] = extract_h(acc0);
      }
    }

    if((sub(frm_count, 128) > 0) && (((sub(pG729EncH->MeanSE,pG729EncH->Min) < 0) &&
                        (sub(SD, 83) < 0)) || (sub(pG729EncH->MeanSE,pG729EncH->Min) > 2048))){
      pG729EncH->MeanSE = pG729EncH->Min;
      pG729EncH->count_update = 0;
    }
  }

  pG729EncH->vad_prev_energy = ENERGY;

}


/* local function */  
static s16 MakeDec(
               s16 dSLE,    /* (i)  : differential low band energy */
               s16 dSE,     /* (i)  : differential full band energy */
               s16 SD,      /* (i)  : differential spectral distortion */
               s16 dSZC     /* (i)  : differential zero crossing rate */
               )
{
  l32 acc0;
  
  /* SD vs dSZC */
  acc0 = L_mult(dSZC, -14680);          /* Q15*Q23*2 = Q39 */  
  acc0 = L_mac(acc0, 8192, -28521);     /* Q15*Q23*2 = Q39 */
  acc0 = L_shr(acc0, 8);                /* Q39 -> Q31 */
  acc0 = L_add(acc0, L_deposit_h(SD));
  if (acc0 > 0) return(VOICE);

  acc0 = L_mult(dSZC, 19065);           /* Q15*Q22*2 = Q38 */
  acc0 = L_mac(acc0, 8192, -19446);     /* Q15*Q22*2 = Q38 */
  acc0 = L_shr(acc0, 7);                /* Q38 -> Q31 */
  acc0 = L_add(acc0, L_deposit_h(SD));
  if (acc0 > 0) return(VOICE);

  /* dSE vs dSZC */
  acc0 = L_mult(dSZC, 20480);           /* Q15*Q13*2 = Q29 */
  acc0 = L_mac(acc0, 8192, 16384);      /* Q13*Q15*2 = Q29 */
  acc0 = L_shr(acc0, 2);                /* Q29 -> Q27 */
  acc0 = L_add(acc0, L_deposit_h(dSE));
  if (acc0 < 0) return(VOICE);

  acc0 = L_mult(dSZC, -16384);          /* Q15*Q13*2 = Q29 */
  acc0 = L_mac(acc0, 8192, 19660);      /* Q13*Q15*2 = Q29 */
  acc0 = L_shr(acc0, 2);                /* Q29 -> Q27 */
  acc0 = L_add(acc0, L_deposit_h(dSE));
  if (acc0 < 0) return(VOICE);
 
  acc0 = L_mult(dSE, 32767);            /* Q11*Q15*2 = Q27 */
  acc0 = L_mac(acc0, 1024, 30802);      /* Q10*Q16*2 = Q27 */
  if (acc0 < 0) return(VOICE);
  
  /* dSE vs SD */
  acc0 = L_mult(SD, -28160);            /* Q15*Q5*2 = Q22 */
  acc0 = L_mac(acc0, 64, 19988);        /* Q6*Q14*2 = Q22 */
  acc0 = L_mac(acc0, dSE, 512);         /* Q11*Q9*2 = Q22 */
  if (acc0 < 0) return(VOICE);

  acc0 = L_mult(SD, 32767);             /* Q15*Q15*2 = Q31 */
  acc0 = L_mac(acc0, 32, -30199);       /* Q5*Q25*2 = Q31 */
  if (acc0 > 0) return(VOICE);

  /* dSLE vs dSZC */
  acc0 = L_mult(dSZC, -20480);          /* Q15*Q13*2 = Q29 */
  acc0 = L_mac(acc0, 8192, 22938);      /* Q13*Q15*2 = Q29 */
  acc0 = L_shr(acc0, 2);                /* Q29 -> Q27 */
  acc0 = L_add(acc0, L_deposit_h(dSE));
  if (acc0 < 0) return(VOICE);

  acc0 = L_mult(dSZC, 23831);           /* Q15*Q13*2 = Q29 */
  acc0 = L_mac(acc0, 4096, 31576);      /* Q12*Q16*2 = Q29 */
  acc0 = L_shr(acc0, 2);                /* Q29 -> Q27 */
  acc0 = L_add(acc0, L_deposit_h(dSE));
  if (acc0 < 0) return(VOICE);
  
  acc0 = L_mult(dSE, 32767);            /* Q11*Q15*2 = Q27 */
  acc0 = L_mac(acc0, 2048, 17367);      /* Q11*Q15*2 = Q27 */
  if (acc0 < 0) return(VOICE);
  
  /* dSLE vs SD */
  acc0 = L_mult(SD, -22400);            /* Q15*Q4*2 = Q20 */
  acc0 = L_mac(acc0, 32, 25395);        /* Q5*Q14*2 = Q20 */
  acc0 = L_mac(acc0, dSLE, 256);        /* Q11*Q8*2 = Q20 */
  if (acc0 < 0) return(VOICE);
  
  /* dSLE vs dSE */
  acc0 = L_mult(dSE, -30427);           /* Q11*Q15*2 = Q27 */
  acc0 = L_mac(acc0, 256, -29959);      /* Q8*Q18*2 = Q27 */
  acc0 = L_add(acc0, L_deposit_h(dSLE));
  if (acc0 > 0) return(VOICE);

  acc0 = L_mult(dSE, -23406);           /* Q11*Q15*2 = Q27 */
  acc0 = L_mac(acc0, 512, 28087);       /* Q19*Q17*2 = Q27 */
  acc0 = L_add(acc0, L_deposit_h(dSLE));
  if (acc0 < 0) return(VOICE);

  acc0 = L_mult(dSE, 24576);            /* Q11*Q14*2 = Q26 */
  acc0 = L_mac(acc0, 1024, 29491);      /* Q10*Q15*2 = Q26 */
  acc0 = L_mac(acc0, dSLE, 16384);      /* Q11*Q14*2 = Q26 */
  if (acc0 < 0) return(VOICE);

  return (NOISE);
}

#endif 



