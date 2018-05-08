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
    double fmin = DBL_MAX;

    std::vector<KeyValue> x = limitEnd;
    //сформировать первую случайную популяцию

    qint64 timer = QDateTime::currentMSecsSinceEpoch();
    for (int pr = 0; pr < iters; pr++)
    {
    for (int i = 0; i < popul; i++)
    {
        for (int k = 0; k < 10; k++)
        {
            Individ individ(limitEnd);
            for (int j = 0; j < varend; j++)
            {
                individ.X[j] = limitStart[j].d + (limitEnd[j].d - limitStart[j].d) * (double)rand()/(RAND_MAX);
                x[j].d = individ.X[j];
            }

            individ.F = f.calc(x);
            if (individ.F < fmin)
            {
                for (int l = 0; l < varend; l++)
                {
                    individ.X[l] = x[l].d;
                    fmin = individ.X[l];
                }
                //fmin = individ.F;
            }
            individ.ToGray(limitStart, limitEnd);
            generation.push_back(individ);
        }
    }
    // Цикл поколений
    double temp;
    for (int gen = 0; gen < gens; gen++)
    {
        std::sort(generation.begin(), generation.end(), [](const Individ& a, const Individ& b)
        {
            return a.F < b.F;
        });
        //generation.Sort(cmpIndivid);

        for (int i = EliteCount; i < popul; i++)
        {
            if((double)rand()/(RAND_MAX)<=mutates) generation[i].Mutation(limitStart, limitEnd);
            if((double)rand()/(RAND_MAX)<=intersecs) generation[i].Crossingover(generation[(int)qFloor((double)(EliteCount-1)*(double)rand()/(RAND_MAX))], limitStart);

            for (int l = 0; l < varend; l++){
                x[l].d = generation[i].X[l];
            }
            generation[i].F = f.calc(x);
        }
    }//Цикл поколений

    // выбрать из последнего поколения лучшего
    //generation.Sort(cmpIndivid);
    std::sort(generation.begin(), generation.end(), [](const Individ& a, const Individ& b)
    {
        return a.F < b.F;
    });
    for(int i = 0; i < generation.size(); i++){
        for(int j = i + 1; j < generation.size(); j++){
            if ((generation[i].F) < (generation[j].F)){
                temp = generation[i].F;
                generation[i].F = generation[j].F;
                generation[j].F = temp;
            }
        }
    }
    //лучший станет на первом месте
    }

    Output o;
    if (generation.size() != 0)
    {
        o.result = x;
        o.timer = QDateTime::currentMSecsSinceEpoch()-timer;
        o.fmin = generation[0].F;
    }
    return o;
}
