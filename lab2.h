#ifndef LAB2_H
#define LAB2_H

#include <QDateTime>
#include <QtMath>
#include <float.h>
#include <algorithm>
#include "parser.h"
#include "indidvid.h"
#include "math_prog.h"

class lab2
{
public:
    lab2();
    static Output calc(Parser f,
         std::vector<KeyValue> limitStart,
         std::vector<KeyValue> limitEnd,
         double iters,
         double intersecs,
         double mutates,
         double popul,
         double gens,
         int EliteCount
    );
};

#endif // LAB2_H
