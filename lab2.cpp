#include "lab2.h"

lab2::lab2()
{

}

Output lab2::calc(Parser f,
                  std::vector<KeyValue> limitStart,
                  std::vector<KeyValue> limitEnd,
                  double iters,
                  double intersecs,
                  double mutates,
                  double popul,
                  double gens,
                  int EliteCount)
{
    int varend = limitEnd.size();
    // формирование рабочей структуры данных
    Individ::g_size = varend;// установить глобально количество хромосом для индивида
    std::vector<Individ> generation;  //одно поколение

    qint64 timer = QDateTime::currentMSecsSinceEpoch();
    Individ individ(limitEnd);

    for (int pr = 0; pr < iters; pr++)
    {
    for (int i = 0; i < popul; i++)
    {
//        for (int k = 0; k < 10; k++)
//        {
            for (int j = 0; j < varend; j++)
            {
                individ.X[j].d = limitStart[j].d + (limitEnd[j].d - limitStart[j].d) * (double)rand()/(RAND_MAX);
            }

            individ.F = f.calc(individ.X);

            individ.ToGray(limitStart, limitEnd);
            generation.push_back(individ);
//        }
    }
    // Цикл поколений
    for (int gen = 0; gen < gens; gen++)
    {
        std::sort(generation.begin(), generation.end(), [](const Individ& a, const Individ& b)
        {
            return a.F < b.F;
        });

        for (int i = EliteCount; i < popul; i++)
        {
            if((double)rand()/RAND_MAX <= mutates)
                generation[i].Mutation(limitStart, limitEnd);
            if((double)rand()/RAND_MAX <= intersecs)
                generation[i].Crossingover(generation[0], limitStart);

            generation[i].F = f.calc(generation[i].X);
        }
    }//Цикл поколений

    std::sort(generation.begin(), generation.end(), [](const Individ& a, const Individ& b)
    {
        return a.F < b.F;
    });
    //лучший станет на первом месте
    }

    Output o;

    o.result = generation[0].X;
    o.timer = QDateTime::currentMSecsSinceEpoch()-timer;
    o.fmin = generation[0].F;
    return o;
}
