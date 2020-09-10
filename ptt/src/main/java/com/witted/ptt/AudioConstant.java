package com.witted.ptt;

import android.media.AudioFormat;

public class AudioConstant {

    //音频的通道，单声道还是立体声
    public static final int AUDIO_CHANNEL_CONFIG= AudioFormat.CHANNEL_CONFIGURATION_MONO;
    //每次采样的位数
    public static final int AUDIO_ENCODING=AudioFormat.ENCODING_PCM_16BIT;
    //采样频率
    public static final int AUDIO_SIMPLERATE=8000;
}
