package com.witted.ptt;

import android.os.Handler;
import android.util.Log;

import com.witted.constant.CodeType;
import com.witted.netty.NettyManager;

import java.net.DatagramSocket;
import java.net.SocketException;

import timber.log.Timber;

public class Call {


    private long callStartTime;

    private long callAcceptTime;

    private long callEndTime;

    private int desPort;

    private String desIp;

    private String remoteDeviceID;

    private String name;

    private String age;

    private String roomID;


    private String caller;

    public String getCaller() {
        return caller;
    }

    public void setCaller(String caller) {
        this.caller = caller;
    }

    /**
     * 接听方的id,挂断的时候需要用到
     */
    private String callee;

    private String callID;

    private String bedName;


    private int codec =-1;
    private DatagramSocket mDs;
    private AudioPlay audioPlay;
    private AudioRec audioRec;


    private State state;
    private Handler mHandler;
    private CallStartRunnable mCallRunnable;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getAge() {
        return age;
    }

    public void setAge(String age) {
        this.age = age;
    }

    public String getRoomID() {
        return roomID;
    }

    public void setRoomID(String roomID) {
        this.roomID = roomID;
    }


    public static class State {

        private final int mState;
        private final String mValue;


        /**
         * 客户端已经初始化socket成功,获取端口发出呼叫请求    是否也需要初始化音频成功
         */
        public static final State OutGoingInit = new State(1, "OutGoingInit");

        /**
         * 接收端已经收到发出呼叫的消息,正在呼叫中
         */
        public static final State OutGoingProgress = new State(2, "OutGoingProgress");


        public static final State IncomingReceived = new State(3, "IncomingReceived");

        /**
         * 接听电话中
         *
         * @param state
         * @param value
         */
        public static final State Connecting = new State(4, "Connecting");

        /**
         * 接听电话
         *
         * @param state
         * @param value
         */

        public static final State Connected = new State(5, "Connected");


        /**
         * 通话中
         */
        public static final State StreamsRunning = new State(6, "StreamsRunning");


//        public static final State Connected=new State(3,"Connected");

        /**
         * 通话结束
         */
        public static final State CallEnd = new State(10, "CallEnd");


        public static final State CallBusy = new State(11, "CallBusy");


        public static final State CallErr = new State(12, "CallErr");


        public static final State Release = new State(66, "Release");


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


    //获取通话时间
    public long getDuration() {

        if (state == State.CallEnd) {

            return callEndTime - callAcceptTime;
        }

        if (callAcceptTime == 0) {
            return -1;
        }
        return System.currentTimeMillis() - callAcceptTime;

    }


    public void removeCallRunnable() {

        if (mCallRunnable != null) {
            mHandler.removeCallbacks(mCallRunnable);
            mCallRunnable = null;
        }

    }

    public void initSocket() throws SocketException {
        mHandler = new Handler();
        mCallRunnable = new CallStartRunnable();
        mDs = new DatagramSocket();
    }

    public void initCall(int codec) throws Exception {

        //初始化socket有异常
//        mDs = initSocket();

        //初始化 AudioPlay 和AudioPlay


        Log.e("test_audio_audioPlay",""+(audioPlay==null));
        Log.e("test_audio_audioPlay",""+(audioRec==null));

        if(audioPlay!=null||audioRec!=null){
            if(audioPlay!=null){
                audioPlay.setStart(false);
                audioPlay=null;
            }
            if(audioRec!=null){
                audioRec.setAudioRecRelease();
                audioRec=null;
            }
            Timber.e("音频未释放");
            throw new Exception("音频未释放");
        }



        audioPlay = new AudioPlay(mDs, codec);
        audioPlay.setName("play");
        audioRec = new AudioRec(mDs);
        audioRec.setName("rec");


//        mHandler.postDelayed(mCallRunnable,5*1000);

    }

    public void setCodec(int code) {
        this.codec = code;
    }

    public int getCodec() {
        if (codec == -1) {
            codec = CodeType.JB_G729_CODEC;
//            codec = CodeType.JB_G711MU_CODEC;
        }
        return codec;
    }




    public int getLocalPort() {
        if (mDs != null) {
            return mDs.getLocalPort();
        }
        return -1;
    }




    /**
     * 要先调用 initCall()
     */
    public void startAudioCall()  {

//        if(audioRec==null||audioPlay==null){
//            throw new Exception("not initCall");
//        }

        Timber.i("desIp%s  desPort%s code%s", desIp, desPort, codec);
        audioRec.setIp(desIp);
        audioRec.setPort(desPort);
        audioRec.setCode(codec);
        audioRec.start();

        callAcceptTime = System.currentTimeMillis();

//        audioPlay.setHandler(handler); 线程还没开始走  handler可能为空
        audioPlay.setAudioPlay(audioRec);
        if (audioPlay.setStart(true)==1) {
            CallManager.testCount++;
            Timber.e("开始播放声音");
            audioPlay.start();
        }
        Timber.e("开始播放声音1%s", CallManager.testCount);

        //212

//        sCall=null;

    }

    public void removeCall() {
        CallManager.getInstance().removeCall(this);
    }


     public void stopCall() {
        if (audioPlay != null) {
            audioPlay.setStart(false);
        }

        if (audioRec != null) {
            audioRec.setAudioRecRelease();
        }
        releaseUdp();
        callEndTime = System.currentTimeMillis();
        audioPlay = null;
        audioRec = null;
        removeCall();
    }

    public void releaseUdp(){
        if(mDs!=null&&!mDs.isClosed()){
            mDs.close();
            mDs=null;
        }
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

    public String getBedName() {
        return bedName;
    }

    public void setBedName(String bedName) {
        this.bedName = bedName;
    }

    public State getState() {
        return state;
    }

    public void setState(State state) {


        //开启一个60s的定时器,如果一直没有接听,就直接挂断  或者对方挂断
        if (state == State.OutGoingProgress) {

//            mHandler.postDelayed()


        }


        this.state = state;
    }

    class CallStartRunnable implements Runnable {
        @Override
        public void run() {
            setState(State.CallEnd);
            NettyManager.INST.handlerCallState(Call.this, getState());
            stopCall();
        }
    }


    @Override
    public String toString() {
        return "Call{" +
                "remoteDeviceID='" + remoteDeviceID + '\'' +
                ", callee='" + callee + '\'' +
                ", callID='" + callID + '\'' +
                ", bedName='" + bedName + '\'' +
                ", codec=" + codec +
                ", state=" + state +
                '}';
    }
}
