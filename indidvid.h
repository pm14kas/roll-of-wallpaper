#ifndef INDIDVID_H
#define INDIDVID_H

#include <vector>
#include "qmath.h"
#include "parser.h"

class Individ
{
public:
    std::vector<long> G;
    std::vector<KeyValue> X;
    static int g_size;
    double F;
    Individ(std::vector<KeyValue> &variables)
    {
        X.resize(variables.size());
        for (unsigned int i = 0; i < variables.size(); i++){
            X[i].s = variables[i].s;
        }
        G.resize(variables.size());
    }
    ~Individ() { }
    void ToGray(std::vector<KeyValue> &limitStart, std::vector<KeyValue> &limitEnd)
    {
        for (unsigned int i = 0; i < limitEnd.size(); i++)
        {
            // получить индекс числа в диапазоне [a;b]
            int index = qFloor((X[i].d - limitStart[i].d) / (limitEnd[i].d - limitStart[i].d) * (qreal)0xFFFFFFFF); //
            // преобразовать в код грэя
            G[i] = index ^ ((index & 0xEEEEEEEE) >> 1); // как взяли из интернета, для алгоритма
        }
    }
    void ToDec(std::vector<KeyValue> &limitStart, std::vector<KeyValue> &limitEnd)
    {
        for(unsigned int i = 0; i < limitEnd.size(); i++)
        {
            long index = G[i];
            index = index ^ ((index&0x88888888)>>3)^((index& 0xCCCCCCCC) >> 2)^((index& 0xEEEEEEEE) >> 1);
            X[i].d = qMax(limitStart[i].d, qMin(limitEnd[i].d, limitStart[i].d + (limitEnd[i].d - limitStart[i].d) / (qreal)0xFFFFFFFF * (qreal)index));
        }
    }

    void Crossingover(Individ &a, std::vector<KeyValue> &variables)
    {
        int pos = qFloor((qreal)(variables.size() - 1) * ((qreal)rand()/RAND_MAX));//позиция по которой хромосомы разделяются
        for (unsigned int i = pos; i < variables.size(); i++)
        {
            X[i].d=a.X[i].d;
            G[i]=a.G[i];
        }
    }

    void Mutation(std::vector<KeyValue> &limitStart, std::vector<KeyValue> &limitEnd)
    {
        int pos = qFloor((qreal)(limitStart.size() - 1) * ((qreal)rand()/RAND_MAX));
        G[pos] =  qFloor((qreal)0xFFFFFFFF * ((qreal)rand()/RAND_MAX));
        long index = G[pos];
        index = index ^ ((index&0x88888888)>>3)^((index& 0xCCCCCCCC) >>2)^((index& 0xEEEEEEEE) >>1);
        X[pos].d = qMax(limitStart[pos].d, qMin(limitEnd[pos].d, limitStart[pos].d + (limitEnd[pos].d - limitStart[pos].d) / (double)0xFFFFFFFF * (double)index));
    }
};


#endif // INDIDVID_H
