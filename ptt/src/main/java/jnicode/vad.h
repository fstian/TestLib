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

#ifndef VAD_H
#define VAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "g729ab.h"

void vad_init(TG729BENCODER *pG729EncH);

void vad(s16 rc,
         s16 *lsf, 
         s16 *r_h,
         s16 *r_l,
         s16 exp_R0,
         s16 *sigpp,
         s16 frm_count,
         s16 prev_marker,
         s16 pprev_marker,
         s16 *marker,
		 TG729BENCODER *pG729EncH);

#ifdef __cplusplus
}
#endif

#endif //end of VAD_H


