#include "../include/asciify.h"
#include "../include/args.h"
#include <string>
#include <fstream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


std::vector<int> asciify::generate_greyscale(int argc, char** argv)
{
	cv::Mat imgtemp = cv::imread(args::image::image_path);

	if (args::image::img_w == 0)
		args::image::img_w = (int)(imgtemp.cols * args::image::resize_percent);
	if (args::image::img_h == 0)
		args::image::img_h = (int)(imgtemp.rows * args::image::resize_percent);

	args::image::img_h /= 2;

	cv::Mat img;

	cv::resize(imgtemp, img, cv::Size(args::image::img_w, args::image::img_h));

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


void asciify::generate_ascii(const std::vector<int>& intensities)
{
	std::string image;

	std::fstream f;

	if (args::image::write_to_file)
	{
		f.open(args::image::output_path, std::ofstream::out | std::ofstream::trunc);
	}

	for (int i = 0; i < intensities.size(); ++i)
	{
		if (intensities[i] >= 200) image += '@';
		else if (intensities[i] >= 150) image += '#';
		else if (intensities[i] >= 100) image += '=';
		else if (intensities[i] >= 65) image += ':';
		else if (intensities[i] >= 45) image += '.';
		else image += ' ';

		if (i % args::image::img_w == 0)
		{
			if (args::image::write_to_file)
			{
				f << image << "\n";
				image.clear();
			}
			else
			{
				image += "\n";
			}
		}
	}

	f.close();

	if (args::image::write_to_file)
	{
		std::cout << "wrote output to " << args::image::output_path << "\n";

		if (args::image::open)
		{
			system(args::image::output_path.c_str());
		}
	}
	else
	{
		if (args::image::open)
		{
			std::cout << "can't open file: output was written to terminal\n";
			exit(1);
		}

		std::cout << image << "\n";
	}

	if ((args::image::img_w >= 120 || args::image::img_h >= 200) && !args::image::write_to_file)
	{
		args::print_colored("ascii output is very large, run asciify image --help for more information", 6);
	}
}


void asciify::generate_video()
{
	std::vector<std::vector<int>> frames;
}