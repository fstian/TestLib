package com.witted.ptt;

import com.witted.bean.AcceptReq;
import com.witted.bean.BaseReq;
import com.witted.bean.CallReq;
import com.witted.bean.HangupReq;
import com.witted.netty.CallConfig;
import com.witted.constant.MsgType;
import com.witted.remote.GeneralCallback;
import com.witted.netty.NettyManager;

import java.util.ArrayList;

import timber.log.Timber;

public class CallManager {


    private static final String TAG = "CallManager";
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
     * @param call
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


    }


    /**
     *
     * @param bedName 若呼叫护士台  bedName可为空
     * @param caller  呼叫人的设备id  此设备
     * @param callee  被叫人的设备id  如果是床头卡呼叫护士台  则为"FFFFFFF"
     * @return
     *
     */
    public Call createCall(String bedName, String caller, String callee)  {

        Call call=null;

        try {
            call = new Call();
            call.setBedName(bedName);
            call.setCodec(CallConfig.getInstance().getCodec());
            call.setCallID(CallConfig.getInstance().getCallID());
            call.setState(Call.State.OutGoingInit);
            call.setCallee(callee);
            call.setCaller(caller);
            call.initCall(CallConfig.getInstance().getCodec());


            //发送呼叫消息
            CallReq callReq = new CallReq();
            callReq.callType = CallConfig.getInstance().getCallType();
            callReq.callDirect = CallConfig.getInstance().getCallDirect();
            callReq.codec = CallConfig.getInstance().getCodec();
            callReq.caller = caller;
            callReq.bedID = bedName;
            callReq.callerIP = CommonUtils.getLocalIP();
            callReq.callerPort = call.getLocalPort();
            callReq.callID = call.getCallID();
            callReq.callee = callee;

            BaseReq baseReq = new BaseReq<>(MsgType.CALLINCOMING,  callReq);
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

    public Call createIncomingCall(String ip, int port, String callId, String caller, int codec, String callee, String bedName) {
        Call call = new Call();
        call.setDesIp(ip);
        call.setDesPort(port);
        call.setCallID(callId);
        call.setRemoteDeviceID(caller);
        call.setCaller(caller);
        call.setCodec(codec);
        call.setCallee(callee);
        call.setBedName(bedName);
        call.setState(Call.State.IncomingReceived);

        mCalls.add(call);

        return call;

    }


    public void removeCall(Call call) {

        mCalls.remove(call);

    }


    public void endCall() {


    }


    /**
     * 接听听话
     * @param call  回调的call
     */
    public void acceptCall(Call call) {
        try {

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
        } catch (Exception e) {

            //  如果音频初始化失败  挂断电话

            hangupCall(call);

            e.printStackTrace();
        }

    }

    /**
     * 如果通话状态处于活跃状态
     *
     * 如果没有就跳转到callfragment
     *
     * @return
     * @param callID
     */
    public boolean callActive(String callID) {

        if (mCalls == null) {
            return false;
        }
        for (Call call : mCalls) {

            Call.State state = call.getState();
            if (callID!=call.getCallID()&&(state == Call.State.IncomingReceived || state == Call.State.OutGoingInit || state == Call.State.OutGoingProgress ||
                    state == Call.State.StreamsRunning || state == Call.State.Connected || state == Call.State.Connecting)) {
                return true;
            }

        }

        return false;

    }

    public Call findCallByCallID(String callID) {


        for (Call call : mCalls) {

            if(callID.equals(call.getCallID())){
                return call;
            }
        }

        return null;


    }
}