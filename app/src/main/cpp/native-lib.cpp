#include <jni.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <string.h>
#include <errno.h>
#include <android/log.h>
#include "touchevent.h"
#include "decodebytearray.h"

static void errno_exp(JNIEnv *env)
{
    env->ThrowNew(
        env->FindClass("java/lang/Exception"),
        strerror(errno)
    );
}

static int emit(
        int fd,
        __u16 type, __u16 code, int val)
{
    struct input_event ie;

    memset(&ie, 0, sizeof(ie));
    gettimeofday(&ie.time,NULL);
    ie.type = type;
    ie.code = code;
    ie.value = val;

    // __android_log_print(ANDROID_LOG_INFO, "EMIT", "emit(%d, %d, %d)", type, code, val);

    if (write(fd, &ie, sizeof(ie)) < 0) {
        return -1;
    }

    return 0;
}

extern "C"
JNIEXPORT jint
JNICALL
Java_com_kgm_multitouchserver_MultiTouchDevice_openMtDevice(
    JNIEnv *env,
    jobject /* this */)
{
    int fd = -1;
    try {
        fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
        if (fd == -1) {
            throw(errno);
        }

        if (ioctl(fd, UI_SET_EVBIT, EV_KEY) == -1) {
            throw(errno);
        }

        if (ioctl(fd, UI_SET_EVBIT, EV_REL) == -1) {
            throw(errno);
        }

        if (ioctl(fd, UI_SET_EVBIT, EV_ABS) == -1) {
            throw(errno);
        }

        if (ioctl(fd, UI_SET_ABSBIT, ABS_MT_POSITION_X) == -1) {
            throw(errno);
        }

        if (ioctl(fd, UI_SET_ABSBIT, ABS_MT_POSITION_Y) == -1) {
            throw(errno);
        }

        struct uinput_user_dev uud;
        memset(&uud, 0, sizeof(uud));
        snprintf(uud.name, UINPUT_MAX_NAME_SIZE, "MultiTouchService Interface");
        uud.id.version = 4;
        uud.id.bustype = BUS_USB;
        uud.absmin[ABS_MT_POSITION_X] = 0;
        uud.absmax[ABS_MT_POSITION_X] = 800;
        uud.absmin[ABS_MT_POSITION_Y] = 0;
        uud.absmax[ABS_MT_POSITION_Y] = 600;

        uud.absmin[ABS_MT_SLOT] = 0;
        uud.absmax[ABS_MT_SLOT] = 9; // track up to 9 fingers
        uud.absmin[ABS_MT_TOUCH_MAJOR] = 0;
        uud.absmax[ABS_MT_TOUCH_MAJOR] = 15;
        uud.absmin[ABS_MT_POSITION_X] = 0; // screen dimension
        uud.absmax[ABS_MT_POSITION_X] = 1919; // screen dimension
        uud.absmin[ABS_MT_POSITION_Y] = 0; // screen dimension
        uud.absmax[ABS_MT_POSITION_Y] = 1079; // screen dimension
        uud.absmax[ABS_X] = 1919;
        uud.absmax[ABS_Y] = 1079;
        uud.absmin[ABS_MT_TRACKING_ID] = 0;
        uud.absmax[ABS_MT_TRACKING_ID] = 65535;
        uud.absmin[ABS_MT_PRESSURE] = 0;
        uud.absmax[ABS_MT_PRESSURE] = 255;

// Setup the uinput device
        ioctl(fd, UI_SET_EVBIT, EV_KEY);
        ioctl(fd, UI_SET_EVBIT, EV_REL);

// Touch
        ioctl(fd, UI_SET_EVBIT, EV_ABS);
        ioctl(fd, UI_SET_EVBIT, EV_SYN);
        ioctl(fd, UI_SET_KEYBIT, BTN_TOUCH);
        ioctl(fd, UI_SET_ABSBIT, ABS_MT_SLOT);
        // ioctl (fd, UI_SET_ABSBIT, ABS_MT_TOUCH_MAJOR);
        ioctl(fd, UI_SET_ABSBIT, ABS_MT_POSITION_X);
        ioctl(fd, UI_SET_ABSBIT, ABS_MT_POSITION_Y);
        ioctl(fd, UI_SET_ABSBIT, ABS_X);
        ioctl(fd, UI_SET_ABSBIT, ABS_Y);
        ioctl(fd, UI_SET_ABSBIT, ABS_MT_TRACKING_ID);
        // ioctl (fd, UI_SET_ABSBIT, ABS_MT_PRESSURE);
        ioctl(fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);

        if (write(fd, &uud, sizeof(uud)) < 0) {
            throw(errno);
        }

        if (ioctl(fd, UI_DEV_CREATE) == -1) {
            throw(errno);
        }
    } catch (...)
    {
        // Generate a java exception
        //
        errno_exp(env);
        fd = -1;
    }

    return fd;
}

extern "C"
JNIEXPORT jint
JNICALL
Java_com_kgm_multitouchserver_MultiTouchDevice_closeMtDevice(
    JNIEnv *env,
    jobject, /* this */
    jint fd)
{
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
    return 0;
}

extern "C"
JNIEXPORT jint
JNICALL
Java_com_kgm_multitouchserver_MultiTouchDevice_injectTouchEvent(
    JNIEnv *env,
    jobject, /* this */
    jint fd,
    jint slot,
    jchar state,
    jint id,
    jint x,
    jint y
)
{
    switch (state)
    {
        case 'p':   // pressed

            emit(fd, EV_ABS, ABS_MT_SLOT, slot);
            emit(fd, EV_ABS, ABS_MT_TRACKING_ID, slot);
            emit(fd, EV_ABS, ABS_MT_POSITION_X, x);
            emit(fd, EV_ABS, ABS_MT_POSITION_Y, y);
            emit(fd, EV_KEY, BTN_TOUCH, 1);
            emit(fd, EV_ABS, ABS_X, x);
            emit(fd, EV_ABS, ABS_Y, y);
            break;

        case 'r':   // released

            emit(fd, EV_ABS, ABS_MT_SLOT, slot);
            emit(fd, EV_ABS, ABS_MT_TRACKING_ID, -1);
            emit(fd, EV_KEY, BTN_TOUCH, 0);
            break;

        case 'm':   // moved

            emit(fd, EV_ABS, ABS_MT_SLOT, slot);
            emit(fd, EV_ABS, ABS_MT_TRACKING_ID, slot);
            emit(fd, EV_ABS, ABS_MT_POSITION_X, x);
            emit(fd, EV_ABS, ABS_MT_POSITION_Y, y);
            emit(fd, EV_ABS, ABS_X, x);
            emit(fd, EV_ABS, ABS_Y, y);
            break;

        case 'z':   // done

            emit(fd, EV_SYN, SYN_REPORT, 0);
            break;
    }

    return 0;
}
