package com.witted.ptt;

import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import com.android.webrtc.audio.MobileAEC;
import com.witted.constant.CodeType;

import java.lang.ref.WeakReference;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

import timber.log.Timber;

public class AudioRec extends Thread {

    private int mPort;
    private String mIp;
    private DatagramSocket mDs;
    private MobileAEC mobileAec;
    private int mCode;


    private int g729pack_len = 20;
    private AudioRecord mAudioRecord;
    private short[] mRecordDatas;
    private byte[] mEncodeDatas;


    private int g711pack_len = 160;
    private int rtpheader_len = 12;

    private int m711PackageLength = 160;
    private int m729PackageLength = m711PackageLength / 4;

    private short[] aecPcmData = new short[160];


    /**
     * 发出呼叫信令前先创建DatagramSocket
     */

    private int recordLength = 160;


    public void init() throws Exception {

        int minBufferSize = AudioRecord.getMinBufferSize(AudioConstant.AUDIO_SIMPLERATE, AudioConstant.AUDIO_CHANNEL_CONFIG, AudioConstant.AUDIO_ENCODING);
        if (minBufferSize == AudioRecord.ERROR_BAD_VALUE) {
            Timber.e("初始化minBufferSize错误");

            throw new Exception("初始化minBufferSize错误");
        }

        mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.VOICE_COMMUNICATION
                , AudioConstant.AUDIO_SIMPLERATE
                , AudioConstant.AUDIO_CHANNEL_CONFIG
                , AudioConstant.AUDIO_ENCODING
                , minBufferSize);

        mRecordDatas = new short[recordLength];


    }

    private Handler mHandler;


    public Handler getHandler() {

        return mHandler;
    }

    public int getPort() {
        return mPort;
    }

    public void setPort(int port) {
        mPort = port;
    }

    public void setIp(String ip) {
        mIp = ip;
    }

    public String getIp() {
        return mIp;
    }


    public DatagramSocket getDatagramSocket() {
        return mDs;
    }

    public int getCode() {
        return mCode;
    }

    public void setCode(int code) {
        mCode = code;

        if (code == CodeType.JB_G729_CODEC) {
            mEncodeDatas = new byte[m729PackageLength + rtpheader_len];
        } else {
            mEncodeDatas = new byte[m711PackageLength + rtpheader_len];
        }
    }

    public static AudioRec mAudioRec;

    private static Object mObject = new Object();

    public static AudioRec getInstance(DatagramSocket ds) {
        synchronized (mObject) {
            try {
                if(mAudioRec==null){
                    mAudioRec = new AudioRec(ds);
                }
            } catch (Exception e) {
                mAudioRec=null;
                e.printStackTrace();
            }
        }
        return mAudioRec;
    }

    public AudioRec(DatagramSocket ds) throws Exception {
        mDs = ds;
        init();
        mobileAec = new MobileAEC(null);
        mobileAec.setAecmMode(MobileAEC.AggressiveMode.MOST_AGGRESSIVE).prepare();
        mAudioRecord.startRecording();
    }


    public void setAudioRecRelease() {
        if (mHandler != null) {
            Message obtain = Message.obtain();
            obtain.obj = null;
            mHandler.sendMessage(obtain);
        } else {
            release();
        }
    }

    public void release() {
        if (mAudioRecord != null) {
            mAudioRecord.release();
            mAudioRecord = null;
        }

        if (mobileAec != null) {
            mobileAec.close();
        }

        if (mDs != null) {
            mDs.close();
            mDs = null;
        }
    }


    @Override
    public void run() {
        Looper.prepare();
        int state = mAudioRecord.getState();

        Timber.i("mAudioRecord.getState%s", state);

        mHandler = new MyHandler(AudioRec.this);

        Looper.loop();
        super.run();
    }


    static class MyHandler extends Handler {
        AudioRec mAudioRec;

        public MyHandler(AudioRec audioRec) {
            WeakReference<AudioRec> audioRecWeakReference = new WeakReference<>(audioRec);
            mAudioRec = audioRecWeakReference.get();

        }

        @Override
        public void handleMessage(Message msg) {

            short[] speakData = (short[]) msg.obj;

            if (speakData == null) {
                Looper looper = Looper.myLooper();
                if (looper != null) {
                    looper.quitSafely();
                }
                mAudioRec.release();
                return;
            }

            if (mAudioRec.mAudioRecord == null) {
                return;
            }

            if (mAudioRec.mAudioRecord.getState() != AudioRecord.STATE_INITIALIZED) {
                return;
            }

            try {
                mAudioRec.mobileAec.farendBuffer(speakData, speakData.length);
                int read = mAudioRec.mAudioRecord.read(mAudioRec.mRecordDatas, 0, mAudioRec.mRecordDatas.length);

                if (read > 0 && mAudioRec.mDs != null && !mAudioRec.mDs.isClosed()) {
                    mAudioRec.mobileAec.echoCancellation(mAudioRec.mRecordDatas, null, mAudioRec.aecPcmData, (short) mAudioRec.aecPcmData.length, (short) 100);
                    encodec(mAudioRec.aecPcmData, mAudioRec.mEncodeDatas, mAudioRec.mCode);
//                        encodec(mRecordDatas, mEncodeDatas);
                    DatagramPacket dp = new DatagramPacket(mAudioRec.mEncodeDatas, 0, mAudioRec.mEncodeDatas.length, InetAddress.getByName(mAudioRec.mIp), mAudioRec.mPort);
                    mAudioRec.mDs.send(dp);
                }


            } catch (Exception e) {
                Timber.i("audioException%s", e.getMessage());
                e.printStackTrace();
            }


        }


        private int g711pack_len = 160;
        private int rtpheader_len = 12;
        private short seq = 100;
        private int timestamp = 160;
        private int ssrc = 0x1234;
        private int csrc = 9999;
        private int BIAS = (0x84);  /* Bias for linear code. */
        private int m711PackageLength = 160;


        private void encodec(short[] src, byte[] dst, int code) throws Exception {

            //包装rtp头
            packrtpheader(dst, 0, seq, timestamp, ssrc, csrc, code);
            seq++;
            timestamp += m711PackageLength;
            packdata(src, 0, dst, rtpheader_len, g711pack_len, code);
        }


        private void packdata(short[] src, int soffset, byte[] dst, int doffset, int len, int code) {
            int itmp;
            // pcm 转 a率
            if (code == CodeType.JB_G711A_CODEC) {
                for (itmp = 0; itmp < len; itmp++) {
                    dst[doffset + itmp] = linear2alaw(src[soffset + itmp]);
                }
                return;
            }

            // pcm 转 u率
            if (code == CodeType.JB_G711MU_CODEC) {
                for (itmp = 0; itmp < len; itmp++) {
                    dst[doffset + itmp] = linear2ulaw(src[soffset + itmp]);
                }
                return;
            }

            if (code == CodeType.JB_G729_CODEC) {

                len = 20;
                byte[] encodes_729 = new byte[20];

                CodeUtils.LinearToG729(src, (short) 160, encodes_729);

                for (itmp = 0; itmp < len; itmp++) {
                    dst[doffset + itmp] = encodes_729[itmp];
//                LogUtils.logi(TAG, "encodes: " + itmp + "     " + encodes[itmp]);
                }
            }
        }


        private void packrtpheader(byte[] dst, int offset, short iseq, int itimestamp, int ssrc, int icsrc, int code) {
            dst[offset] = (byte) 0x80;

            if (code == CodeType.JB_G729_CODEC) {
                dst[offset + 1] = 0x12;
            }

            if (code == CodeType.JB_G711MU_CODEC) {
                dst[offset + 1] = 0x08;
            }

            if (code == CodeType.JB_G711A_CODEC) {
                dst[offset + 1] = 00;
            }


            dst[offset + 2] = (byte) ((iseq & 0xff00) >> 8);
            dst[offset + 3] = (byte) (iseq & 0xff);

            dst[offset + 4] = (byte) ((itimestamp & 0xff000000) >> 24);
            dst[offset + 5] = (byte) ((itimestamp & 0xff0000) >> 16);
            dst[offset + 6] = (byte) ((itimestamp & 0xff00) >> 8);
            dst[offset + 7] = (byte) (itimestamp & 0xff);

            dst[offset + 11] = (byte) ((ssrc & 0xff000000) >> 24);
            dst[offset + 10] = (byte) ((ssrc & 0xff0000) >> 16);
            dst[offset + 9] = (byte) ((ssrc & 0xff00) >> 8);
            dst[offset + 8] = (byte) (ssrc & 0xff);


        }


        private final int SEG_MASK = 0x70;

        private final static int SEG_SHIFT = 4;

        private final int QUANT_MASK = 0xf;

        byte linear2alaw(short pcm_val) {
            short mask;
            int seg;
            char aval;
            if (pcm_val >= 0) {
                mask = 0xD5;
            } else {
                mask = 0x55;
                pcm_val = (short) (-pcm_val - 1);
                if (pcm_val < 0) {
                    pcm_val = 32767;
                }
            }
            /* Convert the scaled magnitude to segment number. */
            seg = search(pcm_val, seg_end, (short) 8);
            /* Combine the sign, segment, and quantization bits. */
            if (seg >= 8) {/* out of range, return maximum value. */
                return (byte) (0x7F ^ mask);

            } else {
                aval = (char) (seg << SEG_SHIFT);
                if (seg < 2) {
                    aval |= (pcm_val >> 4) & QUANT_MASK;
                } else {
                    aval |= (pcm_val >> (seg + 3)) & QUANT_MASK;
                }
                return (byte) (aval ^ mask);
            }
        }

        private short seg_end[] = {0xFF, 0x1FF, 0x3FF, 0x7FF, 0xFFF, 0x1FFF, 0x3FFF, 0x7FFF};


        private int search(int val, short[] table, int size) {
            int i;

            for (i = 0; i < size; i++) {
                if (val <= table[i]) {
                    return (i);
                }
            }
            return (size);
        }

        private byte linear2ulaw(short pcm_val) /* 2's complement (16-bit range) */ {
            int mask;
            int seg;
            byte uval;

            /* Get the sign and the magnitude of the value. */
            if (pcm_val < 0) {
                pcm_val = (short) (BIAS - pcm_val);
                mask = 0x7F;
            } else {
                pcm_val += BIAS;
                mask = 0xFF;
            }

            /* Convert the scaled magnitude to segment number. */
            seg = search(pcm_val, seg_end, 8);
            /*
             * Combine the sign, segment, quantization bits;
             * and complement the code word.
             */
            if (seg >= 8) {/* out of range, return maximum value. */
                return (byte) (0x7F ^ mask);

            } else {
                uval = (byte) ((seg << 4) | ((pcm_val >> (seg + 3)) & 0xF));
                return (byte) (uval ^ mask);
            }
        }

    }


}
