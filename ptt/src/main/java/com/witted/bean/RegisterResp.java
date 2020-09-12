package com.witted.bean;

import java.util.List;

public class RegisterResp {
    public int status;


    public String text;

//    public RegisterBean result;

    private List<DeviceParamsBean> params;

    public RegisterResp() {
    }

    public RegisterResp(int status, String text) {
        this.status = status;
        this.text = text;

    }


    public RegisterResp(int status, String text, List<DeviceParamsBean> params) {
        this.status = status;
        this.text = text;
        this.params = params;
    }

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
    }

    public List<DeviceParamsBean> getParams() {
        return params;
    }

    public void setParams(List<DeviceParamsBean> params) {
        this.params = params;
    }

    public static RegisterResp registerOK(List<DeviceParamsBean> params){

        return new RegisterResp(200,"register ok",params);
    }

    public static RegisterResp heartBeatBack(List<DeviceParamsBean> params){
        return new RegisterResp(200,"heart back");
    }

    public static RegisterResp registerFail(String err){
        return new RegisterResp(400,err);

    }




}
