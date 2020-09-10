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
