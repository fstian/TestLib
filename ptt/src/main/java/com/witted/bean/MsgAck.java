package com.witted.bean;

import com.witted.remote.GeneralCallback;
import com.witted.netty.NettyManager;
import com.witted.remote.GeneralCallback;

import java.util.Timer;
import java.util.TimerTask;

public class MsgAck {


    public GeneralCallback mCallback;
    public  Timer mTimer;


    public MsgAck(BaseReq req, GeneralCallback callback) {

        mTimer = new Timer();
        mTimer.schedule(new TaskRunnable(req),5*1000);

        mCallback = callback;

    }


    class TaskRunnable extends TimerTask {

        public BaseReq mReq;


        public TaskRunnable(BaseReq req) {
         mReq=req;
        }

        @Override
        public void run() {

            NettyManager.INST.removeMsgCallback(mReq,false);

        }
    }

}
