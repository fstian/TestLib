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
/*-------------------------------------------------------------------*
 * Function  Pred_lt_3()                                             *
 *           ~~~~~~~~~~~                                             *
 *-------------------------------------------------------------------*
 * Compute the result of long term prediction with fractional        *
 * interpolation of resolution 1/3.                                  *
 *                                                                   *
 * On return exc[0..L_subfr-1] contains the interpolated signal      *
 *   (adaptive codebook excitation)                                  *
 *-------------------------------------------------------------------*/
#include "basicop.h"
#include "ld8a.h"
#include "tab_ld8a.h"

void Pred_lt_3(
  s16   exc[],       /* in/out: excitation buffer */
  s16   T0,          /* input : integer pitch lag */
  s16   frac,        /* input : fraction of lag   */
  s16   L_subfr      /* input : subframe size     */
)
{
  s16   i, j, k;
  s16   *x0, *x1, *x2, *c1, *c2;
  l32  s;

  x0 = &exc[-T0];
  
  frac = negate(frac);
  if (frac < 0)
  {
    frac = add(frac, UP_SAMP);
    x0--;
  }

  for (j=0; j<L_subfr; j++)
  {
    x1 = x0++;
    x2 = x0;
    c1 = &inter_3l[frac];
    c2 = &inter_3l[sub(UP_SAMP,frac)];

    s = 0;
    for(i=0, k=0; i< L_INTER10; i++, k+=UP_SAMP)
    {
      s = L_mac(s, x1[-i], c1[k]);
      s = L_mac(s, x2[i],  c2[k]);
    }
    exc[j] = round_linux(s);
  }

  return;
}
