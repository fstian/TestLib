package com.witted.constant;

public interface MsgType {

    /**
     * 注册和心跳
     */
    int REGISTER=1;


    /**
     * 注册应答
     */
    int REGISTER_RESP=101;

    /**
     * 呼叫请求
     */
    int CALLINCOMING =2;

    /**
     * 呼叫应答
     */
    int CALLINCOMING_RESP =102;

    /**
     * 接听请求
     */
    int CALLACCEPT=3;


    /**
     * 接听应答
     */
    int CALLACCEPT_RESP=103;


    /**
     * 结束请求
     */
    int CALLEND=4;


    /**
     * 结束应答
     */
    int CALLEND_RESP=104;


    /**
     * 呼叫查询
     */


    /**
     * 获取设备列表
     */
    int GETDEVICE=6;

    /**
     * 获取设备列表应答
     */
    int GETDEVICE_RESP=106;

    /**
     * 床头屏在线离线推送
     */


    /**
     * 获取在线设备列表
     */
    int GETONLINEDEVICE=7;

    int GETONLINEDEVICE_RESP=107;







}
