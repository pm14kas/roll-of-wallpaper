#include "interval.h"

double Interval::min(double v1, double v2, double v3, double v4)
{
    double step1 = v1 < v2 ? v1 : v2;
    double step2 = v3 < v4 ? v3 : v4;
    return step1 < step2 ? step1 : step2;
}

double Interval::max(double v1, double v2, double v3, double v4)
{
    double step1 = v1 > v2 ? v1 : v2;
    double step2 = v3 > v4 ? v3 : v4;
    return step1 > step2 ? step1 : step2;
}

Interval::Interval(double value)
{
    this->left = value;
    this->right = value;
}

Interval::Interval(double left, double right)
{
    if (left > right) {
        this->left = right;
        this->right = left;
    } else {
        this->left = left;
        this->right = right;
    }
}

Interval::Interval(const Interval &other)
{
    this->left = other.left;
    this->right = other.right;
}

Interval::Interval(QString s)
{
    s = s.replace('[', ' ').replace(']', ' ').replace(',', '.');
    s = s.trimmed();
    QStringList values = s.split(';', QString::SkipEmptyParts);
    if (values.size() != 2) {
        throw Interval::Exception::ExceptionParseStructure;
    }
    if (!
            (
                Parser::tryParse(values[0].trimmed().toStdString(), this->left) &&
                Parser::tryParse(values[1].trimmed().toStdString(), this->right)
            )
        ) {
        throw Interval::Exception::ExceptionParseNumber;
    }
}

Interval::Interval(std::string s)
    :Interval(QString::fromStdString(s))
{

}

Interval Interval::operator=(const Interval &other)
{
    this->left = other.left;
    this->right = other.right;

    return (*this);
}

Interval Interval::operator+(const Interval &rhs)
{
    return Interval(this->left + rhs.left, this->right + rhs.right);
}

Interval Interval::operator-(const Interval &rhs)
{
    return Interval(this->left - rhs.right, this->right - rhs.left);
}

Interval Interval::operator*(const Interval &rhs)
{
    double temp1 = this->left * rhs.left;
    double temp2 = this->left * rhs.right;
    double temp3 = this->right * rhs.left;
    double temp4 = this->right * rhs.right;

    return Interval(Interval::min(temp1, temp2, temp3, temp4), Interval::max(temp1, temp2, temp3, temp4));
}

double Interval::getLeft()
{
    return this->left;
}

double Interval::getRight()
{
    return this->right;
}

QString Interval::toString(bool temp)
{
    if (temp) {
        temp = !temp;
    }
    return "[" + QString::number(this->getLeft()) + ";" + QString::number(this->getRight()) + "]";
}

std::string Interval::toString()
{
    return this->toString(false).toStdString();
}

Interval Interval::fromString(QString s)
{
    return Interval(s);
}

Interval Interval::fromString(std::string s)
{
    return Interval(s);
}

Interval Interval::operator/(const Interval &rhs)
{
   double tmp1, tmp2, tmp3, tmp4;
   double min = 0;
   double max = 0;
   if ((rhs.left < 0) && (rhs.right > 0))
   {
       min = -__DBL_MAX__;
       max = __DBL_MAX__;
   }
   else if ((rhs.left == 0) && (rhs.right > 0))
   {
       max = __DBL_MAX__;
       tmp1 = this->left / rhs.right;
       tmp2 = this->right / rhs.right;
       min = tmp1 < tmp2 ? tmp1 : tmp2;
   }
   else if ((rhs.right == 0) && (rhs.left < 0))
   {
       min = -__DBL_MAX__;
       tmp1 = this->left / rhs.left;
       tmp2 = this->right / rhs.left;
       max = tmp1 < tmp2 ? tmp1 : tmp2;
   }
   else if ((rhs.left > 0) || (rhs.right < 0))
   {
       tmp1 = this->left / rhs.left;
       tmp2 = this->right / rhs.right;
       tmp3 = this->left / rhs.right;
       tmp4 = this->right / rhs.left;
       min = Interval::min(tmp1, tmp2, tmp3, tmp4);
       max = Interval::max(tmp1, tmp2, tmp3, tmp4);
   }

   return Interval(min, max);
}

bool operator==(const Interval &lhs, const Interval &rhs)
{
    if((lhs.left == rhs.left && lhs.right == rhs.right) ||
            (lhs.left == rhs.right && lhs.right == rhs.left)) {
        return true;
    }

    return false;
}

bool operator!=(const Interval &lhs, const Interval &rhs)
{
    return !(lhs == rhs);
}

