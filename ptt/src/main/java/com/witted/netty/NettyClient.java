package com.witted.netty;

import android.util.Log;


import com.witted.remote.OnReceiveMessageListener;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import io.netty.bootstrap.Bootstrap;
import io.netty.channel.Channel;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelFutureListener;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.ChannelPipeline;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;
import io.netty.handler.codec.LineBasedFrameDecoder;
import io.netty.handler.timeout.IdleStateHandler;
import timber.log.Timber;

public class NettyClient {


    private static NettyClient mNettyClient;
    Bootstrap bootstrap ;
    ChannelFuture mSync = null;



    static final String TAG = "NettyClient";
    private  ExecutorService mExecutorService;


    public static synchronized NettyClient getInstance(){

        if(mNettyClient==null){
            mNettyClient = new NettyClient();

        }

        return mNettyClient;
    }


    public NettyClient() {

        initThreadPool();
    }

    void initThreadPool(){
        mExecutorService = Executors.newFixedThreadPool(1);

    }

    public  void connect(String host, int port){

//        mExecutorService.shutdown();


        if(mExecutorService==null){
            initThreadPool();
        }

        mExecutorService.execute(new Runnable() {
            @Override
            public void run() {
                nettyClose();
                initNetty();
                doConnect(host,port);

            }
        });
    }

    void initNetty() {
        NioEventLoopGroup workGroup = new NioEventLoopGroup();
        bootstrap = new Bootstrap();
        bootstrap.group(workGroup)
                .channel(NioSocketChannel.class)
                .option(ChannelOption.SO_KEEPALIVE, true)
                .handler(new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(SocketChannel socketChannel) {
                        ChannelPipeline pipeline = socketChannel.pipeline();
                        pipeline.addLast(new LineBasedFrameDecoder(10240));
                        pipeline.addLast(new IdleStateHandler(NettyConfig.DEFAULT_HEARTBEAT_INTERVAL * 2, NettyConfig.DEFAULT_HEARTBEAT_INTERVAL, 0, TimeUnit.SECONDS));
                        pipeline.addLast(new BaseDecoder());
                        pipeline.addLast(new BaseEncoder());
                        pipeline.addLast(new HeartBeatHandler());
                        ClientHandler clientHandler = new ClientHandler();
                        pipeline.addLast(clientHandler);
                    }
                });
    }



    private Channel channel;


    protected void doConnect(String host, int port) {
        if (channel != null && channel.isActive()) {
            return;
        }

        ChannelFuture future = bootstrap.connect(host, port);

        future.addListener(new ChannelFutureListener() {
            @Override
            public void operationComplete(ChannelFuture futureListener) throws Exception {
                if (futureListener.isSuccess()) {
                    channel = futureListener.channel();
                    Timber.i("Connect to server successfully!");

                } else {
                    Timber.i("Failed to connect to server, try connect after 10s");
                    futureListener.channel().eventLoop().schedule(new Runnable() {
                        @Override
                        public void run() {
                            doConnect(host, port);
                        }
                    }, 10, TimeUnit.SECONDS);
                }
            }
        });


    }

    public void nettyClose() {

        if (bootstrap != null) {
            bootstrap.group().shutdownGracefully();
            bootstrap = null;
        }

        if (channel!= null) {
            channel.close();
            channel = null;
        }

//        if(mExecutorService!=null){
//            mExecutorService.shutdownNow();
//            mExecutorService=null;
//        }

    }


    public void sendMsg(Object o) {

        Log.i(TAG, "sendMsg: " + (mSync == null));

        if (channel != null) {
            channel.writeAndFlush(o).addListener(new ChannelFutureListener() {
                @Override
                public void operationComplete(ChannelFuture channelFuture) throws Exception {
                    Log.i(TAG, "operationComplete: " + channelFuture.isSuccess());
                }
            });
        }

    }


}
