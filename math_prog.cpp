#include "math_prog.h"

math_prog::math_prog()
{

}

Output math_prog::MonteCarlo(Parser f, std::vector<KeyValue> limitStart, std::vector<KeyValue> limitEnd, unsigned int maxIterations, bool HJ)
{
    qint64 timer = QDateTime::currentMSecsSinceEpoch();
    double fmin = 1e308;
    std::vector<KeyValue> xmin = limitStart;
    std::vector<KeyValue> iteration = limitStart;
    for (unsigned int i = 0; i < maxIterations; i++)
    {
        iteration = generateRandomValue(limitStart, limitEnd);

        double fcurrent = f.calc(iteration);
        if (fcurrent < fmin)
        {
            fmin = fcurrent;
            xmin = iteration;
        }
    }

    Output result;
    result.timer = QDateTime::currentMSecsSinceEpoch()-timer;
    if(HJ)
        result.result = HookJeeves(f, xmin, limitStart, limitEnd, ((double)qrand()/(double)RAND_MAX)-0.5, 0.01, 3, 100);
    else
        result.result = xmin;
    return result;
}

Output math_prog::ImitOtzh(Parser f, std::vector<KeyValue> limitStart, std::vector<KeyValue> limitEnd, double T, double L, double r, double eps, bool HJ)
{
    qint64 timer = QDateTime::currentMSecsSinceEpoch();
    std::vector<KeyValue> x = generateRandomValue(limitStart, limitEnd);
    std::vector<KeyValue> xPrime = limitStart;
    std::vector<KeyValue> xl = limitStart;
    std::vector<KeyValue> xr = limitStart;
    double delta = 0;
    while (T > 0.01){
        for (int i = 0; i < L; i++)
        {
            xl = generateRandomNeighbor(x, -eps, limitStart, limitEnd);
            xr = generateRandomNeighbor(x, eps, limitStart, limitEnd);
            xPrime = generateRandomValue(xl, xr);
            delta = f.calc(xPrime) - f.calc(x);
            if (delta <= 0 || qExp(-delta / T) > (double)qrand()/(double)RAND_MAX)
            {
                x = xPrime;
            }
        }
        T *= r;
    }
    Output result;
    if(HJ)
        result.result = HookJeeves(f, x, limitStart, limitEnd, ((double)qrand()/(double)RAND_MAX)-0.5, 0.01, 3, 100);
    else
        result.result = x;
    result.timer = QDateTime::currentMSecsSinceEpoch()-timer;
    return result;
}

std::vector<KeyValue> math_prog::generateRandomValue(std::vector<KeyValue>& limitStart, std::vector<KeyValue>& limitEnd)
{
    std::vector<KeyValue> result = limitStart;
    for (unsigned int i = 0; i < result.size(); i++)
    {
        result[i] = result[i] + (qrand()/(RAND_MAX*1.0))*(limitEnd[i]-limitStart[i]).d;
    }
    return result;
}

std::vector<KeyValue> math_prog::generateRandomNeighbor(std::vector<KeyValue>& x, double eps, std::vector<KeyValue> limitStart, std::vector<KeyValue> limitEnd)
{
    std::vector<KeyValue> result = x;
    for (unsigned int i = 0; i < result.size(); i++)
    {
        result[i].d = qMin(qMax(result[i].d + eps * ((double)qrand()/(double)RAND_MAX), limitStart[i].d), limitEnd[i].d);
    }
    return result;
}


std::vector<KeyValue> math_prog::HookJeeves (Parser &func, std::vector<KeyValue> x, std::vector<KeyValue> limitStart, std::vector<KeyValue> limitEnd, double delta, double epsilon, double lambda, unsigned int maxIterations)
{
    unsigned int k = 0;

    while ( (k < maxIterations))
    {
        if (k)
        {
            if (abs(delta) > epsilon)
            {
                delta /= 2;
            }
        }
        std::vector<KeyValue> xkj = research(func, x, delta);
        unsigned int k2 = 0;
        while ((func.calc(x) > func.calc(xkj)) && (k2 < maxIterations))
        {
            std::vector<KeyValue> diff = xkj;
            for (unsigned int i = 0; i < xkj.size(); i++)
            {
                diff[i] -= x[i];
                x[i] = xkj[i];
                xkj[i] += diff[i].d * (lambda - 1);//поиск по образцу
                if (xkj[i].d < limitStart[i].d) xkj[i].d = limitStart[i].d;
                if (xkj[i].d > limitEnd[i].d) xkj[i].d = limitEnd[i].d;
                if (x[i].d < limitStart[i].d) x[i].d = limitStart[i].d;
                if (x[i].d > limitEnd[i].d) x[i].d = limitEnd[i].d;
                xkj = research(func, xkj, delta);
                if (xkj[i].d < limitStart[i].d) xkj[i].d = limitStart[i].d;
                if (xkj[i].d > limitEnd[i].d) xkj[i].d = limitEnd[i].d;
            }
            k2++;
        }

        k++;

        bool test = true;

        if (delta > epsilon)
        {
            delta /= 2;
            test = false;
        }

        if (test == true)
        {
            break;
        }

    }

    return x;
}

std::vector<KeyValue> math_prog::research(Parser &func, std::vector<KeyValue> x, double delta)
{
    for (unsigned int i = 0; i < x.size(); i++)
    {
        std::vector<KeyValue> xtemp = x;
        xtemp[i] += delta;
        if (func.calc(x) > func.calc(xtemp))
        {
            x = xtemp;
            continue;
        }
        else
        {
            xtemp[i] -= delta * 2;
            if (func.calc(x) > func.calc(xtemp))
            {
                x = xtemp;
                continue;
            }
            else
            {
                xtemp = x;
            }
        }

    }
    return x;
}
