#ifndef AFFINE_TRANSFORMS_H
#define AFFINE_TRANSFORMS_H
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class Affine_Transforms
{
public:
	static cv::Point2d shift(const cv::Point2d & point, const cv::Point2d & old_zero_point, const cv::Point2d & new_zero_point)
	{
		double new_x = point.x + (old_zero_point.x - new_zero_point.x);
		double new_y = point.y + (old_zero_point.y - new_zero_point.y);
		return cv::Point2d(new_x, new_y);
	}
};

#endif /* AFFINE_TRANSFORMS_H */
