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
/* Quantize SID gain                                      */
#include <stdio.h>
#include <stdlib.h>

#include "basicop.h"
#include "ld8a.h"
#include "vad.h"
#include "dtx.h"
#include "sid.h"
#include "tab_dtx.h"

/* Local function */
static s16 Quant_Energy(
  l32 L_x,    /* (i)  : Energy                 */
  s16 sh,     /* (i)  : Exponent of the energy */
  s16 *enerq  /* (o)  : quantized energy in dB */
);

/*-------------------------------------------------------------------*
 * Function  Qua_Sidgain                                             *
 *           ~~~~~~~~~~~                                             *
 *-------------------------------------------------------------------*/
void Qua_Sidgain(
  s16 *ener,     /* (i)   array of energies                   */
  s16 *sh_ener,  /* (i)   corresponding scaling factors       */
  s16 nb_ener,   /* (i)   number of energies or               */
  s16 *enerq,    /* (o)   decoded energies in dB              */
  s16 *idx       /* (o)   SID gain quantization index         */
)
{
  s16 i;
  l32 L_x;
  s16 sh1, temp;
  s16 hi, lo;
  l32 L_acc;
  
  if(nb_ener == 0) {
    /* Quantize energy saved for frame erasure case                */
    /* L_x = average_ener                                          */
    L_acc = L_deposit_l(*ener);
    L_acc = L_shl(L_acc, *sh_ener); /* >> if *sh_ener < 0 */
    L_Extract(L_acc, &hi, &lo);
    L_x = Mpy_32_16(hi, lo, fact[0]);
    sh1 = 0;
  }
  else {
    
    /*
     * Compute weighted average of energies
     * ener[i] = enerR[i] x 2**sh_ener[i]
     * L_x = k[nb_ener] x SUM(i=0->nb_ener-1) enerR[i]
     * with k[nb_ener] =  fact_ener / nb_ener x L_FRAME x nbAcf
     */
    sh1 = sh_ener[0];
    for(i=1; i<nb_ener; i++) {
      if(sh_ener[i] < sh1) sh1 = sh_ener[i];
    }
    sh1 = add(sh1, (s16)(16-marg[nb_ener]));
    L_x = 0L;
    for(i=0; i<nb_ener; i++) {
      temp = sub(sh1, sh_ener[i]);
      L_acc = L_deposit_l(ener[i]);
      L_acc = L_shl(L_acc, temp);
      L_x = L_add(L_x, L_acc);
    }
    L_Extract(L_x, &hi, &lo);
    L_x = Mpy_32_16(hi, lo, fact[i]);
  }
  
  *idx = Quant_Energy(L_x, sh1, enerq);
  
  return;
}


/* Local function */

static s16 Quant_Energy(
  l32 L_x,    /* (i)  : Energy                 */
  s16 sh,     /* (i)  : Exponent of the energy */
  s16 *enerq  /* (o)  : quantized energy in dB */
)
{

  s16 exp, frac;
  s16 e_tmp, temp, index;

  Log2(L_x, &exp, &frac);
  temp = sub(exp, sh);
  e_tmp = shl(temp, 10);
  e_tmp = add(e_tmp, mult_r(frac, 1024)); /* 2^10 x log2(L_x . 2^-sh) */
  /* log2(ener) = 10log10(ener) / K */
  /* K = 10 Log2 / Log10 */

  temp = sub(e_tmp, -2721);      /* -2721 -> -8dB */
  if(temp <= 0) {
    *enerq = -12;
    return(0);
  }

  temp = sub(e_tmp, 22111);      /* 22111 -> 65 dB */  
  if(temp > 0) {
    *enerq = 66;
    return(31);
  }

  temp = sub(e_tmp, 4762);       /* 4762 -> 14 dB */
  if(temp <= 0){
    e_tmp = add(e_tmp, 3401);
    index = mult(e_tmp, 24);
    if (index < 1) index = 1;
    *enerq = sub(shl(index, 2), 8);
    return(index);
  }

  e_tmp = sub(e_tmp, 340);
  index = sub(shr(mult(e_tmp, 193), 2), 1);
  if (index < 6) index = 6;
  *enerq = add(shl(index, 1), 4);
  return(index);
}



