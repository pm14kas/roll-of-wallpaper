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
            Parser &simpleParser,
            ParserInterval &parser,
            std::vector<KeyValueInterval> limits,
            double epsilon
            );
    static double wid(std::vector<KeyValueInterval> var, int &index);

    static Interval diff(ParserInterval &func, std::vector<KeyValueInterval> &x, int count, double step = 0.01);
    static Interval diffCopy(ParserInterval &func, std::vector<KeyValueInterval> &x, int count, double step = 0.01);
    static Interval diffCopy1(ParserInterval &func, std::vector<KeyValueInterval> &x, int count1, int count2, double step = 0.01);

    static std::vector<Interval> diff1(ParserInterval &parser, std::vector<KeyValueInterval> x, double precision = 0.01);
    static std::vector<Interval> diff2(ParserInterval &parser, std::vector<KeyValueInterval> x, double precision = 0.01);
    static std::vector<std::vector<Interval>> diff2Copy(ParserInterval &parser, std::vector<KeyValueInterval> x, double precision = 0.01);
};

#endif // INTERVALMETHOD_H
