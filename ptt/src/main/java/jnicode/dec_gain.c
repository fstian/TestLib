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
#include "basicop.h"
#include "ld8a.h"
#include "tab_ld8a.h"

/*---------------------------------------------------------------------------*
 * Function  Dec_gain                                                        *
 * ~~~~~~~~~~~~~~~~~~                                                        *
 * Decode the pitch and codebook gains                                       *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * input arguments:                                                          *
 *                                                                           *
 *   index      :Quantization index                                          *
 *   code[]     :Innovative code vector                                      *
 *   L_subfr    :Subframe size                                               *
 *   bfi        :Bad frame indicator                                         *
 *                                                                           *
 * output arguments:                                                         *
 *                                                                           *
 *   gain_pit   :Quantized pitch gain                                        *
 *   gain_cod   :Quantized codebook gain                                     *
 *                                                                           *
 *---------------------------------------------------------------------------*/
void Dec_gain(
  s16 index,     /* (i)     : Index of quantization.                     */
  s16 code[],    /* (i) Q13 : Innovative vector.                         */
  s16 L_subfr,   /* (i)     : Subframe length.                           */
  s16 bfi,       /* (i)     : Bad frame indicator                        */
  s16 *gain_pit, /* (o) Q14 : Pitch gain.                                */
  s16 *gain_cod,  /* (o) Q1  : Code gain.                                 */
  TG729BDECODER *pG729DecH
)
{
   s16  index1, index2, tmp;
   s16  gcode0, exp_gcode0;
   l32  L_gbk12, L_acc, L_accb;
   void    Gain_predict( s16 past_qua_en[], s16 code[], s16 L_subfr,
                        s16 *gcode0, s16 *exp_gcode0 );
   void    Gain_update( s16 past_qua_en[], l32 L_gbk12 );
   void    Gain_update_erasure( s16 past_qua_en[] );

        /* Gain predictor, Past quantized energies = -14.0 in Q10 */

   //static s16 past_qua_en[4] = { -14336, -14336, -14336, -14336 };


   /*-------------- Case of erasure. ---------------*/

   if(bfi != 0){
      *gain_pit = mult( *gain_pit, 29491 );      /* *0.9 in Q15 */
      if (sub( *gain_pit, 29491) > 0) *gain_pit = 29491;
      *gain_cod = mult( *gain_cod, 32111 );      /* *0.98 in Q15 */

     /*----------------------------------------------*
      * update table of past quantized energies      *
      *                              (frame erasure) *
      *----------------------------------------------*/
      Gain_update_erasure(pG729DecH->past_qua_en);

      return;
   }

   /*-------------- Decode pitch gain ---------------*/

   index1 = imap1[ shr(index,NCODE2_B) ] ;
   index2 = imap2[ index & (NCODE2-1) ] ;
   *gain_pit = add( gbk1[index1][0], gbk2[index2][0] );

   /*-------------- Decode codebook gain ---------------*/

  /*---------------------------------------------------*
   *-  energy due to innovation                       -*
   *-  predicted energy                               -*
   *-  predicted codebook gain => gcode0[exp_gcode0]  -*
   *---------------------------------------------------*/

   Gain_predict( pG729DecH->past_qua_en, code, L_subfr, &gcode0, &exp_gcode0 );

  /*-----------------------------------------------------------------*
   * *gain_code = (gbk1[indice1][1]+gbk2[indice2][1]) * gcode0;      *
   *-----------------------------------------------------------------*/

   L_acc = L_deposit_l( gbk1[index1][1] );
   L_accb = L_deposit_l( gbk2[index2][1] );
   L_gbk12 = L_add( L_acc, L_accb );                       /* Q13 */
   tmp = extract_l( L_shr( L_gbk12,1 ) );                  /* Q12 */
   L_acc = L_mult(tmp, gcode0);             /* Q[exp_gcode0+12+1] */

   L_acc = L_shl(L_acc, add( negate(exp_gcode0),(-12-1+1+16) ));
   *gain_cod = extract_h( L_acc );                          /* Q1 */

  /*----------------------------------------------*
   * update table of past quantized energies      *
   *----------------------------------------------*/
   Gain_update( pG729DecH->past_qua_en, L_gbk12 );

   return;

}
