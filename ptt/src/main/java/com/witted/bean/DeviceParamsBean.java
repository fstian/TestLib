package com.witted.bean;


import androidx.annotation.NonNull;

public class DeviceParamsBean {


    /**
     * param_id : CallAlert
     * param_name : 呼叫是否响铃
     * param_val : 0
     * unit : 0/1
     */
    private String deviceID;
    private String param_id;

    private String param_name;
    private String param_val;
    private String unit;

    @Override
    public String toString() {
        return "DeviceParamsBean{" +
                "deviceID='" + deviceID + '\'' +
                ", param_id='" + param_id + '\'' +
                ", param_name='" + param_name + '\'' +
                ", param_val='" + param_val + '\'' +
                ", unit='" + unit + '\'' +
                '}';
    }

    public String getDeviceID() {
        return deviceID;
    }

    public void setDeviceID(String deviceID) {
        this.deviceID = deviceID;
    }

    public String getParam_id() {
        return param_id;
    }

    public void setParam_id(String param_id) {
        this.param_id = param_id;
    }

    public String getParam_name() {
        return param_name;
    }

    public void setParam_name(String param_name) {
        this.param_name = param_name;
    }

    public String getParam_val() {
        return param_val;
    }

    public void setParam_val(String param_val) {
        this.param_val = param_val;
    }

    public String getUnit() {
        return unit;
    }

    public void setUnit(String unit) {
        this.unit = unit;
    }

}
