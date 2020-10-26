package com.witted.ptt;

import android.content.Context;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Handler;
import android.os.Message;

import com.witted.constant.CodeType;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.util.Arrays;

import timber.log.Timber;

public class AudioPlay extends Thread {


    int JB_STATUS_BUFSIZE = 1024;
    byte[] arStatus = new byte[JB_STATUS_BUFSIZE];

    private final int mCode;
    private  int mJbline;
    private Context mContext;
    private DatagramSocket mDs;
    private boolean mStart;
    private AudioTrack mAudioTrack;


    private boolean mStartPlay;


    private int rtpheader_len = 12;
    private int mCodeLength;
    private int m711PackageLength = 160;
    private int m729PackageLength = m711PackageLength / 8;


    private byte[] receiveDatas = new byte[600];
    private boolean mFirst = true;


    private AudioRec mAudioRec;
    private PlayThread mPlayThread;


    public AudioPlay(DatagramSocket datagramSocket, int code) throws Exception {


        mDs = datagramSocket;
        mCode = code;

        if (mCode == CodeType.JB_G729_CODEC) {
            mCodeLength = m729PackageLength + rtpheader_len;
        } else {
            mCodeLength = m711PackageLength + rtpheader_len;
        }
        init();

        JitterBuffer.closeJb(0);
        mJbline = JitterBuffer.openJb(mCode, 20);
//
//        Timber.i("JitterBuffer.open%s   code%s", mJbline, code);
//

    }


    public void setStart(boolean start) throws Exception {
        mStart = start;

        int state = mAudioTrack.getState();


//        if(state != AudioTrack.STATE_INITIALIZED){
//
//        }

        if (mStart && state != AudioTrack.STATE_INITIALIZED) {
            throw new Exception("audioTrack 初始化失败");
        }

        if (start) {
            mPlayThread = new PlayThread();
            mPlayThread.setName("PlayThread");
            mPlayThread.start();
        }
    }


    public void init() throws Exception {
        int minBufferSize = AudioTrack.getMinBufferSize(AudioConstant.AUDIO_SIMPLERATE, AudioConstant.AUDIO_CHANNEL_CONFIG, AudioConstant.AUDIO_ENCODING);
        if (minBufferSize == AudioTrack.ERROR_BAD_VALUE) {
            throw new Exception("getMinBufferSize err");
        }
        Timber.i("minBufferSize%s", minBufferSize);


        mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC
                , AudioConstant.AUDIO_SIMPLERATE,
                AudioConstant.AUDIO_CHANNEL_CONFIG,
                AudioConstant.AUDIO_ENCODING,
                minBufferSize, AudioTrack.MODE_STREAM);

    }


    private void getSrcFromReceive(byte[] receiveDatas, int length, byte[] src) {

        for (int i = 0; i < length; i++) {
            src[i] = receiveDatas[i];
        }

    }

    byte[] datas = new byte[m711PackageLength + rtpheader_len];

    @Override
    public void run() {
        DatagramPacket dp = new DatagramPacket(receiveDatas, 0, receiveDatas.length);
        while (mDs!=null&&!mDs.isClosed()&&mStart) {
            try {
                mDs.receive(dp);
                Timber.i("receive socket%s", mCodeLength);
//                short seq = GetValueFormBytes(receiveDatas, 2, 2);
//                Timber.i("receive seq%s", seq);
                if (mCode == CodeType.JB_G729_CODEC) {
                    getSrcFromReceive(receiveDatas, m729PackageLength + rtpheader_len, datas);
                } else {
                    getSrcFromReceive(receiveDatas, m711PackageLength + rtpheader_len, datas);
                }
//                synchronized (mObject) {

                int i = JitterBuffer.addPackage(mJbline, datas, mCodeLength);
//                    JitterBuffer.getStatus(mJbline, arStatus, JB_STATUS_BUFSIZE);
//                    Timber.i("jitter1111 i%s    status%s", i, new String(arStatus));

//                }
            } catch (IOException e) {
                Timber.i("ds receive exception%s", e.getMessage());
                e.printStackTrace();
            }
        }
        if(mDs!=null&&!mDs.isClosed()){
            mDs.close();
        }
        super.run();
    }

    private static short GetValueFormBytes(byte[] data, int offset, int iLen) {
        short value = 0;
        for (int iTmp = 0; iTmp < iLen; iTmp++) {
            value = (short) (value * 256);
            if (data[offset + iTmp] >= 0) {
                value += data[offset + iTmp];
            } else {
                value += data[offset + iTmp] + 256;
            }
        }
        return value;
    }


    byte[] encodes_711 = new byte[160];
    byte[] encodes_729 = new byte[20];

    short[] decodes = new short[160];

    public void setAudioPlay(AudioRec audioRec) {
        mAudioRec = audioRec;
    }

    class PlayThread extends Thread {
        private int mPackage;
        Handler handler;

        @Override
        public void run() {
            mAudioTrack.play();
            while (mStart) {

                Timber.i("playthread mPackage %s mJbline%s code%s", mPackage, mJbline, mCode);

                if (mJbline >= 0) {
//                    synchronized (mObject) {
                    if (mCode == CodeType.JB_G729_CODEC) {
                        mPackage = JitterBuffer.getPackage(mJbline, encodes_729, m729PackageLength);
                    } else {
                        mPackage = JitterBuffer.getPackage(mJbline, encodes_711, m711PackageLength);
//                            int status = JitterBuffer.getStatus(mPackage, arStatus, JB_STATUS_BUFSIZE);
//                            Timber.i("playthread mPackage %s arStatus%s", mPackage, new String(arStatus));

                    }
                }
                Timber.i("playthread mPackage %s  code%s", mPackage, mCode);
                switch (mCode) {
                    case CodeType.JB_G729_CODEC:


                        if (mPackage == 20) {
                            CodeUtils.G729ToLinear(encodes_729, (short) 20, decodes);
                        } else {
                            Arrays.fill(decodes, (short) 0);
                        }
                        break;
                    case CodeType.JB_G711A_CODEC: //711 a

                        if (mPackage == 160) {
                            for (int i = 0; i < 160; i++) {
                                decodes[i] = alaw2linear(encodes_711[i]);
                            }

                        } else {
                            Arrays.fill(decodes, (short) 0);
                        }
                        break;
                    case CodeType.JB_G711MU_CODEC:
                        if (mPackage == 160) {
                            for (int i = 0; i < 160; i++) {
                                decodes[i] = ulaw2linear(encodes_711[i]);
                            }
                        } else {
                            Arrays.fill(decodes, (short) 0);
                        }
                        break;
                    default:
                }

                //发送语音包到录音线程


                if (handler == null) {
                    handler = mAudioRec.getHandler();
                }

                Timber.i("handler%s", (handler == null));

                Timber.i("hanler__p%s", handler);


                if (handler != null) {
                    Message obtain = Message.obtain();
                    obtain.obj = decodes;
                    handler.sendMessage(obtain);
                }


                mAudioTrack.write(decodes, 0, decodes.length);


//                    }


//                }


            }
            release();
        }
    }

    public void release() {

        if (mAudioTrack != null) {
            mAudioTrack.release();
        }

        if(mDs!=null){
            mDs.close();
            mDs=null;
        }

        if(mPlayThread!=null){
            mPlayThread.interrupt();
            mPlayThread=null;
        }
    }


    short alaw2linear(byte a_val) {
        int t;
        int seg;

        a_val ^= 0x55;

        t = ((a_val & QUANT_MASK) << 4);
        seg = ((a_val & SEG_MASK) >> SEG_SHIFT);
        switch (seg) {
            case 0:
                t += 8;
                break;
            case 1:
                t += 0x108;
                break;
            default:
                t += 0x108;
                t <<= seg - 1;
        }
        return (a_val & SIGN_BIT) != 0 ? (short) t : (short) -t;
    }

    short ulaw2linear(byte u_val) {
        short t;

        /* Complement to obtain normal u-law value. */
        u_val = (byte) ~u_val;

        /*
         * Extract and bias the quantization bits. Then
         * shift up by the segment number and subtract out the bias.
         */
        t = (short) (((u_val & QUANT_MASK) << 3) + BIAS);
        t <<= (u_val & SEG_MASK) >> SEG_SHIFT;
        return (short) ((u_val & SIGN_BIT) == SIGN_BIT ? (BIAS - t) : (t - BIAS));

    }

    private int SIGN_BIT = (0x80);        /* Sign bit for a A-law byte. */
    private int QUANT_MASK = (0xf);    /* Quantization field mask. */
    private int NSEGS = (8);            /* Number of A-law segments. */
    private int SEG_SHIFT = (4);        /* Left shift for segment number. */
    private int SEG_MASK = (0x70);        /* Segment field mask. */
    private int BIAS = (0x84);
}
