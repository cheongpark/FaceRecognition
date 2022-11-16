#include "stdafx.h"
#include <iostream>

#include "console_control.h"
#include "load_file.h"

#include "cam.h"

#define resnetmodelPath "dlib_face_recognition_resnet_model_v1.dat"
#define TestlandmarkPath "shape_predictor_5_face_landmarks.dat"

#define loadImageView true
#define loadImageViewDelay 300

template <template <int, template<typename>class, int, typename> class block, int n, template<typename>class bn, typename subnet>
using residual = dlib::add_prev1<block<n, bn, 1, dlib::tag1<subnet>>>;

template <template <int, template<typename>class, int, typename> class block, int n, template<typename>class bn, typename subnet>
using residual_down = dlib::add_prev2<dlib::avg_pool<2, 2, 2, 2, dlib::skip1<dlib::tag2<block<n, bn, 2, dlib::tag1<subnet>>>>>>;

template <int n, template <typename> class bn, int stride, typename subnet>
using block = bn<dlib::con<n, 3, 3, 1, 1, dlib::relu<bn<dlib::con<n, 3, 3, stride, stride, subnet>>>>>;

template <int n, typename subnet> using ares = dlib::relu<residual<block, n, dlib::affine, subnet>>;
template <int n, typename subnet> using ares_down = dlib::relu<residual_down<block, n, dlib::affine, subnet>>;

template <typename subnet> using alevel0 = ares_down<256, subnet>;
template <typename subnet> using alevel1 = ares<256, ares<256, ares_down<256, subnet>>>;
template <typename subnet> using alevel2 = ares<128, ares<128, ares_down<128, subnet>>>;
template <typename subnet> using alevel3 = ares<64, ares<64, ares<64, ares_down<64, subnet>>>>;
template <typename subnet> using alevel4 = ares<32, ares<32, ares<32, subnet>>>;

using anet_type = dlib::loss_metric< dlib::fc_no_bias<128, dlib::avg_pool_everything<
    alevel0<
    alevel1<
    alevel2<
    alevel3<
    alevel4<
    dlib::max_pool<3, 3, 2, 2, dlib::relu<dlib::affine<dlib::con<32, 7, 7, 2, 2,
    dlib::input_rgb_image_sized<150>
    >>>>>>>>>>>>;

int main() {
    setColor(COLOR::DARK_WHITE);

    dlib::frontal_face_detector face_detector = dlib::get_frontal_face_detector();

    //���� ����� �Ȱ��������� ����â�� �߰� ��
    dlib::shape_predictor sp;
    anet_type net;
    try {
        dlib::deserialize(TestlandmarkPath) >> sp;
        dlib::deserialize(resnetmodelPath) >> net;
    }
    catch (std::exception& e) {
        setColor(COLOR::RED);
        std::cout << e.what() << " ���� �ҷ����ų� ã�� ���߽��ϴ�." << std::endl;
        setColor(COLOR::DARK_WHITE);
        return -1;
    }
    std::cout << TestlandmarkPath << "�� ���� ã�ҽ��ϴ�." << std::endl;
    std::cout << resnetmodelPath << "�� ���� ã�ҽ��ϴ�." << std::endl;


    Play(0, face_detector);
}
    

    
    /*
    
    
    //������ �̸��� ���� ���� jpg, jpeg, png ���ϸ� ã�Ƽ� ��θ� �����ͼ� celeb_path�� ����
    std::vector<std::filesystem::path> celeb_path = load_image_path("celebrity");
    
    std::vector<dlib::matrix<dlib::rgb_pixel>> celeb_faces;
    std::vector<std::string> celeb_names;

    for (int celeb = 0; celeb < celeb_path.size(); celeb++) {
        dlib::matrix<dlib::rgb_pixel> img;

        
        load_image(img, celeb_path[celeb].string());

        try {
            if (face_detector(img).size() != 1)
                throw face_detector(img).size();
         
            setColor(COLOR::GREEN);
            std::cout << celeb_path[celeb] << " ������ �����Խ��ϴ�.";
            setColor(COLOR::WHITE);
            std::cout << " ���� �� �������� \"" << get_filename(celeb_path[celeb]) << "\" �Դϴ�." << std::endl;
            
            celeb_names.push_back(get_filename(celeb_path[celeb]));
        }
        catch (size_t num) {
            setColor(COLOR::RED);
            std::cout << celeb_path[celeb] << " ����� �������� " << face_detector(img).size() << "���� ���� �߰� �Ǿ����ϴ�. 1�� �ֵ��� �����ּ���." << std::endl;
            setColor(COLOR::DARK_WHITE);
        }

        for (auto celeb_face : face_detector(img)) {
            auto shape = sp(img, celeb_face);
            dlib::matrix<dlib::rgb_pixel> face_chip;
            extract_image_chip(img, get_face_chip_details(shape, 150, 0.25), face_chip);
            celeb_faces.push_back(std::move(face_chip));

            if (loadImageView) {
                dlib::image_window win(img);
                win.add_overlay(celeb_face);
                Sleep(loadImageViewDelay);
            }
        }
    }

    //setColor(9);
    std::cout << std::endl << "128���� ���� ������ ��ȯ ���Դϴ�." << std::endl;

    std::vector<dlib::matrix<float, 0, 1>> face_descriptors = net(celeb_faces);

    //setColor(10);
    std::cout << "�Ϸ�" << std::endl << std::endl;
    */


/*
#define resnetmodelPath "dlib_face_recognition_resnet_model_v1.dat"
#define TestlandmarkPath "shape_predictor_5_face_landmarks.dat"

#define celebrityPath "./celebrity/"

#define loadImageView false
#define loadImageViewDelay 1000

#define ImagePath "./TestImage/2.jpg"
#define ImageRange 0.6
#define Only1st true
#define FaceOnlyOne true

#define useTest true

#define WEBCAM 1

using namespace dlib;
using namespace std;


int main(int argc, char** argv) {
    setColor(7);

    frontal_face_detector face_detector = get_frontal_face_detector();

    shape_predictor sp;
    deserialize(TestlandmarkPath) >> sp;
    cout << TestlandmarkPath << "�� ���� ã�ҽ��ϴ�." << endl;

    anet_type net;
    deserialize(resnetmodelPath) >> net;
    cout << resnetmodelPath << "�� ���� ã�ҽ��ϴ�." << endl;

    std::vector<matrix<rgb_pixel>> faces;

    for (int celebImage = 0; celebImage < celebrity.size(); celebImage++) {
        matrix<rgb_pixel> img;

        char celebPath[50];
        sprintf_s(celebPath, "%s%d.jpg", celebrityPath, celebImage);

        load_image(img, celebPath);
        setColor(10);
        cout << celebPath << " ������ �����Խ��ϴ�. ���� �� �������� \"" << celebrity[celebImage] << "\" �Դϴ�." << endl;

        if (face_detector(img).size() != 1) {
            setColor(12);
            cout << celebPath << " ����� " << celebrity[celebImage] << "�������� " << face_detector(img).size() << "���� ���� �߰� �Ǿ����ϴ�. ���� 1���� �ֵ��� �����ּ���." << endl;
            return 1;
        }

        for (auto face : face_detector(img)) {
            auto shape = sp(img, face);
            matrix<rgb_pixel> face_chip; 
            extract_image_chip(img, get_face_chip_details(shape, 150, 0.25), face_chip); 
            faces.push_back(move(face_chip)); 

            if (loadImageView) {
                image_window win(img); 
                win.add_overlay(face); 
                Sleep(loadImageViewDelay);
            }
        }
    }

    setColor(9);
    cout << endl << "128���� ���� ������ ��ȯ ���Դϴ�." << endl;

    std::vector<matrix<float, 0, 1>> face_descriptors = net(faces); 

    setColor(10);
    cout << "�Ϸ�" << endl << endl;

    // ����----------------------------
    if (useTest) {
        std::vector<matrix<rgb_pixel>> Myfaces;

        matrix<rgb_pixel> Myimg;
        load_image(Myimg, ImagePath);

        setColor(10);
        cout << ImagePath << " �׽�Ʈ �̹����� �����Խ��ϴ�." << endl;

        for (auto Myface : face_detector(Myimg)) {
            auto shape = sp(Myimg, Myface); 
            matrix<rgb_pixel> Myface_chip;
            extract_image_chip(Myimg, get_face_chip_details(shape, 150, 0.25), Myface_chip);
            Myfaces.push_back(move(Myface_chip)); 
        }

        std::vector<matrix<float, 0, 1>> Myface_descriptors = net(Myfaces);

        for (int i = 0; i < Myfaces.size(); i++) {
            setColor(7);
            cout << i << "��° ��" << endl;
            for (int j = 0; j < faces.size(); j++) {
                float distance = length(Myface_descriptors[i] - face_descriptors[j]);
                if (distance <= ImageRange)
                    cout << distance << " " << celebrity[j] << endl;
            }
        }
    }

    cin.get();
}
*/