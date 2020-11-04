package com.witted.netty;

import android.util.Log;

import com.witted.bean.BaseReq;
import com.witted.bean.RegisterRequest;
import com.witted.constant.MsgType;
import com.witted.ptt.Call;
import com.witted.ptt.CallManager;
import com.witted.ptt.CommonUtils;

import java.util.ArrayList;

import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.handler.timeout.IdleState;
import io.netty.handler.timeout.IdleStateEvent;

public class HeartBeatHandler extends ChannelInboundHandlerAdapter {

    private static final String TAG = HeartBeatHandler.class.getName();


    @Override
    public void userEventTriggered(ChannelHandlerContext ctx, Object evt) throws Exception {
        super.userEventTriggered(ctx, evt);
        Log.i(TAG, "userEventTriggered: ");


        if (NettyManager.INST.getRegisterStatus()) {
            RegisterRequest registerRequest = new RegisterRequest();
            registerRequest.deviceID = CallConfig.getInstance().getLocalDeviceId();
            registerRequest.deviceType = CallConfig.getInstance().getDeviceType();
            registerRequest.localIP = CommonUtils.getLocalIP();
            registerRequest.expireTime = NettyConfig.EXPIRETIME;

            ArrayList<Call> calls = CallManager.getInstance().getCalls();
            //每次心跳上传呼叫状态
            if (calls.size() == 0) {
                //客户端idle状态
                registerRequest.callStatus = 0;
                registerRequest.callID="";
            } else {
                Call call = calls.get(0);
                registerRequest.callStatus = call.getState().getState();
                String callID = call.getCallID();
                registerRequest.callID=callID;
            }
            BaseReq<RegisterRequest> request = new BaseReq<>(MsgType.REGISTER, registerRequest);
            ctx.channel().writeAndFlush(request);
        }

        if (evt instanceof IdleStateEvent) {
            IdleStateEvent event = (IdleStateEvent) evt;
            Log.i(TAG, "userEventTriggered: " + event);
            if (event.state() == IdleState.READER_IDLE) {
                ctx.close();
            }
        }
    }

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        super.channelActive(ctx);
    }

    @Override
    public void channelInactive(ChannelHandlerContext ctx) throws Exception {
        super.channelInactive(ctx);

        ctx.fireChannelActive();
    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {
        super.channelRead(ctx, msg);

    }


}
