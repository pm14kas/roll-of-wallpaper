#ifndef INDIDVID_H
#define INDIDVID_H

#include <vector>
#include "qmath.h"
#include "parser.h"

class Individ
{
public:
    std::vector<long> G;
    std::vector<double> X;
    static int g_size;
    double F;
    Individ(std::vector<KeyValue> variables)
    {
        X.resize(variables.size());
        G.resize(variables.size());
    }
    ~Individ() { }
    void ToGray(std::vector<KeyValue> limitStart, std::vector<KeyValue> limitEnd)
    {
        for (int i = 0; i < limitEnd.size(); i++)
        {
            // получить индекс числа в диапазоне [a;b]
            int index = (int)qFloor((X[i] - limitStart[i].d) / (limitEnd[i].d - limitStart[i].d) * (double)0xFFFFFFFF); //
            // преобразовать в код грэя
            G[i] = index ^ ((index & 0xEEEEEEEE) >> 1); // как взяли из интернета, для алгоритма
        }
    }
    void ToDec(std::vector<KeyValue> limitStart, std::vector<KeyValue> limitEnd)
    {
        for(int i = 0; i < limitEnd.size(); i++)
        {
            long index = G[i];
            index = index ^ ((index&0x88888888)>>3)^((index& 0xCCCCCCCC) >> 2)^((index& 0xEEEEEEEE) >> 1);
            X[i] = qMax(limitStart[i].d, qMin(limitEnd[i].d, limitStart[i].d + (limitEnd[i].d - limitStart[i].d) / (double)0xFFFFFFFF * (double)index));
        }
    }

    void Crossingover(Individ a, std::vector<KeyValue> variables)
    {
        int pos = (int)qFloor((double)(variables.size() - 1) * (double)rand()/(RAND_MAX));//позиция по которой хромосомы разделяются
        for (int i = pos; i < variables.size(); i++)
        {
            X[i]=a.X[i];
            G[i]=a.G[i];
        }
    }

    void Mutation(std::vector<KeyValue> limitStart, std::vector<KeyValue> limitEnd)
    {
        int pos = (int)qFloor((double)(limitStart.size() - 1) * (double)rand()/(RAND_MAX));
        G[pos] = (int)qFloor((double)0xFFFFFFFF * (double)rand()/(RAND_MAX));
        long index = G[pos];
        index = index ^ ((index&0x88888888)>>3)^((index& 0xCCCCCCCC) >>2)^((index& 0xEEEEEEEE) >>1);
        X[pos] = qMax(limitStart[pos].d, qMin(limitEnd[pos].d, limitStart[pos].d + (limitEnd[pos].d - limitStart[pos].d) / (double)0xFFFFFFFF * (double)index));
    }
};


#endif // INDIDVID_H
