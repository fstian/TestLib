package com.witted.ptt;

public class JitterBuffer {

    static {
        System.loadLibrary("JitterBuffer");
        getInstance();
    }

    Object mObject=new Object();


    static JitterBuffer jb;


    public JitterBuffer() {
        initJb();
    }

    public static JitterBuffer getInstance(){
        if(jb==null){
            jb=new JitterBuffer();
        }
        return jb;
    }



    private native int InitJb();
    private native int OpenJb(int codec,int ptime);
    private native int CloseJb(int jbline);
    private native int ResetJb(int jbline);
    private native int AddPackage(int jbline,byte[] src,int size);
    private native int GetPackage(int jbline,byte[] dst,int size);
    private native int GetStatus(int jbline,byte[] dst,int size);
    private native int GetJbVer();
    private native int DeInitJb();

    public int initJb(){
        synchronized (mObject)
        {
            return InitJb();
        }
    }

    public int openJb(int codec,int ptime){
        synchronized (mObject)
        {
            return OpenJb(codec,ptime);
        }
    }

    public int resetJb(int jbline){
        synchronized (mObject)
        {
            return ResetJb(jbline);
        }
    }

    public  int closeJb(int jbline){
        synchronized (mObject)
        {
            return CloseJb(jbline);
        }
    }

    public int addPackage(int jbline,byte[] src,int size){
        synchronized (mObject)
        {
            return AddPackage(jbline,src,size);
        }
    }

    public  int getPackage(int jbline,byte[] dst,int size){
        synchronized (mObject)
        {
            return GetPackage(jbline,dst,size);
        }
    }
    public int getStatus(int jbline,byte[] dst,int size){
        synchronized (mObject)
        {
            return GetStatus(jbline,dst,size);
        }

    }
    public int getJbVer(){
        synchronized (mObject)
        {
            return GetJbVer();
        }
    }
    public int deInitJb(){
        synchronized (mObject)
        {
            return DeInitJb();
        }

    }

}
