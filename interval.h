#pragma once

#include <vector>
#include <string>
#include <cctype>
#include 'parser.h'

class Interval
{
private:
	double left;
	double right;
public:
	Interval();
	Interval(double value);
	Interval(const Interval &other);
	~Interval();

	Interval operator+(Interval &other);
}