/*****************************************************************************
ģ����      : g729ab�������
�ļ���      : 
����ļ�    : 
�ļ�ʵ�ֹ���: g729ab����빦�ܾ��ʽ�ӿ�ʵ��
�汾        : V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
����        : �쳬
-----------------------------------------------------------------------------
�޸ļ�¼:
��    ��      �汾        �޸���      �޸�����
2006/02/16    1.0         ��  ��      ��    ��
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "basicop.h"
#include "ld8a.h"
#include "tab_ld8a.h"

/*====================================================================
��   ��   ��:  Init_enc_lsfq_noise
��        ��:  Initialization of variables for the lsf quantization in the SID
����ȫ�ֱ���:  ��
�������˵��:
			 1-(i)pG729EncH     

���� ֵ ˵��:  ��
�� �� ˵  ��:  ��
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
