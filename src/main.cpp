#include "../include/args.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


void generate_ascii(const std::vector<int>& intensities)
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
			std::string command = ".\\" + args::filename;
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
}


int main(int argc, char** argv)
{
	parse_args(argc, argv);

	cv::Mat imgtemp = cv::imread(args::path);

	args::img_w = (int)(imgtemp.cols * args::resize_percent);
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

	generate_ascii(intensities);

	if ((img.cols >= 200 || img.rows >= 200) && !args::write_to_file)
	{
		std::cout << "ascii output is very large, use -f to write the output to a file. run asciify --help for more information\n";
	}

	return 0;
}