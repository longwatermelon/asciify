#pragma once
#include <vector>
#include <string>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>


namespace asciify
{
	std::vector<int> generate_greyscale(int argc, char** argv, const cv::Mat& image_path);

	std::string generate_ascii(const std::vector<int>& intensities, int& img_w);
}