package com.witted.netty;

import android.util.Log;

import com.alibaba.fastjson.JSON;

import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.MessageToByteEncoder;

public class BaseEncoder extends MessageToByteEncoder {
    @Override
    protected void encode(ChannelHandlerContext channelHandlerContext, Object o, ByteBuf byteBuf) throws Exception {

        String string = JSON.toJSON(o).toString();
        Log.i("BaseEncoder", "encode: "+string);

        byte[] bytes = (string + System.getProperty("line.separator")).getBytes("UTF-8");
//        byteBuf.writeInt(bytes.length);
        byteBuf.writeBytes(bytes);

    }
}
