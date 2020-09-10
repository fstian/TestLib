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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ld8a.h"
#include "dtx.h"
//#include "format_prt.h"

#define HG729BENC void * //encoder handle 
#define G729_OK   ((s16)0)		//succeed
#define G729_ERR  ((s16)-1)		//some thing is error now


s16 Lsp_static[M]={
		30000, 26000, 21000, 15000, 8000, 0, -8000,-15000,-21000,-26000};

//l32 l32PrtFlag = 0;


void  GetG729Version(u8 *Version, l32 StrLen, l32 * StrLenUsed)
{
	u8 *pcVer = "g729 40.10.01.01.060301";

	sprintf(Version,"verion: %s  compile time：%s\n",pcVer,__DATE__,__TIME__);
	*StrLenUsed = strlen(Version);
}



/*====================================================================
函   数   名:  bin2int
功        能:  read specified bits from bit array  and convert to integer value
引用全局变量:  无
输入参数说明:
			 1-(i)no_of_bits     number of bits to read
			 2-(i)bitstream      array containing bits

返回 值 说明:  decimal value of bit pattern
特 殊 说  明:  无
======================================================================*/
static s16  bin2int(
					s16  no_of_bits,
					s16 *bitstream
					)
{
	s16  value, i;
	s16  bit;
	
	value = 0;
	for (i = 0; i < no_of_bits; i++)
	{
		value <<= 1;
		bit = *bitstream++;
		if (bit == BIT_1)  value += 1;
   }
   return(value);
}



/*====================================================================
函数名：      g729_encinit
功  能：      初始化编码器句柄
引用全局变量：无
输入参数说明：
返回 值 说明：错误 G729_ERR  成功 G729_OK
====================================================================*/

l32  g729_encinit( //output: success-G729_OK ortherwise :  G729_ERR
				  HG729BENC * r_poEncptr //output: The instance of the encoder 
				  )
{

	l32 i = 0;
	if(NULL == r_poEncptr)
	{
		return G729_ERR;
	}
	
	*r_poEncptr = (HG729BENC)malloc(sizeof(TG729BENCODER));
	if(NULL == *r_poEncptr)
	{
		return G729_ERR;
	}
	
	//initial codec
	memset(*r_poEncptr,0,sizeof(TG729BENCODER));
	
	((TG729BENCODER *)*r_poEncptr)->l32Length = sizeof(TG729BENCODER);
	
	
	sprintf(((TG729BENCODER *)*r_poEncptr)->s8Version,"EC-2.2"); //e: encoder
	
	for (i = 0;i <M; i++)
	{
		((TG729BENCODER *)*r_poEncptr)->lsp_old[i] =  Lsp_static[i]; //from file COD_LD8K.C
	}
	//    { (d64)0.9595,  (d64)0.8413,  (d64)0.6549,  (d64)0.4154,  (d64)0.1423,
	//    (d64)-0.1423, (d64)-0.4154, (d64)-0.6549, (d64)-0.8413, (d64)-0.9595};
	
	Init_Pre_Process((TG729BENCODER*)* r_poEncptr);			//initialize the high fir and iir memory
	Init_Coder_ld8a((TG729BENCODER*)* r_poEncptr);           // Initialize the coder             

	for(i = 0;i<4;i++)
		((TG729BENCODER *)*r_poEncptr)->past_qua_en[i] =  -14336;
	
	//vad initial 
	Init_Cod_cng((TG729BENCODER*)*r_poEncptr);
	
	((TG729BENCODER *)*r_poEncptr)->vad_enable =  G729_VADDISABLE;
	
	return G729_OK;
}


/*====================================================================
	函数名	： 729_SetEncAorB
	功能		：初始化编码器句柄
	引用全局变量：无
输入参数说明：
	vad : input 是否使用729B。	
	G729_VADDISABLE: 不使用G729B。 
	G729_VADENABLE: 使用729B。
	encptr: input handle to  HG729BENC
返回值说明：无
====================================================================*/
void g729_SetEncAorB(enumVAD vad,HG729BENC  encptr)
{
	if(NULL == encptr)
		return;
		//ec-0.0
	if('E' != ((TG729BENCODER *)encptr)->s8Version[0])
		return ;
	if('2' != ((TG729BENCODER *)encptr)->s8Version[3])
		return ;
	if('2' != ((TG729BENCODER *)encptr)->s8Version[5])
		return;

	switch(vad)
	{
	case G729_VADDISABLE:
	case G729_VADENABLE:
		((TG729BENCODER *)encptr)->vad_enable = vad;
		break;
	default:
		((TG729BENCODER *)encptr)->vad_enable = G729_VADDISABLE;
		break;

	}
	return;
}

/*====================================================================
	函数名	：g729_encfree
	功能	：释放编码器句柄
	引用全局变量：无
输入参数说明：见下
	返回值说明：无
====================================================================*/
void g729_encfree(
				  HG729BENC  r_pidecptr  //input: the handle of the encoder
				  )
{
	if(r_pidecptr != NULL)
		free(r_pidecptr);

	r_pidecptr = NULL;

	return;
}

/*====================================================================
函数名：      g729_encode
功  能：      编码输入数据
引用全局变量：无
输入参数说明：见下
返回 值 说明：错误 G729_ERR  成功 G729_OK
====================================================================*/
l32  g729_encode(HG729BENC r_piEncptr, //input: the handle of the encoder
				 s16  *ps16InData, // input: encoder data
				 s16  l32InLen, //input: encoder data length
				 u8 *pvOutBuf, //output: encoded data
				 s16  *pl32OutLen) //output: length in s8 unit.
{
	l32  i, outLen;
	s16 * inDataTmp;
	s16 inDataLenTmp, frameLen;
	
	/*if(l32InLen != L_FRAME)
		return G729_ERR;*/

	if((NULL == ps16InData)||(NULL == pvOutBuf)||(NULL == pl32OutLen)||(NULL == r_piEncptr))
		return G729_ERR;

	//ec-2.2
	if('E' != ((TG729BENCODER *)r_piEncptr)->s8Version[0])
		return G729_TYPEERR;
	if('2' != ((TG729BENCODER *)r_piEncptr)->s8Version[3])
		return G729_VERERR;
	if('2' != ((TG729BENCODER *)r_piEncptr)->s8Version[5])
		return G729_VERERR;

	//*pl32OutLen = 0;
	outLen = 0;

	inDataTmp = ps16InData;
	inDataLenTmp = l32InLen;

    // 一帧一帧编码 
    while (inDataLenTmp >= L_FRAME)
    {
        if (((TG729BENCODER *)r_piEncptr)->Frame == 32767) 
	    {
		    ((TG729BENCODER *)r_piEncptr)->Frame = 256;
	    }
	    else
	    {
		    ((TG729BENCODER *)r_piEncptr)->Frame++;
	    }

	    for (i = 0; i < L_FRAME; i++)  
		    ((TG729BENCODER *)r_piEncptr)->new_speech[i] = inDataTmp[i];
	
        //low and high pass, processed signal is stored in new_speech. 
         Pre_Process( ((TG729BENCODER *)r_piEncptr)->new_speech, L_FRAME,(TG729BENCODER*)r_piEncptr);

         /*test code*/
	     //s16fprt(((TG729BENCODER *)r_piEncptr)->new_speech, L_FRAME, "as16Speech", &l32PrtFlag, 1);
	     //calculate ai
	     Coder_ld8a(((TG729BENCODER *)r_piEncptr)->s16Prm, 
		            ((TG729BENCODER *)r_piEncptr)->Frame,
		            ((TG729BENCODER *)r_piEncptr)->vad_enable,
		            (TG729BENCODER*)r_piEncptr);

		/*test code*/
		//s16fprt(((TG729BENCODER *)r_piEncptr)->s16Prm, 12, "as16Prm", &l32PrtFlag, 0);

        prm2bits_ld8k(((TG729BENCODER *)r_piEncptr)->s16Prm , ((TG729BENCODER *)r_piEncptr)->s16Serial);

        frameLen = ((TG729BENCODER *)r_piEncptr)->s16Serial[1] / 8;

    	for (i =0; i < frameLen; i++)
	    {
		    pvOutBuf[outLen] = bin2int(8 , ((TG729BENCODER *)r_piEncptr)->s16Serial + 2 + 8 * i);
		    outLen++;
	    }
	    
	    inDataTmp+=L_FRAME;
	    inDataLenTmp-=L_FRAME;
    }
    
    *pl32OutLen = outLen;

	return G729_OK;
}

////////////////////////////////////////////
//          decoder routine               // 
////////////////////////////////////////////

static void twrite16(
 s16 * data,           /* input: inputdata */
 s16   length,          /* input: length of data array */
 s16 * ps16OutData	,		//output: decoded data
 l32 * pl32OutLen			//output: length in word unit.
);

static void int2bin(
					l32  value,
					l32  no_of_bits,
					s16 *bitstream
					);
/*====================================================================
函   数   名:  g729_decinit
功        能:  初始化解码器句柄 
引用全局变量:  无
输入参数说明:
			 1-(i/o)r_poDecptr    解码器句柄指针
返回 值 说明:  
               成功:G729_OK
               失败:G729_ERR
特 殊 说  明:  无
======================================================================*/
l32  g729_decinit(HG729BDEC * r_poDecptr) 
{
	l32 i;
	if(NULL == r_poDecptr)
	{
		return G729_ERR; //could not be NULL
	}
	
	//malloc handle for decoder.
	*r_poDecptr = (HG729BDEC) malloc(sizeof(TG729BDECODER));
	
	if(NULL == *r_poDecptr)
	{
		return G729_ERR; //Not enough memoery
	}
	
	memset(*r_poDecptr,0,sizeof(TG729BDECODER));
	
	((TG729BDECODER *)*r_poDecptr)->length = sizeof(TG729BDECODER);
	
	//D: decoder, must less than 9 chars.
	sprintf(((TG729BENCODER *)*r_poDecptr)->s8Version,"%s","FDC-2.2");
	
	for (i=0; i<M; i++)
		((pTG729BDECODER) * r_poDecptr)->synth_buf[i] = (s16)0;
	
	((pTG729BDECODER) * r_poDecptr)->synth = ((pTG729BDECODER) * r_poDecptr)->synth_buf + M;
	
	for (i = 0;i <M; i++)
	{
		((TG729BDECODER *)*r_poDecptr)->lsp_old[i] =  Lsp_static[i]; //from file COD_LD8K.C
	}
	Init_Decod_ld8a((TG729BDECODER*)* r_poDecptr);
	Init_Post_Filter((TG729BDECODER*)* r_poDecptr);
	Init_Post_Process((TG729BDECODER*)* r_poDecptr);

	
	//((pTG729BDECODER) * r_poDecptr)->voicing = 60;
	for(i=0; i<4; i++)
	{
		((pTG729BDECODER) * r_poDecptr)->past_qua_en[i] = -14336;
	}
	
	((pTG729BDECODER) *r_poDecptr)->past_gain=4096; 

	// for G.729b
	Init_Dec_cng((TG729BDECODER*)*r_poDecptr);

	return G729_OK;
}

/*====================================================================
函   数   名:  g729_decfree
功        能:  释放解码器句柄 
引用全局变量:  无
输入参数说明:
			 1-(i)r_poDecptr    解码器句柄
返回 值 说明:  无
特 殊 说  明:  无
======================================================================*/
void g729_decfree(HG729BDEC r_poDecptr)
{
	if(r_poDecptr != NULL)
		free(r_poDecptr);

	r_poDecptr = NULL;
	
	return;
}


/*====================================================================
	函数名	：g729_decode
	功能		：解码输入数据
	算法实现	：G729
	引用全局变量：无
输入参数说明：见下
	返回值说明：错误 G729_ERR 成功 G729_OK
====================================================================*/
l32  g729_decode(
				 HG729BDEC r_hoDecptr,			//input: the input decoder handle
				 u8 *ps8InStream,	            //input: the decoder data
				 s16 l32InLen,					//input: length of the input length
				 s16 *ps16OutData,				//output: decoded data
				 s16 *pl32OutLen
				 )			//output: length in s8 unit.
				
{
	l32 i = 0;
	
	s16 Vad = 0;
	s16 s16LenChar = 0;
	
	s16 *ps16Trans = NULL;
	s8 *ps8Buf = NULL;

	u8 * inDataTmp;
	s16 inDataLenTmp, outLen;
    l32 frameLen;
	
	pTG729BDECODER pp = (pTG729BDECODER)r_hoDecptr;
	
	//test to see if it is valid.
	if( (! r_hoDecptr)||(!ps8InStream ) || (! ps16OutData) ||(!pl32OutLen) )
		return G729_ERR;
	
	if('F'!=pp->version[0])
		return G729_TYPEERR;
	if('D'!=pp->version[1])
		return G729_TYPEERR;
	if('2'!=pp->version[4])
		return G729_VERERR;
	if('2'!=pp->version[6])
		return G729_VERERR;

	*pl32OutLen = 0;

	s16LenChar = 10; //get data length

	inDataTmp = ps8InStream;
    inDataLenTmp = l32InLen;
    outLen = 0;

    while (inDataLenTmp >= s16LenChar)
    {
   	    ps16Trans =  pp->bitconver + 1;
	
    	//add some input data to the buffer cache
	    //if there is some input data add this to the cache.
  		if( BUFFERLENGTH <( pp->DataLength + s16LenChar))
    	{
	    	return G729_BUFFERFULL; //buffer is full now. it should call decoder without data input. 
	    }
	    ps8Buf = pp->DataBuffer + pp->DataLength;
		
	    memcpy(ps8Buf,inDataTmp,s16LenChar);
		
	    pp->DataLength += s16LenChar;
	
	    //check if there are enough data
	    if(0 ==pp->DataLength )
		    return G729_NOTENOUGHDATA; //Buffer is empty. Must send some input data to the decoder. 
	
    	ps8Buf = pp->DataBuffer;
	
	    for(i = 0;i < s16LenChar;i ++)
	    {
		    int2bin(*(ps8Buf + i ) , 8 , ps16Trans + i * 8);
    	}

	    ps16Trans[-1] = s16LenChar*8; //length should be sent to function bits2prm_ld8k;
	    pp->parm[0] = 0;           // No frame erasure 
	    bits2prm_ld8k_org( pp->bitconver, &pp->parm[0]);
	
	    if(ps16Trans[-1] != 0) 
	    {
		    for(i=0; i<ps16Trans[-1]; i++)
		    {
			    if(pp->bitconver[i+1] == 0 )
				    pp->parm[0] = 1;  // frame erased
		    }
	    }
	
	    if(pp->parm[1] == 1)
	    {
		    //check parity and put 1 in parm[5] if parity error
		    pp->parm[5] = Check_Parity_Pitch(pp->parm[4], pp->parm[5]);
	    }
	
	    memcpy(pp->DataBuffer,
		    pp->DataBuffer + s16LenChar, 
		    pp->DataLength - s16LenChar);
	
	    pp->DataLength = pp->DataLength - s16LenChar;
	
	    Decod_ld8a(pp->parm, 
		    pp->synth, 
		    pp->Az_dec, 
		    pp->t2,
		    &Vad,
		    (TG729BDECODER*)r_hoDecptr);  //* Decoder /
	
	    //Post-filter and decision on voicing parameter 
	    Post_Filter(pp->synth, pp->Az_dec, 
		    pp->t2, Vad,(TG729BDECODER*)r_hoDecptr);
	
	    Post_Process(pp->synth, L_FRAME,(TG729BDECODER*)r_hoDecptr);
	
	    twrite16(pp->synth, L_FRAME,
		    ps16OutData+outLen,
		    &frameLen);
	    
	    outLen+=frameLen;
	    
	    inDataTmp+=s16LenChar;
	    inDataLenTmp-=s16LenChar;
    }
    
    *pl32OutLen = outLen;
	
	return G729_OK;
}

/*====================================================================
函   数   名:  int2bin
功        能:  convert integer to binary and write the bits bitstream array
引用全局变量:  无
输入参数说明:
			 1-(i)value         decimal value
			 2-(i)no_of_bits    number of bits to use 
			 3-(o)bitstream     bitstream

返回 值 说明:  无
特 殊 说  明:  无
======================================================================*/
static void int2bin(
					l32  value,
					l32  no_of_bits,
					s16 *bitstream
					)
{
	s16 *pt_bitstream;
	l32    i, bit;
	
	pt_bitstream = bitstream + no_of_bits;
	
	for (i = 0; i < no_of_bits; i++)
	{
		bit = value & 0x0001;      /* get lsb */
		if (bit == 0)
			*--pt_bitstream = BIT_0;
		else
			*--pt_bitstream = BIT_1;
		value >>= 1;
	}
	return;
}


static void twrite16(
 s16 * data,           /* input: inputdata */
 s16   length,          /* input: length of data array */
 s16 * ps16OutData	,		//output: decoded data
 l32 * pl32OutLen			//output: length in word unit.
 //FILE *fp               /* input: file pointer */
)
{
   s16  i;
   s16 sp16[L_FRAME];
   double temp;

   if (length > L_FRAME) {
      printf("error in fwrite16\n");
     return;
   }

   for(i=0; i<length; i++)
    {
      /* round_linux and convert to l32  */
      temp = data[i];
      if (temp >= (double)0.0)
            temp += (double)0.5;
      else  temp -= (double)0.5;
      if (temp >  (double)32767.0 ) temp =  (double)32767.0;
      if (temp < (double)-32768.0 ) temp = (double)-32768.0;
      sp16[i] = (s16) temp;
    }
   // fwrite( sp16, sizeof(s16), length, fp);
   * pl32OutLen = length;
   memcpy(ps16OutData,sp16,length * sizeof(s16));
}

