package com.witted.netty;

import android.util.Log;

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
import io.netty.util.concurrent.ScheduledFuture;
import timber.log.Timber;

public class NettyClient {


    private static NettyClient mNettyClient;
    Bootstrap bootstrap;
    ChannelFuture mSync = null;


    static final String TAG = "NettyClient";
    private ExecutorService mExecutorService;
    private ChannelFuture mFuture;
    private String mHost;
    private int mPort;


    public static synchronized NettyClient getInstance() {

        if (mNettyClient == null) {
            mNettyClient = new NettyClient();

        }

        return mNettyClient;
    }


    public NettyClient() {

        initThreadPool();
    }

    void initThreadPool() {
        mExecutorService = Executors.newFixedThreadPool(1);

    }

    public synchronized void connect(String host, int port) {

//        mExecutorService.shutdown();

        mHost = host;
        mPort = port;


        if (mExecutorService == null) {
            initThreadPool();
        }

        mExecutorService.execute(new Runnable() {
            @Override
            public void run() {
                nettyClose();
                initNetty();
                doConnect(host, port);

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
                        pipeline.addLast(new IdleStateHandler(NettyConfig.DEFAULT_HEARTBEAT_INTERVAL * 20, NettyConfig.DEFAULT_HEARTBEAT_INTERVAL, 0, TimeUnit.SECONDS));
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
        Timber.i("doConnect host :%s    port :%s", host, port);
        if (channel != null && channel.isActive()) {
            return;
        }
        Timber.i("doConnect1");

        mFuture = bootstrap.connect(host, port);


        mFuture.addListener(genericFutureListener);


    }

    ChannelFutureListener genericFutureListener = new ChannelFutureListener() {
        @Override
        public void operationComplete(ChannelFuture futureListener) throws Exception {
            if (futureListener.isSuccess()) {
                channel = futureListener.channel();
                Timber.i("Connect to server successfully!");

            } else {
                Timber.i("Failed to connect to server, try connect after 10s");
                ScheduledFuture<?> schedule = futureListener.channel().eventLoop().schedule(new Runnable() {
                    @Override
                    public void run() {
                        Timber.i("10s后");
                        doConnect(mHost, mPort);
                    }
                }, 10, TimeUnit.SECONDS);
            }
        }
    };

    public void nettyClose() {

        if (mFuture != null) {
            mFuture.removeListener(genericFutureListener);
        }

        if (bootstrap != null) {
            bootstrap.group().shutdownGracefully();
            bootstrap = null;
        }

        if (channel != null) {
            channel.close();
            channel = null;
        }


//        if(mExecutorService!=null){
//            mExecutorService.shutdownNow();
//            mExecutorService=null;
//        }

    }


    public void sendMsg(Object o) {

        Timber.i("sendMsg_client:%s", (mSync == null));

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
