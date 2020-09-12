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




