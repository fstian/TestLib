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
/*-----------------------------------------------------------*
 *  Function  Decod_ACELP()                                  *
 *  ~~~~~~~~~~~~~~~~~~~~~~~                                  *
 *   Algebraic codebook decoder.                             *
 *----------------------------------------------------------*/
#include "basicop.h"
#include "ld8a.h"

void Decod_ACELP(
  s16 sign,      /* (i)     : signs of 4 pulses.                       */
  s16 index,     /* (i)     : Positions of the 4 pulses.               */
  s16 cod[]      /* (o) Q13 : algebraic (fixed) codebook excitation    */
)
{
  s16 i, j;
  s16 pos[4];


  /* Decode the positions */

  i      = index & (s16)7;
  pos[0] = add(i, shl(i, 2));           /* pos0 =i*5 */

  index  = shr(index, 3);
  i      = index & (s16)7;
  i      = add(i, shl(i, 2));           /* pos1 =i*5+1 */
  pos[1] = add(i, 1);

  index  = shr(index, 3);
  i      = index & (s16)7;
  i      = add(i, shl(i, 2));           /* pos2 =i*5+1 */
  pos[2] = add(i, 2);

  index  = shr(index, 3);
  j      = index & (s16)1;
  index  = shr(index, 1);
  i      = index & (s16)7;
  i      = add(i, shl(i, 2));           /* pos3 =i*5+3+j */
  i      = add(i, 3);
  pos[3] = add(i, j);

  /* decode the signs  and build the codeword */

  for (i=0; i<L_SUBFR; i++) {
    cod[i] = 0;
  }

  for (j=0; j<4; j++)
  {

    i = sign & (s16)1;
    sign = shr(sign, 1);

    if (i != 0) {
      cod[pos[j]] = 8191;      /* Q13 +1.0 */
    }
    else {
      cod[pos[j]] = -8192;     /* Q13 -1.0 */
    }
  }

  return;
}
