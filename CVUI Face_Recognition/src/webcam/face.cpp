#include "stdafx.h"
#include "webcam/face.h"

cv::Rect Face::dlibRectangleToOpenCV(dlib::rectangle r) { //�ӵ��� ������ �� ���Ƽ� ��� X
    return cv::Rect(cv::Point2i((int)r.left(), (int)r.top()), cv::Point2i((int)r.right(), (int)r.bottom()));
}

void Face::loadSpModel(std::string spPath, std::string fastspPath) {
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

    dlib::deserialize(spPath) >> sp;
    dlib::deserialize(fastspPath) >> fastsp;
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

    //std::cout << face.size() << std::endl;

    auto faceRect = dlib::get_face_chip_details(fastsp(cimg, face[0]), 1, 0.25).rect; //�̰� ������ ������ �� ����

    cv::rectangle(img, cv::Rect(faceRect.left(), faceRect.top(), faceRect.right(),faceRect.bottom()), cv::Scalar(255, 0, 0), 3, 4, 0);
}