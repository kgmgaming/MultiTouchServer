package com.example.toptal.multitouchserver;

import android.app.IntentService;
import android.content.Intent;
import android.support.annotation.Nullable;
import android.util.Log;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class MultiTouchService extends IntentService {
    private static final String TAG = MultiTouchService.class.getSimpleName();
    private static final int DEFAULT_SERVER_PORT = 9999;

    public MultiTouchService() {
        super(MultiTouchService.class.getName());
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        super.onStartCommand(intent, flags, startId);
        return START_STICKY;
    }

    @Override
    protected void onHandleIntent(@Nullable Intent intent) {
        final int serverPort = intent != null
            ? intent.getIntExtra("serverPort", DEFAULT_SERVER_PORT)
            : DEFAULT_SERVER_PORT;
        @Nullable DatagramSocket socket = null;
        @Nullable MultiTouchDevice mtDevice = null;

        try {
            socket = new DatagramSocket(serverPort, InetAddress.getByName("0.0.0.0"));
            mtDevice = new MultiTouchDevice();

            Log.i(TAG, "MultiTouchService has started");
            // TODO: while(true) is not safe, but this server should never stop
            while (true) {
                byte[] buffer = new byte[256];
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                socket.receive(packet);

                Log.i(TAG, String.format("Received %d bytes", packet.getLength()));

                mtDevice.injectByteArrayAsMtEvent(packet.getData());
            }
        } catch (Exception e) {
            Log.e(TAG, "MultiTouchService has stopped due to an error", e);
        } finally {
            if (socket != null) {
                socket.close();
            }

            if (mtDevice != null) {
                mtDevice.close();
            }
        }
    }
}
