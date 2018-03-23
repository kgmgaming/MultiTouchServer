#include <jni.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <string.h>
#include <errno.h>
#include "touchevent.h"
#include "decodebytearray.h"

static int errno_exp(JNIEnv *env)
{
    return env->ThrowNew(
        env->FindClass("java/lang/Exception"),
        strerror(errno)
    );
}

extern "C"
JNIEXPORT jint
JNICALL
Java_com_example_toptal_multitouchserver_MultiTouchDevice_openMtDevice(
    JNIEnv *env,
    jobject /* this */)
{
    const int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd == -1) {
        return errno_exp(env);
    }

    if (ioctl(fd, UI_SET_EVBIT, EV_KEY) == -1) {
        return errno_exp(env);
    }

    if (ioctl(fd, UI_SET_EVBIT, EV_REL) == -1) {
        return errno_exp(env);
    }

    if (ioctl(fd, UI_SET_EVBIT, EV_ABS) == -1) {
        return errno_exp(env);
    }

    if (ioctl(fd, UI_SET_ABSBIT, ABS_MT_POSITION_X) == -1) {
        return errno_exp(env);
    }

    if (ioctl(fd, UI_SET_ABSBIT, ABS_MT_POSITION_Y) == -1) {
        return errno_exp(env);
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
    if (write(fd, &uud, sizeof(uud)) < 0) {
        return errno_exp(env);
    }

    if (ioctl(fd, UI_DEV_CREATE) == -1) {
        return errno_exp(env);
    }

    return fd;
}

extern "C"
JNIEXPORT jint
JNICALL
Java_com_example_toptal_multitouchserver_MultiTouchDevice_closeMtDevice(
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
Java_com_example_toptal_multitouchserver_MultiTouchDevice_injectByteArrayAsMtEvent(
    JNIEnv *env,
    jobject, /* this */
    jint fd,
    jbyteArray jbytes)
{
    jbyte *bytes = env->GetByteArrayElements(jbytes, NULL);
    char *bytes_iterator = (char *)bytes;
    TouchEvent touchEvent = decodeByteArray<TouchEvent>(&bytes_iterator);
    env->ReleaseByteArrayElements(jbytes, bytes, 0);

    if (touchEvent.inject(fd) < 0) {
        return errno_exp(env);
    }

    return 0;
}
