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
/*------------------------------------------------------*
 * Parity_pitch - compute parity bit for first 6 MSBs   *
 *------------------------------------------------------*/
#include "basicop.h"
#include "ld8a.h"

s16 Parity_Pitch(    /* output: parity bit (XOR of 6 MSB bits)    */
   s16 pitch_index   /* input : index for which parity to compute */
)
{
  s16 temp, sum, i, bit;

  temp = shr(pitch_index, 1);

  sum = 1;
  for (i = 0; i <= 5; i++) {
    temp = shr(temp, 1);
    bit = temp & (s16)1;
    sum = add(sum, bit);
  }
  sum = sum & (s16)1;


  return sum;
}

/*--------------------------------------------------------------------*
 * check_parity_pitch - check parity of index with transmitted parity *
 *--------------------------------------------------------------------*/

s16  Check_Parity_Pitch( /* output: 0 = no error, 1= error */
  s16 pitch_index,       /* input : index of parameter     */
  s16 parity             /* input : parity bit             */
)
{
  s16 temp, sum, i, bit;

  temp = shr(pitch_index, 1);

  sum = 1;
  for (i = 0; i <= 5; i++) {
    temp = shr(temp, 1);
    bit = temp & (s16)1;
    sum = add(sum, bit);
  }
  sum = add(sum, parity);
  sum = sum & (s16)1;

  return sum;
}
