#ifndef SOLVER_H
#define SOLVER_H

#include "Utils.h"
#include "Image_Processing.h"
#include "Utils.h"
#include "Affine_Transforms.h"
#include "Coordinates_Corelation.h"
#include "Polar_Functions.h"
#include <iostream>

class Solver
{
	cv::Mat data;
	curve_type curve;
	Diapason p_range;
	Diapason e_range;
	double p_step;
	double e_step;
	double accuracy = 0.01;
	accum_map map;
//	std::vector<cv::Point2d> interesting_points;

	void find_discretisation()
	{
		double width = data.cols;
		double height = data.rows;
		double p_min, p_max;
		double e_min, e_max;

		switch (curve)
		{
		case ellipse_:
		{
			p_min = 1;
			p_max = (height*height) / (2 * width);
			p_step = p_min;
			e_min = 0.01;
			e_max = 0.99;
			e_step = 0.1;
			break;
		}
		case parabola_: 
		{
			p_min = 1;
			p_max = (width*width) / (2 * height);
			p_step = 1;
			e_min = 1;
			e_max = 1;
			e_step = 1;

		}
		case hyperbola_: 
		{
			p_min = 1;
			p_max = width / 2;
			p_range = Diapason(p_min, p_max);
			p_step = p_min;
			e_min = 1.01;
			e_max = sqrt(1 + width*width / height*height);
			e_step = 0.1;
			break;
		}
		}
		p_range = Diapason(p_min, p_max);
		e_range = Diapason(e_min, e_max);
	}


	void accumulation()
	{
		for (size_t i = 0; i < data.rows; i++)
			for (size_t j = 0; j < data.cols; j++)
			{
				int val = data.at<uchar>(i, j);
				if (val != 0)
				{
					for (double p = p_range.min; p <= p_range.max; p += p_step)
						for (double e = e_range.min; e <= e_range.max; e += e_step)
							for (double x = 10; x < data.rows - 10; x += 1)
								for (double y = 10; y < data.cols - 10; y += 1)
								{
									cv::Point2d cur_focus = cv::Point2d(x, y);
									cv::Point2d cur_point = Affine_Transforms::shift(cv::Point2d(i, j), cv::Point2d(0, 0), cur_focus);
									Coordinates_Corelation cur_corelation(cur_point);
									Params params = Params(p, e, cur_focus);
									double cur_r = Polar_Functions::polar_curve(cur_corelation.polar.phi, params);
									if (abs(cur_r - cur_corelation.polar.r) <= accuracy)
									{
										std::vector<std::pair<Params, Params>> similar_params_in_map;
										for (auto map_it = map.begin(); map_it != map.end(); ++map_it)
										{
											int score = (*map_it).second;

											Params map_params = (*map_it).first;
											double map_p = map_params.p;
											double map_e = map_params.e;
											cv::Point2d map_F = map_params.F;

											double p_test = std::abs(map_p - params.p);
											double e_test = std::abs(map_e - params.e);
											double F_test = std::sqrt((map_F.x - params.F.x)*(map_F.x - params.F.x) + (map_F.y - params.F.y)*(map_F.y - params.F.y));

											if (p_test < p_step / 2 && e_test < e_step / 2 && F_test < 1)
											{
												double new_p = (map_p*score + params.p) / (score + 1);
												double new_e = (map_e*score + params.e) / (score + 1);
												double new_x = (map_F.x*score + params.F.x) / (score + 1);
												double new_y = (map_F.y*score + params.F.y) / (score + 1);
												Params new_params = Params(new_p, new_e, cv::Point2d(new_x, new_y));

												similar_params_in_map.push_back(std::make_pair(map_params, new_params));
											}
										}
										if (similar_params_in_map.empty())
											map.insert(std::make_pair(params, 1));
										else
										{
											for (size_t t = 0; t < similar_params_in_map.size(); ++t)
											{
												Params old = similar_params_in_map[t].first;
												Params new_ = similar_params_in_map[t].second;
												auto cur_it = map.find(old);
												int score = (*cur_it).second;
												map.erase(cur_it);
												map.insert(std::make_pair(new_, score + 1));
											}
										}
									}
								}
				}
			}
	}

	std::pair<Params, int> find_max_score()
	{
		int hit = INT16_MIN;
		Params res;
		for (auto kv : map)
		{
			if (kv.second > hit)
			{
				hit = kv.second;
				res = kv.first;
			}
		}
		return std::make_pair(res, hit);
	}


public: 
	Solver(const std::string & filename, curve_type curve_)
	{
		cv::Mat input = cv::imread(filename, 1);
		curve = curve_;
		data = Image_Processing::process(input);
		//data = input;
		find_discretisation();
	}

	std::pair<Params, int> solve()
	{
		accumulation();
		return find_max_score();
		
	}


	void draw_answer()
	{
		auto answer = solve();
		std::vector<double> params_vector{ answer.first.p, answer.first.e };
		auto res_points = Polar_Functions::get_points_from_polar_func(Polar_Functions::polar_curve, params_vector,
					Diapason(0, 2* Pi), 0.01);
		cv::Mat res = Polar_Functions::draw(res_points);
		cv::imshow("input", data);
		cv::imshow("recognized", res);
		cv::waitKey(0);

		std::cout << "p = " << answer.first.p << std::endl
			<< "e = " << answer.first.e << std::endl
			<< "F = " << "(" << answer.first.F.x << ";" << answer.first.F.y << ")" << std::endl
			<< "score = " << answer.second << std::endl;
	}

};



#endif /* SOLVER_H */
