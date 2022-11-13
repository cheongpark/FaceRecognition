#include "cam.h"

void Play(int WEBCAM, dlib::frontal_face_detector face_detector) {
    cv::VideoCapture cap(WEBCAM); //몇번째 카메라 쓸찌 WEBCAM 번호에 따라 카메라가 달라짐
    if (!cap.isOpened()) { //안열려 있으면
        setColor(12); //빨간색
        std::cout << "카메라에 연결이 안됩니다. 연결 후 다시 시도해주세요." << std::endl;
        return;
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
}
