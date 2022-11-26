#pragma once
#include "stdafx.h"
#include "console/console_control.h"
#include <io.h>

struct Face {
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp, fastsp;

    std::string spName, fastspName;

    cv::Rect dlibRectangleToOpenCV(dlib::rectangle r);
    
    void setSpPath(std::string spPath, std::string fastspPath);

    void loadSpModel();

    void cvFaceRect(cv::Mat &face);
};