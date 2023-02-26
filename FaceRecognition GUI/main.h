#pragma once
#include "stdafx.h"

#define WinName "Celeb Face Recognition"

//���� ���� ������ ������ �ϰ� �Ѿ�� �ƴϸ� �׳� �Ѿ�� ���ϴ°�
#define CheckModel true

//���� ��Ȳ�� ���� �ʿ���°� ���� �� ���� �ִ� �͵�
#define CheckCam true //ī�޶��� ������ Ȯ�� �� ī�޶� ���� ������ ���ų� ��������
#define UseModelAndLogoLoad true //���� �������� �Ȱ�������
#define UsePreSetImage true //�̸� GUI�� �����ϴ� ��
#define TransCelebVector true //�̹����� ���ͷ� �ٲ��� �ȹٲ��� "�̰� �� �� Debug�� �ȵ� Release�� �ؾ���"
#define UseOpenCVGUI true //GUI ����, ���� ������ �� ��
#define UseGetCalTime false //���Ͽ� ��������� �ð��� �ɸ����� �׽�Ʈ �ϱ� ���� ��

//#define ButtonTest true //��ư �׽�Ʈ�� �غ����°�

#define LoadImageView true //����� �̹����� �̸� Ȯ�����ְ� ���� ���� ���ϴ°�
#define LoadImageViewDelay 100 //�̹����� ������ �� �ʹ� ���� �Ѿ�� �ȵǴϱ� �ӵ��� �ణ �����ִ°�
#define ShowMainView LoadImageView //���� ��ķ �� �ʿ� ���� �̹����� ��������
#define MakeExportImage true //���߿� �̹����� ���� �����ؼ� �� �� �ְ� ���� ����

#define TempCamNum 1 //���� ī�޶� ������ �������� �ʱ� ������ �ӽ÷�

//ũ�⸦ ���������� �ٲٱ� ��Ʊ� ������ 16:9 ������ ���� ����
#define GUIWidth 1920/2 //GUI ǥ���ϴ°� ����
#define GUIHeight 1080/2 //����
#define CamWidth GUIHeight //ī�޶� �����ͼ� � ũ��� �������� ����
#define CamHeight CamWidth //����

#define MakeImageWidth 1196 //����� �̹����� ����
#define MakeImageHeight 803 //����� �̹����� ����
#define MakeImageStartImageY 147 //����� �̹������� ���� ū �̹����� ���� �Ǵ� ù Y ��ġ
#define MakeImageUseOverlay true //����� �̹��� ���� �� �� �������� �簢���� �����
#define MakeImageOverlayColor cv::Scalar(0x00, 0x00, 0xFF) //����� �̹��� ���� �� �� �������� �簢���� ����
#define MakeImageOverlaySize 220

//�ؽ�Ʈ ����ϴ� �Լ� �� �� ��� �������� �ؽ�Ʈ�� �������� ���ϴ°�
#define OriLeft 0 //�ؽ�Ʈ�� ������ ��������
#define OriCenter 1 //�ؽ�Ʈ�� �߰��� ��������
#define OriRight 2 //�ؽ�Ʈ�� �������� ��������

#define BackgroundColor RGBScale(0x17, 0x17, 0x17) //��� ��
#define FaceLandMarkColor cv::Scalar(0x00, 0x00, 0xFF) //�󱼿� �簢�� ���°� ��

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
std::string logoPath = "./logo/logo.jpg"; //���߿� �̹����� ���� �� �� �����İ� �ΰ� �̹��� ����
std::string exportImagePath = "./ExportImage/"; //������ �̹����� �����ϴ� ���

std::string SpPath = "./Dlib_Model/shape_predictor_68_face_landmarks_GTX.dat"; //68���� ���� ��� ������ ���� ��Ȯ�� ����
std::string FastSpPath = "./Dlib_Model/shape_predictor_5_face_landmarks.dat"; //5���� ���� ��� ������ ���� ��Ȯ�� ����
std::string ResNetPath = "./Dlib_Model/dlib_face_recognition_resnet_model_v1.dat"; //�� �̹����� 128-D(128��) ���� ������ �ٲ��ִ� �߿��� ��

bool useCapture = false; //ĸ�� ��ư�� �������� üũ�ϴ� ����
bool isUseCapture = false; //���� ĸ�İ� �ǰ� �ִ��� �����ϴ� ����

//������ �̹����� �����ϴ� ��
cv::Mat I_exportImage; 

//�ð� ���� ����
std::chrono::system_clock::time_point start_time; //������ ���� �ð��� ����
std::chrono::system_clock::time_point end_time; //������ ������ �ð��� ����
std::chrono::milliseconds get_cal_time_mill; //������ ��� �ð��� �и�������� �����ϴ� ����	
std::chrono::system_clock::time_point capture_start; //ĸ�İ� ���۵� ���� �ð��� �����ϴ� �� 3, 2, 1�� ��� ���� ��
std::chrono::system_clock::time_point capture_start_now; //ĸ�İ� ���۵ǰ� �� ���� �ð��� ��� �����ϴ� ����
std::chrono::milliseconds get_capture_start_mill; //ĸ�Ļ� ���۵� ���� �ð��� �и�������� �����ϴ� ����	

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

//RGB�� ���� ���� �����ϰ� �������� �׷��� �� �� �ִ� ����ü
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

//OpenCV �̹����� OpenCV�̹����� Ư�� ��ġ���ٰ� �����ϴ� ��
void CPputImage(cv::Mat I_image, cv::Mat& O_image, cv::Rect pos);
void CPputImage(dlib::matrix<dlib::rgb_pixel> I_image, cv::Mat& O_image, cv::Rect pos);
/// <summary>
/// OpenCV���� ��� �ؽ�Ʈ�� ���� ������ �����ϱ� ������ �ѱ��� ���� �������� �ٸ� ������ �̸� ��Ʈ������ �̹����� ����� OpenCV�� �����ϴ� ���
/// </summary>
/// <param name="O_image">������ �̹���</param>
/// <param name="text">�ؽ�Ʈ</param>
/// <param name="org">�ؽ�Ʈ ��ġ</param>
/// <param name="ori">�ؽ�Ʈ�� � ���� ��ġ�� ���� OriLeft, OriCenter, OriRight</param>
/// <param name="fontName">��Ʈ �̸�</param>
/// <param name="fontWeight">��Ʈ �β�</param>
/// <param name="fontScale">��Ʈ ũ��</param>
/// <param name="textColor">�ؽ�Ʈ ��</param>
/// <param name="bkColor">�ؽ�Ʈ ��� ��</param>
void CPputText(cv::Mat& O_image, cv::String text, cv::Point org, int ori, const char* fontName, int fontWeight, double fontScale, RGBScale textColor, RGBScale bkColor);

//GUI�� �������� UI�� ��Ʈ�� �� �� �ִ� �Լ� ����
namespace GUICon {
    namespace Var {
        POINT mouse; //���콺�� ��ġ

        //0�̸� �ƹ��͵� �ƴ� 1����
        int pushButtonNum; //��ư�� ������ �� ���° ��ư�� ���ȴ����� �����ϴ� �Լ�
        
        cv::Rect BR_capture;
        cv::Rect BR_preview;
        cv::Rect BR_export;

        RECT CBR_capture;
        RECT CBR_preview;
        RECT CBR_export;
    }

    //��ư
    void buttonsCheck(int event, int x, int y, int flags, void* userData);

    //�̹��� ���
    //���� ��ķ�ʿ� �̹��� �ִ°�
    void putWebcamView(cv::Mat& I_image, cv::Mat& O_image) {
        CPputImage(I_image, O_image, cv::Rect(0, 0, CamWidth, CamHeight));
    }
    void putWebcamView(dlib::matrix<dlib::rgb_pixel> I_image, cv::Mat& O_image) {
        cv::Mat image = dlib::toMat(I_image);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        CPputImage(image, O_image, cv::Rect(0, 0, CamWidth, CamHeight));
    }
    //������ �ʿ� �̹��� �ִ°�
    void putPreImageView(cv::Mat& I_image, cv::Mat& O_image) {
        CPputImage(I_image, O_image, cv::Rect(GUIHeight + 20, 180 + 20, ((GUIWidth - GUIHeight) / 2) - 40, ((GUIWidth - GUIHeight) / 2) - 40));
    }
    void putPreImageView(dlib::matrix<dlib::rgb_pixel> I_image, cv::Mat& O_image) {
        cv::Mat image = dlib::toMat(I_image);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        CPputImage(image, O_image, cv::Rect(GUIHeight + 20, 180 + 20, ((GUIWidth - GUIHeight) / 2) - 40, ((GUIWidth - GUIHeight) / 2) - 40));
    }

    void cvRect2RECT(cv::Rect& I_rect, RECT& O_rect); //PtlnRect�� ����ϴµ� windef.h�� Rect�� �Ǳ� ������ OpenCV�� ��ư�� ������ �ϴµ� 2������ �� �ҷ��� ������ ������ �ʿ��ؼ�

    void putWebcamViewText(cv::Mat& O_image, cv::String text, RGBScale color = RGBScale(0xED, 0xED, 0xED));
}

namespace Model {
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector(); //�������� �� ���� ã���ִ°���
    dlib::shape_predictor M_sp, M_fastsp; //�󱼿� ���� �� �� �κ��� Ȯ���ϴ°�
    anet_type M_resnet; //�� �̹����� ������ 128���� ���� ������ �ٲ��ִ� ��

    //���� ���� �Ǵ�
    bool modelisAlive(std::string& path);
    //���� ���� Ž����� �ٽ� ã�� �ϴ°� ���� ���� ã�°�
    void useSearchModel(std::string& path, bool& alive_model);
    //�� �������ִ°�
    void applyModel(std::string& path, dlib::shape_predictor& model);
    void applyModel(std::string& path, anet_type& model);
}

//GUI���� ���� �̹����� ����� ���߿� �ʿ��� �� ���� ���� �����ϴ� ������� �ϱ� ����
void GUIInitImage(cv::Mat& O_image);

//�̹����� ���� ��ġ�� ������ �簢������ ���� ��ġ�� ǥ���ؼ� �������� �Լ�
void drawFaceRectangle(dlib::cv_image<dlib::bgr_pixel> I_image, cv::Mat& O_image, dlib::rectangle face, cv::Scalar color, bool useFast = true);
void drawFaceRectangle(cv::Mat& I_image, cv::Mat& O_image, dlib::rectangle face, cv::Scalar color, bool useFast = true);
//Width Height Compare Square Cutting ���� ���� ���̸� ���ؼ� �� ������ �������� �̹����� �߶� ���簢������ �ڸ��� ��
void whCompareSquareCut(cv::Mat& I_image);

//�̹����� ��ε��� �������ִ°�
std::vector<std::filesystem::path> loadImagePath(std::string& folder);
//�̹��� ����� �̸��� �������� ��
std::string getFileName(std::filesystem::path& path);

//������ �󱼰� �ڽ��� ���� ������ ��ȯ���ִ� �Լ�
std::vector<FaceDistance> compareFacesImage(cv::Mat& faceImg, std::vector<dlib::matrix<float, 0, 1>>faces_vector);
//�񱳵� �󱼵��� �Ÿ� ���� ������ �� ����ϴ� �Լ�
bool face_cmp(FaceDistance a, FaceDistance b);
//ĸ���� �̹����� ���� �̹����� ������ ��� �̹����� ������ִ� �Լ�
cv::Mat makeCelebCompareImage(cv::Mat& I_camImage, dlib::matrix<dlib::rgb_pixel> I_celebImage, cv::Mat& I_logo, cv::String celebName, float celebDistance);
//�ΰ� �̹����� �ִ��� �Ǵ��ϴ� �� modelisAlive�� ������ ���
bool ImageisAlive(std::string& path);
//������ �ð��� ��¥�� ���ϰ� ����
std::string getDateTimer();

//�Ÿ��� �ۼ�Ʈ�� �ٲ��ִ°���.. �ٵ� �ٲ��ֱ� �ϴµ� ������ �׷� Ȯ���� �ƴ�
float dis2per(float distance) {
    return (1. - distance) * 100;
}

//�ؽ�Ʈ �÷� ���� (�ؽ�Ʈ �÷�, ��� �÷�)
void setColor(short textcolor = COLOR::DARK_WHITE, short background = COLOR::BLACK);