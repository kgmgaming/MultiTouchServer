#ifndef TOUCHEVENT_H_
#define TOUCHEVENT_H_

#include <vector>
#include <string>
#include <jni.h>
#include "./point.h"

class TouchEvent
{
public:
    TouchEvent(int32_t touchPointState, std::vector<Point> touchPoints);

    int inject(int fd) const;

private:
    const int32_t m_touchPointState;
    const std::vector<Point> m_touchPoints;
};

#endif  // TOUCHEVENT_H_
