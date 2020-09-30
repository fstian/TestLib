package com.witted.remote;

import com.witted.bean.DeviceParamsBean;

import java.util.List;

public interface OnConnectionListener {

//    void onConnectSuccess();

    void onRegisterSuccess();

    void onRegisterFail(int code, String fail);

    void onHeartbeatBack(List<DeviceParamsBean> registerResp);

}
