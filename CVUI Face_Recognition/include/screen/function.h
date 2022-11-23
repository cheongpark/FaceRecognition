#pragma once
#include "screen/cvui.h"
#include <Windows.h>

struct RGBScale
{
	int r = 0;
	int g = 0;
	int b = 0;
	int rgb = RGB(r, g, b);

	RGBScale(int r, int g, int b);
};

void putImage(cv::Mat Image, cv::Mat& outImage, cv::Rect pos);

void _putText(cv::Mat& img, const cv::String& text, const cv::Point& org, int ori, const char* fontname, int fontWeight, double fontScale, bool anti, RGBScale textcolor, RGBScale bkcolor);