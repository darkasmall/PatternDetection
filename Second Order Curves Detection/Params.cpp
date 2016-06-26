#include "Params.h"

bool operator==(const Params & params_1, const Params & params_2)
{
	return params_1.p == params_2.p &&
		params_1.e == params_2.e &&
		params_1.F == params_2.F;
}

bool operator!=(const Params & params_1, const Params & params_2)
{
	return !(params_1 == params_2);
}

size_t Hasher_for_Params::operator()(const Params & p) const
{
	int base = 13;
	return p.p + base*p.e + base*base*p.F.x + base*base*base*p.F.y;
	//hash<double> hashFn;
	//return hashFn(hashFn(p.p) + hashFn(p.F.x) + hashFn(p.F.y);
}
