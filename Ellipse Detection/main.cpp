#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <cstdlib> 
#include <ctime> 
#include <map>
#include <vector>
#include <math.h>
#include <exception>
using namespace cv;
using namespace std;



string filename = "el1.jpg";

Mat img, bimg, cimg, iimg, rimg;  //исходное, размытое, кенни, пересечение, выделенное
Mat ell;
int edgeThresh = 35; // for Canny

const double Pi = 3.14159265;
const int while_threshold = 10;
const double cthresh = sqrt(2);
const double athresh = 3;
const double bthresh = 3;

map<vector<double>, int> accumulator;

struct knowingly_false_values : public exception
{
public:
	string message;
	knowingly_false_values(const string m) : message(m){}
};
struct search_tangent_exception : public knowingly_false_values
{
	search_tangent_exception(string m) :knowingly_false_values(m){}
};
struct bad_center_values_exception : public knowingly_false_values
{
	bad_center_values_exception(string m) :knowingly_false_values(m){}
};
struct bad_semiaxis_values_exception : public  knowingly_false_values
{
	bad_semiaxis_values_exception(string m) :knowingly_false_values(m){}
};


// Возвращает случайную белую точку на черно-белом изображении с черным фоном
Point rand_point()
{
	int x, y;

	while (true)
	{
		y = rand() % (cimg.rows - 5) + 5;   //от 5 до rows - 6  
		x = rand() % (cimg.cols - 5) + 5;   //---/-----/------

		if (cimg.at<uchar>(y, x) == 255)   // если пиксель белый
			return Point(x, y);
	}
}

// Метод наименьших квадратов
pair<double, double> least_square_method(Point p)
{
	double sx = 0;   //sum x
	double sy = 0;   //sum y
	double n = 0;    // count
	double sxx = 0;  // sum x*x
	double sxy = 0;  // sum x*y

	for (int x = p.x - 3; x < p.x + 4; ++x)
	{
		for (int y = p.y - 3; y < p.y + 4; ++y)
		{
			if (cimg.at<uchar>(y, x) == 255)
			{
				n++;
				sx += x;
				sy += y;
				sxx += x*x;
				sxy += x*y;
			}
		}
	}

	// бывает деление на 0
	if (!(n*sxx - sx*sx))
		throw search_tangent_exception("In least_square_method was a division by zero");

	double a = (n*sxy - sx*sy) / (n*sxx - sx*sx);
	double b = (sy - a*sx) / n;

	return make_pair(a, b);
}

// середина отрезка p1-p2
Point middle(Point p1, Point p2)
{
	int x = (p1.x + p2.x) / 2;
	int y = (p1.y + p2.y) / 2;
	return Point(x, y);
}

// пересечение двух прямых, заданных парой параметров
Point intersection(pair<double, double> pr1, pair<double, double> pr2)
{
	double a1 = pr1.first;
	double b1 = pr1.second;
	double a2 = pr2.first;
	double b2 = pr2.second;

	int x = (b2 - b1) / (a1 - a2);
	int y = a2*x + b2;

	return Point(x, y);
}

// найти параметры уравнения прямой по двум точкам
pair<double, double> find_parameters_of_equation(Point p1, Point p2)
{
	double x2x1 = p2.x - p1.x; // x2 - x1
	double y2y1 = p2.y - p1.y;
	double a = y2y1 / x2x1;
	//double b = (p2.x*p1.y - p2.y*p1.x) / x2x1;
	double b = p2.y - a*p2.x;

	return make_pair(a, b);
}

// нахождение центра по трем точкам
Point find_center(Point p1, Point p2, Point p3)
{
	Point mid12 = middle(p1, p2);
	Point mid23 = middle(p2, p3);

	// используя метод наим кв, находим коэфф a,b из уравнения y=ax+b для касательных
	auto ab1 = least_square_method(p1);
	auto ab2 = least_square_method(p2);
	auto ab3 = least_square_method(p3);

	if (ab1.first == ab2.first || ab2.first == ab3.first)
		throw search_tangent_exception("some tangents are parallel");

	// пересечения касательных 1,2 и 2,3
	Point in12 = intersection(ab1, ab2);
	Point in23 = intersection(ab2, ab3);


	//параметры уравнения для медиан, проходящих через середину отрезка между парой точек и точкой пересечения их касательных
	pair<double, double> bisector1 = find_parameters_of_equation(mid12, in12);
	pair<double, double> bisector2 = find_parameters_of_equation(mid23, in23);


	/*line(img, Point(0, ab1.second), p1, Scalar(255, 0, 0));
	line(img, Point(0, ab2.second), p2, Scalar(0, 255, 0));
	line(img, Point(0, ab3.second), p3, Scalar(0, 0, 255));*/


	Point center = intersection(bisector1, bisector2);

	if (center.x < 0 || center.x > img.cols || center.y < 0 || center.y > img.rows)
		throw bad_center_values_exception("bad value of center point");

	return center;
}

//нахождение коэфф. уравнения эллипса по трем точкам и центру, используя метод Крамера для решения системы уравнений 
Vec3d find_coefficients_using_cramer(Point center, Point p1, Point p2, Point p3)
{
	double A, B, C, x11, x22, x33, y11, y22, y33, xy1, xy2, xy3, d, a, b, c;


	x11 = (p1.x - center.x)*(p1.x - center.x);  // (x1 - p)^2    (p,q) - центр
	x22 = (p2.x - center.x)*(p2.x - center.x);
	x33 = (p3.x - center.x)*(p3.x - center.x);

	y11 = (p1.y - center.y)*(p1.y - center.y);  // (y1 - q)^2    (p,q) - центр
	y22 = (p2.y - center.y)*(p2.y - center.y);
	y33 = (p3.y - center.y)*(p3.y - center.y);


	xy1 = 2 * (p1.x - center.x)*(p1.y - center.y);  // 2*(x1 - p)(y1 - q)    (p,q) - центр
	xy2 = 2 * (p2.x - center.x)*(p2.y - center.y);
	xy3 = 2 * (p3.x - center.x)*(p3.y - center.y);

	//   A*x11 + 2 * B*xy1 + C*y11 = 1
	//   A*x22 + 2 * B*xy2 + C*y22 = 1
	//   A*x33 + 2 * B*xy3 + C*y33 = 1

	//   | x11   xy1   y11 |
	//   | x22   xy2   y22 | 
	//   | x33   xy3   y33 | 


	d = (x11)*(xy2)*(y33)+(xy1)*(y22)*(x33)+(x22)*(xy3)*(y11)-(y11)*(xy2)*(x33)-(xy1)*(x22)*(y33)-(x11)*(xy3)*(y22);


	//   | 1   xy1   y11 |
	//   | 1   xy2   y22 | 
	//   | 1   xy3   y33 | 

	a = (xy2)*(y33)+(xy1)*(y22)+(xy3)*(y11)-(y11)*(xy2)-(xy1)*(y33)-(xy3)*(y22);



	//   | x11   1   y11 |
	//   | x22   1   y22 | 
	//   | x33   1   y33 | 


	b = (x11)*(y33)+(y22)*(x33)+(x22)*(y11)-(y11)*(x33)-(x22)*(y33)-(x11)*(y22);


	//   | x11   xy1   1 |
	//   | x22   xy2   1 | 
	//   | x33   xy3   1 | 


	c = (x11)*(xy2)+(xy1)*(x33)+(x22)*(xy3)-(xy2)*(x33)-(xy1)*(x22)-(x11)*(xy3);


	A = a / d;
	B = b / d;
	C = c / d;

	return Vec3d(A, B, C);
}

// вычисление полуосей эллипса при известных параметрах уравнения эллипса
pair<double, double> get_semiaxis(Point center, Point p1, Point p2, Point p3)
{
	double major, minor, A, C;
	Vec3d v = find_coefficients_using_cramer(center, p1, p2, p3);
	A = v[0];
	C = v[2];

	if (!A || !C)
		throw bad_semiaxis_values_exception("Zero semiaxis!");

	major = sqrt(abs(1 / A));
	minor = sqrt(abs(1 / C));

	if (major <= 0 || minor <= 0)
		throw bad_semiaxis_values_exception("Zero semiaxis!");

	return make_pair(major, minor);
}

// Вычисление периметра эллипса (формула Рамануджана)
double find_perimeter(const pair<double, double> semiaxis)
{
	double a = semiaxis.first;
	double b = semiaxis.second;
	//формула Рамануджана:
	return Pi*(3 * (a + b) - sqrt((3 * a + b)*(a + 3 * b)));
}

// Запись найденного эллипса в отдельное изображение
Mat image_of_found_ellipse(const Point center, const pair<double, double> semiaxis)
{
		Mat res(cimg.rows, cimg.cols, CV_8UC1, Scalar(0, 0, 0));
		ellipse(res, center, Size(semiaxis.first, semiaxis.second), 0, 0, 360, Scalar(255));
		return res;
}

// Подсчет совпадений эллипса с его образом
double shooting_count()
{
	double count = 0;

	for (int x = 0; x < rimg.cols; ++x)
	{
		for (int y = 0; y < rimg.rows; ++y)
		{
			if (rimg.at<uchar>(y, x) == 255)
				count++;
		}
	}
	return count;
}




int main()
{
	Point p1, p2, p3, center;
	int count = 0;
	double a, b;
	Rect ell_region;
	double shooper;
	vector<double> cur(4);
	double c,k;

	srand(unsigned(time(0)));

	img = imread(filename, 1);

	if (img.empty())
	{
		cout << "can not open " << filename << endl;
		return -1;
	}

	blur(img, bimg, Size(5, 5));
	Canny(bimg, cimg, edgeThresh, edgeThresh * 3, 3);
	

	while (true)
	{
		if (count == while_threshold)
			break;

		p1 = rand_point();
		p2 = rand_point();
		p3 = rand_point();

		try
		{
			center = find_center(p1, p2, p3);

			
			auto semiaxis = get_semiaxis(center, p1, p2, p3);
			a = semiaxis.first;
			b = semiaxis.second;

			if (center.x - a  - 1 <= 0 || center.x + a +2>= img.cols || center.y - b -1 <= 0 || center.y + b  + 2>= img.rows)
				throw bad_semiaxis_values_exception("The found ellipse is bad!");

			try
			{
				ell = image_of_found_ellipse(center, semiaxis);
			}
			catch (exception e)
			{
				continue;
			}

			iimg = cimg & ell;

			ell_region = Rect(center.x - a - 1, center.y - b - 1, 2 * a + 3, 2 * b + 3);
			iimg(ell_region).copyTo(rimg);

			shooper = find_perimeter(semiaxis)/shooting_count();

			if (shooper > 4) //мало попаданий
				continue;

			k = 0;

			for (auto it = accumulator.begin(); it != accumulator.end(); ++it)
			{
				cur = it->first;
				
				c = sqrt((cur[0] - center.x)*(cur[0] - center.x) + (cur[1] - center.y)*(cur[1] - center.y));
				
				if ((c <= cthresh) && (abs(a - cur[2]) <= athresh) && (abs(b - cur[3]) <= bthresh))
				{
					cur[0] = (cur[0] * it->second + center.x) / it->second + 1;
					cur[1] = (cur[1] * it->second + center.y) / it->second + 1;
					cur[2] = (cur[2] * it->second + a) / it->second + 1;
					cur[3] = (cur[3] * it->second + b) / it->second + 1;
					(it->second)++;
					k++;
				}
			}

			if (!k)
			{
				cur = {(double) center.x, (double)center.y, a, b };
				accumulator[cur]++;
			}
			count++;
		}
		catch (knowingly_false_values e)
		{
			continue;
		}
	}

	// конец while

	k = 0;
	for (auto it = accumulator.begin(); it != accumulator.end(); ++it)
	{
		cout << "p:  " << it->first[0] << "  q:  " << it->first[1] << "  a:  " << it->first[2] << "  b:  " << it->first[3] << "   sc:  " << it->second <<endl;
		if (it->second > k)
		{
			k = it->second;
			cur = it->first;
		}
	}
	
	ellipse(img, Point(cur[0],cur[1]), Size(cur[2], cur[3]), 0, 0, 360, Scalar(100, 255, 0), 2);
	imshow("work", img);

	waitKey(0);
	system("pause");
}