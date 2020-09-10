package com.witted.bean;

public class CommonResp {

    public int status;

    public String result;

    public String callID;

    @Override
    public String toString() {
        return "CommonResp{" +
                "status=" + status +
                ", result='" + result + '\'' +
                ", callID='" + callID + '\'' +
                '}';
    }
}
