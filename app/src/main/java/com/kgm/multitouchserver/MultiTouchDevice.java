package com.kgm.multitouchserver;

public class MultiTouchDevice {
    static {
        System.loadLibrary("native-lib");
    }

    private int fd;

    public MultiTouchDevice() throws Exception {
        this.fd = openMtDevice();
    }

    public void injectTouchEvent(char state, int slot, int id, int x, int y) throws Exception {
        injectTouchEvent(this.fd, slot, state, id, x, y);
    }

    public void close() {
        closeMtDevice(this.fd);
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        close();
    }

    private native int injectTouchEvent(int fd, int slot, char state, int id, int x, int y) throws Exception;
    private native int openMtDevice() throws Exception;
    private native int closeMtDevice(int fd);
}
