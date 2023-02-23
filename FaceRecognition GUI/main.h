#pragma once
#include "stdafx.h"

#define WinName "Celeb Face Recognition"

//모델이 만약 없으면 설정을 하고 넘어갈지 아니면 그냥 넘어갈지 정하는거
#define CheckModel true

//개발 상황에 따라 필요없는건 꺼야 할 수도 있는 것들
#define CheckCam true //카메라의 유무를 확인 등 카메라 관련 세팅을 보거나 설정할지
#define UseModelLoad true //모델을 가져올지 안가져올지
#define UsePreSetImage true //미리 GUI를 세팅하는 것
#define TransCelebVector true //이미지를 벡터로 바꿀지 안바꿀지 "이거 할 땐 Debug로 안됨 Release로 해야함"
#define UseOpenCVGUI true //GUI 관련, 제일 메인이 될 것

//#define ButtonTest true //버튼 테스트좀 해볼려는거

#define LoadImageView true //적용될 이미지를 미리 확인해주게 할지 말지 정하는거
#define LoadImageViewDelay 100 //이미지를 보여줄 때 너무 빨리 넘어가면 안되니깐 속도를 약간 늦춰주는거
#define ShowMainView LoadImageView //메인 웹캠 뷰 쪽에 원본 이미지를 보여줄지
#define MakeExportImage true //나중에 이미지를 따로 저장해서 쓸 수 있게 할지 말지

#define TempCamNum 1 //아직 카메라 선택이 가능하지 않기 때문에 임시로

//크기를 반응형으로 바꾸기 어렵기 때문에 16:9 비율로 제작 예정
#define GUIWidth 1920/2
#define GUIHeight 1080/2
#define CamWidth GUIHeight
#define CamHeight CamWidth

#define OriLeft 0 
#define OriCenter 1
#define OriRight 2

#define BackgroundColor RGBScale(0x17, 0x17, 0x17)
#define FaceLandMarkColor cv::Scalar(0x00, 0x00, 0xFF)

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

struct RGBScale {
    int r = 0;
    int g = 0;
    int b = 0;
    int rgb = 0;

    RGBScale(int r, int g, int b) {
        this->r = r;
        this->g = g;
        this->b = b;

        this->rgb = RGB(r, g, b);
    }
};

//OpenCV 이미지를 OpenCV이미지의 특정 위치에다가 삽입하는 거
void CPputImage(cv::Mat& I_image, cv::Mat& O_image, cv::Rect pos);
void CPputImage(dlib::matrix<dlib::rgb_pixel> I_image, cv::Mat& O_image, cv::Rect pos);
/// <summary>
/// OpenCV에선 영어만 텍스트를 만들어서 적용이 가능하기 때문에 한글을 위해 윈도우의 다른 곳에서 미리 비트맵으로 이미지를 만들고 OpenCV로 적용하는 방식
/// </summary>
/// <param name="O_image">적용할 이미지</param>
/// <param name="text">텍스트</param>
/// <param name="org">텍스트 위치</param>
/// <param name="ori">텍스트를 어떤 기준 위치로 둘지 OriLeft, OriCenter, OriRight</param>
/// <param name="fontName">폰트 이름</param>
/// <param name="fontWeight">폰트 두께</param>
/// <param name="fontScale">폰트 크기</param>
/// <param name="textColor">텍스트 색</param>
/// <param name="bkColor">텍스트 배경 색</param>
void CPputText(cv::Mat& O_image, cv::String text, cv::Point org, int ori, const char* fontName, int fontWeight, double fontScale, RGBScale textColor, RGBScale bkColor);

//GUI의 여러가지 UI를 컨트롤 할 수 있는 함수 모음
namespace GUICon {
    namespace Var {
        POINT mouse; //마우스의 위치

        //0이면 아무것도 아님 1부터
        int pushButtonNum; //버튼이 눌렸을 때 몇번째 버튼이 눌렸던건지 저장하는 함수
        
        cv::Rect BR_capture;
        cv::Rect BR_preview;
        cv::Rect BR_export;

        RECT CBR_capture;
        RECT CBR_preview;
        RECT CBR_export;
    }

    //버튼
    void buttonsCheck(int event, int x, int y, int flags, void* userData);

    //이미지 출력
    //메인 웹캠쪽에 이미지 넣는거
    void putWebcamView(cv::Mat& I_image, cv::Mat& O_image) {
        CPputImage(I_image, O_image, cv::Rect(0, 0, CamWidth, CamHeight));
    }
    void putWebcamView(dlib::matrix<dlib::rgb_pixel> I_image, cv::Mat& O_image) {
        cv::Mat image = dlib::toMat(I_image);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        CPputImage(image, O_image, cv::Rect(0, 0, CamWidth, CamHeight));
    }
    //프리뷰 쪽에 이미지 넣는거
    void putPreImageView(cv::Mat& I_image, cv::Mat& O_image) {
        CPputImage(I_image, O_image, cv::Rect(GUIHeight + 20, 180 + 20, ((GUIWidth - GUIHeight) / 2) - 40, ((GUIWidth - GUIHeight) / 2) - 40));
    }
    void putPreImageView(dlib::matrix<dlib::rgb_pixel> I_image, cv::Mat& O_image) {
        cv::Mat image = dlib::toMat(I_image);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        CPputImage(image, O_image, cv::Rect(GUIHeight + 20, 180 + 20, ((GUIWidth - GUIHeight) / 2) - 40, ((GUIWidth - GUIHeight) / 2) - 40));
    }

    void cvRect2RECT(cv::Rect& I_rect, RECT& O_rect); //PtlnRect를 사용하는데 windef.h의 Rect만 되기 때문에 OpenCV로 버튼도 만들어야 하는데 2가지를 다 할려면 각각의 변수가 필요해서
}

//GUI에서 고정 이미지를 만들고 나중에 필요할 때 마다 새로 삽입하는 방식으로 하기 위해
void GUIInitImage(cv::Mat& O_image);

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