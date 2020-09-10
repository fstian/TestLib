package com.witted.ptt;


import android.util.Log;

/**
 * Created by YT on 2017.8.4.
 */

public class CodeUtils {
//
    static {
        try {
            System.loadLibrary("codeutils");
            G729Init(0);
        }catch (Exception e){
            Log.i("codeutils", "static initializer: "+"jni加载失败");
        }


    }

    public native static String GetG729Ver(int i);
    public native static int G729ToLinear(byte[] src,short srcsize,short[] dst);
    public native static int LinearToG729(short[] src,short srcsize,byte[] dst);
    public native static int G729Init(int i);
    public native static int G929DeInit(int i);





}
