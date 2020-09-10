package com.witted.ptt;

public interface CallState {


    int CALL_STATE_IDLE = 0; //通话空闲

    int CALL_STATE_ACTIVE = 1; //正在通话(活动中)

    int CALL_STATE_HOLDING = 2; //通话挂起(例如我和多个人通话,其中一个通话在活动,而其它通话就会进入挂起状态)

    int CALL_STATE_DIALING = 3; //拨号开始

    int CALL_STATE_ALERTING = 4; //正在呼出(提醒对方接电话)

    int CALL_STATE_INCOMING = 5; //对方来电

    int CALL_STATE_WAITING = 6; //第三方来电等待(例如我正在和某人通话,而其他人打入时就会就进入等待状态)

    int CALL_STATE_DISCONNECTED = 7; //挂断完成

    int CALL_STATE_DISCONNECTING = 8; //正在挂断



}
