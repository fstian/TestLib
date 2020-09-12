
/*****************************************************************************
   ģ����      : Algorithm type 
   �ļ���      : algorithmType.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �㷨���㷨�����������Ͷ���
   ����        : ��ѩ��
   �汾        : V3.6  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2005/03/29  3.6         ��ѩ��        ����
   2005/04/15  3.6         ��  ��      �������"mm.h"�ĳ�ͻ
   2005/04/18  3.6         ��  ��      �����ʹ��timing.hʱ��"basictype.h"�ĳ�ͻ
   2005/04/22  3.6         ½ѩ��	   �������u32�ϲ㶨��͵ײ㶨���ͻ����
   2005/04/28  3.6		   ½ѩ��	   �������s32,int32_t, uint32_t�ϲ㶨��͵ײ㶨���ͻ����
   2005/08/31  4.0         ��  ��      �������linux�汾ʱ�����Ͷ����ͻ
******************************************************************************/
#ifndef _ALGORITHM_TYPE_H_
#define _ALGORITHM_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

/*�Ƽ�ʹ��*/
typedef char            s8;            
typedef short           s16;
typedef int               l32;

typedef unsigned char   u8;   
typedef unsigned short  u16;
typedef unsigned int    u32;
//end Modify

//typedef int BOOL; //xuchao modify
#if !defined(TYPE_BOOL) && !defined(__INCvxTypesh)
typedef int BOOL, *PBOOL;
#define TRUE 1
#define FALSE 0
#define TYPE_BOOL
#endif /* BOOL */

#ifdef _MSC_VER
typedef __int64			s64;
typedef unsigned __int64        u64;
#else 
typedef long 		s64;
typedef unsigned long      u64;
#endif 

typedef double  d64;

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

//add by luxz
#ifndef NULL
  #ifdef  __cplusplus
    #define NULL    0
  #else
    #define NULL    ((void *)0)
  #endif
#endif

#ifdef _MSC_VER
  #define INLINE __inline 
#else
  #define INLINE inline
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _ALGORITHM_TYPE_H_ */
