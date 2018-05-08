#ifndef INTERVAL_H
#define INTERVAL_H

#include <qstring.h>
#include <qstringlist.h>
#include <string.h>
#include "parser.h"

class Interval
{
public:
    double left;
    double right;

    static double min(double v1, double v2, double v3, double v4);
    static double max(double v1, double v2, double v3, double v4);

    static double min(double v1, double v2);
    static double max(double v1, double v2);

public:

    enum Exception {
        ExceptionParseStructure,
        ExceptionParseNumber,
        ExceptionCalculation,
        ExceptionOther
    };

    Interval();
    Interval(double value);
    Interval(double left, double right);
    Interval(const Interval &other);
    Interval(QString s);
    Interval(std::string s);

    Interval operator=(const Interval &other);
    Interval operator+(const Interval &rhs);
    Interval operator-(const Interval &rhs);
    Interval operator*(const Interval &rhs);
    Interval operator/(const Interval &rhs);

    friend bool operator==(const Interval &lhs, const Interval &rhs);
    friend bool operator!=(const Interval &lhs, const Interval &rhs);

    double getLeft();
    double getRight();

    QString toString(bool temp);
    std::string toString();

    static Interval fromString(QString s);
    static Interval fromString(std::string s);

    static bool TryParse(std::string s, Interval& result);


};

#endif // INTERVAL_H
