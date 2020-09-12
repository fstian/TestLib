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
/*-----------------------------------------------------------------*
 *   Functions Init_Decod_ld8a  and Decod_ld8a                     *
 *-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "basicop.h"
#include "ld8a.h"
#include "dtx.h"
#include "sid.h"

/*---------------------------------------------------------------*
 *   Decoder constant parameters (defined in "ld8a.h")           *
 *---------------------------------------------------------------*
 *   L_FRAME     : Frame size.                                   *
 *   L_SUBFR     : Sub-frame size.                               *
 *   M           : LPC order.                                    *
 *   MP1         : LPC order+1                                   *
 *   PIT_MIN     : Minimum pitch lag.                            *
 *   PIT_MAX     : Maximum pitch lag.                            *
 *   L_INTERPOL  : Length of filter for interpolation            *
 *   PRM_SIZE    : Size of vector containing analysis parameters *
 *---------------------------------------------------------------*/
#if  0
/*--------------------------------------------------------*
 *         Static memory allocation.                      *
 *--------------------------------------------------------*/

        /* Excitation vector */

 static s16 old_exc[L_FRAME+PIT_MAX+L_INTERPOL];
 static s16 *exc;

        /* Lsp (Line spectral pairs) */

 static s16 lsp_old[M]={
             30000, 26000, 21000, 15000, 8000, 0, -8000,-15000,-21000,-26000};

        /* Filter's memory */

 static s16 mem_syn[M];

 static s16 sharp;           /* pitch sharpening of previous frame */
 static s16 old_T0;          /* integer delay of previous frame    */
 static s16 gain_code;       /* Code gain                          */
 static s16 gain_pitch;      /* Pitch gain                         */




/* for G.729B */
static s16 seed_fer;
/* CNG variables */
static s16 past_ftyp;
static s16 seed;
static s16 sid_sav, sh_sid_sav;

#endif 

/*-----------------------------------------------------------------*
 *   Function Init_Decod_ld8a                                      *
 *            ~~~~~~~~~~~~~~~                                      *
 *                                                                 *
 *   ->Initialization of variables for the decoder section.        *
 *                                                                 *
 *-----------------------------------------------------------------*/

void Init_Decod_ld8a(TG729BDECODER *pG729DecH)
{
//   pG729DecH->
  /* Initialize static pointer */

  pG729DecH->exc = pG729DecH->old_exc + PIT_MAX + L_INTERPOL;

  /* Static vectors to zero */

  Set_zero(pG729DecH->old_exc, PIT_MAX+L_INTERPOL);
  Set_zero(pG729DecH->mem_syn, M);

  pG729DecH->sharp  = SHARPMIN;
  pG729DecH->old_T0 = 60;
  pG729DecH->gain_code = 0;
  pG729DecH->gain_pitch = 0;

  Lsp_decw_reset(pG729DecH);

  /* for G.729B*/ 
  pG729DecH->seed_fer = 21845;
  pG729DecH->past_ftyp = 1;
  pG729DecH->seed = INIT_SEED;
  pG729DecH->sid_sav = 0;
  pG729DecH->sh_sid_sav = 1;
  //Init_lsfq_noise();
  Init_dec_lsfq_noise(pG729DecH);

  return;
}

/*-----------------------------------------------------------------*
 *   Function Decod_ld8a                                           *
 *           ~~~~~~~~~~                                            *
 *   ->Main decoder routine.                                       *
 *                                                                 *
 *-----------------------------------------------------------------*/


void Decod_ld8a(
  s16  parm[],      /* (i)   : vector of synthesis parameters
                                  parm[0] = bad frame indicator (bfi)  */
  s16  synth[],     /* (o)   : synthesis speech                     */
  s16  A_t[],       /* (o)   : decoded LP filter in 2 subframes     */
  s16  *T2,         /* (o)   : decoded pitch lag in 2 subframes     */
  s16  *Vad,         /* (o)   : VAD                                  */
  TG729BDECODER *pG729DecH
)
{
  s16  *Az;                  /* Pointer on A_t   */
  s16  lsp_new[M];           /* LSPs             */
  s16  code[L_SUBFR];        /* ACELP codevector */

  /* Scalars */

  s16  i, j, i_subfr;
  s16  T0, T0_frac, index;
  s16  bfi;
  l32  L_temp;

  s16 bad_pitch;             /* bad pitch indicator */
  extern s16 bad_lsf;        /* bad LSF indicator   */

  /* for G.729B */
  s16 ftyp;
  s16 lsfq_mem[MA_NP][M];

  /* Test bad frame indicator (bfi) */

  bfi = *parm++;
  /* for G.729B */
  ftyp = *parm;

  if(bfi == 1) {
    if(pG729DecH->past_ftyp == 1) ftyp = 1;
    else ftyp = 0;
    *parm = ftyp;  /* modification introduced in version V1.3 */
  }
 
  *Vad = ftyp;

  /* Processing non active frames (SID & not transmitted) */
  if(ftyp != 1) 
  {
    
    Get_decfreq_prev(lsfq_mem, pG729DecH);
    Dec_cng(pG729DecH->past_ftyp, pG729DecH->sid_sav, \
		pG729DecH->sh_sid_sav, parm, pG729DecH->exc, \
		pG729DecH->lsp_old, A_t, &pG729DecH->seed, lsfq_mem, pG729DecH);
    Update_decfreq_prev(lsfq_mem, pG729DecH);

    Az = A_t;
    for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR) 
	{
      Overflow = 0;
      Syn_filt(Az, &pG729DecH->exc[i_subfr], &synth[i_subfr], L_SUBFR, pG729DecH->mem_syn, 0);
      if(Overflow != 0) {
        /* In case of overflow in the synthesis          */
        /* -> Scale down vector exc[] and redo synthesis */
        
        for(i=0; i<PIT_MAX+L_INTERPOL+L_FRAME; i++)
          pG729DecH->old_exc[i] = shr(pG729DecH->old_exc[i], 2);
        
        Syn_filt(Az, &pG729DecH->exc[i_subfr], &synth[i_subfr], L_SUBFR, pG729DecH->mem_syn, 1);
      }
      else
        Copy(&synth[i_subfr+L_SUBFR-M], pG729DecH->mem_syn, M);
      
      Az += MP1;

      *T2++ = pG729DecH->old_T0;
    }
    pG729DecH->sharp = SHARPMIN;
    
  }
  /* Processing active frame */
  else 
  {
    
    pG729DecH->seed = INIT_SEED;
    parm++;

   /* Decode the LSPs */
    
    D_lsp(parm, lsp_new, bfi, pG729DecH); //add(bfi, bad_lsf)
    parm += 2;
    
    /*
       Note: "bad_lsf" is introduce in case the standard is used with
       channel protection.
       */
    
    /* Interpolation of LPC for the 2 subframes */
    
    Int_qlpc(pG729DecH->lsp_old, lsp_new, A_t);
   
    /* update the LSFs for the next frame */
    
    Copy(lsp_new, pG729DecH->lsp_old, M);
    
    /*------------------------------------------------------------------------*
     *          Loop for every subframe in the analysis frame                 *
     *------------------------------------------------------------------------*
     * The subframe size is L_SUBFR and the loop is repeated L_FRAME/L_SUBFR  *
     *  times                                                                 *
     *     - decode the pitch delay                                           *
     *     - decode algebraic code                                            *
     *     - decode pitch and codebook gains                                  *
     *     - find the excitation and compute synthesis speech                 *
     *------------------------------------------------------------------------*/
    
    Az = A_t;            /* pointer to interpolated LPC parameters */
    
    for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
      {

        index = *parm++;        /* pitch index */

        if(i_subfr == 0)
          {
            i = *parm++;        /* get parity check result */
            bad_pitch = add(bfi, i);
            if( bad_pitch == 0)
              {
                Dec_lag3(index, PIT_MIN, PIT_MAX, i_subfr, &T0, &T0_frac);
                pG729DecH->old_T0 = T0;
              }
            else                /* Bad frame, or parity error */
              {
                T0  =  pG729DecH->old_T0;
                T0_frac = 0;
                pG729DecH->old_T0 = add( pG729DecH->old_T0, 1);
                if( sub(pG729DecH->old_T0, PIT_MAX) > 0) {
                  pG729DecH->old_T0 = PIT_MAX;
                }
              }
          }
        else                    /* second subframe */
          {
            if( bfi == 0)
              {
                Dec_lag3(index, PIT_MIN, PIT_MAX, i_subfr, &T0, &T0_frac);
                pG729DecH->old_T0 = T0;
              }
            else
              {
                T0  =  pG729DecH->old_T0;
                T0_frac = 0;
                pG729DecH->old_T0 = add( pG729DecH->old_T0, 1);
                if( sub(pG729DecH->old_T0, PIT_MAX) > 0) {
                  pG729DecH->old_T0 = PIT_MAX;
                }
              }
          }


        *T2++ = T0;

        /*-------------------------------------------------*
         * - Find the adaptive codebook vector.            *
         *-------------------------------------------------*/

        Pred_lt_3(&pG729DecH->exc[i_subfr], T0, T0_frac, L_SUBFR);

        /*-------------------------------------------------------*
         * - Decode innovative codebook.                         *
         * - Add the fixed-gain pitch contribution to code[].    *
         *-------------------------------------------------------*/

        if(bfi != 0)            /* Bad frame */
          {

            parm[0] = Random(&pG729DecH->seed_fer) & (s16)0x1fff; /* 13 bits random */
            parm[1] = Random(&pG729DecH->seed_fer) & (s16)0x000f; /*  4 bits random */
          }

        Decod_ACELP(parm[1], parm[0], code);

        parm +=2;

        j = shl(pG729DecH->sharp, 1);      /* From Q14 to Q15 */
        if(sub(T0, L_SUBFR) <0 ) {
          for (i = T0; i < L_SUBFR; i++) {
            code[i] = add(code[i], mult(code[i-T0], j));
          }
        }

        /*-------------------------------------------------*
         * - Decode pitch and codebook gains.              *
         *-------------------------------------------------*/

        index = *parm++;        /* index of energy VQ */

        Dec_gain(index, code, L_SUBFR, bfi, &pG729DecH->gain_pitch, &pG729DecH->gain_code, pG729DecH);

        /*-------------------------------------------------------------*
         * - Update pitch sharpening "sharp" with quantized gain_pitch *
         *-------------------------------------------------------------*/

        pG729DecH->sharp = pG729DecH->gain_pitch;
        if (sub(pG729DecH->sharp, SHARPMAX) > 0) { pG729DecH->sharp = SHARPMAX;  }
        if (sub(pG729DecH->sharp, SHARPMIN) < 0) { pG729DecH->sharp = SHARPMIN;  }

        /*-------------------------------------------------------*
         * - Find the total excitation.                          *
         * - Find synthesis speech corresponding to exc[].       *
         *-------------------------------------------------------*/

        for (i = 0; i < L_SUBFR;  i++)
          {
            /* exc[i] = gain_pitch*exc[i] + gain_code*code[i]; */
            /* exc[i]  in Q0   gain_pitch in Q14               */
            /* code[i] in Q13  gain_codeode in Q1              */
            
            L_temp = L_mult(pG729DecH->exc[i+i_subfr], pG729DecH->gain_pitch);
            L_temp = L_mac(L_temp, code[i], pG729DecH->gain_code);
            L_temp = L_shl(L_temp, 1);
            pG729DecH->exc[i+i_subfr] = round_linux(L_temp);
          }
        
        Overflow = 0;
        Syn_filt(Az, &pG729DecH->exc[i_subfr], &synth[i_subfr], L_SUBFR, pG729DecH->mem_syn, 0);
        if(Overflow != 0)
          {
            /* In case of overflow in the synthesis          */
            /* -> Scale down vector exc[] and redo synthesis */

            for(i=0; i<PIT_MAX+L_INTERPOL+L_FRAME; i++)
              pG729DecH->old_exc[i] = shr(pG729DecH->old_exc[i], 2);

            Syn_filt(Az, &pG729DecH->exc[i_subfr], &synth[i_subfr], L_SUBFR, pG729DecH->mem_syn, 1);
          }
        else
          Copy(&synth[i_subfr+L_SUBFR-M], pG729DecH->mem_syn, M);

        Az += MP1;              /* interpolated LPC parameters for next subframe */
      }
  }


  /*------------*
   *  For G729b
   *-----------*/
  if(bfi == 0) 
  {
    L_temp = 0L;
    for(i=0; i<L_FRAME; i++) 
	{
      L_temp = L_mac(L_temp, pG729DecH->exc[i], pG729DecH->exc[i]);
    } /* may overflow => last level of SID quantizer */

    pG729DecH->sh_sid_sav = norm_l(L_temp);
    pG729DecH->sid_sav = round_linux(L_shl(L_temp, pG729DecH->sh_sid_sav));
    pG729DecH->sh_sid_sav = sub(16, pG729DecH->sh_sid_sav);
  }

 /*--------------------------------------------------*
  * Update signal for next frame.                    *
  * -> shift to the left by L_FRAME  exc[]           *
  *--------------------------------------------------*/

  Copy(&pG729DecH->old_exc[L_FRAME], &pG729DecH->old_exc[0], PIT_MAX+L_INTERPOL);

  /* for G729b */
  pG729DecH->past_ftyp = ftyp;

  return;
}







