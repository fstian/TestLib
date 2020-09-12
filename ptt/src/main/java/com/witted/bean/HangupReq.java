package com.witted.bean;

public class HangupReq {


    public String callID;

    /**
     * 对方的deviceID
     */
    public String callee;

    @Override
    public String toString() {
        return "HangupReq{" +
                "callID='" + callID + '\'' +
                ", callee='" + callee + '\'' +
                '}';
    }
}
