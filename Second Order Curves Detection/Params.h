#ifndef PARAMS_H
#define PARAMS_H
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

struct Params
{
	double p;   // фокальный параметр
	double e;   // эксцентриситет
	cv::Point2d F;// точка фокуса
	Params(double p_, double e_, cv::Point2d F_) :
		p(p_), e(e_), F(F_) {}
	Params() {}
	friend bool operator==(const Params & params_1, const Params & params_2);
	friend bool operator!=(const Params & params_1, const Params & params_2);
};

struct Hasher_for_Params
{
	size_t operator()(const Params& p) const;
};

#endif /* PARAMS_H */