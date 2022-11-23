#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <Windows.h>

#include "screen/cvui.h"
#include "screen/function.h"

#define CVUI_IMPLEMENTATION
#define WINDOW_NAME	"Face Recognition (연예인 닮음 찾기)"

class Screen {
public:
	Screen(int width, int height);
	//void getScreen();

private:
	int ScreenW;
	int ScreenH;
};