#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


void generate_ascii(const std::vector<int>& intensities, int xmax, int ymax, bool write_to_file, const std::string& filename)
{
	std::string image;

	std::fstream f;
	
	if (write_to_file)
	{
		f.open(filename, std::ofstream::out | std::ofstream::trunc);
	}

	for (int i = 0; i < intensities.size(); ++i)
	{
		if (intensities[i] >= 200) image += '@';
		else if (intensities[i] >= 150) image += '#';
		else if (intensities[i] >= 100) image += '=';
		else if (intensities[i] >= 65) image += ':';
		else if (intensities[i] >= 45) image += '.';
		else image += ' ';

		if (i % xmax == 0)
		{
			if (write_to_file)
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

	if (write_to_file)
	{
		std::cout << "Wrote output to " << filename << "\n";
	}
	else
	{
		std::cout << image << "\n";
	}
}


std::string next_arg(int argc, char** argv, int i)
{
	if (++i >= argc)
	{
		std::cout << argv[i - 1] << ": missing required argument\n";
		exit(1);
	}

	return argv[i];
}


void help()
{
	std::cout << "Turn images into ascii\n";
	std::cout << "-f <filename>: write ascii output to a file\n";
	std::cout << "-s <size>: set image size to <size>x<size>\n";
	std::cout << "-w  <width>: set image width to <width>\n";
	std::cout << "-h <height>: set image height to <height>\n";
	std::cout << "-rs <resize percent>: resize image to percentage of its original size. -rs 75 resizes the image to 75%\n";
}


int main(int argc, char** argv)
{
	if (argc == 1 || strcmp(argv[1], "--help") == 0)
	{
		help();
		exit(0);
	}

	std::string path = argv[1];
	

	std::ifstream file(path);
	if (!file)
	{
		std::cout << "file doesn't exist: " << path << "\n";
		exit(1);
	}

	bool write_to_file = false;
	std::string filename;

	cv::Mat imgtemp = cv::imread(path);

	int img_w = imgtemp.cols;
	int img_h = imgtemp.rows;

	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i], "-f") == 0)
		{
			write_to_file = true;
			filename = next_arg(argc, argv, i);
		}

		if (strcmp(argv[i], "-s") == 0)
		{
			std::stringstream(next_arg(argc, argv, i)) >> img_w;
			img_h = img_w;
		}

		if (strcmp(argv[i], "-w") == 0)
			std::stringstream(next_arg(argc, argv, i)) >> img_w;

		if (strcmp(argv[i], "-h") == 0)
			std::stringstream(next_arg(argc, argv, i)) >> img_h;

		if (strcmp(argv[i], "-rs") == 0)
		{
			float resize_percent;
			std::stringstream(next_arg(argc, argv, i)) >> resize_percent;

			if (resize_percent == 0)
			{
				std::cout << "can't resize to 0%\n";
				exit(0);
			}

			resize_percent /= 100.f;

			img_w *= resize_percent;
			img_h *= resize_percent;
		}
	}

	img_h /= 2;

	cv::Mat img;

	cv::resize(imgtemp, img, cv::Size(img_w, img_h));	

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

	generate_ascii(intensities, img.cols, img.rows, write_to_file, filename);

	if ((img.cols >= 200 || img.rows >= 200) && !write_to_file)
	{
		std::cout << "ascii output is very large, use -f to write the output to a file. run asciify --help for more information\n";
	}

	return 0;
}