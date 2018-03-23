package com.example.toptal.multitouchserver;

public class MultiTouchDevice {
    static {
        System.loadLibrary("native-lib");
    }

    private int fd;

    public MultiTouchDevice() throws Exception {
        this.fd = openMtDevice();
    }

    public void injectByteArrayAsMtEvent(byte[] bytes) throws Exception {
        injectByteArrayAsMtEvent(this.fd, bytes);
    }

    public void close() {
        closeMtDevice(this.fd);
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        close();
    }

    private native int injectByteArrayAsMtEvent(int fd, byte[] bytes) throws Exception;
    private native int openMtDevice() throws Exception;
    private native int closeMtDevice(int fd);
}
