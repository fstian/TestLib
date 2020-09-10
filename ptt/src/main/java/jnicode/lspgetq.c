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


void Lsp_get_quant(
  s16 lspcb1[][M],      /* (i) Q13 : first stage LSP codebook      */
  s16 lspcb2[][M],      /* (i) Q13 : Second stage LSP codebook     */
  s16 code0,            /* (i)     : selected code of first stage  */
  s16 code1,            /* (i)     : selected code of second stage */
  s16 code2,            /* (i)     : selected code of second stage */
  s16 fg[][M],          /* (i) Q15 : MA prediction coef.           */
  s16 freq_prev[][M],   /* (i) Q13 : previous LSP vector           */
  s16 lspq[],           /* (o) Q13 : quantized LSP parameters      */
  s16 fg_sum[]          /* (i) Q15 : present MA prediction coef.   */
)
{
  s16 j;
  s16 buf[M];           /* Q13 */


  for ( j = 0 ; j < NC ; j++ )
    buf[j] = add( lspcb1[code0][j], lspcb2[code1][j] );

  for ( j = NC ; j < M ; j++ )
    buf[j] = add( lspcb1[code0][j], lspcb2[code2][j] );

  Lsp_expand_1_2(buf, GAP1);
  Lsp_expand_1_2(buf, GAP2);

  Lsp_prev_compose(buf, lspq, fg, freq_prev, fg_sum);

  Lsp_prev_update(buf, freq_prev);

  Lsp_stability( lspq );

  return;
}


void Lsp_expand_1(
  s16 buf[],        /* (i/o) Q13 : LSP vectors */
  s16 gap           /* (i)   Q13 : gap         */
)
{
  s16 j, tmp;
  s16 diff;        /* Q13 */

  for ( j = 1 ; j < NC ; j++ ) {
    diff = sub( buf[j-1], buf[j] );
    tmp = shr( add( diff, gap), 1 );

    if ( tmp >  0 ) {
      buf[j-1] = sub( buf[j-1], tmp );
      buf[j]   = add( buf[j], tmp );
    }
  }
  return;
}


void Lsp_expand_2(
  s16 buf[],       /* (i/o) Q13 : LSP vectors */
  s16 gap          /* (i)   Q13 : gap         */
)
{
  s16 j, tmp;
  s16 diff;        /* Q13 */

  for ( j = NC ; j < M ; j++ ) {
    diff = sub( buf[j-1], buf[j] );
    tmp = shr( add( diff, gap), 1 );

    if ( tmp > 0 ) {
      buf[j-1] = sub( buf[j-1], tmp );
      buf[j]   = add( buf[j], tmp );
    }
  }
  return;
}


void Lsp_expand_1_2(
  s16 buf[],       /* (i/o) Q13 : LSP vectors */
  s16 gap          /* (i)   Q13 : gap         */
)
{
  s16 j, tmp;
  s16 diff;        /* Q13 */

  for ( j = 1 ; j < M ; j++ ) {
    diff = sub( buf[j-1], buf[j] );
    tmp = shr( add( diff, gap), 1 );

    if ( tmp > 0 ) {
      buf[j-1] = sub( buf[j-1], tmp );
      buf[j]   = add( buf[j], tmp );
    }
  }
  return;
}


/*
  Functions which use previous LSP parameter (freq_prev).
*/


/*
  compose LSP parameter from elementary LSP with previous LSP.
*/
void Lsp_prev_compose(
  s16 lsp_ele[],             /* (i) Q13 : LSP vectors                 */
  s16 lsp[],                 /* (o) Q13 : quantized LSP parameters    */
  s16 fg[][M],               /* (i) Q15 : MA prediction coef.         */
  s16 freq_prev[][M],        /* (i) Q13 : previous LSP vector         */
  s16 fg_sum[]               /* (i) Q15 : present MA prediction coef. */
)
{
  s16 j, k;
  l32 L_acc;                 /* Q29 */

  for ( j = 0 ; j < M ; j++ ) {
    L_acc = L_mult( lsp_ele[j], fg_sum[j] );
    for ( k = 0 ; k < MA_NP ; k++ )
      L_acc = L_mac( L_acc, freq_prev[k][j], fg[k][j] );

    lsp[j] = extract_h(L_acc);
  }
  return;
}


/*
  extract elementary LSP from composed LSP with previous LSP
*/
void Lsp_prev_extract(
  s16 lsp[M],                /* (i) Q13 : unquantized LSP parameters  */
  s16 lsp_ele[M],            /* (o) Q13 : target vector               */
  s16 fg[MA_NP][M],          /* (i) Q15 : MA prediction coef.         */
  s16 freq_prev[MA_NP][M],   /* (i) Q13 : previous LSP vector         */
  s16 fg_sum_inv[M]          /* (i) Q12 : inverse previous LSP vector */
)
{
  s16 j, k;
  l32 L_temp;                /* Q19 */
  s16 temp;                  /* Q13 */


  for ( j = 0 ; j < M ; j++ ) {
    L_temp = L_deposit_h(lsp[j]);
    for ( k = 0 ; k < MA_NP ; k++ )
      L_temp = L_msu( L_temp, freq_prev[k][j], fg[k][j] );

    temp = extract_h(L_temp);
    L_temp = L_mult( temp, fg_sum_inv[j] );
    lsp_ele[j] = extract_h( L_shl( L_temp, 3 ) );

  }
  return;
}


/*
  update previous LSP parameter
*/
void Lsp_prev_update(
  s16 lsp_ele[M],             /* (i)   Q13 : LSP vectors           */
  s16 freq_prev[MA_NP][M]     /* (i/o) Q13 : previous LSP vectors  */
)
{
  s16 k;

  for ( k = MA_NP-1 ; k > 0 ; k-- )
    Copy(freq_prev[k-1], freq_prev[k], M);

  Copy(lsp_ele, freq_prev[0], M);
  return;
}

void Lsp_stability(
  s16 buf[]       /* (i/o) Q13 : quantized LSP parameters      */
)
{
  s16 j;
  s16 tmp;
  l32 L_diff;
  l32 L_acc, L_accb;

  for(j=0; j<M-1; j++) {
    L_acc = L_deposit_l( buf[j+1] );
    L_accb = L_deposit_l( buf[j] );
    L_diff = L_sub( L_acc, L_accb );

    if( L_diff < 0L ) {
      /* exchange buf[j]<->buf[j+1] */
      tmp      = buf[j+1];
      buf[j+1] = buf[j];
      buf[j]   = tmp;
    }
  }


  if( sub(buf[0], L_LIMIT) <0 ) {
    buf[0] = L_LIMIT;
    printf("lsp_stability warning Low \n");
  }
  for(j=0; j<M-1; j++) {
    L_acc = L_deposit_l( buf[j+1] );
    L_accb = L_deposit_l( buf[j] );
    L_diff = L_sub( L_acc, L_accb );

    if( L_sub(L_diff, GAP3)<0L ) {
      buf[j+1] = add( buf[j], GAP3 );
    }
  }

  if( sub(buf[M-1],M_LIMIT)>0 ) {
    buf[M-1] = M_LIMIT;
    printf("lsp_stability warning High \n");
  }
  return;
}
