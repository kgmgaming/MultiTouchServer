#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include "touchevent.h"

static int emit(
    int fd,
    __u16 type, __u16 code, int val)
{
    struct input_event ie;

    memset(&ie, 0, sizeof(ie));
    ie.type = type;
    ie.code = code;
    ie.value = val;

    if (write(fd, &ie, sizeof(ie)) < 0) {
        return -1;
    }

    return 0;
}

TouchEvent::TouchEvent(
    int32_t touchPointState,
    std::vector<Point> touchPoints):
    m_touchPointState(touchPointState),
    m_touchPoints(touchPoints)
{}

int TouchEvent::inject(int fd) const
{
    if (m_touchPoints.empty()) {
        if (emit(fd, EV_SYN, SYN_MT_REPORT, 0) == -1) {
            return -1;
        }
    }

    for (const auto &point: m_touchPoints) {
        if (emit(fd, EV_ABS, ABS_MT_POSITION_X, point.x()) == -1) {
            return -1;
        }

        if (emit(fd, EV_ABS, ABS_MT_POSITION_Y, point.y()) == -1) {
            return -1;
        }

        if (emit(fd, EV_SYN, SYN_MT_REPORT, 0) == -1) {
            return -1;
        }
    }

    if (emit(fd, EV_SYN, SYN_REPORT, 0) == -1) {
        return -1;
    }

    return 0;
}
