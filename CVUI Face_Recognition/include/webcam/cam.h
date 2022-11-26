#pragma once
#include "stdafx.h"
#include "console/console_control.h"
#include "webcam/face.h"
#include <conio.h>

void camSetting(cv::VideoCapture& cap, int width, int height, int camNum = 0);

void capFaceImage(cv::Mat& frame, cv::VideoCapture& cap);

//cv::Mat Play(int WEBCAM, dlib::frontal_face_detector face_detector);