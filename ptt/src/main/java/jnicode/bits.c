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
/*****************************************************************************/
/* bit stream manipulation routines                                          */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "ld8a.h"
#include "tab_ld8a.h"
#include "octet.h"

/* prototypes for local functions */
static void  int2bin(s16 value, s16 no_of_bits, s16 *bitstream);
static s16   bin2int(s16 no_of_bits, s16 *bitstream);

/*----------------------------------------------------------------------------
 * prm2bits_ld8k -converts encoder parameter vector into vector of serial bits
 * bits2prm_ld8k - converts serial received bits to  encoder parameter vector
 *
 * The transmitted parameters are:
 *
 *     LPC:     1st codebook           7+1 bit
 *              2nd codebook           5+5 bit
 *
 *     1st subframe:
 *          pitch period                 8 bit
 *          parity check on 1st period   1 bit
 *          codebook index1 (positions) 13 bit
 *          codebook index2 (signs)      4 bit
 *          pitch and codebook gains   4+3 bit
 *
 *     2nd subframe:
 *          pitch period (relative)      5 bit
 *          codebook index1 (positions) 13 bit
 *          codebook index2 (signs)      4 bit
 *          pitch and codebook gains   4+3 bit
 *----------------------------------------------------------------------------
 */

/*====================================================================
函   数   名:  prm2bits_ld8k
功        能:  converts encoder parameter vector into vector of serial bits
引用全局变量:  无
输入参数说明:
			 1-(i)prm     encoded parameters
			 2-(o)bits    serial bits 
返回 值 说明:  无
特 殊 说  明:  无
======================================================================*/
void prm2bits_ld8k(
				   s16  prm[], 
				   s16 bits[] 
				   )
{
	
	l32  i;
	*bits++ = SYNC_WORD;         /* At receiver this bit indicates BFI */
	//*bits++ = SIZE_WORD;       /* Number of bits in this frame       */
	
	switch(prm[0])
	{
		
		/* not transmitted */
	case 0 : 
		{
			*bits = 0;
			break;
		}
		
	case 1 : 
		{ //active
			*bits++ = 80;
			for (i = 0; i < PRM_SIZE; i++)
			{
				int2bin(prm[i+ 1] , bitsno[i], bits);
				bits += bitsno[i];
			}
			break;
			
		}
		
	case 2 : 
		{ /*no active but sid transfered.
			*bits++ = 16;
			for (i = 0; i < 4; i++)
			{
				int2bin(prm[i+ 1] , bitsno2[i], bits);
				bits += bitsno2[i];
			}
			break;*/

#ifndef OCTET_TX_MODE
			*bits++ = RATE_SID;  //15
			for (i = 0; i < 4; i++) {
				int2bin(prm[i+1], bitsno2[i], bits);
				bits += bitsno2[i];
			}
#else
			//以8位字节进行传送
			*bits++ = RATE_SID_OCTET; //16
			for (i = 0; i < 4; i++) {
				int2bin(prm[i+1], bitsno2[i], bits);
				bits += bitsno2[i];
			}
			*bits++ = BIT_0;
#endif
			break;

		}
		
	default : 
		{
			//			printf("Unrecognized frame type\n");
			//			exit(-1);
			*bits++ = -2; //set length to be zero. -2 + 2 = 0;
			break;
		}
	}
	
	return;
}


/*----------------------------------------------------------------------------
 * int2bin convert integer to binary and write the bits bitstream array
 *----------------------------------------------------------------------------
 */
static void int2bin(
 s16 value,             /* input : decimal value */
 s16 no_of_bits,        /* input : number of bits to use */
 s16 *bitstream       /* output: bitstream  */
)
{
   s16 *pt_bitstream;
   s16   i, bit;

   pt_bitstream = bitstream + no_of_bits;

   for (i = 0; i < no_of_bits; i++)
   {
     bit = value & (s16)0x0001;      /* get lsb */
     if (bit == 0)
         *--pt_bitstream = BIT_0;
     else
         *--pt_bitstream = BIT_1;
     value >>= 1;
   }
}
/*====================================================================
函   数   名:  bits2prm_ld8k
功        能:  converts serial received bits to  encoder parameter vector
引用全局变量:  无
输入参数说明:
			 1-(i)bits     serial bits
			 2-(o)prm      decoded parameters

返回 值 说明:  无
特 殊 说  明:  无
======================================================================*/
void bits2prm_ld8k(
				   s16 bits[],
				   s16  prm[]
				   )
{
	l32  i = 0;
	s16 l_WnbWords = 0; //5.17
	s16 l_WnbPrm = 0;
	
	l_WnbWords = * bits ++;
	
	switch(l_WnbWords)
	{
	case 10:
		prm[0] = 1;
		l_WnbPrm = PRM_SIZE;
		for (i = 0; i < l_WnbPrm; i++)
		{
			prm[i+1] = bin2int(bitsno[i], bits);
			bits  += bitsno[i];
		}
		break;
	case 2:
		prm[0] = 2;
		l_WnbPrm = 4;
		for (i = 0; i < l_WnbPrm; i++)
		{
			prm[i+1] = bin2int(bitsno2[i], bits);
			bits  += bitsno2[i];
		}
		break;
	default:
		prm[0] = 0;
		l_WnbPrm = 0;
		break;
	} 
	
	return;
}

/*----------------------------------------------------------------------------
 *  bits2prm_ld8k - converts serial received bits to  encoder parameter vector
 *----------------------------------------------------------------------------
 */
void bits2prm_ld8k_org(
 s16 bits[],          /* input : serial bits (80)                       */
 s16   prm[]          /* output: decoded parameters (11 parameters)     */
)
{
  s16 i;
  s16 nb_bits;

  nb_bits = *bits++;        /* Number of bits in this frame       */

  if(nb_bits == RATE_8000) {
    prm[1] = 1;
    for (i = 0; i < PRM_SIZE; i++) {
      prm[i+2] = bin2int(bitsno[i], bits);
      bits  += bitsno[i];
    }
  }
  else
#ifndef OCTET_TX_MODE
    if(nb_bits == RATE_SID) {
      prm[1] = 2;
      for (i = 0; i < 4; i++) {
        prm[i+2] = bin2int(bitsno2[i], bits);
        bits += bitsno2[i];
      }
    }
#else
  /* the last bit of the SID bit stream under octet mode will be discarded */
  if(nb_bits == RATE_SID_OCTET) {
    prm[1] = 2;
    for (i = 0; i < 4; i++) {
      prm[i+2] = bin2int(bitsno2[i], bits);
      bits += bitsno2[i];
    }
  }
#endif

  else {
    prm[1] = 0;
  }
  return;

}


/*----------------------------------------------------------------------------
 * bin2int - read specified bits from bit array  and convert to integer value
 *----------------------------------------------------------------------------
 */
static s16 bin2int(            /* output: decimal value of bit pattern */
 s16 no_of_bits,        /* input : number of bits to read */
 s16 *bitstream       /* input : array containing bits */
)
{
   s16   value, i;
   s16 bit;

   value = 0;
   for (i = 0; i < no_of_bits; i++)
   {
     value <<= 1;
     bit = *bitstream++;
     if (bit == BIT_1)  value += 1;
   }
   return(value);
}



