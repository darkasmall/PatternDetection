#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class Image_Processing
{
public:
	static cv::Mat process(const cv::Mat & input)
	{
		cv::Mat blur_mat, canny_mat;

		int edgeThresh = 30; // for Canny
		blur(input, blur_mat, cv::Size(3, 3));
		Canny(blur_mat, canny_mat, edgeThresh, edgeThresh * 3, 3);
		return canny_mat;
	}
};
#endif /* IMAGE_PROCESSING_H */
