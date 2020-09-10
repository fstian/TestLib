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
/**************************************************************************
 * Taming functions.                                                      *
 **************************************************************************/
#include "basicop.h"
#include "ld8a.h"
#include "tab_ld8a.h"

//static l32 pG729EncH->L_exc_err[4];

void Init_exc_err(TG729BENCODER *pG729EncH)
{
  s16 i;
  for(i=0; i<4; i++) pG729EncH->L_exc_err[i] = 0x00004000L;   /* Q14 */
}
/**************************************************************************
 * routine test_err - computes the accumulated potential error in the     *
 * adaptive codebook contribution                                         *
 **************************************************************************/

s16 test_err(  /* (o) flag set to 1 if taming is necessary  */
 s16 T0,       /* (i) integer part of pitch delay           */
 s16 T0_frac,   /* (i) fractional part of pitch delay        */
 TG729BENCODER *pG729EncH
)
 {
    s16 i, t1, zone1, zone2, flag;
    l32 L_maxloc, L_acc;

    if(T0_frac > 0) {
        t1 = add(T0, 1);
    }
    else {
        t1 = T0;
    }

    i = sub(t1, (L_SUBFR+L_INTER10));
    if(i < 0) {
        i = 0;
    }
    zone1 = tab_zone[i];

    i = add(t1, (L_INTER10 - 2));
    zone2 = tab_zone[i];

    L_maxloc = -1L;
    flag = 0 ;
    for(i=zone2; i>=zone1; i--) {
        L_acc = L_sub(pG729EncH->L_exc_err[i], L_maxloc);
        if(L_acc > 0L) {
                L_maxloc = pG729EncH->L_exc_err[i];
        }
    }
    L_acc = L_sub(L_maxloc, L_THRESH_ERR);
    if(L_acc > 0L) {
        flag = 1;
    }

    return(flag);
}

/**************************************************************************
 *routine update_exc_err - maintains the memory used to compute the error *
 * function due to an adaptive codebook mismatch between encoder and      *
 * decoder                                                                *
 **************************************************************************/

void update_exc_err(
 s16 gain_pit,      /* (i) pitch gain */
 s16 T0,             /* (i) integer part of pitch delay */
 TG729BDECODER *pG729DecH
)
 {

    s16 i, zone1, zone2, n;
    l32 L_worst, L_temp, L_acc;
    s16 hi, lo;

    L_worst = -1L;
    n = sub(T0, L_SUBFR);

    if(n < 0) {
        L_Extract(pG729DecH->L_exc_err[0], &hi, &lo);
        L_temp = Mpy_32_16(hi, lo, gain_pit);
        L_temp = L_shl(L_temp, 1);
        L_temp = L_add(0x00004000L, L_temp);
        L_acc = L_sub(L_temp, L_worst);
        if(L_acc > 0L) {
                L_worst = L_temp;
        }
        L_Extract(L_temp, &hi, &lo);
        L_temp = Mpy_32_16(hi, lo, gain_pit);
        L_temp = L_shl(L_temp, 1);
        L_temp = L_add(0x00004000L, L_temp);
        L_acc = L_sub(L_temp, L_worst);
        if(L_acc > 0L) {
                L_worst = L_temp;
        }
    }

    else {

        zone1 = tab_zone[n];

        i = sub(T0, 1);
        zone2 = tab_zone[i];

        for(i = zone1; i <= zone2; i++) {
                L_Extract(pG729DecH->L_exc_err[i], &hi, &lo);
                L_temp = Mpy_32_16(hi, lo, gain_pit);
                L_temp = L_shl(L_temp, 1);
                L_temp = L_add(0x00004000L, L_temp);
                L_acc = L_sub(L_temp, L_worst);
                if(L_acc > 0L) L_worst = L_temp;
        }
    }

    for(i=3; i>=1; i--) {
        pG729DecH->L_exc_err[i] = pG729DecH->L_exc_err[i-1];
    }
    pG729DecH->L_exc_err[0] = L_worst;

    return;
}


void enc_update_exc_err(
 s16 gain_pit,      /* (i) pitch gain */
 s16 T0,             /* (i) integer part of pitch delay */
 TG729BENCODER *pG729EncH
)
 {

    s16 i, zone1, zone2, n;
    l32 L_worst, L_temp, L_acc;
    s16 hi, lo;

    L_worst = -1L;
    n = sub(T0, L_SUBFR);

    if(n < 0) {
        L_Extract(pG729EncH->L_exc_err[0], &hi, &lo);
        L_temp = Mpy_32_16(hi, lo, gain_pit);
        L_temp = L_shl(L_temp, 1);
        L_temp = L_add(0x00004000L, L_temp);
        L_acc = L_sub(L_temp, L_worst);
        if(L_acc > 0L) {
                L_worst = L_temp;
        }
        L_Extract(L_temp, &hi, &lo);
        L_temp = Mpy_32_16(hi, lo, gain_pit);
        L_temp = L_shl(L_temp, 1);
        L_temp = L_add(0x00004000L, L_temp);
        L_acc = L_sub(L_temp, L_worst);
        if(L_acc > 0L) {
                L_worst = L_temp;
        }
    }

    else {

        zone1 = tab_zone[n];

        i = sub(T0, 1);
        zone2 = tab_zone[i];

        for(i = zone1; i <= zone2; i++) {
                L_Extract(pG729EncH->L_exc_err[i], &hi, &lo);
                L_temp = Mpy_32_16(hi, lo, gain_pit);
                L_temp = L_shl(L_temp, 1);
                L_temp = L_add(0x00004000L, L_temp);
                L_acc = L_sub(L_temp, L_worst);
                if(L_acc > 0L) L_worst = L_temp;
        }
    }

    for(i=3; i>=1; i--) {
        pG729EncH->L_exc_err[i] = pG729EncH->L_exc_err[i-1];
    }
    pG729EncH->L_exc_err[0] = L_worst;

    return;
}
