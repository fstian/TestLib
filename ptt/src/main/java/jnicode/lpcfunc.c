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
/*-------------------------------------------------------------*
 *  Procedure Lsp_Az:                                          *
 *            ~~~~~~                                           *
 *   Compute the LPC coefficients from lsp (order=10)          *
 *-------------------------------------------------------------*/
#include "basicop.h"
#include "ld8a.h"
#include "tab_ld8a.h"

/* local function */

static void Get_lsp_pol(s16 *lsp, l32 *f);

void Lsp_Az(
  s16 lsp[],    /* (i) Q15 : line spectral frequencies            */
  s16 a[]       /* (o) Q12 : predictor coefficients (order = 10)  */
)
{
  s16 i, j;
  l32 f1[6], f2[6];
  l32 t0;

  Get_lsp_pol(&lsp[0],f1);
  Get_lsp_pol(&lsp[1],f2);

  for (i = 5; i > 0; i--)
  {
    f1[i] = L_add(f1[i], f1[i-1]);        /* f1[i] += f1[i-1]; */
    f2[i] = L_sub(f2[i], f2[i-1]);        /* f2[i] -= f2[i-1]; */
  }

  a[0] = 4096;
  for (i = 1, j = 10; i <= 5; i++, j--)
  {
    t0   = L_add(f1[i], f2[i]);                 /* f1[i] + f2[i]             */
    a[i] = extract_l( L_shr_r(t0, 13) );        /* from Q24 to Q12 and * 0.5 */

    t0   = L_sub(f1[i], f2[i]);                 /* f1[i] - f2[i]             */
    a[j] = extract_l( L_shr_r(t0, 13) );        /* from Q24 to Q12 and * 0.5 */

  }

  return;
}

/*-----------------------------------------------------------*
 * procedure Get_lsp_pol:                                    *
 *           ~~~~~~~~~~~                                     *
 *   Find the polynomial F1(z) or F2(z) from the LSPs        *
 *-----------------------------------------------------------*
 *                                                           *
 * Parameters:                                               *
 *  lsp[]   : line spectral freq. (cosine domain)    in Q15  *
 *  f[]     : the coefficients of F1 or F2           in Q24  *
 *-----------------------------------------------------------*/

static void Get_lsp_pol(s16 *lsp, l32 *f)
{
  s16 i,j, hi, lo;
  l32 t0;

   /* All computation in Q24 */

   *f = L_mult(4096, 2048);             /* f[0] = 1.0;             in Q24  */
   f++;
   *f = L_msu((l32)0, *lsp, 512);    /* f[1] =  -2.0 * lsp[0];  in Q24  */

   f++;
   lsp += 2;                            /* Advance lsp pointer             */

   for(i=2; i<=5; i++)
   {
     *f = f[-2];

     for(j=1; j<i; j++, f--)
     {
       L_Extract(f[-1] ,&hi, &lo);
       t0 = Mpy_32_16(hi, lo, *lsp);         /* t0 = f[-1] * lsp    */
       t0 = L_shl(t0, 1);
       *f = L_add(*f, f[-2]);                /* *f += f[-2]         */
       *f = L_sub(*f, t0);                   /* *f -= t0            */
     }
     *f   = L_msu(*f, *lsp, 512);            /* *f -= lsp<<9        */
     f   += i;                               /* Advance f pointer   */
     lsp += 2;                               /* Advance lsp pointer */
   }

   return;
}

/*___________________________________________________________________________
 |                                                                           |
 |   Functions : Lsp_lsf and Lsf_lsp                                         |
 |                                                                           |
 |      Lsp_lsf   Transformation lsp to lsf                                  |
 |      Lsf_lsp   Transformation lsf to lsp                                  |
 |---------------------------------------------------------------------------|
 |  Algorithm:                                                               |
 |                                                                           |
 |   The transformation from lsp[i] to lsf[i] and lsf[i] to lsp[i] are       |
 |   approximated by a look-up table and interpolation.                      |
 |___________________________________________________________________________|
*/


void Lsf_lsp(
  s16 lsf[],    /* (i) Q15 : lsf[m] normalized (range: 0.0<=val<=0.5) */
  s16 lsp[],    /* (o) Q15 : lsp[m] (range: -1<=val<1)                */
  s16 m         /* (i)     : LPC order                                */
)
{
  s16 i, ind, offset;
  l32 L_tmp;

  for(i=0; i<m; i++)
  {
    ind    = shr(lsf[i], 8);               /* ind    = b8-b15 of lsf[i] */
    offset = lsf[i] & (s16)0x00ff;      /* offset = b0-b7  of lsf[i] */

    /* lsp[i] = table[ind]+ ((table[ind+1]-table[ind])*offset) / 256 */

    L_tmp   = L_mult(sub(table[ind+1], table[ind]), offset);
    lsp[i] = add(table[ind], extract_l(L_shr(L_tmp, 9)));
  }
  return;
}


void Lsp_lsf(
  s16 lsp[],    /* (i) Q15 : lsp[m] (range: -1<=val<1)                */
  s16 lsf[],    /* (o) Q15 : lsf[m] normalized (range: 0.0<=val<=0.5) */
  s16 m         /* (i)     : LPC order                                */
)
{
  s16 i, ind, tmp;
  l32 L_tmp;

  ind = 63;    /* begin at end of table -1 */

  for(i= m-(s16)1; i >= 0; i--)
  {
    /* find value in table that is just greater than lsp[i] */
    while( sub(table[ind], lsp[i]) < 0 )
    {
      ind = sub(ind,1);
    }

    /* acos(lsp[i])= ind*256 + ( ( lsp[i]-table[ind] ) * slope[ind] )/4096 */

    L_tmp  = L_mult( sub(lsp[i], table[ind]) , slope[ind] );
    tmp = round_linux(L_shl(L_tmp, 3));     /*(lsp[i]-table[ind])*slope[ind])>>12*/
    lsf[i] = add(tmp, shl(ind, 8));
  }
  return;
}

/*___________________________________________________________________________
 |                                                                           |
 |   Functions : Lsp_lsf and Lsf_lsp                                         |
 |                                                                           |
 |      Lsp_lsf   Transformation lsp to lsf                                  |
 |      Lsf_lsp   Transformation lsf to lsp                                  |
 |---------------------------------------------------------------------------|
 |  Algorithm:                                                               |
 |                                                                           |
 |   The transformation from lsp[i] to lsf[i] and lsf[i] to lsp[i] are       |
 |   approximated by a look-up table and interpolation.                      |
 |___________________________________________________________________________|
*/

void Lsf_lsp2(
  s16 lsf[],    /* (i) Q13 : lsf[m] (range: 0.0<=val<PI) */
  s16 lsp[],    /* (o) Q15 : lsp[m] (range: -1<=val<1)   */
  s16 m         /* (i)     : LPC order                   */
)
{
  s16 i, ind;
  s16 offset;   /* in Q8 */
  s16 freq;     /* normalized frequency in Q15 */
  l32 L_tmp;

  for(i=0; i<m; i++)
  {
/*    freq = abs_s(freq);*/
    freq = mult(lsf[i], 20861);          /* 20861: 1.0/(2.0*PI) in Q17 */
    ind    = shr(freq, 8);               /* ind    = b8-b15 of freq */
    offset = freq & (s16)0x00ff;      /* offset = b0-b7  of freq */

    if ( sub(ind, 63)>0 ){
      ind = 63;                 /* 0 <= ind <= 63 */
    }

    /* lsp[i] = table2[ind]+ (slope_cos[ind]*offset >> 12) */

    L_tmp   = L_mult(slope_cos[ind], offset);   /* L_tmp in Q28 */
    lsp[i] = add(table2[ind], extract_l(L_shr(L_tmp, 13)));

  }
  return;
}



void Lsp_lsf2(
  s16 lsp[],    /* (i) Q15 : lsp[m] (range: -1<=val<1)   */
  s16 lsf[],    /* (o) Q13 : lsf[m] (range: 0.0<=val<PI) */
  s16 m         /* (i)     : LPC order                   */
)
{
  s16 i, ind;
  s16 offset;   /* in Q15 */
  s16 freq;     /* normalized frequency in Q16 */
  l32 L_tmp;

  ind = 63;           /* begin at end of table2 -1 */

  for(i= m-(s16)1; i >= 0; i--)
  {
    /* find value in table2 that is just greater than lsp[i] */
    while( sub(table2[ind], lsp[i]) < 0 )
    {
      ind = sub(ind,1);
      if ( ind <= 0 )
        break;
    }

    offset = sub(lsp[i], table2[ind]);

    /* acos(lsp[i])= ind*512 + (slope_acos[ind]*offset >> 11) */

    L_tmp  = L_mult( slope_acos[ind], offset );   /* L_tmp in Q28 */
    freq = add(shl(ind, 9), extract_l(L_shr(L_tmp, 12)));
    lsf[i] = mult(freq, 25736);           /* 25736: 2.0*PI in Q12 */

  }
  return;
}

/*-------------------------------------------------------------*
 *  procedure Weight_Az                                        *
 *            ~~~~~~~~~                                        *
 * Weighting of LPC coefficients.                              *
 *   ap[i]  =  a[i] * (gamma ** i)                             *
 *                                                             *
 *-------------------------------------------------------------*/


void Weight_Az(
  s16 a[],      /* (i) Q12 : a[m+1]  LPC coefficients             */
  s16 gamma,    /* (i) Q15 : Spectral expansion factor.           */
  s16 m,        /* (i)     : LPC order.                           */
  s16 ap[]      /* (o) Q12 : Spectral expanded LPC coefficients   */
)
{
  s16 i, fac;

  ap[0] = a[0];
  fac   = gamma;
  for(i=1; i<m; i++)
  {
    ap[i] = round_linux( L_mult(a[i], fac) );
    fac   = round_linux( L_mult(fac, gamma) );
  }
  ap[m] = round_linux( L_mult(a[m], fac) );

  return;
}

/*----------------------------------------------------------------------*
 * Function Int_qlpc()                                                  *
 * ~~~~~~~~~~~~~~~~~~~                                                  *
 * Interpolation of the LPC parameters.                                 *
 *----------------------------------------------------------------------*/

/* Interpolation of the quantized LSP's */

void Int_qlpc(
 s16 lsp_old[], /* input : LSP vector of past frame              */
 s16 lsp_new[], /* input : LSP vector of present frame           */
 s16 Az[]       /* output: interpolated Az() for the 2 subframes */
)
{
  s16 i;
  s16 lsp[M];

  /*  lsp[i] = lsp_new[i] * 0.5 + lsp_old[i] * 0.5 */

  for (i = 0; i < M; i++) {
    lsp[i] = add(shr(lsp_new[i], 1), shr(lsp_old[i], 1));
  }

  Lsp_Az(lsp, Az);              /* Subframe 1 */

  Lsp_Az(lsp_new, &Az[MP1]);    /* Subframe 2 */

  return;
}

