package com.witted.remote;

import com.witted.ptt.Call;

public interface OnCallStateChangeListener {

    void callState(Call call, Call.State state);


}
