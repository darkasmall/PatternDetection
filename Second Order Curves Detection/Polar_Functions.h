#ifndef POLAR_FUNCTIONS_H
#define POLAR_FUNCTIONS_H

#include <vector>
#include "Params.h"
#include "Coordinates_Corelation.h"
#include "Utils.h"


typedef double(*polar_func)(double, const std::vector<double> &);

/// ƒл€ демонстрации пол€рных функций
const std::vector<double> params_for_circle = { 50 };  // радиус
const std::vector<double> params_for_heart = { 150, 150, 150 };
const std::vector<double> params_for_rose = { 200, 19 }; // длина лепестков, их количество
const std::vector<double> params_for_leminiscata = { 100, 3 }; // 1/2 рассто€ни€ между фокусами, 1/2 количества "лепестков"


class Polar_Functions
{
public:
	static double polar_circle(double phi, const std::vector<double> & params)
	{
		double rad = params[0];
		return rad;
	}

	static double polar_heart(double phi, const std::vector<double> & params)
	{
		double a = params[0];
		double b = params[1];
		double c = params[2];
		return a - b * sin(phi) + sin(phi) * sqrt(abs(cos(phi))) / (sin(phi) + c);
	}

	static double polar_rose(double phi, const std::vector<double> & params)
	{
		double a = params[0];
		double b = params[1];
		return a * cos(b * phi);
	}

	static double polar_leminiscata(double phi, const std::vector<double> & params)
	{
		double c = params[0];
		double k = params[1];
		return sqrt(abs(2 * c * c * cos(phi * k)));
	}

	static double polar_curve(double phi, const std::vector<double> & params)
	{
		double p = params[0];
		double eps = params[1];
		return p / (1 + eps * cos(phi));
	}

	static double polar_curve(double phi, const Params & params)
	{
		return params.p / (1 + params.e * cos(phi));
	}

	static std::vector<Coordinates_Corelation> get_points_from_polar_func(polar_func func, const std::vector<double> & params, Diapason phi_diapason, double step)
	{
		std::vector<Coordinates_Corelation> res;
		for (double phi = phi_diapason.min; phi <= phi_diapason.max; phi += step)
			res.push_back(Coordinates_Corelation(Polar_Point(func(phi, params), phi)));
		return res;
	}

	static std::pair<Diapason, Diapason> get_diapasons_of_xy(const std::vector<Coordinates_Corelation> & points)
	{
		double min_x = INT16_MAX;
		double min_y = INT16_MAX;
		double max_x = INT16_MIN;
		double max_y = INT16_MIN;

		for (size_t i = 0; i < points.size(); ++i)
		{
			if (points[i].decart.x < min_x)
				min_x = points[i].decart.x;
			if (points[i].decart.x > max_x)
				max_x = points[i].decart.x;
			if (points[i].decart.y < min_y)
				min_y = points[i].decart.y;
			if (points[i].decart.y > max_y)
				max_y = points[i].decart.y;
		}
		return std::make_pair(Diapason(min_x, max_x), Diapason(min_y, max_y));
	}

	static cv::Mat draw(const std::vector<Coordinates_Corelation> & points)
	{
		auto diapasons_pair = get_diapasons_of_xy(points);
		double min_x = diapasons_pair.first.min;
		double min_y = diapasons_pair.second.min;
		double max_x = diapasons_pair.first.max;
		double max_y = diapasons_pair.second.max;

		double dif_x = 0 - min_x;
		double dif_y = 0 - min_y;

		int width = ceil(max_x - min_x);
		int height = ceil(max_y - min_y);

		cv::Mat res = cv::Mat(height, width, CV_8UC1, cv::Scalar(0, 0, 0));
		//cv::Mat res = cv::Mat(600, 600, CV_8UC1, cv::Scalar(0, 0, 0));
		for (size_t i = 0; i < points.size(); ++i)
		{
			int cur_x = points[i].decart.x + dif_x;
			int cur_y = points[i].decart.y + dif_y;
			//if (cur_x > 0 && cur_x < res.cols && cur_y > 0 && cur_y < res.rows)
			//{
			//	res.at<uchar>(cur_y, cur_x) = 255;
			//	if (i>0)
			//	cv::line(res,cv:: Point(points[i - 1].decart.x + dif_x, points[i - 1].decart.y + dif_y), cv::Point(cur_x, cur_y), cv::Scalar(255, 0, 0));
			//}
			res.at<uchar>(cur_y, cur_x) = 255;
		}

		return res;
	}
};


#endif /* POLAR_FUNCTIONS_H */
