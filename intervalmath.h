#ifndef INTERVALMATH_H
#define INTERVALMATH_H

#include <math.h>
#include <algorithm>
#include "interval.h"

class IntervalMath {

public:
    static Interval abs(Interval x);

    static Interval sqrt(Interval x);

    static Interval pow(Interval x, Interval y);

    static Interval sin(Interval x);

    static Interval cos(Interval x);

};

#endif // INTERVALMATH_H
