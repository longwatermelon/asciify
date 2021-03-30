#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


namespace args
{
	bool open = false;

	std::string path;

	bool write_to_file = false;
	std::string filename;

	int img_w = 0;
	int img_h = 0;
}


std::string next_arg(int argc, char** argv, int& i)
{
	if (++i >= argc)
	{
		std::cout << argv[i - 1] << ": missing required argument\n";
		exit(1);
	}

	return argv[i++];
}


void help()
{
	std::cout << "Turn images into ascii\n";
	std::cout << "-f <filename>: write ascii output to a file\n";
	std::cout << "-s <size>: set image size to <size>x<size>\n";
	std::cout << "-w  <width>: set image width to <width>\n";
	std::cout << "-h <height>: set image height to <height>\n";
	std::cout << "-rs <resize percent>: resize image to percentage of its original size. -rs 75 resizes the image to 75%\n";
	std::cout << "--open: opens file automatically after writing output\n";
}


void parse_args(int argc, char** argv)
{
	if (argc == 1)
	{
		help();
		exit(0);
	}

	args::path = argv[1];

	if (args::path == "--help")
	{
		help();
		exit(0);
	}

	std::ifstream temp(args::path);

	if (!temp)
	{
		std::cout << "invalid file: " << args::path << " does not exist\n";
		exit(1);
	}

	int i = 2;
	while (i < argc)
	{
		if (strcmp(argv[i], "-f") == 0)
		{
			args::write_to_file = true;
			args::filename = next_arg(argc, argv, i);
		}

		else if (strcmp(argv[i], "-s") == 0)
		{
			std::stringstream(next_arg(argc, argv, i)) >> args::img_w;
			args::img_h = args::img_w;
		}

		else if (strcmp(argv[i], "-w") == 0)
			std::stringstream(next_arg(argc, argv, i)) >> args::img_w;

		else if (strcmp(argv[i], "-h") == 0)
			std::stringstream(next_arg(argc, argv, i)) >> args::img_h;

		else if (strcmp(argv[i], "-rs") == 0)
		{
			float resize_percent;
			std::stringstream(next_arg(argc, argv, i)) >> resize_percent;

			if (resize_percent <= 0)
			{
				std::cout << "can't resize to 0% or less\n";
				exit(0);
			}

			resize_percent /= 100.f;

			args::img_w *= resize_percent;
			args::img_h *= resize_percent;
		}

		else if (strcmp(argv[i], "--open") == 0)
			args::open = true;

		else
		{
			std::cout << "unrecognized argument: " << argv[i] << "\n";
			exit(1);
		}
	}
}


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
		std::cout << "Wrote output to " << args::filename << "\n";

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

	if (args::img_w == 0) args::img_w = imgtemp.cols;
	if (args::img_h == 0) args::img_h = imgtemp.rows;

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