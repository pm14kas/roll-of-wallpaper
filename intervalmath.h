#ifndef INTERVALMATH_H
#define INTERVALMATH_H

#include <math.h>
#include "interval.h";

Interval sqrt(Interval x)
{
    double min;
    double max;

    if (x.a < 0) {
        min = 0;
    } else {
        min = sqrt(x.a);
    }

    if (x.b < 0) {
        max = 0;
    } else {
        max = sqrt(x.b);
    }

    return Interval(min, max);
}

Interval sin(Interval x)
{
    if ((__DBL_MAX__ == abs(x.a)) || (__DBL_MAX__ == abs(x.b)))
    {
        return Interval(((double)rand() / (double)RAND_MAX) * 2 - 1, ((double)rand() / (double)RAND_MAX) * 2 - 1);
    }
    double min = sin(x.a);
    double max = sin(x.b);
    int k = (int)x.b;
    bool low = true;
    bool high = true;
    while ((2 * k * M_PI - M_PI / 2.0 > x.a) && (2 * k * M_PI + M_PI / 2.0 > x.a))
    {
        if (low == true)
        {
            if (2 * k * M_PI - M_PI / 2.0 < x.b)
            {
                min = -1.0;
                low = false;
            }
        }
        if (high == true)
        {
            if (2 * k * M_PI + M_PI / 2.0 < x.b)
            {
                max = 1.0;
                high = false;
            }
        }
        --k;
    }
    if (min != -1.0)
    {
        if (min > sin(x.b))
            min = sin(x.b);
    }
    if (max != 1.0)
    {
        if (max < sin(x.a))
            max = sin(x.a);
    }
    return Interval(min, max);
}

#endif // INTERVALMATH_H
