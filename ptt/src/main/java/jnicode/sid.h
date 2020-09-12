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

#ifndef SID_H
#define SID_H
#ifdef __cplusplus
extern "C" {
#endif

#define         TRUE 1
#define         FALSE 0
#define         sqr(a)  ((a)*(a))
#define         R_LSFQ 10

#include "g729ab.h"

void Init_dec_lsfq_noise(TG729BDECODER *pG729DecH);

void lsfq_noise(s16 *lsp_new, s16 *lspq,
                s16 freq_prev[MA_NP][M], s16 *idx, TG729BENCODER *pG729EncH);
void sid_lsfq_decode(s16 *index, s16 *lspq, 
                     s16 freq_prev[MA_NP][M],
					 TG729BDECODER *pG729DecH); 

#ifdef __cplusplus
}
#endif

#endif  //end of SID_H
