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
/* DTX and Comfort Noise Generator - Encoder part */
#include <stdio.h>
#include <stdlib.h>

#include "basicop.h"
#include "ld8a.h"
#include "dtx.h"
#include "tab_dtx.h"
#include "sid.h"

//#include "format_prt.h"
//extern l32 l32PrtFlag;

//Local functions

static void Calc_pastfilt(s16 *Coeff, TG729BENCODER *pG729EncH);

static void Calc_RCoeff(s16 *Coeff, s16 *RCoeff, s16 *sh_RCoeff);
static s16 Cmp_filt(s16 *RCoeff, s16 sh_RCoeff, s16 *acf,
                                        s16 alpha, s16 Fracthresh);
static void Calc_sum_acf(s16 *acf, s16 *sh_acf,
                    s16 *sum, s16 *sh_sum, s16 nb);
static void Update_sumAcf(TG729BENCODER *pG729EncH);


/*-----------------------------------------------------------*
 * procedure Init_Cod_cng:                                   *
 *           ~~~~~~~~~~~~                                    *
 *   Initialize variables used for dtx at the encoder        *
 *-----------------------------------------------------------*/
void Init_Cod_cng(TG729BENCODER *pG729EncH)
{
  s16 i;

  for(i=0; i<SIZ_SUMACF; i++) pG729EncH->sumAcf[i] = 0;
  for(i=0; i<NB_SUMACF; i++) pG729EncH->sh_sumAcf[i] = 40;

  for(i=0; i<SIZ_ACF; i++) pG729EncH->Acf[i] = 0;
  for(i=0; i<NB_CURACF; i++) pG729EncH->sh_Acf[i] = 40;

  for(i=0; i<NB_GAIN; i++) pG729EncH->sh_ener[i] = 40;
  for(i=0; i<NB_GAIN; i++) pG729EncH->ener[i] = 0;

  pG729EncH->cur_gain = 0;
  pG729EncH->fr_cur = 0;
  pG729EncH->flag_chang = 0;

  return;
}


/*-----------------------------------------------------------*
 * procedure Cod_cng:                                        *
 *           ~~~~~~~~                                        *
 *   computes DTX decision                                   *
 *   encodes SID frames                                      *
 *   computes CNG excitation for encoder update              *
 *-----------------------------------------------------------*/
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
)
{
	
	s16 i;
	
	s16 curAcf[MP1];
	s16 bid[M], zero[MP1];
	s16 curCoeff[MP1];
	s16 lsp_new[M];
	s16 *lpcCoeff;
	s16 cur_igain;
	s16 energyq, temp;
	
	/*test code*/
	//s16fprt(lsp_old_q, 10, "as16loq1", &l32PrtFlag, 1);

	/* Update Ener and pG729EncH->sh_ener */
	for(i = NB_GAIN-1; i>=1; i--) {
		pG729EncH->ener[i] = pG729EncH->ener[i-1];
		pG729EncH->sh_ener[i] = pG729EncH->sh_ener[i-1];
	}
	
	/* Compute current Acfs */
	Calc_sum_acf(pG729EncH->Acf, pG729EncH->sh_Acf, curAcf, &pG729EncH->sh_ener[0], NB_CURACF);
	
	/* Compute LPC coefficients and residual energy */
	if(curAcf[0] == 0) {
		pG729EncH->ener[0] = 0;                /* should not happen */
	}
	else {
		Set_zero(zero, MP1);
		Levinson(curAcf, zero, curCoeff, bid, &pG729EncH->ener[0]);
	}
	
	/* if first frame of silence => SID frame */
	if(pastVad != 0) {
		ana[0] = 2;
		pG729EncH->count_fr0 = 0;
		pG729EncH->nb_ener = 1;

	/*test code*/
	//s16fprt(&pG729EncH->prev_energy, 1, "as16prev_energy01", &l32PrtFlag, 1);
	//s16fprt(&energyq, 1, "as16energyq01", &l32PrtFlag, 1);

		Qua_Sidgain(pG729EncH->ener, pG729EncH->sh_ener, pG729EncH->nb_ener, &energyq, &cur_igain);
	/*test code*/
	//s16fprt(&pG729EncH->prev_energy, 1, "as16prev_energy02", &l32PrtFlag, 1);
	//s16fprt(&energyq, 1, "as16energyq02", &l32PrtFlag, 1);
		
	}
	else {
		pG729EncH->nb_ener = add(pG729EncH->nb_ener, 1);
		if(sub(pG729EncH->nb_ener, NB_GAIN) > 0) pG729EncH->nb_ener = NB_GAIN;

	/*test code*/
	//s16fprt(&pG729EncH->prev_energy, 1, "as16prev_energy11", &l32PrtFlag, 1);
	//s16fprt(&energyq, 1, "as16energyq11", &l32PrtFlag, 1);

		Qua_Sidgain(pG729EncH->ener, pG729EncH->sh_ener, pG729EncH->nb_ener, &energyq, &cur_igain);
		
		/* Compute stationarity of current filter   */
		/* versus reference filter                  */
		if(Cmp_filt(pG729EncH->RCoeff, pG729EncH->sh_RCoeff, curAcf, pG729EncH->ener[0], FRAC_THRESH1) != 0) {
			pG729EncH->flag_chang = 1;
		}
		
		/* compare energy difference between current frame and last frame */
	/*test code*/
	//s16fprt(&pG729EncH->prev_energy, 1, "as16prev_energy12", &l32PrtFlag, 1);
	//s16fprt(&energyq, 1, "as16energyq12", &l32PrtFlag, 1);

		temp = abs_s(sub(pG729EncH->prev_energy, energyq));
		temp = sub(temp, 2);
		if (temp > 0) pG729EncH->flag_chang = 1;
		
		pG729EncH->count_fr0 = add(pG729EncH->count_fr0, 1);
		if(sub(pG729EncH->count_fr0, FR_SID_MIN) < 0) {
			ana[0] = 0;               /* no transmission */
		}
		else {
			if(pG729EncH->flag_chang != 0) {
				ana[0] = 2;             /* transmit SID frame */
			}
			else{
				ana[0] = 0;
			}
			
			pG729EncH->count_fr0 = FR_SID_MIN;   /* to avoid overflow */
		}
	}
	
	
	
	if(sub(ana[0], 2) == 0) {
		
		/* Reset frame count and change flag */
		pG729EncH->count_fr0 = 0;
		pG729EncH->flag_chang = 0;
		
		/* Compute past average filter */
		Calc_pastfilt(pG729EncH->pastCoeff, pG729EncH);
		Calc_RCoeff(pG729EncH->pastCoeff, pG729EncH->RCoeff, &pG729EncH->sh_RCoeff);
		
		/* Compute stationarity of current filter   */
		/* versus past average filter               */
		
		
		/* if stationary */
		/* transmit average filter => new ref. filter */
		if(Cmp_filt(pG729EncH->RCoeff, pG729EncH->sh_RCoeff, curAcf, pG729EncH->ener[0], FRAC_THRESH2) == 0) {
			lpcCoeff = pG729EncH->pastCoeff;
		}
		
		/* else */
		/* transmit current filter => new ref. filter */
		else {
			lpcCoeff = curCoeff;
			Calc_RCoeff(curCoeff, pG729EncH->RCoeff, &pG729EncH->sh_RCoeff);
		}
		/* Compute SID frame codes */
		
	/*test code*/
	//s16fprt(lsp_old_q, 10, "as16loq2", &l32PrtFlag, 1);

		Az_lsp(lpcCoeff, lsp_new, lsp_old_q); /* From A(z) to lsp */
		
											  /*test code
											  {
											  l32 i =1;
											  
												//s16fprt(lsp_new, 10, "as16LspNew", &i, 0);
	}*/
	/*test code
	{
	l32 i =1;
	
	  //s16fprt(pG729EncH->noise_fg, 20, "as16Nfg", &i, 0);
	}*/

	/*test code*/
	//s16fprt(lsp_old_q, 10, "as16loq3", &l32PrtFlag, 1);
		/* LSP quantization */
		lsfq_noise(lsp_new, pG729EncH->lspSid_q, freq_prev, &ana[1], pG729EncH);
		
	/*test code*/
	//s16fprt(lsp_old_q, 10, "as16loq4", &l32PrtFlag, 1);
		pG729EncH->prev_energy = energyq;
		ana[4] = cur_igain;
		pG729EncH->sid_gain = tab_Sidgain[cur_igain];
		
		
	} /* end of SID frame case */
	
	/* Compute new excitation */
	if(pastVad != 0) {
		pG729EncH->cur_gain = pG729EncH->sid_gain;
	}
	else {
		pG729EncH->cur_gain = mult_r(pG729EncH->cur_gain, A_GAIN0);
		pG729EncH->cur_gain = add(pG729EncH->cur_gain, mult_r(pG729EncH->sid_gain, A_GAIN1));
	}
	
	/*test code*/
	//s16fprt(lsp_old_q, 10, "as16loq5", &l32PrtFlag, 1);

	//Calc_exc_rand(pG729EncH->cur_gain, exc, seed, FLAG_COD);
	Enc_Calc_exc_rand(pG729EncH->cur_gain, exc, seed, FLAG_COD, pG729EncH);
	
	Int_qlpc(lsp_old_q, pG729EncH->lspSid_q, Aq);

	/*test code*/
	//s16fprt(lsp_old_q, 10, "as16loq6", &l32PrtFlag, 1);
	
	for(i=0; i<M; i++) {
		lsp_old_q[i]   = pG729EncH->lspSid_q[i];
	}

	/*test code*/
	//s16fprt(lsp_old_q, 10, "as16loq7", &l32PrtFlag, 1);
	
	/* Update pG729EncH->sumAcf if pG729EncH->fr_cur = 0 */
	if(pG729EncH->fr_cur == 0) {
		Update_sumAcf(pG729EncH);
	}
	
	return;
}

/*-----------------------------------------------------------*
 * procedure Update_cng:                                     *
 *           ~~~~~~~~~~                                      *
 *   Updates autocorrelation arrays                          *
 *   used for DTX/CNG                                        *
 *   If Vad=1 : updating of array pG729EncH->sumAcf                     *
 *-----------------------------------------------------------*/
void Update_cng(
  s16 *r_h,      /* (i) :   MSB of frame autocorrelation        */
  s16 exp_r,     /* (i) :   scaling factor associated           */
  s16 Vad,        /* (i) :   current Vad decision                */
  TG729BENCODER *pG729EncH
)
{
  s16 i;
  s16 *ptr1, *ptr2;

  /* Update pG729EncH->Acf and shAcf */
  ptr1 = pG729EncH->Acf + SIZ_ACF - 1;
  ptr2 = ptr1 - MP1;
  for(i=0; i<(SIZ_ACF-MP1); i++) {
    *ptr1-- = *ptr2--;
  }
  for(i=NB_CURACF-1; i>=1; i--) {
    pG729EncH->sh_Acf[i] = pG729EncH->sh_Acf[i-1];
  }

  /* Save current pG729EncH->Acf */
  pG729EncH->sh_Acf[0] = negate(add(16, exp_r));
  for(i=0; i<MP1; i++) {
    pG729EncH->Acf[i] = r_h[i];
  }

  pG729EncH->fr_cur = add(pG729EncH->fr_cur, 1);
  if(sub(pG729EncH->fr_cur, NB_CURACF) == 0) {
    pG729EncH->fr_cur = 0;
    if(Vad != 0) {
      Update_sumAcf(pG729EncH);
    }
  }

  return;
}


/*-----------------------------------------------------------*
 *         Local procedures                                  *
 *         ~~~~~~~~~~~~~~~~                                  *
 *-----------------------------------------------------------*/

/* Compute scaled autocorr of LPC coefficients used for Itakura distance */
/*************************************************************************/
static void Calc_RCoeff(s16 *Coeff, s16 *RCoeff, s16 *sh_RCoeff)
{
  s16 i, j;
  s16 sh1;
  l32 L_acc;
  
  /* pG729EncH->RCoeff[0] = SUM(j=0->M) Coeff[j] ** 2 */
  L_acc = 0L;
  for(j=0; j <= M; j++) {
    L_acc = L_mac(L_acc, Coeff[j], Coeff[j]);
  }
  
  /* Compute exponent pG729EncH->RCoeff */
  sh1 = norm_l(L_acc);
  L_acc = L_shl(L_acc, sh1);
  RCoeff[0] = round_linux(L_acc);
  
  /* pG729EncH->RCoeff[i] = SUM(j=0->M-i) Coeff[j] * Coeff[j+i] */
  for(i=1; i<=M; i++) {
    L_acc = 0L;
    for(j=0; j<=M-i; j++) {
      L_acc = L_mac(L_acc, Coeff[j], Coeff[j+i]);
    }
    L_acc = L_shl(L_acc, sh1);
    RCoeff[i] = round_linux(L_acc);
  }
  *sh_RCoeff = sh1;
  return;
}

/* Compute Itakura distance and compare to threshold */
/*****************************************************/
static s16 Cmp_filt(s16 *RCoeff, s16 sh_RCoeff, s16 *acf,
                                        s16 alpha, s16 FracThresh)
{
  l32 L_temp0, L_temp1;
  s16 temp1, temp2, sh[2], ind;
  s16 i;
  s16 diff, flag;
  extern l32 Overflow;

  sh[0] = 0;
  sh[1] = 0;
  ind = 1;
  flag = 0;
  do {
    Overflow = 0;
    temp1 = shr(RCoeff[0], sh[0]);
    temp2 = shr(acf[0], sh[1]);
    L_temp0 = L_shr(L_mult(temp1, temp2),1);
    for(i=1; i <= M; i++) {
      temp1 = shr(RCoeff[i], sh[0]);
      temp2 = shr(acf[i], sh[1]);
      L_temp0 = L_mac(L_temp0, temp1, temp2);
    }
    if(Overflow != 0) {
      sh[(l32)ind] = add(sh[(l32)ind], 1);
      ind = sub(1, ind);
    }
    else flag = 1;
  } while (flag == 0);
  
  
  temp1 = mult_r(alpha, FracThresh);
  L_temp1 = L_add(L_deposit_l(temp1), L_deposit_l(alpha));
  temp1 = add(sh_RCoeff, 9);  /* 9 = Lpc_justif. * 2 - 16 + 1 */
  temp2 = add(sh[0], sh[1]);
  temp1 = sub(temp1, temp2);
  L_temp1 = L_shl(L_temp1, temp1);
  
  L_temp0 = L_sub(L_temp0, L_temp1);
  if(L_temp0 > 0L) diff = 1;
  else diff = 0;

  return(diff);
}

/* Compute past average filter */
/*******************************/
static void Calc_pastfilt(s16 *Coeff,TG729BENCODER *pG729EncH)
{
  s16 i;
  s16 s_sumAcf[MP1];
  s16 bid[M], zero[MP1];
  s16 temp;
  
  Calc_sum_acf(pG729EncH->sumAcf, pG729EncH->sh_sumAcf, s_sumAcf, &temp, NB_SUMACF);
  
  if(s_sumAcf[0] == 0L) {
    Coeff[0] = 4096;
    for(i=1; i<=M; i++) Coeff[i] = 0;
    return;
  }

  Set_zero(zero, MP1);
  Levinson(s_sumAcf, zero, Coeff, bid, &temp);
  return;
}

/* Update pG729EncH->sumAcf */
/*****************/
static void Update_sumAcf(TG729BENCODER *pG729EncH)
{
  s16 *ptr1, *ptr2;
  s16 i;

  /*** Move pG729EncH->sumAcf ***/
  ptr1 = pG729EncH->sumAcf + SIZ_SUMACF - 1;
  ptr2 = ptr1 - MP1;
  for(i=0; i<(SIZ_SUMACF-MP1); i++) {
    *ptr1-- = *ptr2--;
  }
  for(i=NB_SUMACF-1; i>=1; i--) {
    pG729EncH->sh_sumAcf[i] = pG729EncH->sh_sumAcf[i-1];
  }

  /* Compute new pG729EncH->sumAcf */
  Calc_sum_acf(pG729EncH->Acf, pG729EncH->sh_Acf, pG729EncH->sumAcf, pG729EncH->sh_sumAcf, NB_CURACF);
  return;
}

/* Compute sum of acfs (curAcf, pG729EncH->sumAcf or s_sumAcf) */
/****************************************************/
static void Calc_sum_acf(s16 *acf, s16 *sh_acf,
                         s16 *sum, s16 *sh_sum, s16 nb)
{

  s16 *ptr1;
  l32 L_temp, L_tab[MP1];
  s16 sh0, temp;
  s16 i, j;
  
  /* Compute sum = sum of nb acfs */
  /* Find sh_acf minimum */
  sh0 = sh_acf[0];
  for(i=1; i<nb; i++) {
    if(sub(sh_acf[i], sh0) < 0) sh0 = sh_acf[i];
  }
  sh0 = add(sh0, 14);           /* 2 bits of margin */

  for(j=0; j<MP1; j++) {
    L_tab[j] = 0L;
  }
  ptr1 = acf;
  for(i=0; i<nb; i++) {
    temp = sub(sh0, sh_acf[i]);
    for(j=0; j<MP1; j++) {
      L_temp = L_deposit_l(*ptr1++);
      L_temp = L_shl(L_temp, temp); /* shift right if temp<0 */
      L_tab[j] = L_add(L_tab[j], L_temp);
    }
  } 
  temp = norm_l(L_tab[0]);
  for(i=0; i<=M; i++) {
    sum[i] = extract_h(L_shl(L_tab[i], temp));
  }
  temp = sub(temp, 16);
  *sh_sum = add(sh0, temp);
  return;
}

