#ifndef INTERVALMETHOD_H
#define INTERVALMETHOD_H

#include <QList>
#include <math.h>
#include "parser.h"
#include "math_prog.h"

struct OutputInterval
{
public:
    std::vector<KeyValueInterval> result;
    Interval fmin;
    qint64 timer;

    OutputInterval()
    {
        timer = 0;
    }
    OutputInterval(const OutputInterval& other)
    {
        result = other.result;
        timer = other.timer;
    }
};

class IntervalMethod
{
public:
    IntervalMethod();
    static OutputInterval calc(
            Parser simpleParser,
            ParserInterval parser,
            std::vector<KeyValueInterval> limits,
            double epsilon
            );
    static double wid(std::vector<KeyValueInterval> var, int &index);
};

#endif // INTERVALMETHOD_H
