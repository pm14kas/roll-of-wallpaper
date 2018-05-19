#include "intervalmethod.h"

IntervalMethod::IntervalMethod()
{

}

double IntervalMethod::wid(std::vector<KeyValueInterval> var, int &index)
{
    double res = 0;
    for (int i = 0; i < var.size(); ++i)
    {
        if (res < var[i].d.right - var[i].d.left)
        {
            res = var[i].d.right - var[i].d.left;
            index = i;
        }
    }
    return res;
}

Interval IntervalMethod::diff(ParserInterval &func, std::vector<KeyValueInterval> &x, int count, double step)
{
    if (step == 0)
    {
        step = 0.01;
    }

    std::vector<KeyValueInterval> tempA = x;
    std::vector<KeyValueInterval> tempB = x;

    tempA[count] -= step;
    tempB[count] += step;

    Interval df = (func.calc(tempB) - func.calc(tempA)) / (2 * step);
    return df;
}

Interval IntervalMethod::diffCopy(ParserInterval &func, std::vector<KeyValueInterval> &x, int count, double step)
{
    if (step == 0)
    {
        step = 0.01;
    }

    std::vector<KeyValueInterval> tempA = x;
    std::vector<KeyValueInterval> tempB = x;
    std::vector<KeyValueInterval> tempC = x;

    tempA[count] -= step;
    tempB[count] += step;

    Interval df = (func.calc(tempB) - func.calc(tempC) * 2 + func.calc(tempA)) / (step * step);
    return df;
}

Interval IntervalMethod::diffCopy1(ParserInterval &func, std::vector<KeyValueInterval> &x, int count1, int count2, double step)
{
    if (step == 0)
    {
        step = 0.01;
    }

    std::vector<KeyValueInterval> tempA = x;
    std::vector<KeyValueInterval> tempB = x;
    std::vector<KeyValueInterval> tempC = x;
    std::vector<KeyValueInterval> tempD = x;

    tempA[count1] += step;
    tempA[count2] += step;

    tempB[count1] -= step;
    tempB[count2] += step;

    tempC[count1] += step;
    tempC[count2] -= step;

    tempD[count1] -= step;
    tempD[count2] -= step;


    Interval df = (func.calc(tempA) - func.calc(tempB) - func.calc(tempC) + func.calc(tempD)) / (step * step * 4);
    return df;

}

std::vector<Interval> IntervalMethod::diff1(ParserInterval &func, std::vector<KeyValueInterval> x, double step)
{
    std::vector<Interval> E;
    for (unsigned int i = 0; i < x.size(); i++)
    {
        E.push_back(diff(func, x, i, step));
    }
    return E;
}

std::vector<Interval> IntervalMethod::diff2(ParserInterval &func, std::vector<KeyValueInterval> x, double step)
{
    std::vector<Interval> E;
    for (unsigned int i = 0; i < x.size(); i++)
    {
        E.push_back(diffCopy(func, x, i, step));
    }
    return E;
}

std::vector<std::vector<Interval>> IntervalMethod::diff2Copy(ParserInterval &parser, std::vector<KeyValueInterval> x, double precision)
{
    std::vector<std::vector<Interval>> E;
    for (int i = 0; i < x.size(); i++)
    {
        E.push_back(std::vector<Interval>());
        for (int j = 0; j < x.size(); j++)
        {
            E[i].push_back(diffCopy1(parser, x, i, j, precision));
        }
    }

    return E;
}


OutputInterval IntervalMethod::calc(Parser &simpleParser, ParserInterval &parser, std::vector<KeyValueInterval> limits, double epsilon)
{
    QList<std::vector<KeyValueInterval>> L;
    QList<double> f;
    //начальные значения
    L.append(limits);
    f.append(parser.calc(limits).left);

//    if (method == "Естественная")
//        f.Add(f_natural(limits).a);
//    if (method == "Центрированная")
//        f.Add(f_centroid(limits).a);
//    if (method == "Тейлоровская 2 порядка")
//        f.Add(f_teylor(limits).a);

    QList<int> to_remove;//не прошедщие тест брусы сразу не удаляем,т.к. сбивается индекс. Заносим в список и удаляем после полного перебора списка

    double best_f = f[0];//самая маленькая нижняя граница. Чтобы не сортировать список каждый раз при оценке интервала f
    double f_mid = INFINITY;//значение "средней точки" - по сути, наименьшее значение функции, которое мы знаем
    double w_max = INFINITY;//максимальная ширина бруса


    qint64 timer = QDateTime::currentMSecsSinceEpoch();
    //условие останова- пока только  точность значения функции.
    //Если будет совсем плохо - добавить ширину интервала
    while (w_max > epsilon && f_mid - best_f > epsilon && L.size() > 0)
    {

        //п.4.3
        //просматриваем список L
        w_max = 0;
        best_f = f[L.size() - 1];//список уже не тот, что прежде, best_f придется находить заново
        int i_max = L.size();//т.к. размер списка L будет меняться в цикле
        for (int i = 0; i < i_max; ++i)
        {
            std::vector<std::vector<KeyValueInterval>> l;
            l.push_back(limits);
            l.push_back(limits);
            int bisection_index = 0;
            //производим бисекцию
            w_max = qMax(w_max, wid(L[i], bisection_index));
            for (unsigned int j = 0; j < L[i].size(); ++j)
            {
                if (j == bisection_index)
                {
                    l[0][j].d.left = L[i][j].d.left;
                    l[0][j].d.right = L[i][j].d.left + (L[i][j].d.right - L[i][j].d.left) / 2;
                    l[1][j].d.left = L[i][j].d.left + (L[i][j].d.right - L[i][j].d.left) / 2;
                    l[1][j].d.right = L[i][j].d.right;
                }
                else
                {
                    l[0][j] = L[i][j]; //если разбиваем не по этой координате, оставляем границы теми же
                    l[1][j] = L[i][j];
                }
            }
            to_remove.append(i);
            //исследуем два новых подбруса, находим f_c
            for (int k = 0; k < 2; ++k)
            {
                Interval f_nat = parser.calc(l[k]);
                Interval f_c(0, 0);
                std::vector<KeyValue> x_mid(l[k].size());
                for (unsigned int j = 0; j < l[k].size(); ++j){
                    x_mid[j].s = limits[j].s;
                    x_mid[j].d = l[k][j].d.left + (l[k][j].d.right - l[k][j].d.left) / 2;
                }

                double f_mid_tmp = simpleParser.calc(x_mid);
                if (f_mid > f_mid_tmp)
                    f_mid = f_mid_tmp;//обновляем среднюю точку, если нашли значение меньше
                //находим центрированную функцию только если do_NOU_test=true
//                if (do_NOU_test)
//                {
//                    f_c = f_c + f_mid_tmp;
//                    Interval[] g = grad(l[k]);
//                    for (int j = 0; j < l[k].Length; ++j)
//                        f_c = f_c + g[j] * (l[k][j] - x_mid[j]);
//                }
//                else
                    f_c = f_nat;

                double new_f = qMax(f_nat.left, f_c.left);
                if (new_f <= f_mid + std::numeric_limits<double>::epsilon())//тест на среднюю точку
                {
                    L.append(l[k]);
                    f.append(new_f);

                    //проверяем, не лучший ли это брус
                    if (new_f < best_f)
                        best_f = new_f;
                }
            }


        }//конец перебора списка
        //удаляем косячные брусы, которые мы занесли в очередь to_remove
        std::sort(to_remove.begin(), to_remove.end());//сортировка в порядке возрастания
        for (int q = to_remove.size() - 1; q >= 0 && L.size() > 0; q--) //удаляем с конца, чтобы нумерация в L не сбивалась
        {
            L.removeAt(to_remove[q]);
            f.removeAt(to_remove[q]);
        }
        to_remove.clear();

        //п.4.4 - удаление брусов, не проходящих тест на среднюю точку
        for (int i = 0; i < L.size(); ++i)
        {
            if (f[i] > f_mid)
                to_remove.append(i);
        }
        //удаляем косячные брусы, которые мы занесли в очередь to_remove
        std::sort(to_remove.begin(), to_remove.end());//сортировка в порядке возрастания
        for (int q = to_remove.size() - 1; q >= 0 && L.size() > 0; q--) //удаляем с конца, чтобы нумерация в L не сбивалась
        {
            L.removeAt(to_remove[q]);
            f.removeAt(to_remove[q]);
        }
        to_remove.clear();

    }//конец цикла while wid>epssilon


    //сортируем брусы по возрастанию значения нижней границы
    bool flag = true;
    while (flag == true)
    {
        flag = false;
        for (int i = 0; i < f.size() - 1; ++i)
        {
            if (f[i] > f[i + 1])
            {
                double q = f[i];
                f[i] = f[i + 1];
                f[i + 1] = q;
                std::vector<KeyValueInterval> t = L[i];
                L[i] = L[i + 1];
                L[i + 1] = t;
                flag = true;
            }

        }
    }

    OutputInterval o;

    o.result = L[0];
    o.timer = QDateTime::currentMSecsSinceEpoch()-timer;
    o.fmin = Interval(best_f, f_mid);
    return o;

    //return new Interval(best_f, f_mid);
}
