#pragma once
#include <vector>
#include <string>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>


namespace asciify
{
	std::vector<int> generate_greyscale(int argc, char** argv, const cv::Mat& image_path);

	std::string generate_ascii(const std::vector<int>& intensities, int& img_w);

	std::vector<std::string> generate_video(int argc, char** argv);
	std::vector<std::string> load_ascii_video();
	void save_ascii_video(const std::vector<std::string>& ascii_frames);

	void play_video(std::vector<std::string>& frames);
}