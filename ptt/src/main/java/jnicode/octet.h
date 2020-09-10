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

/* Definition for Octet Transmission mode */
/* When Annex B is used for transmission systems that operate on octet boundary, 
   an extra bit (with value zero) will be packed at the end of a SID bit stream. 
   This will change the number of bits in a SID bit stream from 15 bits to
   16 bits (i.e., 2 bytes).
*/
#ifndef OCTET_H
#define OCTET_H

#ifdef __cplusplus
extern "C" {
#endif

#define OCTET_TX_MODE
#define RATE_SID_OCTET    16     /* number of bits in Octet Transmission mode */

#ifdef __cplusplus
}
#endif

#endif //end of OCTET_H

