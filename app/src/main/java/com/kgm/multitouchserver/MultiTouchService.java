package com.kgm.multitouchserver;

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

    //private Vector slots = new Vector(100);
    private int maxSlots = 100;
    private int maxSlot = -1;
    private MultiTouchPoint[] slots = new MultiTouchPoint[maxSlots];
    private int nextId = 1000;

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
        DatagramSocket socket = null;
        MultiTouchDevice mtDevice = null;

        // Loop forever trying to connect
        while (true) {
            try {
                socket = new DatagramSocket(serverPort, InetAddress.getByName("0.0.0.0"));
                mtDevice = new MultiTouchDevice();
                byte[] buffer = new byte[1024];

                Log.i(TAG, "MultiTouchService has started");
                // TODO: while(true) is not safe, but this server should never stop
                while (true) {
                    DatagramPacket packet = new DatagramPacket(buffer, buffer.length - 1);
                    socket.receive(packet);

                    String s = new String(packet.getData(), 0, packet.getLength());
                    // Log.i(TAG, String.format("Received %d bytes: %s", packet.getLength(),s));

                    String[] points = s.split("\\|");
                    try {
                        handlePoints(mtDevice, points);
                    } catch (Exception e) {
                        Log.i(TAG, "Invalid points ignored [" + e.getMessage() + "]");
                    }

                    // mtDevice.injectByteArrayAsMtEvent(packet.getData());
                }
            } catch (Exception e) {
                Log.e(TAG, "MultiTouchService has stopped: " + e.getMessage());
            } finally {
                if (socket != null) {
                    socket.close();
                }

                if (mtDevice != null) {
                    mtDevice.close();
                }
            }
            try {
                Thread.sleep(1000);
            } catch (Exception e) {}
            Log.w(TAG,"Restarting...");
        }
    }

    protected void handlePoints(MultiTouchDevice mtDevice, String[] points)
    {
        for (String point : points
                ) {
            // Log.i(TAG, String.format("  %s",point));
            MultiTouchPoint mP = new MultiTouchPoint(point);
            int i = findSlot(mP);
            if (i >= 0)
            {
                MultiTouchPoint oP = slots[i];
                oP.state = mP.state;
                oP.x = mP.x;
                oP.y = mP.y;
                if (mP.state != 'r') {
                    // Log.i(TAG, "Updated slot " + i + ", " + oP);
                }
            }
            else
            {
                mP.shadowId = (++nextId) % 65536;
                i = addSlot(mP);
                Log.i(TAG, "Added slot   " + i + ", " + mP + ", maxSlot=" + maxSlot );
            }
        }

        //
        // Apply current points
        //
        boolean touchesGenerated = false;
        for (int i = 0; i <= maxSlot; i++)
        {
            MultiTouchPoint t = slots[i];
            // Log.i("handlePoints", "Processing point " + t.id + ", state=" + t.state);
            try {
                switch (t.state) {
                    case 'p':   // Pressed

                        mtDevice.injectTouchEvent(t.state, i, t.id, t.x, t.y);
                        touchesGenerated = true;
                        t.state = 's';
                        break;

                    case 'r':   // released

                        mtDevice.injectTouchEvent(t.state, i, t.id, t.x, t.y);
                        touchesGenerated = true;
                        removeSlot(i);
                        Log.i(TAG, "Removed slot " + i + ", " + t + ", maxSlot=" + maxSlot );
                        break;

                    case 'm':   // moved

                        mtDevice.injectTouchEvent(t.state, i, t.id, t.x, t.y);
                        touchesGenerated = true;
                        t.state = 's';
                        break;

                    case 's':   // stationary

                        break;
                }
            }
            catch (Exception e)
            {
                Log.e(TAG,e.getMessage());
            }
        }
        try {
            if (touchesGenerated) {
                mtDevice.injectTouchEvent('z', 0, 0, 0, 0);
            }
        }
        catch (Exception e)
        {
            Log.e(TAG,e.getMessage());
        }
    }

    protected int findSlot(MultiTouchPoint p)
    {
        for(int i = 0; i <= maxSlot; i++)
        {
            if (p.id == slots[i].id) return i;
        }
        //
        // Not found
        //
        return -1;
    }

    protected int addSlot(MultiTouchPoint p)
    {
        //
        // Look for an imbedded inactive slot
        //
        for(int i = 0; i <= maxSlot; i++)
        {
            if (slots[i].state == 'i')
            {
                slots[i] = p;
                return i;
            }
        }
        //
        // No imbedded available slots. Add to the end.
        //
        if (maxSlot + 1 < maxSlots)
        {
            maxSlot++;
            slots[maxSlot] = p;
            return maxSlot;
        }
        //
        // No available slots
        //
        Log.i(TAG,"Too many touch points (>" + maxSlots + ")");
        return -1;
    }

    protected void removeSlot(int i)
    {
        if (i >= 0 && i < maxSlots)
        {
            // valid slot
            MultiTouchPoint p = slots[i];
            p.state = 'i';
            p.id = -1;
            //
            // Update maxSlot if we removed the hightest slot
            // Back up until we find a valid slot
            //
            if (i >= maxSlot) {
                i--;
                while (i >= 0 && slots[i].state == 'i')
                {
                    i--;
                }
                maxSlot = i;
            }
        }
    }

    protected void handlePoint(String point)
    {
        String[] pInfo = point.split(",");
        if (pInfo.length == 4) {
            String state = pInfo[0];
            int id = Integer.parseInt(pInfo[1]);
            int x = Integer.parseInt(pInfo[2]);
            int y = Integer.parseInt(pInfo[3]);
            Log.i(TAG, String.format(" --> state=%s, id=%d, x=%d, y=%d", state,id,x,y));
        }
    }
}
