#pragma once
#include "stdafx.h"
#include "console/console_control.h"
#include <io.h>

struct Face {
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp, fastsp;

    cv::Rect dlibRectangleToOpenCV(dlib::rectangle r);

    void loadSpModel(std::string spPath, std::string fastspPath);

    void cvFaceRect(cv::Mat &face);
};