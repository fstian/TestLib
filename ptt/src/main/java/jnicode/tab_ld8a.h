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

#ifndef TAB_LD8A_H
#define TAB_LD8A_H

#ifdef __cplusplus
extern "C" {
#endif

extern s16 hamwindow[L_WINDOW];
extern s16 lag_h[M+2];
extern s16 lag_l[M+2];
extern s16 table[65];
extern s16 slope[64];
extern s16 table2[64];
extern s16 slope_cos[64];
extern s16 slope_acos[64];
extern s16 lspcb1[NC0][M];
extern s16 lspcb2[NC1][M];
extern s16 fg[2][MA_NP][M];
extern s16 fg_sum[2][M];
extern s16 fg_sum_inv[2][M];
extern s16 grid[GRID_POINTS+1];
extern s16 freq_prev_reset[M];
extern s16 inter_3l[FIR_SIZE_SYN];
extern s16 pred[4];
extern s16 gbk1[NCODE1][2];
extern s16 gbk2[NCODE2][2];
extern s16 map1[NCODE1];
extern s16 map2[NCODE2];
extern s16 coef[2][2];
extern l32 L_coef[2][2];
extern s16 thr1[NCODE1-NCAN1];
extern s16 thr2[NCODE2-NCAN2];
extern s16 imap1[NCODE1];
extern s16 imap2[NCODE2];
extern s16 b100[3];
extern s16 a100[3];
extern s16 b140[3];
extern s16 a140[3];
extern s16 bitsno[PRM_SIZE];
extern s16 bitsno2[4]; 
extern s16 tabpow[33];
extern s16 tablog[33];
extern s16 tabsqr[49];
extern s16 tab_zone[PIT_MAX+L_INTERPOL-1];

#ifdef __cplusplus
}
#endif

#endif  //end of TAB_LD8A_H




