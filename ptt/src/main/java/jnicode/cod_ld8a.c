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
 *   Functions Coder_ld8a and Init_Coder_ld8a                      *
 *             ~~~~~~~~~~     ~~~~~~~~~~~~~~~                      *
 *                                                                 *
 *  Init_Coder_ld8a(void);                                         *
 *                                                                 *
 *   ->Initialization of variables for the coder section.          *
 *                                                                 *
 *                                                                 *
 *  Coder_ld8a(s16 ana[]);                                      *
 *                                                                 *
 *   ->Main coder function.                                        *
 *                                                                 *
 *                                                                 *
 *  Input:                                                         *
 *                                                                 *
 *    80 pG729EncH->speech data should have beee copy to vector pG729EncH->new_speech[]. *
 *    This vector is global and is declared in this function.      *
 *                                                                 *
 *  Ouputs:                                                        *
 *                                                                 *
 *    ana[]      ->analysis parameters.                            *
 *                                                                 *
 *-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "basicop.h"
#include "ld8a.h"
#include "vad.h"
#include "dtx.h"

//#include "format_prt.h"

//extern l32 l32PrtFlag;

/*-----------------------------------------------------------*
 *    Coder constant parameters (defined in "ld8a.h")        *
 *-----------------------------------------------------------*
 *   L_WINDOW    : LPC analysis window size.                 *
 *   L_NEXT      : Samples of next frame needed for autocor. *
 *   L_FRAME     : Frame size.                               *
 *   L_SUBFR     : Sub-frame size.                           *
 *   M           : LPC order.                                *
 *   MP1         : LPC order+1                               *
 *   L_TOTAL     : Total size of pG729EncH->speech buffer.              *
 *   PIT_MIN     : Minimum pitch lag.                        *
 *   PIT_MAX     : Maximum pitch lag.                        *
 *   L_INTERPOL  : Length of filter for interpolation        *
 *-----------------------------------------------------------*/

/*--------------------------------------------------------*
 *         Static memory allocation.                      *
 *--------------------------------------------------------*/
#if 0
        /* Speech vector */

 static s16 pG729EncH->old_speech[L_TOTAL];
 static s16 *pG729EncH->speech, *pG729EncH->p_window;
 s16 *pG729EncH->new_speech;                    /* Global variable */

        /* Weighted pG729EncH->speech vector */

 static s16 pG729EncH->old_wsp[L_FRAME+PIT_MAX];
 static s16 *pG729EncH->wsp;

        /* Excitation vector */

 static s16 pG729EncH->old_exc[L_FRAME+PIT_MAX+L_INTERPOL];
 static s16 *pG729EncH->exc;

        /* Lsp (Line spectral pairs) */

 static s16 pG729EncH->lsp_old[M]={
              30000, 26000, 21000, 15000, 8000, 0, -8000,-15000,-21000,-26000};
 static s16 pG729EncH->lsp_old_q[M];

        /* Filter's memory */

 static s16  pG729EncH->mem_w0[M], pG729EncH->mem_w[M], pG729EncH->mem_zero[M];
 static s16  pG729EncH->sharp;

        /* For G.729B */
        /* DTX variables */
 static s16 pG729EncH->pastVad;   
 static s16 pG729EncH->pastVad;
 static s16 pG729EncH->seed;

#endif 

/*-----------------------------------------------------------------*
 *   Function  Init_Coder_ld8a                                     *
 *            ~~~~~~~~~~~~~~~                                      *
 *                                                                 *
 *  Init_Coder_ld8a(void);                                         *
 *                                                                 *
 *   ->Initialization of variables for the coder section.          *
 *       - initialize pointers to pG729EncH->speech buffer                    *
 *       - initialize static  pointers                             *
 *       - set static vectors to zero                              *
 *                                                                 *
 *-----------------------------------------------------------------*/

 //void Init_Coder_ld8a(void)
 void Init_Coder_ld8a(TG729BENCODER *pG729EncH)
 {
	 
 /*----------------------------------------------------------------------*
 *      Initialize pointers to pG729EncH->speech vector.                            *
 *                                                                       *
 *                                                                       *
 *   |--------------------|-------------|-------------|------------|     *
 *     previous pG729EncH->speech           sf1           sf2         L_NEXT        *
 *                                                                       *
 *   <----------------  Total pG729EncH->speech vector (L_TOTAL)   ----------->     *
 *   <----------------  LPC analysis window (L_WINDOW)  ----------->     *
 *   |                   <-- present frame (L_FRAME) -->                 *
 * pG729EncH->old_speech            |              <-- new pG729EncH->speech (L_FRAME) -->     *
 * pG729EncH->p_window              |              |                                *
 *                     pG729EncH->speech           |                                *
 *                             pG729EncH->new_speech                                *
	 *-----------------------------------------------------------------------*/
	 
	 pG729EncH->new_speech = pG729EncH->old_speech + L_TOTAL - L_FRAME;         /* New pG729EncH->speech     */
	 pG729EncH->speech     = pG729EncH->new_speech - L_NEXT;                    /* Present frame  */
	 pG729EncH->p_window   = pG729EncH->old_speech + L_TOTAL - L_WINDOW;        /* For LPC window */
	 
	 /* Initialize static pointers */
	 
	 pG729EncH->wsp    = pG729EncH->old_wsp + PIT_MAX;
	 pG729EncH->exc    = pG729EncH->old_exc + PIT_MAX + L_INTERPOL;
	 
	 /* Static vectors to zero */
	 
	 Set_zero(pG729EncH->old_speech, L_TOTAL);
	 Set_zero(pG729EncH->old_exc, PIT_MAX+L_INTERPOL);
	 Set_zero(pG729EncH->old_wsp, PIT_MAX);
	 Set_zero(pG729EncH->mem_w,   M);
	 Set_zero(pG729EncH->mem_w0,  M);
	 Set_zero(pG729EncH->mem_zero, M);
	 pG729EncH->sharp = SHARPMIN;
	 
	 // Initialize pG729EncH->lsp_old_q[]
	 
	 Copy(pG729EncH->lsp_old, pG729EncH->lsp_old_q, M);
	 
	 Lsp_encw_reset(pG729EncH);
	 Init_exc_err(pG729EncH);
	 
	 //For G.729B
	 //Initialize VAD/DTX parameters
	 pG729EncH->pastVad = 1;
	 pG729EncH->ppastVad = 1;
	 pG729EncH->seed = INIT_SEED;
	 vad_init(pG729EncH);

	 Init_enc_lsfq_noise(pG729EncH);
	 return;
 }

/*-----------------------------------------------------------------*
 *   Functions Coder_ld8a                                          *
 *            ~~~~~~~~~~                                           *
 *  Coder_ld8a(s16 ana[]);                                      *
 *                                                                 *
 *   ->Main coder function.                                        *
 *                                                                 *
 *                                                                 *
 *  Input:                                                         *
 *                                                                 *
 *    80 pG729EncH->speech data should have beee copy to vector pG729EncH->new_speech[]. *
 *    This vector is global and is declared in this function.      *
 *                                                                 *
 *  Ouputs:                                                        *
 *                                                                 *
 *    ana[]      ->analysis parameters.                            *
 *                                                                 *
 *-----------------------------------------------------------------*/
void Coder_ld8a(
     s16 ana[],       /* output  : Analysis parameters */
     s16 frame,       /* input   : frame counter       */
     s16 vad_enable,   /* input   : VAD enable flag     */
	 TG729BENCODER *pG729EncH
)
{

  /* LPC analysis */

  s16 Aq_t[(MP1)*2];         /* A(z)   quantized for the 2 subframes */
  s16 Ap_t[(MP1)*2];         /* A(z/gamma)       for the 2 subframes */
  s16 *Aq, *Ap;              /* Pointer on Aq_t and Ap_t             */

  /* Other vectors */

  s16 h1[L_SUBFR];            /* Impulse response h1[]              */
  s16 xn[L_SUBFR];            /* Target vector for pitch search     */
  s16 xn2[L_SUBFR];           /* Target vector for codebook search  */
  s16 code[L_SUBFR];          /* Fixed codebook excitation          */
  s16 y1[L_SUBFR];            /* Filtered adaptive excitation       */
  s16 y2[L_SUBFR];            /* Filtered fixed codebook excitation */
  s16 g_coeff[4];             /* Correlations between xn & y1       */

  s16 g_coeff_cs[5];
  s16 exp_g_coeff_cs[5];      /* Correlations between xn, y1, & y2
                                     <y1,y1>, -2<xn,y1>,
                                          <y2,y2>, -2<xn,y2>, 2<y1,y2> */

  /* Scalars */

  s16 i, j, k, i_subfr;
  s16 T_op, T0, T0_min, T0_max, T0_frac;
  s16 gain_pit, gain_code, index;
  s16 temp, taming;
  l32 L_temp;

	/*test code*/
	//s16fprt(pG729EncH->mem_w0, 10, "as16mem_w0_st", &l32PrtFlag, 1);
	/*test code*/
	//s16fprt(&pG729EncH->prev_energy, 1, "as16prev_energy002", &l32PrtFlag, 1);


/*------------------------------------------------------------------------*
 *  - Perform LPC analysis:                                               *
 *       * autocorrelation + lag windowing                                *
 *       * Levinson-durbin algorithm to find a[]                          *
 *       * convert a[] to lsp[]                                           *
 *       * quantize and code the LSPs                                     *
 *       * find the interpolated LSPs and convert to a[] for the 2        *
 *         subframes (both quantized and unquantized)                     *
 *------------------------------------------------------------------------*/

	
	{
     /* Temporary vectors */
 
    s16 r_l[NP+1], r_h[NP+1];     /* Autocorrelations low and hi          */
    s16 rc[M];                    /* Reflection coefficients.             */
    s16 lsp_new[M], lsp_new_q[M]; /* LSPs at 2th subframe                 */

    /* For G.729B */
    s16 rh_nbe[MP1];             
    s16 lsf_new[M];
    s16 lsfq_mem[MA_NP][M];
    s16 exp_R0, Vad; 
    /* LP analysis */
    Autocorr(pG729EncH->p_window, NP, r_h, r_l, &exp_R0);     /* Autocorrelations */
    Copy(r_h, rh_nbe, MP1);
    Lag_window(NP, r_h, r_l);                      /* Lag windowing    */
	      /*test code*/
		  //s16fprt(Ap_t, 22, "as16Ap1L", &l32PrtFlag, 1);
 		  //s16fprt(Aq_t, 22, "as16Aq1L", &l32PrtFlag, 1);
    Levinson(r_h, r_l, Ap_t, rc, &temp);          /* Levinson Durbin  */
	      /*test code*/
		  //s16fprt(Ap_t, 22, "as16Ap2L", &l32PrtFlag, 1);
 		  //s16fprt(Aq_t, 22, "as16Aq2L", &l32PrtFlag, 1);
     Az_lsp(Ap_t, lsp_new, pG729EncH->lsp_old);               /* From A(z) to lsp */

    /* For G.729B */
    /* ------ VAD ------- */
    Lsp_lsf(lsp_new, lsf_new, M);

	/*test code*/
	//s16fprt(&pG729EncH->prev_energy, 1, "as16prev_energy003", &l32PrtFlag, 1);

    vad(rc[1], lsf_new, r_h, r_l, exp_R0, pG729EncH->p_window, frame, 
        pG729EncH->pastVad, pG729EncH->ppastVad, &Vad, pG729EncH);

	/*test code*/
	//s16fprt(&pG729EncH->prev_energy, 1, "as16prev_energy004", &l32PrtFlag, 1);

    Update_cng(rh_nbe, exp_R0, Vad, pG729EncH);
    
    /* ---------------------- */
    /* Case of Inactive frame */
    /* ---------------------- */
	      /*test code
	      //s16fprt(pG729EncH->old_exc, 234, "as16old_exc0w", &l32PrtFlag, 1);*/

	      /*test code*/
		  //s16fprt(Aq_t, 22, "as16Aq3L", &l32PrtFlag, 1);
	/*test code*/
	//s16fprt(&pG729EncH->prev_energy, 1, "as16prev_energy001", &l32PrtFlag, 1);

    if ((Vad == 0) && (vad_enable == 1))
	{

      Get_freq_prev(lsfq_mem, pG729EncH);
	  /*test code
	      //s16fprt(pG729EncH->old_exc, 234, "as16old_exc0l", &l32PrtFlag, 1);

	      //s16fprt(lsfq_mem, 40, "as16lsfq_mem", &l32PrtFlag, 1);
	      //s16fprt(&pG729EncH->seed, 1, "as16seed", &l32PrtFlag, 1);*/
	      //s16fprt(pG729EncH->lsp_old_q, 10, "as16lsp_old_q", &l32PrtFlag, 1);
	      //s16fprt(ana, 2, "as16ana", &l32PrtFlag, 1);
	      //s16fprt(&pG729EncH->pastVad, 1, "as16pastVad", &l32PrtFlag, 1);

	      /*test code*/
		  //s16fprt(Aq_t, 22, "as16Aq4L", &l32PrtFlag, 1);

      Cod_cng(pG729EncH->exc, pG729EncH->pastVad, pG729EncH->lsp_old_q, Aq_t, ana, lsfq_mem, &pG729EncH->seed, pG729EncH);

	  /*test code
	      //s16fprt(pG729EncH->old_exc, 234, "as16old_exc1l", &l32PrtFlag, 1);
	      //s16fprt(&pG729EncH->pastVad, 1, "as16pastVad1", &l32PrtFlag, 1);
	      //s16fprt(lsfq_mem, 40, "as16lsfq_mem1", &l32PrtFlag, 1);
	      //s16fprt(&pG729EncH->seed, 1, "as16seed1", &l32PrtFlag, 1);*/
	      //s16fprt(pG729EncH->lsp_old_q, 10, "as16lsp_old_q1", &l32PrtFlag, 1);

	      /*test code*/
		  //s16fprt(Aq_t, 22, "as16Aq5L", &l32PrtFlag, 1);

      Update_freq_prev(lsfq_mem, pG729EncH);
      pG729EncH->ppastVad = pG729EncH->pastVad;
      pG729EncH->pastVad = Vad;

      /* Update pG729EncH->wsp, pG729EncH->mem_w and pG729EncH->mem_w0 */
      Aq = Aq_t;

      for(i_subfr=0; i_subfr < L_FRAME; i_subfr += L_SUBFR) 
	  {
		  
	/*test code*/
	//s16fprt(Aq_t, 22, "as16Aq1", &l32PrtFlag, 1);
	//s16fprt(Ap_t, 22, "as16Ap1", &l32PrtFlag, 1);

		  /* Residual signal in xn */
		  Residu(Aq, &pG729EncH->speech[i_subfr], xn, L_SUBFR);

	/*test code*/
	//s16fprt(xn, 40, "as16Synxn1", &l32PrtFlag, 1);
 		  
		  Weight_Az(Aq, GAMMA1, M, Ap_t);
		  
		  /* Compute pG729EncH->wsp and pG729EncH->mem_w */
		  Ap = Ap_t + MP1;
		  Ap[0] = 4096;
		  for(i=1; i<=M; i++)    /* Ap[i] = Ap_t[i] - 0.7 * Ap_t[i-1]; */
			  Ap[i] = sub(Ap_t[i], mult(Ap_t[i-1], 22938));
		  Syn_filt(Ap, xn, &pG729EncH->wsp[i_subfr], L_SUBFR, pG729EncH->mem_w, 1);
		  
		  /* Compute pG729EncH->mem_w0 */
		  for(i=0; i<L_SUBFR; i++) 
		  {
			  xn[i] = sub(xn[i], pG729EncH->exc[i_subfr+i]);  /* residu[] - pG729EncH->exc[] */
		  }
	  /*test code*/
	//s16fprt(pG729EncH->mem_w0, 10, "as16mem_w0_st01", &l32PrtFlag, 1);
	//s16fprt(Ap_t, 22, "as16Ap01", &l32PrtFlag, 1);
	//s16fprt(xn, 40, "as16Synxn01", &l32PrtFlag, 1);

		  Syn_filt(Ap_t, xn, xn, L_SUBFR, pG729EncH->mem_w0, 1);
 	  /*test code*/
	//s16fprt(pG729EncH->mem_w0, 10, "as16mem_w0_st1", &l32PrtFlag, 1);
               
       Aq += MP1;
      }
      
      
      pG729EncH->sharp = SHARPMIN;
      
      /* Update memories for next frames */
      Copy(&pG729EncH->old_speech[L_FRAME], &pG729EncH->old_speech[0], L_TOTAL-L_FRAME);
      Copy(&pG729EncH->old_wsp[L_FRAME], &pG729EncH->old_wsp[0], PIT_MAX);
      Copy(&pG729EncH->old_exc[L_FRAME], &pG729EncH->old_exc[0], PIT_MAX+L_INTERPOL);

	  /*test code
	//s16fprt(pG729EncH->mem_w0, 10, "as16mem_w0_st1", &l32PrtFlag, 1);*/
 
	  return;
    }  /* End of inactive frame case */
    

    /* -------------------- */
    /* Case of Active frame */
    /* -------------------- */
    
    /* Case of active frame */
    *ana++ = 1;
    pG729EncH->seed = INIT_SEED;
    pG729EncH->ppastVad = pG729EncH->pastVad;
    pG729EncH->pastVad = Vad;

    /* LSP quantization */
		/*test code
		{
			l32 i =0;
			
			//s16fprt(lsp_new, 10, "as16LspNew", &i, 0);
		}*/

    Qua_lsp(lsp_new, lsp_new_q, ana, pG729EncH);
		/*test code
		{
			l32 i =0;
			
			//s16fprt(lsp_new_q, 10, "as16LspNewQ", &i, 0);
		}*/
    ana += 2;                         /* Advance analysis parameters pointer */

    /*--------------------------------------------------------------------*
     * Find interpolated LPC parameters in all subframes                  *
     * The interpolated parameters are in array Aq_t[].                   *
     *--------------------------------------------------------------------*/

    Int_qlpc(pG729EncH->lsp_old_q, lsp_new_q, Aq_t);

    /* Compute A(z/gamma) */

    Weight_Az(&Aq_t[0],   GAMMA1, M, &Ap_t[0]);
    Weight_Az(&Aq_t[MP1], GAMMA1, M, &Ap_t[MP1]);

    /* update the LSPs for the next frame */

    Copy(lsp_new,   pG729EncH->lsp_old,   M);
    Copy(lsp_new_q, pG729EncH->lsp_old_q, M);
  }

 /*----------------------------------------------------------------------*
  * - Find the weighted input pG729EncH->speech w_sp[] for the whole pG729EncH->speech frame   *
  * - Find the open-loop pitch delay                                     *
  *----------------------------------------------------------------------*/

  Residu(&Aq_t[0], &pG729EncH->speech[0], &pG729EncH->exc[0], L_SUBFR);
  Residu(&Aq_t[MP1], &pG729EncH->speech[L_SUBFR], &pG729EncH->exc[L_SUBFR], L_SUBFR);

  {
    s16 Ap1[MP1];

    Ap = Ap_t;
    Ap1[0] = 4096;
    for(i=1; i<=M; i++)    /* Ap1[i] = Ap[i] - 0.7 * Ap[i-1]; */
       Ap1[i] = sub(Ap[i], mult(Ap[i-1], 22938));
    Syn_filt(Ap1, &pG729EncH->exc[0], &pG729EncH->wsp[0], L_SUBFR, pG729EncH->mem_w, 1);

    Ap += MP1;
    for(i=1; i<=M; i++)    /* Ap1[i] = Ap[i] - 0.7 * Ap[i-1]; */
       Ap1[i] = sub(Ap[i], mult(Ap[i-1], 22938));
    Syn_filt(Ap1, &pG729EncH->exc[L_SUBFR], &pG729EncH->wsp[L_SUBFR], L_SUBFR, pG729EncH->mem_w, 1);
  }

  /* Find open loop pitch lag */

  T_op = Pitch_ol_fast(pG729EncH->wsp, PIT_MAX, L_FRAME);

  /* Range for closed loop pitch search in 1st subframe */

  T0_min = sub(T_op, 3);
  if (sub(T0_min,PIT_MIN)<0) {
    T0_min = PIT_MIN;
  }

  T0_max = add(T0_min, 6);
  if (sub(T0_max ,PIT_MAX)>0)
  {
     T0_max = PIT_MAX;
     T0_min = sub(T0_max, 6);
  }


 /*------------------------------------------------------------------------*
  *          Loop for every subframe in the analysis frame                 *
  *------------------------------------------------------------------------*
  *  To find the pitch and innovation parameters. The subframe size is     *
  *  L_SUBFR and the loop is repeated 2 times.                             *
  *     - find the weighted LPC coefficients                               *
  *     - find the LPC residual signal res[]                               *
  *     - compute the target signal for pitch search                       *
  *     - compute impulse response of weighted synthesis filter (h1[])     *
  *     - find the closed-loop pitch parameters                            *
  *     - encode the pitch delay                                           *
  *     - find target vector for codebook search                           *
  *     - codebook search                                                  *
  *     - VQ of pitch and codebook gains                                   *
  *     - update states of weighting filter                                *
  *------------------------------------------------------------------------*/

  Aq = Aq_t;    /* pointer to interpolated quantized LPC parameters */
  Ap = Ap_t;    /* pointer to weighted LPC coefficients             */

  for (i_subfr = 0;  i_subfr < L_FRAME; i_subfr += L_SUBFR)
  {

    /*---------------------------------------------------------------*
     * Compute impulse response, h1[], of weighted synthesis filter  *
     *---------------------------------------------------------------*/

    h1[0] = 4096;
    Set_zero(&h1[1], L_SUBFR-1);
    Syn_filt(Ap, h1, h1, L_SUBFR, &h1[1], 0);

   /*----------------------------------------------------------------------*
    *  Find the target vector for pitch search:                            *
    *----------------------------------------------------------------------*/
	/*test code*/
	//s16fprt(Ap, 22, "as16Aq", &l32PrtFlag, 1);
	//s16fprt(&pG729EncH->exc[i_subfr], 40, "as16exc", &l32PrtFlag, 1);
	//s16fprt(pG729EncH->mem_w0, 10, "as16mem_w0", &l32PrtFlag, 1);

    Syn_filt(Ap, &pG729EncH->exc[i_subfr], xn, L_SUBFR, pG729EncH->mem_w0, 0);

	/*test code*/
	//s16fprt(xn, 40, "as16Synxn", &l32PrtFlag, 1);

    /*---------------------------------------------------------------------*
     *                 Closed-loop fractional pitch search                 *
     *---------------------------------------------------------------------*/

    T0 = Pitch_fr3_fast(&pG729EncH->exc[i_subfr], xn, h1, L_SUBFR, T0_min, T0_max,
                    i_subfr, &T0_frac);

    index = Enc_lag3(T0, T0_frac, &T0_min, &T0_max,PIT_MIN,PIT_MAX,i_subfr);

    *ana++ = index;

    if (i_subfr == 0) {
      *ana++ = Parity_Pitch(index);
    }

   /*-----------------------------------------------------------------*
    *   - find filtered pitch pG729EncH->exc                                     *
    *   - compute pitch gain and limit between 0 and 1.2              *
    *   - update target vector for codebook search                    *
    *-----------------------------------------------------------------*/

    Syn_filt(Ap, &pG729EncH->exc[i_subfr], y1, L_SUBFR, pG729EncH->mem_zero, 0);
	/* test code*/
	//s16fprt(xn, 40, "as16xn", &l32PrtFlag, 1);
	//s16fprt(y1, 40, "as16y1", &l32PrtFlag, 1);

    gain_pit = G_pitch(xn, y1, g_coeff, L_SUBFR);

	/* test code*/
	//s16fprt(g_coeff, 4, "as16g_coeff", &l32PrtFlag, 1);

    /* clip pitch gain if taming is necessary */

    taming = test_err(T0, T0_frac, pG729EncH);

    if( taming == 1){
      if (sub(gain_pit, GPCLIP) > 0) {
        gain_pit = GPCLIP;
      }
    }

    /* xn2[i]   = xn[i] - y1[i] * gain_pit  */

    for (i = 0; i < L_SUBFR; i++)
    {
      L_temp = L_mult(y1[i], gain_pit);
      L_temp = L_shl(L_temp, 1);               /* gain_pit in Q14 */
      xn2[i] = sub(xn[i], extract_h(L_temp));
    }


   /*-----------------------------------------------------*
    * - Innovative codebook search.                       *
    *-----------------------------------------------------*/

    index = ACELP_Code_A(xn2, h1, T0, pG729EncH->sharp, code, y2, &i);

    *ana++ = index;        /* Positions index */
    *ana++ = i;            /* Signs index     */


   /*-----------------------------------------------------*
    * - Quantization of gains.                            *
    *-----------------------------------------------------*/

    g_coeff_cs[0]     = g_coeff[0];            /* <y1,y1> */
    exp_g_coeff_cs[0] = negate(g_coeff[1]);    /* Q-Format:XXX -> JPN */
    g_coeff_cs[1]     = negate(g_coeff[2]);    /* (xn,y1) -> -2<xn,y1> */
    exp_g_coeff_cs[1] = negate(add(g_coeff[3], 1)); /* Q-Format:XXX -> JPN */

    Corr_xy2( xn, y1, y2, g_coeff_cs, exp_g_coeff_cs );  /* Q0 Q0 Q12 ^Qx ^Q0 */
                         /* g_coeff_cs[3]:exp_g_coeff_cs[3] = <y2,y2>   */
                         /* g_coeff_cs[4]:exp_g_coeff_cs[4] = -2<xn,y2> */
                         /* g_coeff_cs[5]:exp_g_coeff_cs[5] = 2<y1,y2>  */
	/*test code*/
	//s16fprt(code, 40, "as16Code", &l32PrtFlag, 1);
	//s16fprt(g_coeff_cs, 5, "as16Gcs", &l32PrtFlag, 1);
	//s16fprt(exp_g_coeff_cs, 5, "as16EGcs", &l32PrtFlag, 1);

    *ana++ = Qua_gain(code, g_coeff_cs, exp_g_coeff_cs,
                         L_SUBFR, &gain_pit, &gain_code, taming, pG729EncH);


   /*------------------------------------------------------------*
    * - Update pitch sharpening "pG729EncH->sharp" with quantized gain_pit  *
    *------------------------------------------------------------*/

    pG729EncH->sharp = gain_pit;
    if (sub(pG729EncH->sharp, SHARPMAX) > 0) { pG729EncH->sharp = SHARPMAX;         }
    if (sub(pG729EncH->sharp, SHARPMIN) < 0) { pG729EncH->sharp = SHARPMIN;         }

   /*------------------------------------------------------*
    * - Find the total excitation                          *
    * - update filters memories for finding the target     *
    *   vector in the next subframe                        *
    *------------------------------------------------------*/

    for (i = 0; i < L_SUBFR;  i++)
    {
      /* pG729EncH->exc[i] = gain_pit*pG729EncH->exc[i] + gain_code*code[i]; */
      /* pG729EncH->exc[i]  in Q0   gain_pit in Q14               */
      /* code[i] in Q13  gain_cod in Q1                */

      L_temp = L_mult(pG729EncH->exc[i+i_subfr], gain_pit);
      L_temp = L_mac(L_temp, code[i], gain_code);
      L_temp = L_shl(L_temp, 1);
      pG729EncH->exc[i+i_subfr] = round_linux(L_temp);
    }

    //update_exc_err(gain_pit, T0);
    enc_update_exc_err(gain_pit, T0, pG729EncH);

    for (i = L_SUBFR-M, j = 0; i < L_SUBFR; i++, j++)
    {
      temp       = extract_h(L_shl( L_mult(y1[i], gain_pit),  1) );
      k          = extract_h(L_shl( L_mult(y2[i], gain_code), 2) );
      pG729EncH->mem_w0[j]  = sub(xn[i], add(temp, k));
    }

    Aq += MP1;           /* interpolated LPC parameters for next subframe */
    Ap += MP1;

  }

 /*--------------------------------------------------*
  * Update signal for next frame.                    *
  * -> shift to the left by L_FRAME:                 *
  *     pG729EncH->speech[], pG729EncH->wsp[] and  pG729EncH->exc[]                   *
  *--------------------------------------------------*/

  Copy(&pG729EncH->old_speech[L_FRAME], &pG729EncH->old_speech[0], L_TOTAL-L_FRAME);
  Copy(&pG729EncH->old_wsp[L_FRAME], &pG729EncH->old_wsp[0], PIT_MAX);
  Copy(&pG729EncH->old_exc[L_FRAME], &pG729EncH->old_exc[0], PIT_MAX+L_INTERPOL);
	/*test code*/
	//s16fprt(pG729EncH->mem_w0, 10, "as16mem_w0_st2", &l32PrtFlag, 1);

  return;
}
