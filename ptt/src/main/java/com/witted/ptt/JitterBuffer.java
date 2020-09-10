package com.witted.ptt;

import android.util.Log;



public class JitterBuffer {


    private static final String TAG ="JitterBuffer" ;

    static {
        System.loadLibrary("jitterbuffer");
        initJb();
        Log.i(TAG, "static initializer: ");
    }

    public static native int initJb();

    public static native int openJb(int codec,int ptime);
    public static native int closeJb(int jbline);

    public static native int addPackage(int jbline,byte[] src,int size);

    public static native int getPackage(int jbline,byte[] dst,int size);
    public static native int getStatus(int jbline,byte[] dst,int size);
    public static native int getJbVer();
    public static native int deInitJb();
}
