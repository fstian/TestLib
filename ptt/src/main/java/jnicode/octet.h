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

/* Definition for Octet Transmission mode */
/* When Annex B is used for transmission systems that operate on octet boundary, 
   an extra bit (with value zero) will be packed at the end of a SID bit stream. 
   This will change the number of bits in a SID bit stream from 15 bits to
   16 bits (i.e., 2 bytes).
*/
#ifndef OCTET_H
#define OCTET_H

#ifdef __cplusplus
extern "C" {
#endif

#define OCTET_TX_MODE
#define RATE_SID_OCTET    16     /* number of bits in Octet Transmission mode */

#ifdef __cplusplus
}
#endif

#endif //end of OCTET_H

