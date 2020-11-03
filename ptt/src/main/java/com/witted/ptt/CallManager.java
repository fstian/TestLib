package com.witted.ptt;

import com.witted.bean.AcceptReq;
import com.witted.bean.BaseReq;
import com.witted.bean.CallReq;
import com.witted.bean.HangupReq;
import com.witted.constant.DeviceType;
import com.witted.constant.MsgType;
import com.witted.netty.CallConfig;
import com.witted.netty.NettyManager;
import com.witted.remote.GeneralCallback;

import java.util.ArrayList;

import timber.log.Timber;

public class CallManager {


    private static CallManager sCallManager;

    private ArrayList<Call> mCalls = new ArrayList<>();


    public static synchronized CallManager getInstance() {
        if (sCallManager == null) {
            sCallManager = new CallManager();
            return sCallManager;
        }
        return sCallManager;
    }

    public ArrayList<Call> getCalls() {
        return mCalls;
    }


    public Call getCurrentCall() {


        for (Call call : mCalls) {

            Call.State state = call.getState();
            if (state == Call.State.Connected || state == Call.State.StreamsRunning) {

                return call;
            }
        }


        return null;

    }

    /**
     * 挂断电话
     * @param call  当前通话
     */
    public void hangupCall(Call call) {

        if (call != null) {
            call.setState(Call.State.CallEnd);
            HangupReq hangupReq = new HangupReq();
            hangupReq.callID = call.getCallID();
            hangupReq.callee = call.getRemoteDeviceID();
            BaseReq<HangupReq> hangupReqBaseReq = new BaseReq<>(MsgType.CALLEND, hangupReq);
            NettyManager.INST.sendMsg(hangupReqBaseReq);
            NettyManager.INST.handlerCallState(call,call.getState());
            call.stopCall();
        }

        //如果是床头屏,挂断电话的时候把call都清除
        if(CallConfig.getInstance().deviceType== DeviceType.BED_SCREEN){
            CallManager.getInstance().clearAllCalls();
        }

    }


    /**
     * @param callerName 呼叫人的姓名 床头卡有
     * @param age 呼叫人的年龄  床头卡有
     * @param bedName 若呼叫护士台  bedName可为空
     * @param caller  呼叫人的设备id  此设备
     * @param callee  被叫人的设备id  如果是床头卡呼叫护士台  则为"FFFFFFF"
     * @return 通话
     *
     */
    public Call createCall(String callerName, String age, String bedName, String caller, String callee)  {

        Call call;

        try {
            call = new Call();
            call.setBedName(bedName);
            call.setCodec(CallConfig.getInstance().getCodec());
            call.setCallID(CallConfig.getInstance().getCallID());
            call.setCallee(callee);
            call.setCaller(caller);
            call.setName(callerName);
            call.setAge(age);
            call.setCodec(CallConfig.getInstance().getCodec());

            call.setState(Call.State.OutGoingInit);
            call.initSocket();
//            call.initCall(CallConfig.getInstance().getCodec());


            //发送呼叫消息
            CallReq callReq = new CallReq();
            callReq.callType = CallConfig.getInstance().getCallType();
            callReq.callDirect = CallConfig.getInstance().getCallDirect();
            callReq.codec = CallConfig.getInstance().getCodec();
            callReq.caller = caller;
            callReq.bedID = bedName;
            callReq.name=callerName;
            callReq.age=age;
            callReq.callerIP = CommonUtils.getLocalIP();
            callReq.callerPort = call.getLocalPort();
            callReq.callID = call.getCallID();
            callReq.callee = callee;
            callReq.callerType= CallConfig.getInstance().getDeviceType();

            BaseReq baseReq = new BaseReq<>(MsgType.CALLASK,  callReq);
            Timber.i("basereq%s", baseReq);
//        NettyManager.INST.sendMsg(baseReq);
            NettyManager.INST.sendMsg(baseReq, new GeneralCallback() {
                @Override
                public void sendMsgSuccess() {
                    Timber.i("sendMsgSuccess: ");
                }
                @Override
                public void sendMsgFail(String err) {
                }
            });
            mCalls.add(call);


        } catch (Exception e) {
            call=null;
            e.printStackTrace();
        }

        return call;
    }

    public Call createIncomingCall(CallReq callReq) {

        Call call = new Call();
        call.setDesIp(callReq.callerIP);
        call.setDesPort(callReq.callerPort);
        call.setCallID(callReq.callID);
        call.setRemoteDeviceID(callReq.caller);
        call.setCaller(callReq.caller);
        call.setCallee(callReq.callee);
        call.setCodec(callReq.codec);
        call.setBedName(callReq.bedID);
        call.setState(Call.State.IncomingReceived);
        call.setAge(callReq.age);
        call.setName(callReq.name);
        call.setRoomID(callReq.roomID);


        mCalls.add(call);

        return call;

    }


    public void removeCall(Call call) {

        mCalls.remove(call);

    }


    public void endCall() {


    }


    /**
     *
     * @param call
     * @return  接听电话成功 就发送接听信令等待返回就开启语音.  如果接听有异常,就挂断电话
     */
    public boolean acceptCall(Call call) {
        try {
            call.initSocket();
            call.initCall(call.getCodec());
            AcceptReq acceptReq = new AcceptReq();
            acceptReq.callID = call.getCallID();
            acceptReq.caller = call.getCaller();
            acceptReq.callee = CallConfig.getInstance().getLocalDeviceId();
            acceptReq.calleeIP = CommonUtils.getLocalIP();
            acceptReq.calleePort = call.getLocalPort();
            acceptReq.calleeType = CallConfig.getInstance().getDeviceType();

            BaseReq<AcceptReq> answerReqBaseReq = new BaseReq<>(MsgType.CALLACCEPT, acceptReq);
            NettyManager.INST.sendMsg(answerReqBaseReq);

            call.setState(Call.State.Connecting);
            return true;
        } catch (Exception e) {
            //  如果音频初始化失败  挂断电话
            hangupCall(call);
            e.printStackTrace();
            return false;
        }

    }


    public Call findCallByCallID(String callID) {
        for (Call call : mCalls) {
            if(callID.equals(call.getCallID())){
                return call;
            }
        }
        return null;
    }

    public void clearAllCalls(){
        //登陆失败  清除所有的通话
        if (mCalls != null) {
            mCalls.clear();
        }
    }
}
