#include "stdafx.h"
#include "webcam/face.h"

cv::Rect Face::dlibRectangleToOpenCV(dlib::rectangle r) {
    return cv::Rect(cv::Point2i((int)r.left(), (int)r.top()), cv::Point2i((int)r.right(), (int)r.bottom()));
}

void Face::setSpPath(std::string spPath, std::string fastspPath) {
    if (access(spPath.c_str(), 0) != -1) {
        setColor(COLOR::GREEN);
        std::cout << spPath << " ���� ã�ҽ��ϴ�." << std::endl;
        setColor();
    }
    else {
        setColor(COLOR::RED);
        std::cout << spPath << " ���� ã�� ���߽��ϴ�." << std::endl;
        setColor();
        ErrorExit();
    }

    if (access(fastspPath.c_str(), 0) != -1) {
        setColor(COLOR::GREEN);
        std::cout << fastspPath << " ���� ã�ҽ��ϴ�." << std::endl;
        setColor();
    }
    else {
        setColor(COLOR::RED);
        std::cout << fastspPath << " ���� ã�� ���߽��ϴ�." << std::endl;
        setColor();
        ErrorExit();
    }
}

void Face::loadSpModel() {
    dlib::deserialize(spName) >> sp;
    dlib::deserialize(fastspName) >> fastsp;
}


void Face::cvFaceRect(cv::Mat &img) {
    dlib::cv_image<dlib::bgr_pixel> cimg(img);
    auto face = detector(cimg);
    if (face.size() >= 2) {
        setColor(12);
        std::cout << "���� ������ �ֳ׿�." << std::endl;
        setColor();
        return;
    }
    else if (face.size() == 0)
        return;

    dlib::shape_predictor shape;

    cv::rectangle(img, dlibRectangleToOpenCV(dlib::get_face_chip_details(shape(cimg, face[0]), 1, 0.25).rect), cv::Scalar(255, 0, 0), 3, 4, 0);
}

