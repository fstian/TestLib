package com.witted.utils;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.content.Context;
import android.media.MediaPlayer;
import android.text.TextUtils;

import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Enumeration;
import java.util.List;
import java.util.Locale;


public class Utils {


    /**
     * 判断rl格式   172.16.1.172:12312
     * @param url
     * @return
     */
    public static boolean isUrl(String url){

        if (!url.contains(":")) {

            return false;
        }

        String[] split = url.trim().split(":");

        if(split.length!=2){
            return false;
        }

        if(!isIP(split[0])){
            return false;
        }

        if(!isPort(split[1])){
            return false;
        }

        return true;
    }

    /**
     * ip正则
     * @param ip
     * @return
     */
    public static boolean isIP(String ip){

        if(TextUtils.isEmpty(ip)){
            return false;
        }
        String regex = "(((25[0-5])|(2[0-4]\\d)|(1\\d{2})|([1-9]?\\d))\\.){3}((25[0-5])|(2[0-4]\\d)|(1\\d{2})|([1-9]?\\d))$";

       return ip.matches(regex);

    }

    /**
     * 1024-65535 端口正则
     * @param port
     * @return
     */
    public static boolean isPort(String port){
        if(TextUtils.isEmpty(port)){
            return false;
        }
        String regex="^(1(02[4-9]|0[3-9][0-9]|[1-9][0-9]{2})|[2-9][0-9]{3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$";
        return port.matches(regex);
    }


    /**
     * 秒
     * @param time
     * @return
     */

    public static String unitFormat(int i) {
        String retStr = "";
        if (i >= 0 && i < 10){
            retStr = "0" + Integer.toString(i);

        } else{
            retStr = "" + i;

        }
        return retStr;
    }


    /**
     *
     * @param time  单位秒
     * @return
     */
    public static String  callTimeToStr(int time){
        String timeStr = null;
        int hour = 0;
        int minute = 0;
        int second = 0;

        if (time <= 0){
            return "00:00";
        }else if(time>0&&time<=9){
            return "00:0"+time;
        }else if(time<60){
            return "00:"+time;
        } else {
            minute = time / 60;
            if (minute < 60) {
                second = time % 60;
                timeStr = unitFormat(minute) + ":" + unitFormat(second);
            } else {
                hour = minute / 60;
                if (hour > 99){
                    return "99:59:59";
                }
                minute = minute % 60;
                second = time - hour * 3600 - minute * 60;
                timeStr = unitFormat(hour) + ":" + unitFormat(minute) + ":" + unitFormat(second);
            }
        }
        return timeStr;

    }


    /**
     * 时间戳转化为时间格式
     *
     * @param timeStamp
     * @return
     */
    public static String timeStampToStr(long timeStamp) {
        @SuppressLint("SimpleDateFormat") SimpleDateFormat sdf = new SimpleDateFormat("a hh:mm:ss");
        return sdf.format(timeStamp);
    }
    public static String timeStampToStr_d(long timeStamp) {
        @SuppressLint("SimpleDateFormat") SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
        return sdf.format(timeStamp);
    }


    /**
     * 获取ip地址
     * @return
     */
    public static String getLocalIP() {

        String hostIp = "";
        try {
            Enumeration nis = NetworkInterface.getNetworkInterfaces();
            InetAddress ia = null;
            while (nis.hasMoreElements()) {
                NetworkInterface ni = (NetworkInterface) nis.nextElement();
                Enumeration<InetAddress> ias = ni.getInetAddresses();
                while (ias.hasMoreElements()) {
                    ia = ias.nextElement();
                    if (ia instanceof Inet6Address) {
                        continue;// skip ipv6
                    }
                    String ip = ia.getHostAddress();
                    if (!"127.0.0.1".equals(ip)) {
                        hostIp = ia.getHostAddress();
                        break;
                    }
                }
            }
        } catch (SocketException e) {
            e.printStackTrace();
        }
        return hostIp;
    }


    public static int getScreenWidthsPx(Context context){
        return context.getResources().getDisplayMetrics().widthPixels;

    }

    public static int getScreenHeightsPx(Context context){
        return context.getResources().getDisplayMetrics().heightPixels;

    }

    public static int getScreenWidthsDp(Context context){
        return (int) (context.getResources().getDisplayMetrics().widthPixels/getScreenDensity(context));

    }

    public static int getScreenHeightsDp(Context context){
        return (int) (context.getResources().getDisplayMetrics().heightPixels/getScreenDensity(context));

    }

    public static float getScreenDensity(Context context){
        return context.getResources().getDisplayMetrics().density;

    }

//    public static void setVibrator(Context context){
//        Vibrator vibrator = (Vibrator) context.getSystemService(Service.VIBRATOR_SERVICE);
//        vibrator.vibrate(1000);
//    }

    protected void showSound(Context context, int raw) {
        MediaPlayer mediaPlayer = null;
        mediaPlayer = MediaPlayer.create(context, raw);
        mediaPlayer.setLooping(true);
        mediaPlayer.setVolume(0.05f, 0.05f);
        mediaPlayer.start();
    }



    /*
* 判断服务是否启动,context上下文对象 ，className服务的name
*/
    public static boolean isServiceRunning(Context mContext, String className) {

        boolean isRunning = false;
        ActivityManager activityManager = (ActivityManager) mContext
                .getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningServiceInfo> serviceList = activityManager
                .getRunningServices(200);

        if (!(serviceList.size() > 0)) {
            return false;
        }

        for (int i = 0; i < serviceList.size(); i++) {
            if (serviceList.get(i).service.getClassName().equals(className) == true) {
                isRunning = true;
                break;
            }
        }
        return isRunning;
    }

    public static String ms2Date(long _ms) {
        Date date = new Date(_ms);
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS", Locale.getDefault());
        return format.format(date);
    }


    public static long getLongDate(String time){
        SimpleDateFormat sdf =new SimpleDateFormat("yyyy-mm-dd HH:mm:ss");
        try {
            Date date = sdf.parse(time);
            return date.getTime();
        } catch (ParseException e) {
            e.printStackTrace();
        }
        return 0;
    }

}
