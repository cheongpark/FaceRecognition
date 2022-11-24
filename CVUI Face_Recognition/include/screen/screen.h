#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <Windows.h>

#include "screen/cvui.h"

#define CVUI_IMPLEMENTATION
#define WINDOW_NAME	"Face Recognition (연예인 닮음 찾기)"

struct RGBScale {
	int r = 0;
	int g = 0;
	int b = 0;
	int rgb = 0;

	RGBScale(int r, int g, int b);
};

void screenInit(int width, int height);

void frameRender(cv::Mat& frame, const int& LRMargin, const int& Margin, bool& useLand);

void putImage(const cv::Mat Image, cv::Mat& outImage, const cv::Rect pos);

void _putText(cv::Mat& img, const cv::String& text, const cv::Point& org, const int ori, const char* fontname, const int fontWeight, const double fontScale, const bool anti, const RGBScale textcolor, const RGBScale bkcolor);