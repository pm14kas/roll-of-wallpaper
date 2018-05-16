#include "intervalmath.h"

    Interval IntervalMath::abs(Interval x)
    {
        double min = 0.0;
        double max = 0.0;
        if ((x.left < 0) && (x.right > 0))
        {
            min = 0.0;
            if (std::abs(x.right) > std::abs(x.left))
                max = std::abs(x.right);
            else
                max = std::abs(x.left);
        }
        else if (x.left > 0.0 && x.right > 0.0)
        {
            min = std::abs(x.left);
            max = std::abs(x.right);
        }
        else if (x.left < 0.0 && x.right < 0.0)
        {
            max = std::abs(x.left);
            min = std::abs(x.right);
        }
        return Interval(min, max);
    }

    Interval IntervalMath::sqrt(Interval x)
    {
        double min;
        double max;

        if (x.left < 0) {
            min = 0;
        } else {
            min = std::sqrt(x.left);
        }

        if (x.right < 0) {
            max = 0;
        } else {
            max = std::sqrt(x.right);
        }

        return Interval(min, max);
    }

    Interval IntervalMath::pow(Interval x, Interval y)
    {
       Interval result(-1, 1);
       if (y.left == y.right)
       {
           if ((y.left - (int)y.left) == 0)
           {
               if (y.left > 0)
               {
                   if ((int)y.left % 2 == 0)
                   {
                       if ((x.left < 0) && (x.right > 0))
                           return Interval(0, std::max(std::pow(x.left, y.left), std::pow(x.right, y.left)));
                       else return Interval(std::pow(x.left, y.left), std::pow(x.right, y.left));
                   }
                   else
                   {
                       return Interval(std::pow(x.left, y.left), std::pow(x.right, y.left));
                   }
               }
               else
               {
                   if ((int)y.left % 2 == 0)
                   {
                       if (x.left < 0 && x.right > 0)
                           return Interval(std::min(std::pow(x.left, y.left), std::pow(x.right, y.left)), INFINITY);
                       else return Interval(std::pow(x.left, y.left), std::pow(x.right, y.left));
                   }
                   else
                   {
                       if (x.left < 0 && x.right > 0)
                           return Interval(-INFINITY, INFINITY);
                       else
                           return Interval(std::pow(x.left, y.left), std::pow(x.right, y.left));
                   }
               }
           }
           else
           {
               if (y.left > 0)
               {
                   if (x.left < 0 && x.right < 0)
                       throw ("Невозможно посчитать функцию на данном интервале");
                   else if (x.left < 0 && x.right > 0)
                       return Interval(0, std::pow(x.right, y.left));
                   else if (x.left > 0)
                       return Interval(std::pow(x.left, y.left), std::pow(x.right, y.left));
               }
               else
               {
                   if (x.left < 0 && x.right < 0)
                       throw ("Невозможно посчитать функцию на данном интервале");
                   else if (x.left < 0 && x.right > 0)
                       return Interval(std::pow(x.right, y.left), INFINITY);
                   else if (x.left > 0)
                       return Interval(std::pow(x.left, y.left), std::pow(x.right, y.left));
               }
           }
       }
       else
           throw ("Невозможно возвести интервал в степень интервал");
       return result;
    }

    Interval IntervalMath::sin(Interval x)
    {
        if ((__DBL_MAX__ == std::abs(x.left)) || (__DBL_MAX__ == std::abs(x.right)))
        {
            return Interval(((double)rand() / (double)RAND_MAX) * 2 - 1, ((double)rand() / (double)RAND_MAX) * 2 - 1);
        }
        double min = std::sin(x.left);
        double max = std::sin(x.right);
        int k = (int)x.right;
        bool low = true;
        bool high = true;
        while ((2 * k * M_PI - M_PI / 2.0 > x.left) && (2 * k * M_PI + M_PI / 2.0 > x.left))
        {
            if (low == true)
            {
                if (2 * k * M_PI - M_PI / 2.0 < x.right)
                {
                    min = -1.0;
                    low = false;
                }
            }
            if (high == true)
            {
                if (2 * k * M_PI + M_PI / 2.0 < x.right)
                {
                    max = 1.0;
                    high = false;
                }
            }
            --k;
        }
        if (min != -1.0)
        {
            if (min > std::sin(x.right))
                min = std::sin(x.right);
        }
        if (max != 1.0)
        {
            if (max < std::sin(x.left))
                max = std::sin(x.left);
        }
        return Interval(min, max);
    }

    Interval IntervalMath::cos(Interval x)
    {
        if ((__DBL_MAX__ == std::abs(x.left)) || (__DBL_MAX__ == std::abs(x.right)))
        {
            return Interval(((double)rand() / (double)RAND_MAX) * 2 - 1, ((double)rand() / (double)RAND_MAX) * 2 - 1);
        }

       double min = std::cos(x.left);
       double max = std::cos(x.right);
       int k = (int)x.right;
       bool low = true;
       bool high = true;
       while (((2 * k + 1) * M_PI > x.left) && (2 * k * M_PI > x.left))
       {
           if (low == true)
           {
               if ((2 * k + 1) * M_PI < x.right)
               {
                   min = -1.0;
                   low = false;
               }
           }
           if (high == true)
           {
               if (2 * k * M_PI < x.right)
               {
                   max = 1.0;
                   high = false;
               }
           }
           --k;
       }
       if (min != -1.0)
       {
           if (min > std::cos(x.right))
               min = std::cos(x.right);
       }
       if (max != 1.0)
       {
           if (max < std::cos(x.left))
               max = std::cos(x.left);
       }
       return Interval(min, max);
    }
