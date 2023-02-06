#pragma once
#include "stdafx.h"

//모델이 만약 없으면 설정을 하고 넘어갈지 아니면 그냥 넘어갈지 정하는거
#define CheckModel false

//개발 상황에 따라 필요없는건 꺼야 할 수도 있는 것들
#define CheckCam false //카메라의 유무를 확인 등 카메라 관련 세팅을 보거나 설정할지
#define UseModelLoad true //모델을 가져올지 안가져올지
#define TransCelebVector true //이미지를 벡터로 바꿀지 안바꿀지 "이거 할 땐 Debug로 안됨 Release로 해야함"

#define LoadImageView true //적용될 이미지를 미리 확인해주게 할지 말지 정하는거
#define LoadImageViewDelay 100 //이미지를 보여줄 때 너무 빨리 넘어가면 안되니깐 속도를 약간 늦춰주는거
#define MakeExportImage true //나중에 이미지를 따로 저장해서 쓸 수 있게 할지 말지

//-----모델을 사용하기 위한 필수 명령어 들-----
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

std::string CelebrityImagePath = "./CelebrityImg"; //연예인 이미지가 있는 폴더

std::string SpPath = "./Dlib_Model/shape_predictor_68_face_landmarks_GTX.dat"; //68개의 점을 찍기 때문에 느림 정확도 높음
std::string FastSpPath = "./Dlib_Model/shape_predictor_5_face_landmarks.dat"; //5개의 점만 찍기 때문에 빠름 정확도 낮음
std::string ResNetPath = "./Dlib_Model/dlib_face_recognition_resnet_model_v1.dat"; //얼굴 이미지를 128-D(128개) 벡터 값으로 바꿔주는 중요한 모델

//텍스트 컬러 색과 번호
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

//모델의 유무 판단
bool modelisAlive(std::string& path);
//모델을 파일 탐색기로 다시 찾게 하는걸 할지 묻고 찾는거
void useSearchModel(std::string& path, bool& alive_model);
//모델 적용해주는거
void applyModel(std::string& path, dlib::shape_predictor& model);
void applyModel(std::string& path, anet_type& model);

//이미지의 경로들을 가져와주는거
std::vector<std::filesystem::path> loadImagePath(std::string& folder);
//이미지 경로의 이름을 가져오는 것
std::string getFileName(std::filesystem::path& path);

//텍스트 컬러 지정 (텍스트 컬러, 배경 컬러)
void setColor(short textcolor = COLOR::DARK_WHITE, short background = COLOR::BLACK);