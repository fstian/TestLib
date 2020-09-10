package com.witted.remote;

public interface OnConnectionListener {

//    void onConnectSuccess();

    void onRegisterSuccess();

    void onRegisterFail(int code,String fail);

}
