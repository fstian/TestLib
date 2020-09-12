package com.witted.bean;

public class OnLineDevice {



    public String deviceID;


    public String bedName;


    public String getDeviceID() {
        return deviceID;
    }

    public void setDeviceID(String deviceID) {
        this.deviceID = deviceID;
    }

    public String getBedName() {
        return bedName;
    }

    public void setBedName(String bedName) {
        this.bedName = bedName;
    }


    @Override
    public String toString() {
        return "OnLineDevice{" +
                "localDeviceID='" + deviceID + '\'' +
                ", bedName='" + bedName + '\'' +
                '}';
    }
}
