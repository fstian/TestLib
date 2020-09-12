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
