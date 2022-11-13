#include "cam.h"

void Play(int WEBCAM, dlib::frontal_face_detector face_detector) {
    cv::VideoCapture cap(WEBCAM); //���° ī�޶� ���� WEBCAM ��ȣ�� ���� ī�޶� �޶���
    if (!cap.isOpened()) { //�ȿ��� ������
        setColor(12); //������
        std::cout << "ī�޶� ������ �ȵ˴ϴ�. ���� �� �ٽ� �õ����ּ���." << std::endl;
        return;
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
}
