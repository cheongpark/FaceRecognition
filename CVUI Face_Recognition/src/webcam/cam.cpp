#include "stdafx.h"
#include "webcam/cam.h"

void camSetting(cv::VideoCapture& cap, int width, int height, int camNum) {
    cap.open(camNum); //OpenCV�� ��ķ�� ��ȣ�� �����ϴ� ��

    if (!cap.isOpened()) { //��ķ ã�� ���ϸ�
        setColor(COLOR::RED);
        std::cout << "Error: ��ķ�� ã�� ���߽��ϴ�. \a" << std::endl;
        setColor();
    }
    else {
        setColor(COLOR::GREEN);
        std::cout << "Success: ��ķ�� ã�ҽ��ϴ�." << std::endl;
        setColor();
    }

    //�̰� �� �ȵ�? �ǰ� ������ �̹����� ��
    //�̰� ���ϸ� 640 * 480���� �ۿ� �ȵ�
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width); //��ķ�� ���� ����� ������
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height); //��ķ�� ���� ����� ������
}

void capFaceImage(cv::Mat& frame, cv::VideoCapture& cap) {
    //���� �޼����� screen�� puttext�� ȭ�� ���� ���� ������.
    struct Face face; //�Ʒ� cvFaceRect�� ���� ������..

    if (!cap.isOpened()) {
        setColor(COLOR::RED);
        std::cout << "Error: ��ķ�� ã�� ���߽��ϴ�." << std::endl;
        setColor();
    }

    if (!cap.read(frame)) {
        setColor(COLOR::RED);
        std::cout << "Error: ��ķ���� �̹����� ���� �� �����ϴ�." << std::endl;
        setColor();
    }

    face.cvFaceRect(frame);
}

/*
cv::Mat Play(int WEBCAM, dlib::frontal_face_detector face_detector) {
    cv::VideoCapture cap(WEBCAM); //���° ī�޶� ���� WEBCAM ��ȣ�� ���� ī�޶� �޶���
    if (!cap.isOpened()) { //�ȿ��� ������
        setColor(12); //������
        std::cout << "ī�޶� ������ �ȵ˴ϴ�. ���� �� �ٽ� �õ����ּ���." << std::endl;
        exit(0);
    }
    else { //����������
        setColor(10); //�ʷϻ�
        std::cout << WEBCAM << "��° ī�޶� ���������� ���� �Ǿ����ϴ�." << std::endl;
    }

    dlib::image_window win;

    while (!win.is_closed()) { //�̹��� ���� â�� ��ĥ �� ����
        cv::Mat temp; //��ķ���� matrix<rgb_pixel> ���� ��ó�� matrix�� ��ȯ���ִ� ����
        if (!cap.read(temp)) {
            setColor(12);
            std::cout << "��ķ���� �̹����� ���� �� �����ϴ�." << std::endl;
            break; //��Ʈ������ ���� �� ������ �ǳʶٱ�
        }

        if (_kbhit()) {
            if (_getch() == 32) {
                std::cout << "check" << std::endl;
                return temp;
            }
        }

        dlib::cv_image<dlib::bgr_pixel> cimg(temp); //�̹����� ��Ʈ���� ���·� ����
        std::vector<dlib::rectangle> faceRect = face_detector(cimg);

        win.set_image(cimg);

        if (faceRect.size() >= 2) { //cimg�� ���� 2�� �̻� ������ ������ �ִٸ鼭 �˷��ֱ�
            setColor(12);
            win.clear_overlay();
            std::cout << "���� ������ �ֳ׿�." << std::endl;
            continue;
        }
        win.clear_overlay();
        win.add_overlay(faceRect);
    }

    //���� â�� ������ �ݾ��� ��
    std::cout << "���� â�� �������ϴ�." << std::endl;
    exit(0);
}
*/