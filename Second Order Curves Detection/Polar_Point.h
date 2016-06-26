#ifndef POLAR_POINT_H
#define POLAR_POINT_H

struct Polar_Point
{
	double r;
	double phi;
	Polar_Point(double r_, double phi_) : r(r_), phi(phi_) {}
	Polar_Point() {}
};


#endif /* POLAR_POINT_H */
