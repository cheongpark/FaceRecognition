#include "stdafx.h"
#include "webcam/cam.h"

void camSetting(cv::VideoCapture& cap, int width, int height, int camNum) {
    cap.open(camNum); //OpenCV의 웹캠의 번호를 세팅하는 것

    if (!cap.isOpened()) { //웹캠 찾지 못하면
        setColor(COLOR::RED);
        std::cout << "Error: 웹캠을 찾지 못했습니다. \a" << std::endl;
        setColor();
    }
    else {
        setColor(COLOR::GREEN);
        std::cout << "Success: 웹캠을 찾았습니다." << std::endl;
        setColor();
    }

    //이거 왜 안됨? 되게 만들어야 이미지가 들어감
    //이거 안하면 640 * 480으로 밖에 안됨
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width); //웹캠의 가로 사이즈를 설정함
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height); //웹캠의 세로 사이즈를 설정함
}

void capFaceImage(cv::Mat& frame, cv::VideoCapture& cap) {
    //실패 메세지는 screen의 puttext로 화면 내에 넣을 생각임.
    struct Face face; //아래 cvFaceRect를 쓰기 위함임..

    if (!cap.isOpened()) {
        setColor(COLOR::RED);
        std::cout << "Error: 웹캠을 찾지 못했습니다." << std::endl;
        setColor();
    }

    if (!cap.read(frame)) {
        setColor(COLOR::RED);
        std::cout << "Error: 웹캠에서 이미지를 읽을 수 없습니다." << std::endl;
        setColor();
    }

    face.cvFaceRect(frame);
}

/*
cv::Mat Play(int WEBCAM, dlib::frontal_face_detector face_detector) {
    cv::VideoCapture cap(WEBCAM); //몇번째 카메라 쓸찌 WEBCAM 번호에 따라 카메라가 달라짐
    if (!cap.isOpened()) { //안열려 있으면
        setColor(12); //빨간색
        std::cout << "카메라에 연결이 안됩니다. 연결 후 다시 시도해주세요." << std::endl;
        exit(0);
    }
    else { //열려있으면
        setColor(10); //초록색
        std::cout << WEBCAM << "번째 카메라에 성공적으로 연결 되었습니다." << std::endl;
    }

    dlib::image_window win;

    while (!win.is_closed()) { //이미지 띄우는 창이 닫칠 때 까지
        cv::Mat temp; //웹캠에서 matrix<rgb_pixel> 위의 것처럼 matrix로 변환해주는 거임
        if (!cap.read(temp)) {
            setColor(12);
            std::cout << "웹캠에서 이미지를 읽을 수 없습니다." << std::endl;
            break; //매트릭스로 읽을 수 없으면 건너뛰기
        }

        if (_kbhit()) {
            if (_getch() == 32) {
                std::cout << "check" << std::endl;
                return temp;
            }
        }

        dlib::cv_image<dlib::bgr_pixel> cimg(temp); //이미지를 매트릭스 형태로 저장
        std::vector<dlib::rectangle> faceRect = face_detector(cimg);

        win.set_image(cimg);

        if (faceRect.size() >= 2) { //cimg에 얼굴이 2개 이상 있으면 여러개 있다면서 알려주기
            setColor(12);
            win.clear_overlay();
            std::cout << "얼굴이 여러개 있네요." << std::endl;
            continue;
        }
        win.clear_overlay();
        win.add_overlay(faceRect);
    }

    //영상 창을 강제로 닫았을 때
    std::cout << "영상 창이 닫혔습니다." << std::endl;
    exit(0);
}
*/