package com.witted.netty;

import android.app.Application;
import android.content.Context;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;


import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.witted.bean.AcceptReq;
import com.witted.bean.BaseReq;
import com.witted.bean.CallReq;
import com.witted.bean.CommonResp;
import com.witted.bean.HangupReq;
import com.witted.bean.MsgAck;
import com.witted.bean.RegisterRequest;
import com.witted.constant.MsgType;
import com.witted.ptt.Call;
import com.witted.ptt.CallManager;
import com.witted.ptt.CommonUtils;
import com.witted.receiver.NetworkReceiver;
import com.witted.remote.GeneralCallback;
import com.witted.remote.OnCallStateChangeListener;
import com.witted.remote.OnConnectionListener;
import com.witted.remote.OnReceiveMessageListener;


import org.jetbrains.annotations.NotNull;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

import timber.log.Timber;

public class NettyManager {

    private static final String TAG = NettyManager.class.getName();

    private static Context sContext;

    public static NettyManager INST;
    private static int sMsgId;


    private Handler mainHandler;

    private boolean registerStatus = false;


    private ArrayList<OnConnectionListener> mConnectListeners = new ArrayList();


    private ArrayList<OnReceiveMessageListener> mOnMsgReceiveListeners = new ArrayList();


    private ArrayList<OnCallStateChangeListener> mOnCallStateChangeListeners = new ArrayList();

    private static String mUrl;


    private static NettyManager instance() throws Exception {
        if (INST == null) {
            throw new Exception("NettyManager not init");
        }
        return INST;
    }


    /**
     * 连接到管理主机
     *
     * @param url
     */
    public void nettyConnect(String url) {
        mUrl = url;
        if (!url.contains(":")) {
            return;
        }
        String[] split = url.split(":");
        String ip = split[0];
        int port = Integer.valueOf(split[1]);

        NettyClient.getInstance().connect(ip, port);

    }

    public static String getMsgId() {
        sMsgId++;
        return CallConfig.getInstance().getLocalDeviceId() + "___" + sMsgId;
    }

    public boolean getRegisterStatus() {
        return registerStatus;
    }

    public void setRegisterStatus(boolean registerStatus) {

        this.registerStatus = registerStatus;
    }

    HashMap<String, MsgAck> msgLists = new HashMap();


    public void removeMsgCallback(BaseReq req, boolean isSuccess) {

        MsgAck msgAck = msgLists.remove(req.msgID);
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                if (msgAck != null) {
                    if (isSuccess) {
                        msgAck.mTimer.cancel();
                        msgAck.mCallback.sendMsgSuccess();
                    } else {
                        msgAck.mCallback.sendMsgFail("time out");
                    }
                }
            }
        });

    }


    public void sendMsg(Object o, GeneralCallback callback) {
        BaseReq req = (BaseReq) o;
        Timber.i("sendMsg%s", req);

        msgLists.put(req.msgID, new MsgAck(req, callback));

//        if (INST.mNettyClient != null) {
//            INST.mNettyClient.sendMsg(o);
//        }

        NettyClient.getInstance().sendMsg(o);
    }


    public void sendMsg(Object o) {
        BaseReq req = (BaseReq) o;
        Timber.i("sendMsg%s", req);

//        msgLists.put(req.msgID,new MsgAck(req.msgID, callback));


        NettyClient.getInstance().sendMsg(o);
    }


    public static void init(Application application) {
        if (INST != null) {
            return;
        }

        sContext = application.getApplicationContext();
        INST = new NettyManager();

        INST.mainHandler = new Handler();


        //网络改变监听

        new NetworkReceiver(sContext, mNetworkCallback);


    }

    static NetworkReceiver.NetworkCallback mNetworkCallback = new NetworkReceiver.NetworkCallback() {

        @Override
        public void onAvailable() {
            Timber.i("NetworkCallback%s", "NetworkCallback");


            if (TextUtils.isEmpty(mUrl)) {
                return;
            }
            NettyManager.INST.nettyConnect(mUrl);
        }

        @Override
        public void onLost() {
            Timber.i("NetworkCallback%s", "onLost");

        }
    };


    //对方挂断电话
    private void doHangUpReq(BaseReq common) {


        //当前正在通话的状态   接通的或者打出的

        //找到当前正在通话的 并挂断

        ArrayList<Call> calls = CallManager.getInstance().getCalls();

        HangupReq hangupReq = ((JSONObject) common.context).toJavaObject(HangupReq.class);

        String callID = hangupReq.callID;

        Timber.i("hangupcallee%s", callID);


        for (int i = 0; i < calls.size(); i++) {
            Call call = calls.get(i);
            if (call.getCallID().equals(callID)) {
                call.setState(Call.State.CallEnd);
                handlerCallState(call, call.getState());
                call.stopCall();
                sendHangUpResp(call, common);
            }
        }

    }

    private void sendHangUpResp(Call call, BaseReq common) {


        HangupReq hangupReq = ((JSONObject) common.context).toJavaObject(HangupReq.class);
        CommonResp commonResp = new CommonResp();
        commonResp.status = 200;
        commonResp.result = "ok";
        commonResp.callID = hangupReq.callID;

        BaseReq baseReq = new BaseReq(104, common.msgID, commonResp);
        NettyManager.INST.sendMsg(baseReq);


    }

    private void doCallAcceptResp(BaseReq common) {

        //接听电话 返回
        CommonResp commonResp = ((JSONObject) common.context).toJavaObject(CommonResp.class);
        int status = commonResp.status;
        if (status == 200) {
            ArrayList<Call> calls = CallManager.getInstance().getCalls();
            for (Call call : calls) {
                if (commonResp.callID.equals(call.getCallID())) {
                    try {
                        call.startAudioCall();
                        call.setState(Call.State.Connected);
                        handlerCallState(call, call.getState());
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }
        }

    }

    private void doCallAcceptReq(BaseReq common) {

        //对方接通了电话

        ArrayList<Call> calls = CallManager.getInstance().getCalls();

        AcceptReq acceptReq = ((JSONObject) common.context).toJavaObject(AcceptReq.class);
        for (Call call : calls) {
            String callID = call.getCallID();
            //callid相同 而且callstatus不等于callend
            if (callID.equals(call.getCallID())) {

                if (call.getState() == Call.State.CallEnd) {
                    sendAcceptBackMsg(call, common, false);
                    return;
                }
                call.setRemoteDeviceID(acceptReq.callee);
                call.setDesIp(acceptReq.calleeIP);
                call.setDesPort(acceptReq.calleePort);
                call.setState(Call.State.Connected);
                //开启通话线程
                try {
                    call.startAudioCall();
                    handlerCallState(call, call.getState());
                    //发送回复消息
                    sendAcceptBackMsg(call, common, true);
                } catch (Exception e) {
                    sendAcceptBackMsg(call, common, false);
                    e.printStackTrace();
                }
                break;
            }
        }


    }

    private void sendAcceptBackMsg(Call call, BaseReq common, boolean success) {


        AcceptReq acceptReq = ((JSONObject) common.context).toJavaObject(AcceptReq.class);

        call.setDesIp(acceptReq.calleeIP);
        call.setDesPort(acceptReq.calleePort);


        CommonResp commonResp = new CommonResp();

        if (success) {
            commonResp.status = 200;
            commonResp.result = "ok";
        } else {
            commonResp.status = 404;
            commonResp.result = "callend";
        }

        commonResp.callID = acceptReq.callID;
        call.setCallee(acceptReq.callee);

        BaseReq baseReq = new BaseReq(103, common.msgID, commonResp);
        NettyManager.INST.sendMsg(baseReq);
    }

    private void doCallInComing(@NotNull BaseReq msg) {

        CallReq callReq = ((JSONObject) msg.context).toJavaObject(CallReq.class);

        String callerIP = callReq.callerIP;

        int callerPort = callReq.callerPort;

        String callID = callReq.callID;

        String caller = callReq.caller;

        String callee = callReq.callee;

        String bedName = callReq.bedID;

        int codec = callReq.codec;

        Call incomingCall = CallManager.getInstance().createIncomingCall(callerIP, callerPort, callID, caller, codec, callee, bedName);

        handlerCallState(incomingCall, incomingCall.getState());

        sendCallbackMsg(msg, callID);


    }

    private void sendCallbackMsg(BaseReq msg, String callID) {
        CommonResp commonResp = new CommonResp();
        commonResp.status = 200;
        commonResp.callID = callID;
        BaseReq<CommonResp> backMsg = new BaseReq<>(MsgType.CALLINCOMING_RESP, msg.msgID, commonResp);
        sendMsg(backMsg);
    }


    private void doCallOutBack(BaseReq msg) {
        //呼出电话时的回复
        //如果回复200,
        //如果回复其他,呼叫失败  释放call
        ArrayList<Call> calls = CallManager.getInstance().getCalls();
        CommonResp commonResp = ((JSON) msg.context).toJavaObject(CommonResp.class);


        Iterator<Call> iterator = calls.iterator();
        while (iterator.hasNext()) {
            Call call = iterator.next();
            if (call.getCallID().equals(commonResp.callID)) {
                call.removeCallRunnable();
                if (commonResp.status == 200) {
                    call.setState(Call.State.OutGoingProgress);
                    handlerCallState(call, call.getState());
                } else {
                    call.setState(Call.State.CallEnd);
                    handlerCallState(call, call.getState());
                    //服务器返回失败 释放call
                    iterator.remove();
                }
                break;
            }

        }

    }

    public void handlerCallState(Call call, Call.State state) {

        Timber.i("handlerCallState%s", state);

        mainHandler.post(() -> {

            for (int i = 0; i < mOnCallStateChangeListeners.size(); i++) {
                OnCallStateChangeListener onCallStateChangeListener = mOnCallStateChangeListeners.get(i);
                onCallStateChangeListener.callState(call, state);
            }

//            for (OnCallStateChangeListener onCallStateChangeListener : mOnCallStateChangeListeners) {
//                Timber.i("onCallStateChangeListener%s",onCallStateChangeListener.toString());
//                onCallStateChangeListener.callState(call, state);
//            }
        });

    }


    public void addOnCallStateChangeListener(OnCallStateChangeListener listener) {

        if (listener == null) {
            return;
        }

        mOnCallStateChangeListeners.add(listener);

    }

    public void removeOnCallStateChangeListener(OnCallStateChangeListener listener) {

        if (listener == null) {
            return;
        }

        mOnCallStateChangeListeners.remove(listener);

    }


    public void addOnMessageReceiveListener(OnReceiveMessageListener listener) {
        if (listener == null) {
            return;
        }
        if (!mOnMsgReceiveListeners.contains(listener)) {
            mOnMsgReceiveListeners.add(listener);
        }

    }

    public void removeMessageReceiverListener(OnReceiveMessageListener listener) {

        if (listener == null) {
            return;
        }

        mOnMsgReceiveListeners.remove(listener);


    }


    public void addOnConnectionListener(OnConnectionListener listener) {
        if (listener == null) {
            return;
        }
        if (!mConnectListeners.contains(listener)) {
            mConnectListeners.add(listener);
        }

    }

    public void removeOnConnectionListener(OnConnectionListener listener) {
        if (listener == null) {
            return;
        }

        mConnectListeners.remove(listener);

//        Iterator<OnConnectionListener> iterator = mConnectListeners.iterator();
//        while (iterator.hasNext()){
//            if(iterator.next()==listener){
//                iterator.remove();
//            }
//        }

    }


    public void onMsgReceive(Object msg) {

        BaseReq common = (BaseReq) msg;
        Log.i(TAG, "onMsgReceive: " + common);
        mainHandler.post(() -> {
            for (OnReceiveMessageListener onReceiveMessageListener : mOnMsgReceiveListeners) {
                onReceiveMessageListener.onReceiveMessage(common);
            }
        });

        removeMsgCallback(common, true);

        int msgType = common.msgType;

        switch (msgType) {
            case 101:
                break;
            //有电话呼叫进来
            case MsgType.CALLINCOMING:
                doCallInComing(common);
                break;
            //电话呼出时返回
            case MsgType.CALLINCOMING_RESP:
                doCallOutBack(common);
                break;
            //呼叫后,对方接听返回
            case MsgType.CALLACCEPT:
                doCallAcceptReq(common);
                break;
            //接听电话时返回
            case MsgType.CALLACCEPT_RESP:
                doCallAcceptResp(common);
                break;
            //对方挂断电话
            case MsgType.CALLEND:
                doHangUpReq(common);
                break;
            //自己挂断电话时返回
            case MsgType.CALLEND_RESP:
                break;
            //获取设备列表回复
            case MsgType.GETDEVICE_RESP:
                break;

            default:
        }

    }




    //连接成功就登陆
    public void setConnectSuccess() {

        NettyManager.INST.login(new GeneralCallback() {
            @Override
            public void sendMsgSuccess() {

            }

            @Override
            public void sendMsgFail(String err) {

            }
        });

//        mainHandler.post(() -> {
//            for (OnConnectionListener connectListener : mConnectListeners) {
//                connectListener.onConnectSuccess();
//            }
//        });


    }

    public void setRegisterFail(int code, String fail) {

        mainHandler.post(() -> {
//            for (OnConnectionListener connectListener : mConnectListeners) {
//                connectListener.onRegisterFail(fail);
//            }
            for (int i = 0; i < mConnectListeners.size(); i++) {
                mConnectListeners.get(i).onRegisterFail(code, fail);

            }

        });

        //登陆失败  清除所有的通话
        ArrayList<Call> calls = CallManager.getInstance().getCalls();

        if (calls != null) {
            Iterator<Call> iterator = calls.iterator();
            while (iterator.hasNext()) {
                iterator.remove();
            }
        }


    }

    public void setRegisterSuccess() {
        mainHandler.post(() -> {
            for (OnConnectionListener connectListener : mConnectListeners) {
                connectListener.onRegisterSuccess();
            }
        });

    }

    public void login(GeneralCallback callback) {

        RegisterRequest registerRequest = new RegisterRequest();
        registerRequest.deviceID = CallConfig.getInstance().getLocalDeviceId();
        registerRequest.deviceType = CallConfig.getInstance().getDeviceType();
        String localIP = CommonUtils.getLocalIP();
        registerRequest.localIP = localIP;
        registerRequest.expireTime = 30;
        BaseReq<RegisterRequest> request = new BaseReq<>(MsgType.REGISTER, getMsgId(), registerRequest);

        if (TextUtils.isEmpty(localIP)) {
            callback.sendMsgFail("获取ip失败");
            return;
        }
        NettyManager.INST.sendMsg(request, callback);
    }


    public void getDeviceList() {

        BaseReq<String> request = new BaseReq<>(MsgType.GETDEVICE, getMsgId(), CallConfig.getInstance().getLocalDeviceId());
        sendMsg(request);

    }

    public void getOnLineDeviceList(String zoneId) {

        BaseReq<String> request = new BaseReq<>(MsgType.GETONLINEDEVICE, getMsgId(), zoneId);
        sendMsg(request);

    }
}
