#ifndef COMBINEDMETHOD_H
#define COMBINEDMETHOD_H

#include "parser.h"
#include "parser_interval.h"
#include "indidvid.h"
#include "individinterval.h"
#include "math_prog.h"
#include "intervalmethod.h"

class CombinedMethod
{
public:
    CombinedMethod();
    static Output calc(Parser fsimple,ParserInterval f,
         std::vector<KeyValueInterval> limits,
         double iters,
         double intersecs,
         double mutates,
         double popul,
         double gens,
         int EliteCount
    , double maxWidth);
};

#endif // COMBINEDMETHOD_H
