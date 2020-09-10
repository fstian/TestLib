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
#include <stdlib.h>

#include "basicop.h"
#include "ld8a.h"
#include "tab_ld8a.h"

/*====================================================================
函   数   名:  Init_enc_lsfq_noise
功        能:  Initialization of variables for the lsf quantization in the SID
引用全局变量:  无
输入参数说明:
			 1-(i)pG729EncH     

返回 值 说明:  无
特 殊 说  明:  无
======================================================================*/
/*
void Init_enc_lsfq_noise( 
						 TG729BENCODER  *pG729EncH
						 )
{
	s16 i, j;
	l32 acc0;
	
	//initialize the noise_fg 
	for (i=0; i<4; i++)
	{
		Copy(fg[0][i], pG729EncH->noise_fg[0][i], M);
	}
	
	for (i=0; i<4; i++)
	{
		for (j=0; j<M; j++)
		{
			acc0 = L_mult(fg[0][i][j], 19660);
			acc0 = L_mac(acc0, fg[1][i][j], 13107);
			pG729EncH->noise_fg[1][i][j] = extract_h(acc0);
		}
	}
}
*/
void Init_enc_lsfq_noise(TG729BENCODER  *pG729EncH)
{
  s16 i, j;
  l32 acc0;

  /* initialize the noise_fg */
  for (i=0; i<4; i++)
    Copy(fg[0][i], pG729EncH->noise_fg[0][i], M);
  
  for (i=0; i<4; i++)
    for (j=0; j<M; j++){
      acc0 = L_mult(fg[0][i][j], 19660);
      acc0 = L_mac(acc0, fg[1][i][j], 13107);
      pG729EncH->noise_fg[1][i][j] = extract_h(acc0);
    }
}
