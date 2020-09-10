package com.witted.receiver;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.os.Build;
import android.widget.Toast;

import com.witted.utils.NetworkUtils;

import timber.log.Timber;


/**
 * 监听反馈当前网络连接状态
 */
public class NetworkReceiver extends BroadcastReceiver {

    private NetworkCallback networkCallback;
    private ConnectivityManager.NetworkCallback callback;

    public NetworkReceiver(Context context, NetworkCallback networkCallback) {
        this.networkCallback = networkCallback;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            ConnectivityManager connManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);

            callback = new ConnectivityManager.NetworkCallback() {

                @Override
                public void onAvailable(Network network) {
                    super.onAvailable(network);
                    networkCallback.onAvailable();
                    ConnectivityManager connectMgr = (ConnectivityManager) context.getApplicationContext().getSystemService(Context.CONNECTIVITY_SERVICE);
                    NetworkCapabilities networkCapabilities = connectMgr.getNetworkCapabilities(network);
                    boolean b = networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_ETHERNET);
                    boolean b1 = networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI);



                }

                @Override
                public void onLost(Network network) {
                    super.onLost(network);
                    networkCallback.onLost();
                }
            };
            connManager.registerDefaultNetworkCallback(callback);
        } else {
            IntentFilter intentFilter = new IntentFilter();
            intentFilter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);

            context.registerReceiver(this, intentFilter);
        }
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent.getAction().equals(ConnectivityManager.CONNECTIVITY_ACTION)) {
            if (NetworkUtils.isConnected(context)) {
                this.networkCallback.onAvailable();
            } else {
                this.networkCallback.onLost();
            }
        }
    }

    /**
     * 取消网络监听
     *
     * @param activity
     */
    public void unRegister(Activity activity) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            ConnectivityManager connManager = (ConnectivityManager) activity.getSystemService(Context.CONNECTIVITY_SERVICE);
            connManager.unregisterNetworkCallback(callback);
        } else {
            activity.unregisterReceiver(this);
        }
    }

    public interface NetworkCallback {
        /**
         * 网络可用
         */
        void onAvailable();

        /**
         * 网络不可用，丢失连接
         */
        void onLost();
    }

}