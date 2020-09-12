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
/*------------------------------------------------------------------------*
 * Function Post_Process()                                                *
 *                                                                        *
 * Post-processing of output speech.                                      *
 *   - 2nd order high pass filter with cut off frequency at 100 Hz.       *
 *   - Multiplication by two of output speech with saturation.            *
 *-----------------------------------------------------------------------*/
#include "basicop.h"
#include "ld8a.h"
#include "tab_ld8a.h"

/*------------------------------------------------------------------------*
 * 2nd order high pass filter with cut off frequency at 100 Hz.           *
 * Designed with SPPACK efi command -40 dB att, 0.25 ri.                  *
 *                                                                        *
 * Algorithm:                                                             *
 *                                                                        *
 *  y[i] = b[0]*x[i]   + b[1]*x[i-1]   + b[2]*x[i-2]                      *
 *                     + a[1]*y[i-1]   + a[2]*y[i-2];                     *
 *                                                                        *
 *     b[3] = {0.93980581E+00, -0.18795834E+01, 0.93980581E+00};          *
 *     a[3] = {0.10000000E+01, 0.19330735E+01, -0.93589199E+00};          *
 *-----------------------------------------------------------------------*/

/* Static values to be preserved between calls */
/* y[] values is keep in double precision      */

//static s16 y2_hi, y2_lo, y1_hi, y1_lo, x0, x1;


/* Initialization of static values */

void Init_Post_Process(TG729BDECODER *pG729DecH)
{
  pG729DecH->y2_hi = 0;
  pG729DecH->y2_lo = 0;
  pG729DecH->y1_hi = 0;
  pG729DecH->y1_lo = 0;
  pG729DecH->x0   = 0;
  pG729DecH->x1   = 0;
}

void Post_Process(
  s16 signal[],    /* input/output signal */
  s16 lg,          /* length of signal    */
  TG729BDECODER *pG729DecH)
{
  s16 i, x2;
  l32 L_tmp;

  for(i=0; i<lg; i++)
  {
     x2 = pG729DecH->x1;
     pG729DecH->x1 = pG729DecH->x0;
     pG729DecH->x0 = signal[i];

     /*  y[i] = b[0]*x[i]   + b[1]*x[i-1]   + b[2]*x[i-2]    */
     /*                     + a[1]*y[i-1] + a[2] * y[i-2];      */

     L_tmp     = Mpy_32_16(pG729DecH->y1_hi, pG729DecH->y1_lo, a100[1]);
     L_tmp     = L_add(L_tmp, Mpy_32_16(pG729DecH->y2_hi, pG729DecH->y2_lo, a100[2]));
     L_tmp     = L_mac(L_tmp, pG729DecH->x0, b100[0]);
     L_tmp     = L_mac(L_tmp, pG729DecH->x1, b100[1]);
     L_tmp     = L_mac(L_tmp, x2, b100[2]);
     L_tmp     = L_shl(L_tmp, 2);      /* Q29 --> Q31 (Q13 --> Q15) */

     /* Multiplication by two of output speech with saturation. */
     signal[i] = round_linux(L_shl(L_tmp, 1));

     pG729DecH->y2_hi = pG729DecH->y1_hi;
     pG729DecH->y2_lo = pG729DecH->y1_lo;
     L_Extract(L_tmp, &pG729DecH->y1_hi, &pG729DecH->y1_lo);
  }
  return;
}
