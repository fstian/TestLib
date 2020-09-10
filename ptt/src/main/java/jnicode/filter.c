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
 * Function  Convolve:                                               *
 *           ~~~~~~~~~                                               *
 *-------------------------------------------------------------------*
 * Perform the convolution between two vectors x[] and h[] and       *
 * write the result in the vector y[].                               *
 * All vectors are of length N.                                      *
 *-------------------------------------------------------------------*/
#include "basicop.h"
#include "ld8a.h"

void Convolve(
  s16 x[],      /* (i)     : input vector                           */
  s16 h[],      /* (i) Q12 : impulse response                       */
  s16 y[],      /* (o)     : output vector                          */
  s16 L         /* (i)     : vector size                            */
)
{
   s16 i, n;
   l32 s;

   for (n = 0; n < L; n++)
   {
     s = 0;
     for (i = 0; i <= n; i++)
       s = L_mac(s, x[i], h[n-i]);

     s    = L_shl(s, 3);                   /* h is in Q12 and saturation */
     y[n] = extract_h(s);
   }

   return;
}

/*-----------------------------------------------------*
 * procedure Syn_filt:                                 *
 *           ~~~~~~~~                                  *
 * Do the synthesis filtering 1/A(z).                  *
 *-----------------------------------------------------*/


void Syn_filt(
  s16 a[],     /* (i) Q12 : a[m+1] prediction coefficients   (m=10)  */
  s16 x[],     /* (i)     : input signal                             */
  s16 y[],     /* (o)     : output signal                            */
  s16 lg,      /* (i)     : size of filtering                        */
  s16 mem[],   /* (i/o)   : memory associated with this filtering.   */
  s16 update   /* (i)     : 0=no update, 1=update of memory.         */
)
{
  s16 i, j;
  l32 s;
  s16 tmp[100];     /* This is usually done by memory allocation (lg+M) */
  s16 *yy;

  /* Copy mem[] to yy[] */

  yy = tmp;
  for(i=0; i<M; i++)
  {
    *yy++ = mem[i];
  }

  /* Do the filtering. */

  for (i = 0; i < lg; i++)
  {
    s = L_mult(x[i], a[0]);
    for (j = 1; j <= M; j++)
      s = L_msu(s, a[j], yy[-j]);

    s = L_shl(s, 3);
    *yy++ = round_linux(s);
  }

  for(i=0; i<lg; i++)
  {
    y[i] = tmp[i+M];
  }

  /* Update of memory if update==1 */

  if(update != 0)
     for (i = 0; i < M; i++)
     {
       mem[i] = y[lg-M+i];
     }

 return;
}

/*-----------------------------------------------------------------------*
 * procedure Residu:                                                     *
 *           ~~~~~~                                                      *
 * Compute the LPC residual  by filtering the input speech through A(z)  *
 *-----------------------------------------------------------------------*/

void Residu(
  s16 a[],    /* (i) Q12 : prediction coefficients                     */
  s16 x[],    /* (i)     : speech (values x[-m..-1] are needed         */
  s16 y[],    /* (o)     : residual signal                             */
  s16 lg      /* (i)     : size of filtering                           */
)
{
  s16 i, j;
  l32 s;

  for (i = 0; i < lg; i++)
  {
    s = L_mult(x[i], a[0]);
    for (j = 1; j <= M; j++)
      s = L_mac(s, a[j], x[i-j]);

    s = L_shl(s, 3);
    y[i] = round_linux(s);
  }
  return;
}
