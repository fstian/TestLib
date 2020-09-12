package com.witted.netty;

import com.alibaba.fastjson.JSONObject;
import com.witted.bean.BaseReq;
import com.witted.bean.RegisterResp;
import com.witted.constant.MsgType;
import com.witted.constant.StatusCode;

import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import timber.log.Timber;

public class ClientHandler extends ChannelInboundHandlerAdapter {


    @Override
    public void channelRegistered(ChannelHandlerContext ctx) throws Exception {
        super.channelRegistered(ctx);


        Timber.i("channelRegistered: %s" , Thread.currentThread().getName());
    }


    @Override
    public void channelUnregistered(ChannelHandlerContext ctx) throws Exception {
        super.channelUnregistered(ctx);
        Timber.i("channelUnregistered: %s" , Thread.currentThread().getName());
        NettyManager.INST.setRegisterFail(StatusCode.TCP_CONNECT_ERR,"channelUnregistered");
    }

    @Override
    public void channelInactive(ChannelHandlerContext ctx) throws Exception {
        super.channelInactive(ctx);

//        NettyManager.INST.setRegisterFail("channelInactive");

    }

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {

        NettyManager.INST.setConnectSuccess();

    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg)  {
        Timber.i("channelRead: %s" , msg);

        BaseReq resp = (BaseReq) msg;

        if(resp.msgType== MsgType.REGISTER_RESP){

            RegisterResp registerResp = ((JSONObject) ((BaseReq) msg).context).toJavaObject(RegisterResp.class);

            if(registerResp.status==200){
                NettyManager.INST.setRegisterSuccess();
            }else {
                NettyManager.INST.setRegisterFail(registerResp.status,registerResp.text);
            }

        }

        NettyManager.INST.onMsgReceive(msg);
    }



    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        ctx.disconnect();
        ctx.close();
//        NettyManager.INST.setRegisterFail(""+cause.getMessage());
    }






}
