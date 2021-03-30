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

	for (int i = 0; i < intensities.size(); ++i)
	{
		if (intensities[i] >= 210) image += '@';
		else if (intensities[i] >= 170) image += '#';
		else if (intensities[i] >= 130) image += '=';
		else if (intensities[i] >= 100) image += ':';
		else if (intensities[i] >= 70) image += '.';
		else image += ' ';

		if (i % xmax == 0) image += "\n";
	}

	if (write_to_file)
	{
		std::fstream f;
		f.open(filename, std::ofstream::out | std::ofstream::trunc);
		f << image;
		f.close();

		std::cout << "Wrote output to " << filename << "\n";
	}
	else
	{
		std::cout << image << "\n";
	}
}


int main(int argc, char** argv)
{
	std::string path = argv[1];

	bool write_to_file = false;
	std::string filename;

	int img_w = 44;
	int img_h = 44;

	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i], "-f") == 0)
		{
			write_to_file = true;
			filename = argv[++i];
		}

		if (strcmp(argv[i], "-s") == 0)
		{
			std::stringstream(argv[++i]) >> img_w;
			img_h = img_w;
		}

		if (strcmp(argv[i], "-w") == 0)
			std::stringstream(argv[++i]) >> img_w;

		if (strcmp(argv[i], "-h") == 0)
			std::stringstream(argv[++i]) >> img_h;
	}

	cv::Mat imgtemp = cv::imread(path);
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

	return 0;
}