package com.witted.ptt;

import android.util.Log;

import java.net.DatagramSocket;
import java.net.SocketException;

public class SocketEngine {


    private static final String TAG = "SocketEngine";
    private DatagramSocket mDatagramSocket;




    public DatagramSocket initSocket(){
        try {
           return new DatagramSocket();

        } catch (SocketException e) {
            Log.i(TAG, "initSocket: "+e.getMessage());
            e.printStackTrace();
        }
        return null;
    }

    public int getPort(){
        if(mDatagramSocket!=null){
            return mDatagramSocket.getPort();
        }

        return 0;
    }

    public void release(){
        if(mDatagramSocket==null){
            return;
        }
        mDatagramSocket.close();
        mDatagramSocket=null;
    }

}
