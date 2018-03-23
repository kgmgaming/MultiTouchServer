#ifndef POINT_H_
#define POINT_H_

class Point
{
public:
    Point(int32_t x, int32_t y):
        m_x(x), m_y(y)
    {}

    int32_t x() const { return m_x; }
    int32_t y() const { return m_y; }

private:
    int32_t m_x;
    int32_t m_y;
};

#endif  // POINT_H_
