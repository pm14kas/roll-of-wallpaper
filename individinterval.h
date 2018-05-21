#ifndef INDIVIDINTERVAL_H
#define INDIVIDINTERVAL_H

#include <vector>
#include <algorithm>
#include <float.h>
#include "qmath.h"
#include "parser_interval.h"

class IndividInterval
{
public:
    long *G;
    std::vector<KeyValueInterval> X;
    std::vector<double> interval_width;
    static int g_size;
    Interval F;
    IndividInterval(std::vector<KeyValueInterval> &variables)
    {
        X.resize(variables.size());
        interval_width.resize(variables.size());
        for (unsigned int i = 0; i < variables.size(); i++){
            X[i] = variables[i];
            interval_width[i] = X[i].d.right - X[i].d.left;
        }
        G = new long[variables.size()];
        F = Interval(DBL_MAX);
    }
    IndividInterval() { }
    ~IndividInterval() { }
    void ToGray(std::vector<KeyValueInterval> &limits)
    {
        for (unsigned int i = 0; i < limits.size(); i++)
        {
            // получить индекс числа в диапазоне [a;b]
            interval_width[i] = X[i].d.right - X[i].d.left;
            double center = X[i].d.left + (X[i].d.right - X[i].d.left) / 2;
            int index = qFloor((center - limits[i].d.left) / (limits[i].d.right - limits[i].d.left) * (qreal)0xFFFFFFFF); //
            // преобразовать в код грэя
            G[i] = index ^ ((index & 0xEEEEEEEE) >> 1); // как взяли из интернета, для алгоритма
        }
    }
    void ToDec(std::vector<KeyValueInterval> &limits)
    {
        for(unsigned int i = 0; i < limits.size(); i++)
        {
            long index = G[i];
            double center = 0;
            index = index ^ ((index&0x88888888)>>3)^((index& 0xCCCCCCCC) >> 2)^((index& 0xEEEEEEEE) >> 1);
            center = qMax(limits[i].d.left, qMin(limits[i].d.right, limits[i].d.left + (limits[i].d.right - limits[i].d.left) / (qreal)0xFFFFFFFF * (qreal)index));
            X[i].d.left  = center - interval_width[i] / 2;
            X[i].d.right = center + interval_width[i] / 2;
        }
    }

    void Crossingover(IndividInterval &a, std::vector<KeyValueInterval> &variables)
    {
        int pos = qFloor((qreal)(variables.size() - 1) * ((qreal)rand()/RAND_MAX));//позиция по которой хромосомы разделяются
        for (unsigned int i = pos; i < variables.size(); i++)
        {
            X[i].d=a.X[i].d;
            G[i]=a.G[i];
            interval_width[i] = a.interval_width[i];
        }
    }

    void Mutation(std::vector<KeyValueInterval> &limits)
    {
        int pos = qFloor((qreal)(limits.size() - 1) * ((qreal)rand()/RAND_MAX));
        G[pos] =  qFloor((qreal)0xFFFFFFFF * ((qreal)rand()/RAND_MAX));
        long index = G[pos];
        double center = 0;
        index = index ^ ((index&0x88888888)>>3)^((index& 0xCCCCCCCC) >>2)^((index& 0xEEEEEEEE) >>1);
        center = qMax(limits[pos].d.left, qMin(limits[pos].d.right, limits[pos].d.left + (limits[pos].d.right - limits[pos].d.left) / (double)0xFFFFFFFF * (double)index));
        X[pos].d.left  = center - interval_width[pos] / 2;
        X[pos].d.right = center + interval_width[pos] / 2;
    }
};
#endif // INDIVIDINTERVAL_H
