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
* Function  Qua_lsp:                                                *
*           ~~~~~~~~                                                *
*-------------------------------------------------------------------*/
#include "basicop.h"
#include "ld8a.h"
#include "tab_ld8a.h"

void Qua_lsp(
			 s16 lsp[],       /* (i) Q15 : Unquantized LSP            */
			 s16 lsp_q[],     /* (o) Q15 : Quantized LSP              */
			 s16 ana[],        /* (o)     : indexes                    */
			 TG729BENCODER *pG729EncH
			 )
{
	s16 lsf[M], lsf_q[M];  /* domain 0.0<= lsf <PI in Q13 */
	/* Convert LSPs to LSFs */
	Lsp_lsf2(lsp, lsf, M);
		/*test code
		{
			l32 i =1;
			
			//s16fprt(lsf, 10, "as16Lsf", &i, 0);
		}*/
	
	Lsp_qua_cs(lsf, lsf_q, ana, pG729EncH);
		/*test code
		{
			l32 i =1;
			
			//s16fprt(lsf_q, 10, "as16LsfQ", &i, 0);
		}*/	
	/* Convert LSFs to LSPs */
	Lsf_lsp2(lsf_q, lsp_q, M);
	
	return;
}


/* static memory

static s16 freq_prev[MA_NP][M];    //Q13:previous LSP vector
 */
void Lsp_encw_reset(TG729BENCODER *pG729EncH)
{
	s16 i;
	
	for(i=0; i<MA_NP; i++)
		Copy( &freq_prev_reset[0], &pG729EncH->freq_prev[i][0], M );
}

void Lsp_qua_cs(
				s16 flsp_in[M],    /* (i) Q13 : Original LSP parameters    */
				s16 lspq_out[M],   /* (o) Q13 : Quantized LSP parameters   */
				s16 *code,          /* (o)     : codes of the selected LSP  */
				TG729BENCODER *pG729EncH
				)
{
	s16 wegt[M];       /* Q11->normalized : weighting coefficients */
	
	Get_wegt( flsp_in, wegt );
		/*test code
		{
			l32 i =1;
			
			//s16fprt(wegt, 10, "as16Wgt", &i, 0);
		}*/
		/*test code
		{
			l32 i =1;
			
			//s16fprt(pG729EncH->freq_prev, 40, "as16Fp", &i, 0);
		}*/
	Relspwed( flsp_in, wegt, lspq_out, lspcb1, lspcb2, fg,
		pG729EncH->freq_prev, fg_sum, fg_sum_inv, code);
}

void Relspwed(
  s16 lsp[],                 /* (i) Q13 : unquantized LSP parameters */
  s16 wegt[],                /* (i) norm: weighting coefficients     */
  s16 lspq[],                /* (o) Q13 : quantized LSP parameters   */
  s16 lspcb1[][M],           /* (i) Q13 : first stage LSP codebook   */
  s16 lspcb2[][M],           /* (i) Q13 : Second stage LSP codebook  */
  s16 fg[MODE][MA_NP][M],    /* (i) Q15 : MA prediction coefficients */
  s16 freq_prev[MA_NP][M],   /* (i) Q13 : previous LSP vector        */
  s16 fg_sum[MODE][M],       /* (i) Q15 : present MA prediction coef.*/
  s16 fg_sum_inv[MODE][M],   /* (i) Q12 : inverse coef.              */
  s16 code_ana[]             /* (o)     : codes of the selected LSP  */
)
{
  s16 mode, j;
  s16 index, mode_index;
  s16 cand[MODE], cand_cur;
  s16 tindex1[MODE], tindex2[MODE];
  l32 L_tdist[MODE];         /* Q26 */
  s16 rbuf[M];               /* Q13 */
  s16 buf[M];                /* Q13 */

  for(mode = 0; mode<MODE; mode++) {
    Lsp_prev_extract(lsp, rbuf, fg[mode], freq_prev, fg_sum_inv[mode]);

    Lsp_pre_select(rbuf, lspcb1, &cand_cur );
    cand[mode] = cand_cur;

    Lsp_select_1(rbuf, lspcb1[cand_cur], wegt, lspcb2, &index);

    tindex1[mode] = index;

    for( j = 0 ; j < NC ; j++ )
      buf[j] = add( lspcb1[cand_cur][j], lspcb2[index][j] );

    Lsp_expand_1(buf, GAP1);

    Lsp_select_2(rbuf, lspcb1[cand_cur], wegt, lspcb2, &index);

    tindex2[mode] = index;

    for( j = NC ; j < M ; j++ )
      buf[j] = add( lspcb1[cand_cur][j], lspcb2[index][j] );

    Lsp_expand_2(buf, GAP1);

    Lsp_expand_1_2(buf, GAP2);

    Lsp_get_tdist(wegt, buf, &L_tdist[mode], rbuf, fg_sum[mode]);
  }

  Lsp_last_select(L_tdist, &mode_index);

  code_ana[0] = shl( mode_index,NC0_B ) | cand[mode_index];
  code_ana[1] = shl( tindex1[mode_index],NC1_B ) | tindex2[mode_index];

  Lsp_get_quant(lspcb1, lspcb2, cand[mode_index],
      tindex1[mode_index], tindex2[mode_index],
      fg[mode_index], freq_prev, lspq, fg_sum[mode_index]) ;

  return;
}


void Lsp_pre_select(
					s16 rbuf[],              /* (i) Q13 : target vetor             */
					s16 lspcb1[][M],         /* (i) Q13 : first stage LSP codebook */
					s16 *cand                /* (o)     : selected code            */
					)
{
	s16 i, j;
	s16 tmp;                 /* Q13 */
	l32 L_dmin;              /* Q26 */
	l32 L_tmp;               /* Q26 */
	l32 L_temp;
	
	/* avoid the worst case. (all over flow) */
	
	*cand = 0;
	L_dmin = MAX_32;
	for ( i = 0 ; i < NC0 ; i+=1 ) {
		L_tmp = 0;
		for ( j = 0 ; j < M ; j++ ) {
			tmp = sub(rbuf[j], lspcb1[i][j]);
			L_tmp = L_mac( L_tmp, tmp, tmp );
			if (L_tmp>=L_dmin)
				j = M;
		}
		
		L_temp = L_sub(L_tmp,L_dmin);
		if (  L_temp< 0L) {
			L_dmin = L_tmp;
			*cand = i;
		}
	}
	return;
}



void Lsp_select_1(
				  s16 rbuf[],              /* (i) Q13 : target vector             */
				  s16 lspcb1[],            /* (i) Q13 : first stage lsp codebook  */
				  s16 wegt[],              /* (i) norm: weighting coefficients    */
				  s16 lspcb2[][M],         /* (i) Q13 : second stage lsp codebook */
				  s16 *index               /* (o)     : selected codebook index   */
				  )
{
	s16 j, k1;
	s16 buf[M];              /* Q13 */
	l32 L_dist;              /* Q26 */
	l32 L_dmin;              /* Q26 */
	s16 tmp,tmp2;            /* Q13 */
	l32 L_temp;
	
	for ( j = 0 ; j < NC ; j++ )
		buf[j] = sub(rbuf[j], lspcb1[j]);
	
	/* avoid the worst case. (all over flow) */
	*index = 0;
	L_dmin = MAX_32;
	for ( k1 = 0 ; k1 < NC1 ; k1++ ) {
		L_dist = 0;
		for ( j = 0 ; j < NC ; j++ ) {
			tmp = sub(buf[j], lspcb2[k1][j]);
			tmp2 = mult( wegt[j], tmp );
			L_dist = L_mac( L_dist, tmp2, tmp );
		}
		
		L_temp =L_sub(L_dist,L_dmin);
		if ( L_temp <0L ) {
			L_dmin = L_dist;
			*index = k1;
		}
	}
	return;
}



void Lsp_select_2(
				  s16 rbuf[],              /* (i) Q13 : target vector             */
				  s16 lspcb1[],            /* (i) Q13 : first stage lsp codebook  */
				  s16 wegt[],              /* (i) norm: weighting coef.           */
				  s16 lspcb2[][M],         /* (i) Q13 : second stage lsp codebook */
				  s16 *index               /* (o)     : selected codebook index   */
				  )
{
	s16 j, k1;
	s16 buf[M];              /* Q13 */
	l32 L_dist;              /* Q26 */
	l32 L_dmin;              /* Q26 */
	s16 tmp,tmp2;            /* Q13 */
	l32 L_temp;
	
	for ( j = NC ; j < M ; j++ )
		buf[j] = sub(rbuf[j], lspcb1[j]);
	
	/* avoid the worst case. (all over flow) */
	*index = 0;
	L_dmin = MAX_32;
	for ( k1 = 0 ; k1 < NC1 ; k1++ ) {
		L_dist = 0;
		for ( j = NC ; j < M ; j++ ) {
			tmp = sub(buf[j], lspcb2[k1][j]);
			tmp2 = mult( wegt[j], tmp );
			L_dist = L_mac( L_dist, tmp2, tmp );
		}
		
		L_temp = L_sub(L_dist, L_dmin);
		if ( L_temp <0L ) {
			L_dmin = L_dist;
			*index = k1;
		}
	}
	return;
}



void Lsp_get_tdist(
				   s16 wegt[],        /* (i) norm: weight coef.                */
				   s16 buf[],         /* (i) Q13 : candidate LSP vector        */
				   l32 *L_tdist,      /* (o) Q27 : distortion                  */
				   s16 rbuf[],        /* (i) Q13 : target vector               */
				   s16 fg_sum[]       /* (i) Q15 : present MA prediction coef. */
				   )
{
	s16 j;
	s16 tmp, tmp2;     /* Q13 */
	l32 L_acc;         /* Q25 */
	
	*L_tdist = 0;
	for ( j = 0 ; j < M ; j++ ) {
		/* tmp = (buf - rbuf)*fg_sum */
		tmp = sub( buf[j], rbuf[j] );
		tmp = mult( tmp, fg_sum[j] );
		
		/* *L_tdist += wegt * tmp * tmp */
		L_acc = L_mult( wegt[j], tmp );
		tmp2 = extract_h( L_shl( L_acc, 4 ) );
		*L_tdist = L_mac( *L_tdist, tmp2, tmp );
	}
	
	return;
}



void Lsp_last_select(
					 l32 L_tdist[],     /* (i) Q27 : distortion         */
					 s16 *mode_index    /* (o)     : the selected mode  */
					 )
{
    l32 L_temp;
	*mode_index = 0;
	L_temp =L_sub(L_tdist[1] ,L_tdist[0]);
	if (  L_temp<0L){
		*mode_index = 1;
	}
	return;
}

void Get_wegt(
			  s16 flsp[],    /* (i) Q13 : M LSP parameters  */
			  s16 wegt[]     /* (o) Q11->norm : M weighting coefficients */
			  )
{
	s16 i;
	s16 tmp;
	l32 L_acc;
	s16 sft;
	s16 buf[M]; /* in Q13 */
	
	
	buf[0] = sub( flsp[1], (PI04+8192) );           /* 8192:1.0(Q13) */
	
	for ( i = 1 ; i < M-1 ; i++ ) {
		tmp = sub( flsp[i+1], flsp[i-1] );
		buf[i] = sub( tmp, 8192 );
	}
	
	buf[M-1] = sub( (PI92-8192), flsp[M-2] );
	
	/* */
	for ( i = 0 ; i < M ; i++ ) {
		if ( buf[i] > 0 ){
			wegt[i] = 2048;                    /* 2048:1.0(Q11) */
		}
		else {
			L_acc = L_mult( buf[i], buf[i] );           /* L_acc in Q27 */
			tmp = extract_h( L_shl( L_acc, 2 ) );       /* tmp in Q13 */
			
			L_acc = L_mult( tmp, CONST10 );             /* L_acc in Q25 */
			tmp = extract_h( L_shl( L_acc, 2 ) );       /* tmp in Q11 */
			
			wegt[i] = add( tmp, 2048 );                 /* wegt in Q11 */
		}
	}
	
	/* */
	L_acc = L_mult( wegt[4], CONST12 );             /* L_acc in Q26 */
	wegt[4] = extract_h( L_shl( L_acc, 1 ) );       /* wegt in Q11 */
	
	L_acc = L_mult( wegt[5], CONST12 );             /* L_acc in Q26 */
	wegt[5] = extract_h( L_shl( L_acc, 1 ) );       /* wegt in Q11 */
	
	/* wegt: Q11 -> normalized */
	tmp = 0;
	for ( i = 0; i < M; i++ ) {
		if ( sub(wegt[i], tmp) > 0 ) {
			tmp = wegt[i];
		}
	}
	
	sft = norm_s(tmp);
	for ( i = 0; i < M; i++ ) {
		wegt[i] = shl(wegt[i], sft);                  /* wegt in Q(11+sft) */
	}
	
	return;
}


void Get_freq_prev(s16 x[MA_NP][M],TG729BENCODER *pG729EncH)
{
	s16 i;
	
	for (i=0; i<MA_NP; i++)
		Copy(&pG729EncH->freq_prev[i][0], &x[i][0], M);
}

void Update_freq_prev(s16 x[MA_NP][M], TG729BENCODER *pG729EncH)
{
	s16 i;
	
	for (i=0; i<MA_NP; i++)
		Copy(&x[i][0], &pG729EncH->freq_prev[i][0], M);
}




