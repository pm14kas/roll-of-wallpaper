#ifndef MATH_PROG_H
#define MATH_PROG_H

#include <QDateTime>
#include <QtMath>
#include "parser.h"

struct Output
{
public:
    std::vector<KeyValue> result;
    double fmin;
    qint64 timer;

    Output()
    {
        timer = 0;
    }
    Output(const Output& other)
    {
        result = other.result;
        timer = other.timer;
    }
};

class math_prog
{
public:
    math_prog();
    static Output MonteCarlo(Parser f, std::vector<KeyValue> limitStart, std::vector<KeyValue> limitEnd, unsigned int maxIterations, bool HJ);
    static Output ImitOtzh(Parser f, std::vector<KeyValue> limitStart, std::vector<KeyValue> limitEnd, double T, double L, double r, double eps, bool HJ);

    static std::vector<KeyValue> generateRandomValue(std::vector<KeyValue>& limitStart, std::vector<KeyValue>& limitEnd);
    static std::vector<KeyValue> generateRandomNeighbor(std::vector<KeyValue>& x, double eps, std::vector<KeyValue> limitStart, std::vector<KeyValue> limitEnd);


    static std::vector<KeyValue> research(Parser &func, std::vector<KeyValue> x, double delta);
    static std::vector<KeyValue> HookJeeves(Parser &func, std::vector<KeyValue> x, std::vector<KeyValue> limitStart, std::vector<KeyValue> limitEnd,  double delta, double epsilon, double lambda, unsigned int maxIterations);


};

#endif // MATH_PROG_H
