#ifndef COORDINATES_CORELATION_H
#define COORDINATES_CORELATION_H
#include "Polar_Point.h"
#include "Affine_Transforms.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

struct Coordinates_Corelation
{
	cv::Point2d decart;
	Polar_Point polar;
	Coordinates_Corelation(const cv::Point2d & decart_)
	{
		decart = decart_;
		double x = decart.x;
		double y = decart.y;
		double r = sqrt(x*x + y*y);
		double phi = atan2(y, x);
		polar = Polar_Point(r, phi);
	}
	Coordinates_Corelation(const Polar_Point & polar_)
	{
		polar = polar_;
		double r = polar.r;
		double phi = polar.phi;
		double x = r * cos(phi);
		double y = r * sin(phi);
		decart = cv::Point2d(x, y);
	}
	Coordinates_Corelation() {}
};

#endif /* COORDINATES_CORELATION_H */
