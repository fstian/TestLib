package com.witted.ptt;

import android.os.Handler;

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



    private int codec ;
    private DatagramSocket mDs;
    private AudioPlay audioPlay;
    private AudioRec audioRec;


    private State state;
    private Handler mHandler;
    private CallStartRunnable mCallRunnable;


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


        public static final State IncomingReceived = new State(2, "IncomingReceived");

        /**
         * 接听电话中
         *
         * @param state
         * @param value
         */

        public static final State Connecting = new State(3, "Connecting");

        /**
         * 接听电话
         *
         * @param state
         * @param value
         */

        public static final State Connected = new State(4, "Connected");


        /**
         * 通话中
         */
        public static final State StreamsRunning = new State(5, "StreamsRunning");


//        public static final State Connected=new State(3,"Connected");

        /**
         * 通话结束
         */
        public static final State CallEnd = new State(20, "CallEnd");


        public static final State Release = new State(10, "Release");


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
    public long getDuration(){

        if(state==State.CallEnd){

            return callEndTime-callAcceptTime;
        }

        if(callAcceptTime==0){
            return -1;
        }
        return System.currentTimeMillis()-callAcceptTime;

    }




    public void removeCallRunnable(){

        if(mCallRunnable!=null){
            mHandler.removeCallbacks(mCallRunnable);
            mCallRunnable=null;
        }

    }


    public void initCall(int codec) throws Exception {

        mDs = initSocket();
        audioPlay = new AudioPlay(mDs, codec);
        audioRec = new AudioRec();

        mHandler = new Handler();

        mCallRunnable = new CallStartRunnable();

//        mHandler.postDelayed(mCallRunnable,5*1000);

    }

    public void setCodec(int code) {
        this.codec = code;
    }
    public int getCodec() {
        if(codec==-1){
            codec= CodeType.JB_G729_CODEC;
        }
        return codec;
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


    /**
     * 要先调用 initCall()
     */
    public void startAudioCall() throws Exception {

        if(audioRec==null||audioPlay==null){
            throw new Exception("not initCall");
        }

        Timber.i("desIp%s  desPort%s code%s",desIp,desPort,codec);
        audioRec.setIp(desIp);
        audioRec.setPort(desPort);
        audioRec.setDatagramSocket(mDs);
        audioRec.setCode(codec);
        audioRec.start();

        callAcceptTime=System.currentTimeMillis();

//        audioPlay.setHandler(handler); 线程还没开始走  handler可能为空
        audioPlay.setAudioPlay(audioRec);
        audioPlay.setStart(true);
        audioPlay.start();


//        sCall=null;

    }

    public void release() {
        CallManager.getInstance().removeCall(this);
    }


    public void stopCall() {
        setState(State.CallEnd);
        if(audioPlay!=null){
            try {
                audioPlay.setStart(false);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        if(audioRec!=null){
            audioRec.setAudioRecRelease();
        }
        callEndTime=System.currentTimeMillis();

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
        if(state== State.OutGoingProgress){

//            mHandler.postDelayed()


        }

















        this.state = state;
    }

    class CallStartRunnable implements Runnable{
        @Override
        public void run() {
            setState(State.CallEnd);
            NettyManager.INST.handlerCallState(Call.this,getState());
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
