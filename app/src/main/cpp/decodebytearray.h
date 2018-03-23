#ifndef DECODEBYTEARRAY_H_
#define DECODEBYTEARRAY_H_

#include "touchevent.h"

template <typename T>
T decodeByteArray(char **bytes);

template<>
int32_t decodeByteArray<int32_t>(char **bytes)
{
    const int32_t x = *reinterpret_cast<int32_t*>(*bytes);
    *bytes += sizeof(int32_t);
    return x;
}

template<>
Point decodeByteArray<Point>(char **bytes)
{
    const int32_t x = decodeByteArray<int32_t>(bytes);
    const int32_t y = decodeByteArray<int32_t>(bytes);
    return Point(x, y);
}

template<>
std::vector<Point> decodeByteArray<std::vector<Point> >(char **bytes)
{
    std::vector<Point> points;
    const int32_t size = decodeByteArray<int32_t>(bytes);

    for (int32_t i = 0; i < size; ++i) {
        points.push_back(decodeByteArray<Point>(bytes));
    }

    return points;
}

template <>
TouchEvent decodeByteArray<TouchEvent>(char **bytes)
{
    const int32_t touchPointState = decodeByteArray<int32_t>(bytes);
    const std::vector<Point> touchPoints = decodeByteArray<std::vector<Point> >(bytes);
    return TouchEvent(touchPointState, touchPoints);
}

#endif  // DECODEBYTEARRAY_H_
