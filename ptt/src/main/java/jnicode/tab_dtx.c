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
/*********************************************************************/
/******             Tables used for VAD/DTX/CNG                 ******/
/*********************************************************************/
#include <stdio.h>

#include "ld8a.h"
#include "vad.h"
#include "dtx.h"
#include "tab_dtx.h"

/* VAD constants */
s16 lbf_corr[NP+1] = {
  7869, 7011, 4838, 2299, 321, -660, -782, -484, -164, 3, 39, 21, 4};
s16 shift_fx[33] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                         2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 5,0};
s16 factor_fx[33] = {32767, 16913, 17476, 18079, 18725, 19418,
                          20165, 20972, 21845, 22795, 23831, 24966,
                          26214, 27594, 29127, 30840, 32767, 17476,
                          18725, 20165, 21845, 23831, 26214, 29127,
                          32767, 18725, 21845, 26214, 32767, 21845,
                          32767, 32767,0};

/* Quantization of SID gain */
s16 fact[NB_GAIN+1] = {410, 26, 13};
s16 marg[NB_GAIN+1] = {0, 0, 1};
s16 tab_Sidgain[32] = {
    2,    5,    8,   13,   20,   32,   50,   64,
   80,  101,  127,  160,  201,  253,  318,  401,
  505,  635,  800, 1007, 1268, 1596, 2010, 2530,
 3185, 4009, 5048, 6355, 8000,10071,12679,15962 };

/* Quantization of LSF vector */
//s16 noise_fg[MODE][MA_NP][M];

s16 noise_fg_sum[MODE][M] = {
  {7798, 8447, 8205, 8293, 8126, 8477, 8447, 8703, 9043, 8604},
  {10514, 12402, 12833, 11914, 11447, 11670, 11132, 11311, 11844, 11447}
};

s16 noise_fg_sum_inv[MODE][M] = {
  {17210, 15888, 16357, 16183, 16516, 15833, 15888, 15421, 14840, 15597},
  {12764, 10821, 10458, 11264, 11724, 11500, 12056, 11865, 11331, 11724}
};

s16 PtrTab_1[32] = {96,52,20,54,86,114,82,68,36,121,48,92,18,120,
                         94,124,50,125,4,100,28,76,12,117,81,22,90,116,
                         127,21,108,66};
s16 PtrTab_2[2][16]= {{31,21,9,3,10,2,19,26,4,3,11,29,15,27,21,12},
                           {16,1,0,0,8,25,22,20,19,23,20,31,4,31,20,31}};

s16 Mp[MODE] = {8644, 16572};











