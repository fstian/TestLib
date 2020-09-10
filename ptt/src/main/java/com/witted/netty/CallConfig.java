package com.witted.netty;

import android.text.TextUtils;

import com.witted.constant.CallDirect;
import com.witted.constant.CallType;
import com.witted.constant.CodeType;
import com.witted.constant.DeviceType;

public class CallConfig {


    /**
     * 本机的设备ID    每次重启的时候需要初始化
     */
    private   String localDeviceId ;

    /**
     * 管理模块的设备id,固定
     */
    public  final String hostID="host";

    /**
     * 设备类型
     */
    public  int deviceType =0;


    /**
     * 呼叫方向
     */
    public   int callDirect =0 ;


    /**
     * 呼叫类型  只有卫生间的呼叫是紧急呼叫   不可修改
     */
    public  final int callType = CallType.NORMAL_CALL;


    /**
     * 语音编码,现在默认是729
     */
    public  int codec= CodeType.JB_G729_CODEC;


    /**
     * 呼叫id  本机设备id+时间戳
     * @return
     */
    public  String getCallID(){

        long l = System.currentTimeMillis() / 1000;

        return localDeviceId+"_"+l;
    }



    private static CallConfig instance;

    public static CallConfig  getInstance(){

        if(instance==null){
            synchronized (CallConfig.class){
                if(instance==null){
                    instance=new CallConfig();
                }
            }
        }
        return instance;

    }


    public String getLocalDeviceId() {
        return localDeviceId;
    }

    public void setLocalDeviceId(String localDeviceId) {
        this.localDeviceId = localDeviceId;
    }

    public int getDeviceType() {
        if(callDirect==0){
            throw new RuntimeException("设备类型没有初始化");
        }

        return deviceType;
    }

    public void setDeviceType(int deviceType) {
        this.deviceType = deviceType;
    }

    public int getCallType() {
        return callType;
    }


    public int getCallDirect() {

        if(callDirect==0){
            throw new RuntimeException("呼叫方向没有初始化");
        }
        return callDirect;
    }

    public void setCallDirect(int callDirect) {
        this.callDirect = callDirect;
    }

    public int getCodec() {
        return codec;
    }

    public void setCodec(int codec) {
        this.codec = codec;
    }

    public  String getHostID() {
        return hostID;
    }
}
