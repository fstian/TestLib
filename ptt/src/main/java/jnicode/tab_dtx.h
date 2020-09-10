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
#ifndef TAB_DTX_H
#define TAB_DTX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "algorithmtype_xc.h"
#include "g729ab.h"

/* VAD constants */
extern s16 lbf_corr[NP+1];
extern s16 shift_fx[33];
extern s16 factor_fx[33];

/* SID LSF quantization */
//extern s16 noise_fg[MODE][MA_NP][M];
extern s16 noise_fg_sum[MODE][M];
extern s16 noise_fg_sum_inv[MODE][M];
extern s16 PtrTab_1[32];
extern s16 PtrTab_2[2][16];
extern s16 Mp[MODE];

/* SID gain quantization */
extern s16 fact[NB_GAIN+1];
extern s16 marg[NB_GAIN+1];
extern s16 tab_Sidgain[32];

#ifdef __cplusplus
}
#endif

#endif //end of TAB_DTX_H




