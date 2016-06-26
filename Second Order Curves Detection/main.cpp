#include <iostream>
#include "Solver.h"
using namespace std;
using namespace cv;

int main()
{
	string filename = "test(13;0.8).jpg";
	Solver s(filename, ellipse_);
	s.draw_answer();

	//vector<double> params{50, 5};

//	auto ell = Polar_Functions::get_points_from_polar_func(Polar_Functions::polar_curve, params, Diapason(0, 2 * Pi), 0.1);
//	auto res = Polar_Functions::draw(ell);
//	imshow("ggf", res);
////	imwrite("rr.jpg", res);
//	waitKey(0);

	//string filename = "mirror.jpg";
	//Mat im = imread(filename, 1);
	//Mat canny = Image_Processing::process(im);
	////imshow("cds", canny);
	//imwrite("1.jpg", canny);
	//waitKey(0);
	system("pause");
}