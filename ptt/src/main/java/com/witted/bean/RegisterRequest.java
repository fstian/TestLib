package com.witted.bean;

public class RegisterRequest {

    public String deviceID;

    public int deviceType;

    public String localIP;

    public int expireTime;

    public int callStatus;

    @Override
    public String toString() {
        return "RegisterRequest{" +
                "deviceID='" + deviceID + '\'' +
                ", deviceType=" + deviceType +
                ", localIP='" + localIP + '\'' +
                ", expireTime=" + expireTime +
                ", callStatus=" + callStatus +
                '}';
    }
}
