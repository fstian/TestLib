package com.witted.netty;

import com.witted.bean.BaseReq;
import com.witted.bean.RegisterRequest;
import com.witted.constant.MsgType;
import com.witted.constant.StatusCode;
import com.witted.ptt.Call;
import com.witted.ptt.CallManager;
import com.witted.ptt.CommonUtils;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

import timber.log.Timber;

public class HeartBeatTimer {

    int count =0;

    private final Timer mTimer;

    public HeartBeatTimer() {
        mTimer = new Timer();
        mTimer.schedule(mTimerTask,0,5*1000);
    }


    TimerTask mTimerTask=new TimerTask() {
        @Override
        public void run() {
            Timber.i("mTimerTask");
            if(!NettyManager.INST.getRegisterStatus()){
                count++;
            }else {
                count=0;
            }
            if(count>0&&count%6==0){
                NettyManager.INST.setRegisterFail(StatusCode.TCP_CONNECT_REGISTER_FAILED,"定时失败回调");
            }
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
                NettyClient.getInstance().sendMsg(request);
            }


        }
    };


    public void cancel(){
        mTimer.cancel();
    }

}
