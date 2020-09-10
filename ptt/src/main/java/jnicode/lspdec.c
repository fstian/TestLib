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
#include "basicop.h"
#include "ld8a.h"
#include "tab_ld8a.h"

/* static memory  

static s16 freq_prev[MA_NP][M];   // Q13  

// static memory for frame erase operation  
static s16 prev_ma;                  // previous MA prediction coef. 
static s16 prev_lsp[M];              // previous LSP vector          
*/

/*----------------------------------------------------------------------------
 * Lsp_decw_reset -   set the previous LSP vectors
 *----------------------------------------------------------------------------
 */
void Lsp_decw_reset(TG729BDECODER *pG729DecH)
{
  s16 i;

  for(i=0; i<MA_NP; i++)
    Copy( &freq_prev_reset[0], &pG729DecH->freq_prev[i][0], M );

  pG729DecH->prev_ma = 0;

  Copy( freq_prev_reset, pG729DecH->prev_lsp, M);
}


/*----------------------------------------------------------------------------
 * Lsp_iqua_cs -  LSP main quantization routine
 *----------------------------------------------------------------------------
 */
void Lsp_iqua_cs(
 s16 prm[],          /* (i)     : indexes of the selected LSP */
 s16 lsp_q[],        /* (o) Q13 : Quantized LSP parameters    */
 s16 erase,           /* (i)     : frame erase information     */
 TG729BDECODER *pG729DecH
)
{
	s16 mode_index;
	s16 code0;
	s16 code1;
	s16 code2;
	s16 buf[M];     /* Q13 */
	
	if( erase==0 ) 
	{  /* Not frame erasure */
		mode_index = shr(prm[0] ,NC0_B) & (s16)1;
		code0 = prm[0] & (s16)(NC0 - 1);
		code1 = shr(prm[1] ,NC1_B) & (s16)(NC1 - 1);
		code2 = prm[1] & (s16)(NC1 - 1);
		
		/* compose quantized LSP (lsp_q) from indexes */
		
		Lsp_get_quant(lspcb1, lspcb2, code0, code1, code2,
			fg[mode_index], pG729DecH->freq_prev, lsp_q, fg_sum[mode_index]);
		
		/* save parameters to use in case of the frame erased situation */
		
		Copy(lsp_q, pG729DecH->prev_lsp, M);
		pG729DecH->prev_ma = mode_index;
	}
	else 
	{           /* Frame erased */
		/* use revious LSP */
		
		Copy(pG729DecH->prev_lsp, lsp_q, M);
		
		/* update freq_prev */
		
		Lsp_prev_extract(pG729DecH->prev_lsp, buf,
			fg[pG729DecH->prev_ma], pG729DecH->freq_prev, fg_sum_inv[pG729DecH->prev_ma]);
		Lsp_prev_update(buf, pG729DecH->freq_prev);
	}
	
	return;
}



/*-------------------------------------------------------------------*
 * Function  D_lsp:                                                  *
 *           ~~~~~~                                                  *
 *-------------------------------------------------------------------*/
void D_lsp(
  s16 prm[],          /* (i)     : indexes of the selected LSP */
  s16 lsp_q[],        /* (o) Q15 : Quantized LSP parameters    */
  s16 erase,           /* (i)     : frame erase information     */
  TG729BDECODER *pG729DecH
)
{
  s16 lsf_q[M];       /* domain 0.0<= lsf_q <PI in Q13 */


  Lsp_iqua_cs(prm, lsf_q, erase, pG729DecH);

  /* Convert LSFs to LSPs*/

  Lsf_lsp2(lsf_q, lsp_q, M); 

  return;
}

void Get_decfreq_prev(s16 x[MA_NP][M], TG729BDECODER *pG729DecH)
{
  s16 i;

  for (i=0; i<MA_NP; i++)
    Copy(&pG729DecH->freq_prev[i][0], &x[i][0], M);
}
  
void Update_decfreq_prev(s16 x[MA_NP][M], TG729BDECODER *pG729DecH)
{
  s16 i;


  for (i=0; i<MA_NP; i++)
    Copy(&x[i][0], &pG729DecH->freq_prev[i][0], M);
}


