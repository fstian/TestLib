package com.witted.netty;

import com.alibaba.fastjson.JSON;
import com.witted.bean.BaseReq;

import java.nio.charset.Charset;
import java.util.List;

import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.MessageToMessageDecoder;
import io.netty.util.internal.ObjectUtil;

public class BaseDecoder extends MessageToMessageDecoder<ByteBuf> {

    private final Charset mCharset;

    public BaseDecoder() {
        mCharset = ObjectUtil.checkNotNull(Charset.defaultCharset(), "charset");
    }

    @Override
    protected void decode(ChannelHandlerContext channelHandlerContext, ByteBuf byteBuf, List<Object> list) throws Exception {

        String string = byteBuf.toString(mCharset);

        list.add(covertToObject(string));

    }

    private BaseReq covertToObject(String string) {
        BaseReq baseReq = JSON.parseObject(string).toJavaObject(BaseReq.class);
        return baseReq;

    }
}
