#include "../include/asciify.h"
#include "../include/args.h"
#include "../include/utils.h"
#include <string>
#include <fstream>
#include <sstream>
#include <opencv2/highgui.hpp>


std::vector<int> asciify::generate_greyscale(int argc, char** argv, const cv::Mat& imgtemp)
{
	cv::Mat img;

	if (args::image::active)
	{
		if (args::image::img_w == 0)
			args::image::img_w = (int)(imgtemp.cols * args::image::resize_percent);
		if (args::image::img_h == 0)
			args::image::img_h = (int)(imgtemp.rows * args::image::resize_percent);

		args::image::img_h /= 2;

		cv::resize(imgtemp, img, cv::Size(args::image::img_w, args::image::img_h));
	}

	std::vector<int> intensities;

	for (int i = 0; i < img.rows; ++i)
	{
		for (int j = 0; j < img.cols; ++j)
		{
			cv::Vec3b intensity = img.at<cv::Vec3b>(i, j);
			std::array<int, 3> rgb;

			for (int k = 0; k < img.channels(); ++k)
			{
				rgb[k] = intensity.val[k];
			}

			int in = (int)(0.2126f * (float)rgb[2] + 0.7152f * (float)rgb[1] + 0.0722f * (float)rgb[0]);

			intensities.emplace_back(in);
		}
	}

	return intensities;
}


std::string asciify::generate_ascii(const std::vector<int>& intensities, int& img_w)
{
	std::string image;

	for (int i = 0; i < intensities.size(); ++i)
	{
		if (intensities[i] >= 200) image += '@';
		else if (intensities[i] >= 150) image += '#';
		else if (intensities[i] >= 100) image += '=';
		else if (intensities[i] >= 65) image += ':';
		else if (intensities[i] >= 45) image += '.';
		else image += ' ';

		if (i % img_w == 0)
		{
			image += "\n";
		}
	}

	return image;
}