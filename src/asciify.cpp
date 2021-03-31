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
	args::parse_args(argc, argv);

	cv::Mat imgtemp = cv::imread(args::path);

	if (args::img_w == 0)
		args::img_w = (int)(imgtemp.cols * args::resize_percent);
	if (args::img_h == 0)
		args::img_h = (int)(imgtemp.rows * args::resize_percent);

	args::img_h /= 2;

	cv::Mat img;

	cv::resize(imgtemp, img, cv::Size(args::img_w, args::img_h));

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

	if (args::write_to_file)
	{
		f.open(args::filename, std::ofstream::out | std::ofstream::trunc);
	}

	for (int i = 0; i < intensities.size(); ++i)
	{
		if (intensities[i] >= 200) image += '@';
		else if (intensities[i] >= 150) image += '#';
		else if (intensities[i] >= 100) image += '=';
		else if (intensities[i] >= 65) image += ':';
		else if (intensities[i] >= 45) image += '.';
		else image += ' ';

		if (i % args::img_w == 0)
		{
			if (args::write_to_file)
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

	if (args::write_to_file)
	{
		std::cout << "wrote output to " << args::filename << "\n";

		if (args::open)
		{
			std::string command = args::filename;
			system(command.c_str());
		}
	}
	else
	{
		if (args::open)
		{
			std::cout << "can't open file: output was written to terminal\n";
			exit(1);
		}

		std::cout << image << "\n";
	}

	if ((args::img_w >= 120 || args::img_h >= 200) && !args::write_to_file)
	{
		args::print_colored("ascii output is very large, run asciify image --help for more information", 6);
	}
}