package com.witted.ptt;

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

    public Call getCurrentCall() {

        for (Call call : mCalls) {
            Call.State state = call.getState();
            if (state == Call.State.Connected || state == Call.State.StreamsRunning) {
                return call;
            }
        }
        return null;

    }

    public ArrayList<Call> getCalls() {
        return mCalls;
    }


    public Call createCall() throws Exception {
        Call call = new Call();
        call.setCodec(CodeType.JB_G729_CODEC);

        call.initCall();
        call.setCallID(""+CommonUtils.getRandom());
        call.setState(Call.State.OutGoingInit);
        mCalls.add(call);
        Timber.i("hhhhhh%s", mCalls);
        return call;
    }

    public Call createIncomingCall(String ip, int port, String callId, String deviceID, int codec, String callee) {
        Call call = new Call();
        call.setDesIp(ip);
        call.setDesPort(port);
        call.setCallID(callId);
        call.setRemoteDeviceID(deviceID);
        call.setCodec(codec);
        call.setCallee(callee);

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
     * 如果通话状态处于活跃状态
     * @return
     */
    public boolean  callActive(){

        if(mCalls==null){
            return false;
        }
        for (Call call : mCalls) {

            Call.State state = call.getState();
            if (state == Call.State.IncomingReceived || state == Call.State.OutGoingInit || state == Call.State.OutGoingProgress ||
                    state == Call.State.StreamsRunning || state == Call.State.Connected || state == Call.State.Connecting) {
                return true;
            }

        }

        return false;

    }

}
