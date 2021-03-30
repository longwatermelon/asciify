#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


void generate_ascii(const std::vector<int>& intensities, int xmax, int ymax)
{
	std::string image;

	for (int i = 0; i < intensities.size(); ++i)
	{
		if (intensities[i] >= 200) image += '@';
		else if (intensities[i] >= 150) image += '#';
		else if (intensities[i] >= 100) image += '=';
		else if (intensities[i] >= 50) image += ':';
		else image += ' ';

		if (i % xmax == 0) image += "\n";
	}

	std::fstream f;
	f.open("output.txt", std::ofstream::out | std::ofstream::trunc);
	f << image;
	f.close();

	std::cout << image << "\n";

	std::cout << "Wrote output to images/output.txt\n";
}


int main(int argc, char** argv)
{
	std::string path = argv[1];
	cv::Mat imgtemp = cv::imread(path);
	cv::Mat img;

	cv::resize(imgtemp, img, cv::Size(50, 50));
	

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

	generate_ascii(intensities, img.cols, img.rows);

	return 0;
}