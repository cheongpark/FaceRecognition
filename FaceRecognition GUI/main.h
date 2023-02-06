#pragma once
#include "stdafx.h"

//���� ���� ������ ������ �ϰ� �Ѿ�� �ƴϸ� �׳� �Ѿ�� ���ϴ°�
#define CheckModel false

//���� ��Ȳ�� ���� �ʿ���°� ���� �� ���� �ִ� �͵�
#define CheckCam false //ī�޶��� ������ Ȯ�� �� ī�޶� ���� ������ ���ų� ��������
#define UseModelLoad true //���� �������� �Ȱ�������
#define TransCelebVector true //�̹����� ���ͷ� �ٲ��� �ȹٲ��� "�̰� �� �� Debug�� �ȵ� Release�� �ؾ���"

#define LoadImageView true //����� �̹����� �̸� Ȯ�����ְ� ���� ���� ���ϴ°�
#define LoadImageViewDelay 100 //�̹����� ������ �� �ʹ� ���� �Ѿ�� �ȵǴϱ� �ӵ��� �ణ �����ִ°�
#define MakeExportImage true //���߿� �̹����� ���� �����ؼ� �� �� �ְ� ���� ����

//-----���� ����ϱ� ���� �ʼ� ��ɾ� ��-----
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
//-----		-----

std::string CelebrityImagePath = "./CelebrityImg"; //������ �̹����� �ִ� ����

std::string SpPath = "./Dlib_Model/shape_predictor_68_face_landmarks_GTX.dat"; //68���� ���� ��� ������ ���� ��Ȯ�� ����
std::string FastSpPath = "./Dlib_Model/shape_predictor_5_face_landmarks.dat"; //5���� ���� ��� ������ ���� ��Ȯ�� ����
std::string ResNetPath = "./Dlib_Model/dlib_face_recognition_resnet_model_v1.dat"; //�� �̹����� 128-D(128��) ���� ������ �ٲ��ִ� �߿��� ��

//�ؽ�Ʈ �÷� ���� ��ȣ
enum COLOR {
    BLACK = 0,
    DARK_BLUE = 1,
    DARK_GREEN = 2,
    DARK_AQUA = 3,
    DARK_RED = 4,
    DARK_PURPLE = 5,
    DARK_YELLOW = 6,
    DARK_WHITE = 7,
    GRAY = 8,
    BLUE = 9,
    GREEN = 10,
    AQUA = 11,
    RED = 12,
    PURPLE = 13,
    YELLOW = 14,
    WHITE = 15
};

//���� ���� �Ǵ�
bool modelisAlive(std::string& path);
//���� ���� Ž����� �ٽ� ã�� �ϴ°� ���� ���� ã�°�
void useSearchModel(std::string& path, bool& alive_model);
//�� �������ִ°�
void applyModel(std::string& path, dlib::shape_predictor& model);
void applyModel(std::string& path, anet_type& model);

//�̹����� ��ε��� �������ִ°�
std::vector<std::filesystem::path> loadImagePath(std::string& folder);
//�̹��� ����� �̸��� �������� ��
std::string getFileName(std::filesystem::path& path);

//�ؽ�Ʈ �÷� ���� (�ؽ�Ʈ �÷�, ��� �÷�)
void setColor(short textcolor = COLOR::DARK_WHITE, short background = COLOR::BLACK);