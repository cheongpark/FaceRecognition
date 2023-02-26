#pragma once
#include "stdafx.h"

#define WinName "Celeb Face Recognition"

//모델이 만약 없으면 설정을 하고 넘어갈지 아니면 그냥 넘어갈지 정하는거
#define CheckModel true

//개발 상황에 따라 필요없는건 꺼야 할 수도 있는 것들
#define CheckCam true //카메라의 유무를 확인 등 카메라 관련 세팅을 보거나 설정할지
#define UseModelAndLogoLoad true //모델을 가져올지 안가져올지
#define UsePreSetImage true //미리 GUI를 세팅하는 것
#define TransCelebVector true //이미지를 벡터로 바꿀지 안바꿀지 "이거 할 땐 Debug로 안됨 Release로 해야함"
#define UseOpenCVGUI true //GUI 관련, 제일 메인이 될 것
#define UseGetCalTime false //한턴에 어느정도의 시간이 걸리는지 테스트 하기 위한 것

//#define ButtonTest true //버튼 테스트좀 해볼려는거

#define LoadImageView true //적용될 이미지를 미리 확인해주게 할지 말지 정하는거
#define LoadImageViewDelay 100 //이미지를 보여줄 때 너무 빨리 넘어가면 안되니깐 속도를 약간 늦춰주는거
#define ShowMainView LoadImageView //메인 웹캠 뷰 쪽에 원본 이미지를 보여줄지
#define MakeExportImage true //나중에 이미지를 따로 저장해서 쓸 수 있게 할지 말지

#define TempCamNum 1 //아직 카메라 선택이 가능하지 않기 때문에 임시로

//크기를 반응형으로 바꾸기 어렵기 때문에 16:9 비율로 제작 예정
#define GUIWidth 1920/2 //GUI 표시하는거 가로
#define GUIHeight 1080/2 //세로
#define CamWidth GUIHeight //카메라를 가져와서 어떤 크기로 저장할지 가로
#define CamHeight CamWidth //세로

#define MakeImageWidth 1196 //출력할 이미지의 가로
#define MakeImageHeight 803 //출력할 이미지의 세로
#define MakeImageStartImageY 147 //출력할 이미지에서 대형 큰 이미지가 시작 되는 첫 Y 위치
#define MakeImageUseOverlay true //출력할 이미지 만들 때 얼굴 사진에서 사각형을 띄울지
#define MakeImageOverlayColor cv::Scalar(0x00, 0x00, 0xFF) //출력할 이미지 만들 때 얼굴 사진에서 사각형의 색깔
#define MakeImageOverlaySize 220

//텍스트 출력하는 함수 쓸 때 어느 기준으로 텍스트를 생성할지 정하는거
#define OriLeft 0 //텍스트의 왼쪽을 기준으로
#define OriCenter 1 //텍스트의 중간을 기준으로
#define OriRight 2 //텍스트의 오른쪽을 기준으로

#define BackgroundColor RGBScale(0x17, 0x17, 0x17) //배경 색
#define FaceLandMarkColor cv::Scalar(0x00, 0x00, 0xFF) //얼굴에 사각형 띄우는거 색

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
std::string logoPath = "./logo/logo.jpg"; //나중에 이미지를 만들 때 쓸 세명컴고 로고 이미지 폴더
std::string exportImagePath = "./ExportImage/"; //내보낼 이미지를 저장하는 경로

std::string SpPath = "./Dlib_Model/shape_predictor_68_face_landmarks_GTX.dat"; //68개의 점을 찍기 때문에 느림 정확도 높음
std::string FastSpPath = "./Dlib_Model/shape_predictor_5_face_landmarks.dat"; //5개의 점만 찍기 때문에 빠름 정확도 낮음
std::string ResNetPath = "./Dlib_Model/dlib_face_recognition_resnet_model_v1.dat"; //얼굴 이미지를 128-D(128개) 벡터 값으로 바꿔주는 중요한 모델

bool useCapture = false; //캡쳐 버튼을 눌렀는지 체크하는 변수
bool isUseCapture = false; //현재 캡쳐가 되고 있는지 저장하는 변수

//내보낼 이미지를 저장하는 곳
cv::Mat I_exportImage; 

//시간 측정 변수
std::chrono::system_clock::time_point start_time; //한턴의 시작 시간을 저장
std::chrono::system_clock::time_point end_time; //한턴이 끝나는 시간을 저장
std::chrono::milliseconds get_cal_time_mill; //한턴의 계산 시간을 밀리세컨드로 저장하는 변수	
std::chrono::system_clock::time_point capture_start; //캡쳐가 시작된 후의 시간을 측정하는 것 3, 2, 1를 재기 위한 것
std::chrono::system_clock::time_point capture_start_now; //캡쳐가 시작되고 난 후의 시간을 계속 측정하는 변수
std::chrono::milliseconds get_capture_start_mill; //캡쳐사 시작된 후의 시간을 밀리세컨드로 저장하는 변수	

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

//RGB에 대한 값을 저장하고 가져오고 그렇게 할 수 있는 구조체
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

struct FaceDistance {
    int faceNum;
    float faceDistance;
};

//OpenCV 이미지를 OpenCV이미지의 특정 위치에다가 삽입하는 거
void CPputImage(cv::Mat I_image, cv::Mat& O_image, cv::Rect pos);
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

    void putWebcamViewText(cv::Mat& O_image, cv::String text, RGBScale color = RGBScale(0xED, 0xED, 0xED));
}

namespace Model {
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector(); //사진에서 얼굴 갯수 찾아주는거임
    dlib::shape_predictor M_sp, M_fastsp; //얼굴에 점을 찍어서 얼굴 부분을 확인하는거
    anet_type M_resnet; //얼굴 이미지를 넣으면 128개의 벡터 값으로 바꿔주는 모델

    //모델의 유무 판단
    bool modelisAlive(std::string& path);
    //모델을 파일 탐색기로 다시 찾게 하는걸 할지 묻고 찾는거
    void useSearchModel(std::string& path, bool& alive_model);
    //모델 적용해주는거
    void applyModel(std::string& path, dlib::shape_predictor& model);
    void applyModel(std::string& path, anet_type& model);
}

//GUI에서 고정 이미지를 만들고 나중에 필요할 때 마다 새로 삽입하는 방식으로 하기 위해
void GUIInitImage(cv::Mat& O_image);

//이미지와 얼굴의 위치를 넣으면 사각형으로 얼굴의 위치를 표시해서 내보내는 함수
void drawFaceRectangle(dlib::cv_image<dlib::bgr_pixel> I_image, cv::Mat& O_image, dlib::rectangle face, cv::Scalar color, bool useFast = true);
void drawFaceRectangle(cv::Mat& I_image, cv::Mat& O_image, dlib::rectangle face, cv::Scalar color, bool useFast = true);
//Width Height Compare Square Cutting 가로 세로 높이를 비교해서 더 긴쪽을 기준으로 이미지를 잘라서 정사각형으로 자르는 것
void whCompareSquareCut(cv::Mat& I_image);

//이미지의 경로들을 가져와주는거
std::vector<std::filesystem::path> loadImagePath(std::string& folder);
//이미지 경로의 이름을 가져오는 것
std::string getFileName(std::filesystem::path& path);

//연예인 얼굴과 자신의 얼굴을 넣으면 변환해주는 함수
std::vector<FaceDistance> compareFacesImage(cv::Mat& faceImg, std::vector<dlib::matrix<float, 0, 1>>faces_vector);
//비교된 얼굴들의 거리 값을 정렬할 때 사용하는 함수
bool face_cmp(FaceDistance a, FaceDistance b);
//캡쳐한 이미지와 계산된 이미지를 넣으면 출력 이미지로 만들어주는 함수
cv::Mat makeCelebCompareImage(cv::Mat& I_camImage, dlib::matrix<dlib::rgb_pixel> I_celebImage, cv::Mat& I_logo, cv::String celebName, float celebDistance);
//로고 이미지가 있는지 판단하는 것 modelisAlive와 동일한 방식
bool ImageisAlive(std::string& path);
//현재의 시간과 날짜를 구하고 리턴
std::string getDateTimer();

//거리를 퍼센트로 바꿔주는거임.. 근데 바꿔주긴 하는데 실제로 그런 확률은 아님
float dis2per(float distance) {
    return (1. - distance) * 100;
}

//텍스트 컬러 지정 (텍스트 컬러, 배경 컬러)
void setColor(short textcolor = COLOR::DARK_WHITE, short background = COLOR::BLACK);