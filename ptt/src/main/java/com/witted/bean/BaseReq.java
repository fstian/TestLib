package com.witted.bean;

import com.witted.netty.CallConfig;
import com.witted.netty.NettyManager;

public class BaseReq<T> {

    public int msgType;

    public String msgID;

    public String  senderID;

    public String receiverID;

    public T context;

    public BaseReq() {
    }



    public BaseReq(int msgType, String msgID,  T context) {
        this.msgType = msgType;
        this.msgID = msgID;
        this.senderID = CallConfig.getInstance().getLocalDeviceId();
        this.receiverID = CallConfig.getInstance().getHostID();
        this.context = context;
    }


    public BaseReq(int msgType,T context) {
        this.msgType = msgType;
        this.msgID = NettyManager.getMsgId();
        this.senderID = CallConfig.getInstance().getLocalDeviceId();
        this.receiverID = CallConfig.getInstance().getHostID();
        this.context = context;
    }

    @Override
    public String toString() {
        return "BaseReq{" +
                "msgType=" + msgType +
                ", msgID='" + msgID + '\'' +
                ", senderID='" + senderID + '\'' +
                ", receiverID='" + receiverID + '\'' +
                ", context=" + context +
                '}';
    }
}
