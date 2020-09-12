package com.witted.bean;

public class RegisterRequest {

    public String deviceID;

    public int deviceType;

    public String localIP;

    public int expireTime;

    @Override
    public String toString() {
        return "RegisterRequest{" +
                "localDeviceID='" + deviceID + '\'' +
                ", deviceType=" + deviceType +
                ", localIP='" + localIP + '\'' +
                ", expireTime=" + expireTime +
                '}';
    }
}
