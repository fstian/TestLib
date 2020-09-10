package com.witted.bean;

public class AcceptReq {

    //
    public String callID;


    /**
     * 拨号人deviceID
     */

    public String caller;

    /**
     * 接听方deviceID
     */
    public String callee;

    /**
     * 接听方ip
     */
    public String calleeIP;

    /**
     * 接听方设备端口
     */
    public int calleePort;

    /**
     * 接听方设备类型
     */
    public int calleeType;


}
