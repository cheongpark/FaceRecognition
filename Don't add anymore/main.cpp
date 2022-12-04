//dlib::get_face_chip_details(sp(celeb_overlay, detector(celeb_overlay)[0]), OverlayImgSize, 0.25) 이거 OverlayImgSize 부분은 아무거나 넣으면 안됨 나오는 이미지가 입력한거 * 입력한거임

#include "stdafx.h"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define WINDOW_NAME	"Face Recognition (연예인 닮음 찾기)"

#define Celebrity_Comparison true
#define CelebrityImagePath "celebrity"
#define loadImageView true
#define loadImageViewDelay 100
#define Calculate_Comparison true

#define MakeExportImage true
#define CanvasWidth 1196
#define CanvasHeight 803
#define ImageStartY 147
#define OverlayImgSize 220
#define USE_OVERLAY true
#define OverlayThickness 2
#define OverlayColor dlib::rgb_pixel(255, 0, 0)

#define SCREENW 1280
#define SCREENH 720

//학교 노트북 1280 * 720
//폰카 640 * 480

#define WEBCAMW 640
#define WEBCAMH 480 //원래 크기대로 입력받고 나머지는 짤라야 할 듯

#define WEBCAMCUT (WEBCAMW - WEBCAMH) / 2

#define camNum 1

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

struct FaceDistance
{
	int faceNum = 0;
	float distance = 1;
};

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

void putImage(const cv::Mat Image, cv::Mat& outImage, const cv::Rect pos) {
	cv::Mat imageROI(outImage, pos);
	Image.copyTo(imageROI);
}

void _putText(cv::Mat& img, const cv::String& text, const cv::Point& org, const int ori, const char* fontname, const int fontWeight, const double fontScale, const bool anti, const RGBScale textcolor, const RGBScale bkcolor) {
	int fontSize = (int)(10 * fontScale);
	int width = img.cols;
	int height = fontSize * 3 / 2;

	HDC hdc = CreateCompatibleDC(NULL);

	//채우는 방법? 인 듯, 색을 이걸로 정하면 되는 듯
	HBRUSH hBrush = CreateSolidBrush(bkcolor.rgb);

	//처음 큰 비트맵의 색을 채우기 위해 크기를 지정한 것임
	//https://stackoverflow.com/questions/53175915/creating-a-new-bitmap-with-given-dimensions-filled-with-given-color
	RECT rect;
	rect.left = rect.top = 0;
	rect.right = width;
	rect.bottom = height;

	BITMAPINFOHEADER header;
	ZeroMemory(&header, sizeof(BITMAPINFOHEADER));
	header.biSize = sizeof(BITMAPINFOHEADER);
	header.biWidth = width;
	header.biHeight = height;
	header.biPlanes = 1;
	header.biBitCount = 24;
	BITMAPINFO bitmapInfo;
	bitmapInfo.bmiHeader = header;
	HBITMAP hbmp = CreateDIBSection(NULL, (LPBITMAPINFO)&bitmapInfo, DIB_RGB_COLORS, NULL, NULL, 0);
	SelectObject(hdc, hbmp);

	//텍스트 배경 외의 색이 원래 검은색인데 채우는거임
	FillRect(hdc, &rect, hBrush);

	BITMAP  bitmap;
	GetObject(hbmp, sizeof(BITMAP), &bitmap);

	HFONT hFont = CreateFontA(
		fontSize,
		0,
		0,
		0,
		fontWeight,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, //SHIFTJIS_CHARSET -> DEFAULT_CHARSET으로 변경
		CLIP_DEFAULT_PRECIS,
		(anti ? ANTIALIASED_QUALITY : DEFAULT_QUALITY), //안티를 허용하면 해주고 아니면 안해주고
		VARIABLE_PITCH | FF_ROMAN,
		fontname);
	SelectObject(hdc, hFont);
	SetTextColor(hdc, textcolor.rgb);
	SetBkColor(hdc, bkcolor.rgb);

	//넓이 높이 구하기
	SIZE size;
	GetTextExtentPoint32A(hdc, text.c_str(), (int)text.length(), &size);

	TextOutA(hdc, 0, height / 3 * 1, text.c_str(), (int)text.length());
	int posX = (ori == 0 ? org.x : (ori == 1 ? org.x - (size.cx / 2) : org.x - size.cx)); //기준이 왼쪽이면 그대로, 중간이면 기준점 - 넓이 / 2, 오른쪽이면 기준점 - 넓이
	int posY = org.y - (size.cy / 2 + 5);

	unsigned char* _tmp;
	unsigned char* _img;
	for (int y = 0; y < bitmap.bmHeight; y++) {
		if (posY + y >= 0 && posY + y < img.rows) {
			_img = img.data + (int)(3 * posX + (posY + y) * (((bitmap.bmBitsPixel / 8) * img.cols) & ~3));
			_tmp = (unsigned char*)(bitmap.bmBits) + (int)((bitmap.bmHeight - y - 1) * (((bitmap.bmBitsPixel / 8) * bitmap.bmWidth) & ~3));
			for (int x = 0; x < bitmap.bmWidth; x++) {
				if (x + posX >= img.cols) {
					break;
				}

				if (_tmp[0] != bkcolor.b || _tmp[1] != bkcolor.g || _tmp[2] != bkcolor.r) { //순서를 bgr를 써서 bgr로 한거, rgb 순서이면 그걸로 해야함
					_img[0] = (unsigned char)_tmp[0];
					_img[1] = (unsigned char)_tmp[1];
					_img[2] = (unsigned char)_tmp[2];
				}
				_img += 3;
				_tmp += 3;
			}
		}
	}

	DeleteObject(hFont);
	DeleteObject(hbmp);
	DeleteDC(hdc);
}

std::vector<std::filesystem::path> load_image_path(std::string folder) {
	std::vector<std::filesystem::path> path;

	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(std::filesystem::current_path() / folder))
		if (!entry.is_directory() && (entry.path().extension() == ".jpg" ||
			entry.path().extension() == ".png" ||
			entry.path().extension() == ".jpeg"))
			path.push_back(entry.path());

	return path;
}

std::string get_filename(std::filesystem::path& path) {
	return path.filename().string().substr(0, path.filename().string().find('.'));
}

void setColor(short textColor = COLOR::DARK_WHITE, short background = COLOR::BLACK) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textColor + background * 16);
}

void ErrorExit() {
    system("pause");
    exit(1);
}

int main(int argv, char* args) {
	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
	dlib::shape_predictor sp, fastsp;
	anet_type net;

	std::vector<dlib::matrix<float, 0, 1>> Celebface_descriptors;

	std::string spPath = "Dlib_Model/shape_predictor_68_face_landmarks_GTX.dat";
	std::string fastspPath = "Dlib_Model/shape_predictor_5_face_landmarks.dat";
	std::string ResNetPath = "Dlib_Model/dlib_face_recognition_resnet_model_v1.dat";

	std::vector<dlib::matrix<dlib::rgb_pixel>> celeb_faces;

	std::vector<dlib::matrix<dlib::rgb_pixel>> celeb_faces_chip;
	std::vector<std::string> celeb_names;

	dlib::matrix<dlib::bgr_pixel> logo; //나중에 이미지 출력할 때 세명컴고 로고임
	std::string logoPath = "logo/logo.jpg";
	if (MakeExportImage) {
		if (_access(spPath.c_str(), 0) != -1) {
			dlib::load_image(logo, logoPath);
			setColor(COLOR::GREEN);
			std::cout << logoPath << " 사진을 찾았습니다." << std::endl;
			setColor();
		}
		else {
			setColor(COLOR::RED);
			std::cout << logoPath << " 사진을 찾지 못했습니다." << std::endl;
			setColor();
			ErrorExit();
		}
	}

	//모델 로딩
	//캠과 여러가지 요소들을 렌더링
	//캠 세팅 - 카메라 선택 때문에 아래에 있는거임
	//캠요소를 가져와서 비교해서 저장하고 표시

	//모델 로딩
	{
		{
			if (_access(spPath.c_str(), 0) != -1) {
				setColor(COLOR::GREEN);
				std::cout << spPath << " 모델을 찾았습니다." << std::endl;
				setColor();
			}
			else {
				setColor(COLOR::RED);
				std::cout << spPath << " 모델을 찾지 못했습니다." << std::endl;
				setColor();
				ErrorExit();
			}

			if (_access(fastspPath.c_str(), 0) != -1) {
				setColor(COLOR::GREEN);
				std::cout << fastspPath << " 모델을 찾았습니다." << std::endl;
				setColor();
			}
			else {
				setColor(COLOR::RED);
				std::cout << fastspPath << " 모델을 찾지 못했습니다." << std::endl;
				setColor();
				ErrorExit();
			}

			if (_access(ResNetPath.c_str(), 0) != -1) {
				setColor(COLOR::GREEN);
				std::cout << ResNetPath << " 모델을 찾았습니다." << std::endl;
				setColor();
			}
			else {
				setColor(COLOR::RED);
				std::cout << ResNetPath << " 모델을 찾지 못했습니다." << std::endl;
				setColor();
				ErrorExit();
			}
		}
		dlib::deserialize(spPath) >> sp;
		dlib::deserialize(fastspPath) >> fastsp;
		dlib::deserialize(ResNetPath) >> net;
	}
	//이미지 로드 및 벡터화
	{
		if (Celebrity_Comparison) {
			dlib::image_window showCelebImg;
			std::vector<std::filesystem::path> celeb_path = load_image_path(CelebrityImagePath);

			int celebCount = 0;

			for (int celeb = 0; celeb < celeb_path.size(); celeb++) {
				dlib::matrix<dlib::rgb_pixel> img;

				load_image(img, celeb_path[celeb].string());
				try {
					if (detector(img).size() != 1)
						throw detector(img).size();

					setColor(COLOR::GREEN);
					std::cout << celeb_path[celeb] << " 사진을 가져왔습니다.";
					setColor(COLOR::WHITE);
					std::cout << " 설정 된 유명인은 \"" << get_filename(celeb_path[celeb]) << "\" 입니다." << std::endl;

					auto celeb_face = detector(img);

					dlib::matrix<dlib::rgb_pixel> face_chip;
					dlib::extract_image_chip(img, dlib::get_face_chip_details(sp(img, celeb_face[0]), 150, 0.25), face_chip);

					if (loadImageView) {
						showCelebImg.set_image(face_chip);
						Sleep(loadImageViewDelay);
					}

					if (MakeExportImage) //나중에 이미지 만들어서 저장할 때 쓸려고
						celeb_faces.push_back(img);
					celeb_faces_chip.push_back(std::move(face_chip));
					celeb_names.push_back(get_filename(celeb_path[celeb]));

					celebCount++;
				}
				catch (size_t num) {
					setColor(COLOR::RED);
					std::cout << celeb_path[celeb] << " 경로의 사진에서 " << detector(img).size() << "개의 얼굴이 발견 되었습니다. 1명만 있도록 맞춰주세요." << std::endl;
					setColor(COLOR::DARK_WHITE);
				}
			}
			setColor(COLOR::AQUA);
			std::cout << celebCount << "명의 이미지를 가져왔습니다." << std::endl;
			setColor();

			if (loadImageView)
				showCelebImg.close_window();

			setColor(COLOR::YELLOW);
			std::cout << std::endl << "128개의 벡터 값으로 변환 중입니다." << std::endl;
			Celebface_descriptors = net(celeb_faces_chip);
			setColor(COLOR::GREEN);

			std::cout << "완료" << std::endl << std::endl;
			setColor();
		}
	}
	
	//캠과 여러가지 요소들을 렌더링 
	{
		//opencv 윈도우 이름 정하는거
		cvui::init(WINDOW_NAME);

		cv::VideoCapture cap;
		cap.open(camNum);

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

		//이건 이 카메라의 원래 크기대로 설정하고 거기에서 짤라야 함
		//여기선 이 부분 지금 안됨
		cap.set(cv::CAP_PROP_FRAME_WIDTH, WEBCAMW); //웹캠의 가로 사이즈를 설정함
		cap.set(cv::CAP_PROP_FRAME_HEIGHT, WEBCAMH); //웹캠의 세로 사이즈를 설정함ㅍ

		//cv::Mat Test;
		//cap.read(Test);
		//cv::imshow("test", Test);

		int LRMargin = 20;
		int Margin = 30;

		bool useLand = true;

		clock_t start, timer;
		bool iscapture = false;
		int captureReadyCount = 0;

		char CaptureCount[5] = "";

		bool usecapture = false;

		cv::Mat CaptureImg(WEBCAMH, WEBCAMW, CV_8UC3); //정사각형 캡쳐하면 이 곳으로 넘길꺼
		cv::Mat camImg(WEBCAMH, WEBCAMW, CV_8UC3); //일단 여기에서 16:9로 맞추고 나중에 정사각형으로 바꿀꺼임

		cv::Mat frame(SCREENH, SCREENW, CV_8UC3);

		frame = cv::Scalar(0x11, 0x11, 0x11);
		_putText(frame, "Face Recognition", cv::Point(frame.cols - ((frame.cols - frame.rows) / 2), 30), 1, "맑은 고딕", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x11, 0x11, 0x11));
		cv::line(frame, cv::Point(frame.rows + 1, 0), cv::Point(frame.rows + 1, frame.rows), cv::Scalar(0xED, 0xED, 0xED), 2);
		//위에서 미리 하고 매번 고정 된 위치에 나오는게 아닌 것들 부분에만 배경 색을 덮어서 업데이트 되게 하는 것.

		//int frameCnt = 0;
		//float timeElapsed = 0.0f;

		while (true) {
			std::cout << "check" << std::endl;

			{ //fps 계산
				//frameCnt++;
				//if((mTimer))
			}

			//프레임 렌더링
			{
				//cols가 가로 , rows가 세로
				//배경 컬러 지정
				//frame = cv::Scalar(0x11, 0x11, 0x11);
				cv::rectangle(frame, cv::Rect(cv::Point2i(frame.rows + 1 + 2, 30 + 60), cv::Point2i(frame.cols, frame.rows)), cv::Scalar(0x11, 0x11, 0x11), -1);

				//메인 제목
				//_putText(frame, "Face Recognition", cv::Point(frame.cols - ((frame.cols - frame.rows) / 2), 30), 1, "맑은 고딕", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x11, 0x11, 0x11));

				if (true) { //카메라를 읽어야 할때
					if (!cap.read(camImg)) {
						setColor(12);
						std::cout << "웹캠에서 이미지를 읽을 수 없습니다." << std::endl;
						goto webcamEnd; //매트릭스로 읽을 수 없으면 건너뛰기
					}
					
					//여기서 미리 바꾸는 이유는 화면에서 표시되지 않는 부분도 감지하기 때문
					camImg = camImg(cv::Range(0, WEBCAMH), cv::Range(WEBCAMCUT, WEBCAMCUT + WEBCAMH));

					dlib::cv_image<dlib::bgr_pixel> cimg(camImg); //이미지를 매트릭스 형태로 저장
					auto face = detector(cimg);

					char faceText[30];
					sprintf_s(faceText, "얼굴 %d명 감지 되었습니다.", face.size());
					_putText(frame, faceText, cv::Point(frame.rows + LRMargin, 100), 0, "맑은 고딕", FW_BOLD, 3, true, (face.size() >= 2 ? RGBScale(0xDA, 0x00, 0x37) : RGBScale(0xED, 0xED, 0xED)), RGBScale(0x11, 0x11, 0x11));

					if (face.size() >= 2) {
						setColor(12);
						std::cout << "얼굴이 여러개 있네요." << std::endl;
						setColor();
						usecapture = false;
						goto webcamEnd;
					}
					else if (face.size() == 0) {
						usecapture = false;
						goto webcamEnd;
					}
						
					usecapture = true;

					auto faceRect = dlib::get_face_chip_details(fastsp(cimg, face[0]), WEBCAMH, 0.2).rect;

					cv::rectangle(camImg, cv::Rect(faceRect.left(), faceRect.top(), faceRect.width(), faceRect.height()), cv::Scalar(0, 0, 255), 1, 4, 0);
				}

				webcamEnd:
				
				if (true) { //카메라를 읽어야 할때
					cv::resize(camImg, camImg, cv::Size(frame.rows, frame.rows));
					//cv::imshow("test", camImg);

					putImage(camImg, frame, cv::Rect(0, 0, frame.rows, frame.rows));
					//if (true) {
					//	_putText(frame, "웹캠 연결 없음", cv::Point(frame.rows / 2, frame.rows / 2), 1, "맑은 고딕", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x4E, 0x9F, 0x3D));
					//}
				}

				if (cvui::button(frame, frame.rows + LRMargin, frame.rows - 100, "Capture", 0.5, 0xFF3333)) {
					setColor(COLOR::PURPLE);
					std::cout << "Capture" << std::endl;
					setColor();

					iscapture = true;
					captureReadyCount = 0;
					sprintf_s(CaptureCount, "");

					start = clock();
				}

				if (iscapture) {
					timer = clock();
					
					if (timer - start >= (clock_t)3000) {
						iscapture = false;

						setColor(COLOR::GREEN);
						std::cout << "Capture!" << std::endl;
						setColor();

						//캡쳐한 후 captureImg로 넘기고 따른 곳으로 잠시 넘겨서 계산한 뒤에 다시 움직이게 할 예정
						cap.read(CaptureImg);
						CaptureImg = CaptureImg(cv::Range(0, WEBCAMH), cv::Range(WEBCAMCUT, WEBCAMCUT + WEBCAMH));
						//cv::imshow("test2", CaptureImg);

						if (Calculate_Comparison) {
							if (usecapture) {
								setColor(COLOR::YELLOW);
								std::cout << "계산 중" << std::endl;

								dlib::cv_image<dlib::bgr_pixel> Myface(CaptureImg);

								auto my_face = detector(Myface);

								dlib::matrix<dlib::rgb_pixel> face_chip;
								extract_image_chip(Myface, dlib::get_face_chip_details(sp(Myface, my_face[0]), 150, 0.25), face_chip); //150은 비교하는거라 dlib에서 기본으로 추천하는게 저거임

								dlib::matrix<float, 0, 1> Myface_descriptor = net(face_chip);

								FaceDistance faceDistance, faceLowDistance;
								setColor(COLOR::AQUA);
								for (int j = 0; j < Celebface_descriptors.size(); j++) {
									faceDistance = { j, dlib::length(Myface_descriptor - Celebface_descriptors[j]) };
									//float distance = dlib::length(Myface_descriptor - Celebface_descriptors[j]);
									if (faceDistance.distance < faceLowDistance.distance) {
										faceLowDistance = { faceDistance.faceNum, faceDistance.distance };
										std::cout << faceDistance.distance << " " << celeb_names[faceDistance.faceNum] << std::endl;
									}
								}
								setColor(COLOR::GREEN);
								std::cout << "가장 닮은 얼굴은 " << celeb_names[faceLowDistance.faceNum] << ", 거리 : " << faceLowDistance.distance << std::endl;
								setColor();
								//테스트로 저장하는거임
								dlib::save_jpeg(face_chip, "face_chip.jpg");
								dlib::save_jpeg(Myface, "My_Face.jpg");

								//이미지 이쁘게 만들어서 출력하는 것
								if (MakeExportImage) {
									cv::Mat outImg(CanvasHeight, CanvasWidth, CV_8UC3, cv::Scalar(255, 255, 255)); //Scalar는 채우기임

									dlib::matrix<dlib::bgr_pixel> celeb_face_chip, my_face_chip;
									extract_image_chip(celeb_faces[faceLowDistance.faceNum], dlib::get_face_chip_details(sp(celeb_faces[faceLowDistance.faceNum], detector(celeb_faces[faceLowDistance.faceNum])[0]), 150, 0.6).rect, celeb_face_chip); //get_face_chip_details 는 얼굴 위치, 가로세로 사이즈(정사각형), 어느정도 나올지 숫자가 높아질 수록 많이 포함됨 rect로 하는건 회전 값을 안주기 위해
									extract_image_chip(Myface, dlib::get_face_chip_details(sp(Myface, detector(Myface)[0]), 150, 0.6).rect, my_face_chip);
									//사이즈 부분은 전체 이미지에서 얼굴 부분이 CanvasWidth / 2 보다 크면 잘릴 수 있음
									//발생 이유 이미지에서 어느정도 거리를 주며 자르게 되는데 그 거리가 사진을 넘어버리면 넘어버린 상태로 자르기 때문에 이래 되는거임

									cv::Mat cv_celeb_face_chip = dlib::toMat(celeb_face_chip);
									cv::Mat cv_my_face_chip = dlib::toMat(my_face_chip);
									cv::Mat cv_logo = dlib::toMat(logo);

									cv::resize(cv_celeb_face_chip, cv_celeb_face_chip, cv::Size(CanvasWidth / 2, CanvasWidth / 2));
									cv::resize(cv_my_face_chip, cv_my_face_chip, cv::Size(CanvasWidth / 2, CanvasWidth / 2));
									cv::resize(cv_logo, cv_logo, cv::Size(535, 147));

									putImage(cv_logo, outImg, cv::Rect(0, 0, 535, 147));

									putImage(cv_my_face_chip, outImg, cv::Rect(0, ImageStartY, CanvasWidth / 2, CanvasWidth / 2)); //Rect에서 왼쪽 위하고 크기 지정임 Rect(x1, y1, width, height)
									putImage(cv_celeb_face_chip, outImg, cv::Rect(CanvasWidth / 2, ImageStartY, CanvasWidth / 2, CanvasWidth / 2));

									if (USE_OVERLAY) {
										dlib::matrix<dlib::bgr_pixel> celeb_overlay = celeb_face_chip, my_overlay = my_face_chip;
										dlib::matrix<dlib::bgr_pixel> celeb_overlay_resize(OverlayImgSize, OverlayImgSize), my_overlay_resize(OverlayImgSize, OverlayImgSize);

										dlib::resize_image(celeb_overlay, celeb_overlay_resize);
										dlib::resize_image(my_overlay, my_overlay_resize);

										dlib::draw_rectangle(celeb_overlay_resize, dlib::get_face_chip_details(sp(celeb_overlay_resize, detector(celeb_overlay_resize)[0]), 150, 0.25).rect, OverlayColor, OverlayThickness);
										dlib::draw_rectangle(my_overlay_resize, dlib::get_face_chip_details(sp(my_overlay_resize, detector(my_overlay_resize)[0]), 150, 0.25).rect, OverlayColor, OverlayThickness);

										cv::Mat cv_celeb_overlay_resize = dlib::toMat(celeb_overlay_resize);
										cv::Mat cv_my_overlay_resize = dlib::toMat(my_overlay_resize);

										putImage(cv_my_overlay_resize, outImg, cv::Rect(0, 525, OverlayImgSize, OverlayImgSize));
										putImage(cv_celeb_overlay_resize, outImg, cv::Rect(CanvasWidth - OverlayImgSize, 525, OverlayImgSize, OverlayImgSize));

										cv::rectangle(outImg, cv::Rect(0, 525, OverlayImgSize, OverlayImgSize), cv::Scalar(0, 0, 0), 3, 4, 0);
										cv::rectangle(outImg, cv::Rect(CanvasWidth - OverlayImgSize, 525, OverlayImgSize, OverlayImgSize), cv::Scalar(0, 0, 0), 3, 4, 0);
									}

									cv::rectangle(outImg, cv::Rect(0, ImageStartY, CanvasWidth / 2, CanvasWidth / 2), cv::Scalar(0, 0, 0), 3, 4, 0); //cv::copyMakeBorder 이건 밖 만 테두리 채워져서 사진 이미지 크기가 달라져서 안됨
									cv::rectangle(outImg, cv::Rect(CanvasWidth / 2, ImageStartY, CanvasWidth / 2, CanvasWidth / 2), cv::Scalar(0, 0, 0), 3, 4, 0); //3은 두께임

									//여기 아래 텍스트 바꾸는거 하셈
									//한글 텍스트
									char celebCompare[20];
									sprintf_s(celebCompare, "%.1f%%", (1 - faceLowDistance.distance) * 100);
									_putText(outImg, celebCompare, cvPoint(CanvasWidth, 95), 2, "맑은 고딕", FW_BOLD, 6, true, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //한글로 되게 바꿔야함 (완료)
									_putText(outImg, "2022-11-08 오후 03:16:32", cvPoint(CanvasWidth, 0), 2, "맑은 고딕", FW_DEMIBOLD, 4, true, RGBScale(255, 0, 0), RGBScale(255, 255, 255));

									_putText(outImg, "나", cvPoint(CanvasWidth / 4, 745), 1, "맑은 고딕", FW_BOLD, 6, true, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //CanvasWidth / 4 첫번째 사진의 중간
									_putText(outImg, celeb_names[faceLowDistance.faceNum], cvPoint(CanvasWidth / 4 * 3, 745), 1, "맑은 고딕", FW_BOLD, 6, true, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //CanvasWidth / 4 * 3 는 2번째 사진의 중간을 가르키기 위해

									//cv::imshow("oImg", outImg);

									//cv::waitKey(); //이거 안하면 렉걸림 키 기다리는 듯
									//cv::destroyAllWindows(); //이미지 띄운거 끄는건가봄

									cv::imwrite("01.jpg", outImg);
								}
							}
							else {
								setColor(COLOR::RED);
								std::cout << "얼굴이 감지되지 않았습니다." << std::endl;
								setColor();
							}
						}
					}
					if (iscapture && timer - start >= (clock_t)(1000 * captureReadyCount)) {
						sprintf_s(CaptureCount, "%d", 3 - captureReadyCount);
						setColor(COLOR::AQUA);
						std::cout << 3 - captureReadyCount++ << std::endl;
						setColor();
					}

					if (CaptureCount != "")
						_putText(frame, CaptureCount, cv::Point(frame.rows / 2, frame.rows / 2), 1, "맑은 고딕", FW_BOLD, 10, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x4E, 0x9F, 0x3D));

					//std::cout << clock() << std::endl;
				}

				//cv::line(frame, cv::Point(frame.rows + 1, 0), cv::Point(frame.rows + 1, frame.rows), cv::Scalar(0xED, 0xED, 0xED), 2);
			}

			cvui::update();

			cv::imshow(WINDOW_NAME, frame);

			if (cv::waitKey(20) == 27) { // 27 = ESC
				break;
			}
		}
	}
	

	
    //캠 세팅
    
}