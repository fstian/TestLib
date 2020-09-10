package com.witted.ptt;

import java.net.DatagramSocket;
import java.net.SocketException;

import timber.log.Timber;

public class Call {



    private long startTime;

    private long endTime;
    private int desPort;

    private String desIp;
    private String remoteDeviceID;

    private String deviceID;


    /**
     * 接听方的id,挂断的时候需要用到
     */

    private String callee;

    private String callID;
    private int code;
    private DatagramSocket mDs;
    private AudioPlay audioPlay;
    private AudioRec audioRec;


    private State state;

    private static Call sCall;


    public static class State {

        private final int mState;
        private final String mValue;


        /**
         * 客户端已经初始化socket成功,获取端口发出呼叫请求    是否也需要初始化音频成功
         */
        public static final State OutGoingInit = new State(0, "OutGoingInit");

        /**
         * 接收端已经收到发出呼叫的消息,正在呼叫中
         */
        public static final State OutGoingProgress = new State(1, "OutGoingProgress");


        public static final State IncomingReceived = new State(3, "IncomingReceived");

        /**
         * 通话中
         *
         * @param state
         * @param value
         */

        public static final State Connected = new State(4, "Connected");



        /**
         * 接听电话中
         *
         * @param state
         * @param value
         */
        public static final State Connecting = new State(4, "Connecting");


        /**
         * 接通中  跳转到接听界面
         */
        public static final State StreamsRunning = new State(5, "StreamsRunning");


//        public static final State Connected=new State(3,"Connected");


        /**
         * 通话结束
         */
        public static final State CallEnd = new State(20, "CallEnd");





        public State(int state, String value) {
            mState = state;
            mValue = value;
        }

        public int getState() {
            return mState;
        }

        public String getValue() {
            return mValue;
        }


        @Override
        public String toString() {
            return "State{" +
                    "mValue='" + mValue + '\'' +
                    '}';
        }
    }






    public void initCall() throws Exception {

        mDs = initSocket();
        audioPlay = new AudioPlay(mDs, code);
        audioRec = new AudioRec();
    }

    public void setCodec(int code) {
        this.code = code;
    }

    public int getCodec() {
        return code;
    }

    public DatagramSocket initSocket() {
        try {
            return new DatagramSocket();

        } catch (SocketException e) {
            Timber.e("initSocket%s", e.getMessage());
            e.printStackTrace();
        }
        return null;
    }


    public int getLocalPort() {
        if (mDs != null) {
            return mDs.getLocalPort();
        }
        return -1;
    }

    public void startCall() {

        setState(State.Connected);

        Timber.i("audioklajdlkasjd%s",audioPlay==null);
        Timber.i("audioklajdlkasjd%s",audioRec==null);

        if(audioRec==null||audioPlay==null){
            throw new RuntimeException("not  initCall");
        }

        audioRec.setIp(desIp);
        audioRec.setPort(desPort);
        audioRec.setDatagramSocket(mDs);
        audioRec.setCode(code);
        audioRec.start();

        startTime=System.currentTimeMillis();


//        audioPlay.setHandler(handler); 线程还没开始走  handler可能为空
        audioPlay.setAudioPlay(audioRec);
        audioPlay.setStart(true);
        audioPlay.start();


//        sCall=null;

    }

    public void release() {

        CallManager.getInstance().removeCall(this);
    }

    //获取通话时间
    public long getDuration(){

        if(state==State.CallEnd){

            return endTime-startTime;
        }

        if(startTime==0){
            return -1;
        }
        return System.currentTimeMillis()-startTime;

    }


    public void stopCall() {

        setState(State.CallEnd);

        if(audioRec!=null){
            audioRec.setAudioRecRelease();

        }
        if(audioPlay!=null){
            audioPlay.setStart(false);
        }

        audioPlay=null;

        audioRec=null;

        release();
    }

    public String getCallID() {
        return callID;
    }

    public void setCallID(String callID) {
        this.callID = callID;
    }

    public String getCallee() {
        return callee;
    }

    public void setCallee(String callee) {
        this.callee = callee;
    }

    public String getRemoteDeviceID() {
        return remoteDeviceID;
    }

    public void setRemoteDeviceID(String remoteDeviceID) {
        this.remoteDeviceID = remoteDeviceID;
    }

    public String getDeviceID() {
        return deviceID;
    }

    public void setDeviceID(String deviceID) {
        this.deviceID = deviceID;
    }


    public int getDesPort() {
        return desPort;
    }

    public void setDesPort(int desPort) {
        this.desPort = desPort;
    }

    public String getDesIp() {
        return desIp;
    }

    public void setDesIp(String desIp) {
        this.desIp = desIp;
    }


    public State getState() {
        return state;
    }

    public void setState(State state) {
        this.state = state;
    }


}
