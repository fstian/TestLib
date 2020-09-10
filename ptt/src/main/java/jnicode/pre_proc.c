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
 * Function Pre_Process()                                                 *
 *                                                                        *
 * Preprocessing of input speech.                                         *
 *   - 2nd order high pass filter with cut off frequency at 140 Hz.       *
 *   - Divide input by two.                                               *
 *-----------------------------------------------------------------------*/
#include "basicop.h"
#include "ld8a.h"
#include "tab_ld8a.h"

/*------------------------------------------------------------------------*
 * 2nd order high pass filter with cut off frequency at 140 Hz.           *
 * Designed with SPPACK efi command -40 dB att, 0.25 ri.                  *
 *                                                                        *
 * Algorithm:                                                             *
 *                                                                        *
 *  y[i] = b[0]*x[i]/2 + b[1]*x[i-1]/2 + b[2]*x[i-2]/2                    *
 *                     + a[1]*y[i-1]   + a[2]*y[i-2];                     *
 *                                                                        *
 *     b[3] = {0.92727435E+00, -0.18544941E+01, 0.92727435E+00};          *
 *     a[3] = {0.10000000E+01, 0.19059465E+01, -0.91140240E+00};          *
 *                                                                        *
 *  Input are divided by two in the filtering process.                    *
 *-----------------------------------------------------------------------*/


/* Static values to be preserved between calls */
/* y[] values is keep in double precision      */

//static s16 y2_hi, y2_lo, y1_hi, y1_lo, x0, x1;


/* Initialization of static values */

void Init_Pre_Process(TG729BENCODER *pG729EncH)
{
	pG729EncH->y2_hi = 0;
	pG729EncH->y2_lo = 0;
	pG729EncH->y1_hi = 0;
	pG729EncH->y1_lo = 0;
	pG729EncH->x0   = 0;
	pG729EncH->x1   = 0;
}


void Pre_Process(
				 s16 signal[],    /* input/output signal */
				 s16 lg,          /* length of signal    */
				 TG729BENCODER *pG729EncH
				 )
{
	s16 i, x2;
	l32 L_tmp;
	
	for(i=0; i<lg; i++)
	{
		x2 = pG729EncH->x1;
		pG729EncH->x1 = pG729EncH->x0;
		pG729EncH->x0 = signal[i];
		
		/*  y[i] = b[0]*x[i]/2 + b[1]*x[i-1]/2 + b140[2]*x[i-2]/2  */
		/*                     + a[1]*y[i-1] + a[2] * y[i-2];      */
		
		L_tmp     = Mpy_32_16(pG729EncH->y1_hi, pG729EncH->y1_lo, a140[1]);
		L_tmp     = L_add(L_tmp, Mpy_32_16(pG729EncH->y2_hi, pG729EncH->y2_lo, a140[2]));
		L_tmp     = L_mac(L_tmp, pG729EncH->x0, b140[0]);
		L_tmp     = L_mac(L_tmp, pG729EncH->x1, b140[1]);
		L_tmp     = L_mac(L_tmp, x2, b140[2]);
		L_tmp     = L_shl(L_tmp, 3);      /* Q28 --> Q31 (Q12 --> Q15) */
		signal[i] = round_linux(L_tmp);
		
		pG729EncH->y2_hi = pG729EncH->y1_hi;
		pG729EncH->y2_lo = pG729EncH->y1_lo;
		L_Extract(L_tmp, &pG729EncH->y1_hi, &pG729EncH->y1_lo);
	}
	return;
}
