#ifndef UTILS_H
#define UTILS_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>
#include "Params.h"

const double Pi = M_PI;
enum curve_type { ellipse_, parabola_, hyperbola_ };
typedef std::unordered_map<Params, int, Hasher_for_Params> accum_map;

struct Diapason
{
	double min;
	double max;
	Diapason(double min_val, double max_val) : min(min_val), max(max_val) {}
	Diapason() {}
};

#endif /* UTILS_H */