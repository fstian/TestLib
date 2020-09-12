#include <stdio.h>
#include "com_witted_ptt_CodeUtils.h"

#include "g729ab.h"

static HG729BENC G729BEncHd;
static HG729BDEC G729BDecHd;


/*
 * Class:     com_dgk_jnitest_CodeUtils
 * Method:    GetG729Ver
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_witted_ptt_CodeUtils_GetG729Ver
(JNIEnv *env, jobject obj,jint i)
{
    unsigned char version[200];
    long usedsize;

    short src[160];
    unsigned char encode[100];
    short decode[160];
    short encodelen;
    short decodelen;
    int itmp;

    GetG729Version(version,200,&usedsize);

    return (*env)->NewStringUTF(env, (char *)version);
}

/*
 * Class:     com_dgk_jnitest_CodeUtils
 * Method:    G729ToLinear
 * Signature: ([BI[S)I
 */
JNIEXPORT jint JNICALL Java_com_witted_ptt_CodeUtils_G729ToLinear
 (JNIEnv *env, jobject obj, jbyteArray src, jshort srcsize, jshortArray dst)
{
    unsigned char *psrc;
    jbyte *srcjbarray;
    jsize srclen;
    jsize dstlen;
    short decodesize=0;
    unsigned short arDecode[400];

    srclen = (*env)->GetArrayLength(env,src);
    srcjbarray = (*env)->GetByteArrayElements(env,src,NULL);
    psrc = (unsigned char *)srcjbarray;

    dstlen = (*env)->GetArrayLength(env,dst);

    g729_decode(G729BDecHd,psrc,srclen,arDecode,&decodesize);

    (*env)->SetShortArrayRegion(env,dst,0,decodesize,arDecode);

     return  decodesize;
}



/*
 * Class:     com_dgk_jnitest_CodeUtils
 * Method:    LinearToG729
 * Signature: ([SI[B)I
 */
JNIEXPORT jint JNICALL Java_com_witted_ptt_CodeUtils_LinearToG729
(JNIEnv *env, jobject obj, jshortArray src, jshort srcsize, jbyteArray dst)
{
    short *psrc;
    jshort *srcjsarray=NULL;
    jsize srclen;
    jsize dstlen;
    short encodesize=0;
    unsigned char arEncode[100];

    srclen = (*env)->GetArrayLength(env,src);
    srcjsarray = (*env)->GetShortArrayElements(env,src,NULL);
    psrc = (short *)srcjsarray;

    dstlen = (*env)->GetArrayLength(env,dst);

    g729_encode(G729BEncHd,psrc,srclen,arEncode,&encodesize);

    (*env)->SetByteArrayRegion(env,dst,0,encodesize,arEncode);

    return encodesize;
}


/*
 * Class:     com_dgk_jnitest_CodeUtils
 * Method:    G729Init
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_witted_ptt_CodeUtils_G729Init
 (JNIEnv *enav, jobject obj, jint i)
 {
     g729_encinit(&G729BEncHd);
     g729_decinit(&G729BDecHd);
     return 0;
 }

/*
 * Class:     com_dgk_jnitest_CodeUtils
 * Method:    G929DeInit
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_witted_ptt_CodeUtils_G929DeInit
   (JNIEnv *env, jobject obj, jint i)
 {
     g729_encfree(G729BEncHd);
     g729_decfree(G729BDecHd);
     return 0;
 }