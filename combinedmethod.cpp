#include "combinedmethod.h"

CombinedMethod::CombinedMethod()
{

}

Output CombinedMethod::calc(Parser fsimple, ParserInterval f,
                  std::vector<KeyValueInterval> limits,
                  double iters,
                  double intersecs,
                  double mutates,
                  double popul,
                  double gens,
                  int EliteCount,
                  double maxWidth)
{
    int varend = limits.size();
    // формирование рабочей структуры данных
    Individ::g_size = varend;// установить глобально количество хромосом для индивида
    std::vector<IndividInterval> generation;  //одно поколение

    qint64 timer = QDateTime::currentMSecsSinceEpoch();
    IndividInterval individ(limits);

    for (int pr = 0; pr < iters; pr++)
    {
    for (int i = 0; i < popul; i++)
    {
//        for (int k = 0; k < 10; k++)
//        {
            for (int j = 0; j < varend; j++)
            {
                individ.X[j].d.left = limits[j].d.left + (limits[j].d.right - limits[j].d.left) * (double)rand()/(RAND_MAX);
                individ.X[j].d.right = individ.X[j].d.left + maxWidth * (double)rand()/(RAND_MAX);
            }

            individ.F = f.calc(individ.X);

            individ.ToGray(limits);
            generation.push_back(individ);
//        }
    }
    // Цикл поколений
    for (int gen = 0; gen < gens; gen++)
    {
        std::sort(generation.begin(), generation.end(), [](const IndividInterval& a, const IndividInterval& b)
        {
            return a.F.left < b.F.left;
            //return (a.F.right - a.F.left) / 2 < (b.F.right - b.F.left) / 2;
        });

        for (int i = EliteCount; i < popul; i++)
        {
            if((double)rand()/RAND_MAX <= mutates)
                generation[i].Mutation(limits);
            if((double)rand()/RAND_MAX <= intersecs)
                generation[i].Crossingover(generation[0], limits);

            generation[i].F = f.calc(generation[i].X);
        }
    }//Цикл поколений

    std::sort(generation.begin(), generation.end(), [](const IndividInterval& a, const IndividInterval& b)
    {
        return a.F.left < b.F.left;
        //return (a.F.right - a.F.left) / 2 < (b.F.right - b.F.left) / 2;
    });
    //лучший станет на первом месте
    }

    Output o;
    double m_w = 0;
    std::vector<KeyValue> x;
    std::vector<KeyValue> start;
    std::vector<KeyValue> end;
    for(unsigned i = 0; i < varend; i++){
        x.push_back(KeyValue(generation[0].X[i].s, (generation[0].X[i].d.right - generation[0].X[i].d.left) / 2));
        m_w = qMax(m_w, generation[0].X[i].d.right - generation[0].X[i].d.left);
        start.push_back(KeyValue(generation[0].X[i].s, limits[i].d.left));
        end.push_back(KeyValue(generation[0].X[i].s, limits[i].d.right));
    }
    o.result = math_prog::HookJeeves(fsimple, x, start, end, m_w / 2, 0.000001, 3, 1000000);
    o.timer = QDateTime::currentMSecsSinceEpoch()-timer;
    //o.fmin = generation[0].F;
    return o;
}
