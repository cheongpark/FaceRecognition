#include "stdafx.h"
#include "main.h"

#include <iostream>
#include <Windows.h>

int main(int argv, char* args[]) {
	//std::cout << argv << std::endl << args[0] << std::endl << args[1]; //파라미터 테스트

	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector(); //사진에서 얼굴 갯수 찾아주는거임
	dlib::shape_predictor M_sp, M_fastsp; //얼굴에 점을 찍어서 얼굴 부분을 확인하는거
	anet_type M_resnet; //얼굴 이미지를 넣으면 128개의 벡터 값으로 바꿔주는 모델

	std::vector<dlib::matrix<dlib::rgb_pixel>> I_celeb_faces; //연예인 원본 이미지
	std::vector<dlib::matrix<dlib::rgb_pixel>> I_celeb_faces_chip; //연예인 얼굴 이미지
	std::vector<std::string> S_celeb_names; //연예인 이름

	std::vector<dlib::matrix<float, 0, 1>> celeb_face_128_vector; //128-D 의 벡터 값들

	cv::Mat frame(GUIHeight, GUIWidth, CV_8UC3); //미리 정한 가로 세로 대로 frame 만드는거
	cv::Mat preSetImage(GUIHeight, GUIWidth, CV_8UC3); //고정 GUI를 저장하는 곳

	bool useFaceLandMark = true; //메인 웹캠뷰 쪽에서 얼굴을 인식해서 빨간 상자를 띄어주는걸 할지 말지

	/*
		구현된 중요 기술들

		한글이나 영어를 컴퓨터의 폰트로 OpenCV 이미지에 넣는 것 (제일 어려웠음 일주일 이상 걸림)
		모델들이 있는지 판단하고 가져오는 것
		모델이 감지가 안되는 경우 가져올지 다른걸 사용할지 정하는거 (가져오는 기능 아직 미구현)
		특정 경로에 있는 사진들을 모두 가져와 얼굴이 1개인 것을 확인하고 벡터로 저장하는거
		카메라들 감지 하는 것
		원본 이미지를 정사각형에 표시할 때 이미지를 정사각형에 맞춰 축소하고 빈 공간을 채우는 것 (어려웠음)
		마우스가 눌러진 상태에서 다른 곳에서 마우스를 떼면 감지가 안되는 것으로 (어려웠음)
	*/

	/*
		cols 가로
		rows 세로

		//랜드마크 버튼은 메뉴로 지정할 수 있도록 제작
		
		cv::Rect 인수는 (x, y, width, height)임 즉 어디에 넣을껀지와 가로 세로 길이만 입력하면 됨.

		이름 규칙
		이미지 : I_변수 //Image
		프리뷰 이미지 : PI_변수 //Preview Image
		비디오 : V_변수 //Video
		프리뷰 비디오 : PV_변수 //Preview Video
		String : S_변수 //String
		모델 : M_변수 //Model
		버튼 Rect : BR_변수 //Button Rect
		버튼 위치 비교용 Rect : CBR_변수 //Compare Button Rect
		 
		해볼꺼
		1. 모델 파일을 파일 탐색기로 찾게 하는 기능 만드릭
		2. 메모리를 아끼기 위해 이미지 경로를 저장할 때 이름과 확장자만 가져와서 나중에 어디로 바꿀 때 기본 이미지 경로와 이름과 확장자를 결합해서 가져올 수 있게도 바꿔보기
		3. 카메라 이름 확인하는거는 DirectShow 를 사용해야 하고 순서도 다를 수 있기 때문에 나중에 도전

		1. 카메라 확인
		2. 모델을 불러오기
		3. 연예인 이미지의 얼굴을 확인하고 벡터화
	*/
	/*
	if (ButtonTest) {
		cv::Mat test(400, 400, CV_8UC3);
		
		cv::namedWindow("TestButton");
		cv::setMouseCallback("TestButton", TestClicked);
		while (1) {
			test = cv::Scalar(0xFF);
			cv::imshow("TestButton", test);
			cv::waitKey();
		}	
	}
	*/
	
	if (CheckCam) {
		//카메라가 잘 있는지 확인, 만약 잘찍히지 않거나 없거나 발견되지 않으면 사진으로 하거나 프로그램을 끄거나 등등 하기

		//카메라 갯수 확인하기
		cv::VideoCapture cap;
		int CamCount = 0; //카메라 갯수 

		setColor(COLOR::GREEN);
		while (true) {
			cap.open(CamCount);
			if (cap.isOpened()) std::cout << CamCount++ << " 번의 카메라가 감지 되었습니다." << std::endl;
			else break;
		}
		setColor();
		std::cout << "카메라가 " << CamCount << "개 발견되었습니다." << std::endl;

		//이렇게 해서 카메라 번호를 확인하고 OpenCV 메뉴에서 카메라 번호 설정을 띄우게 하는 것
	}
	if (UseModelLoad) {
		//모델 로드
		bool alive_SpModel = false, alive_FastSpModel = false, alive_ResNetModel = false;

		//ResNet 모델부터 불러옴 제일 중요하기 때문에 없으면 정지
		if (modelisAlive(ResNetPath)) alive_ResNetModel = true;
		else if (CheckModel) useSearchModel(ResNetPath, alive_ResNetModel);
		else {
			setColor();
			std::cout << ResNetPath;
			setColor(COLOR::RED);
			std::cout << " 모델이 발견되지 않았기 때문에 프로그램을 종료합니다." << std::endl;
			exit(-1);
		}

		//빠른 모델이 없거나 느린 모델이 없으면 작동 방식
		//모델이 모두 있는지 없는지 확인 후 판단 하는 방식으로
		if (modelisAlive(SpPath)) alive_SpModel = true;
		else if (CheckModel) useSearchModel(SpPath, alive_SpModel);
		if (modelisAlive(FastSpPath)) alive_FastSpModel = true;
		else if (CheckModel) useSearchModel(FastSpPath, alive_FastSpModel);

		std::cout << std::endl;

		//-----판단-----
		//하나만 있을 때 램 사용량 때문에 모델 하나만 적용하게 해야하는데 그럴려면 코드를 많이 복잡하게 될 수 있어서 나중에 적용할 수 있으면 하는걸로
		//느린 모델만 있을 때
		if (!alive_SpModel && !alive_FastSpModel) {
			setColor(COLOR::RED);
			std::cout << " 모델이 모두 발견되지 않았기 때문에 프로그램을 종료합니다." << std::endl;
			exit(-1);
		}
		if (alive_SpModel && !alive_FastSpModel) {
			setColor();
			std::cout << SpPath;
			setColor(COLOR::BLUE);
			std::cout << " 모델이 확인되었지만 ";
			setColor();
			std::cout << FastSpPath;
			setColor(COLOR::BLUE);
			std::cout << " 모델은 확인되지 않았습니다." << std::endl;

			std::cout << "그렇기 때문에 속도가 느린 것으로 모두 적용합니다." << std::endl;

			dlib::deserialize(SpPath) >> M_sp;
			dlib::deserialize(SpPath) >> M_fastsp;
		}
		if (!alive_SpModel && alive_FastSpModel) {
			setColor();
			std::cout << FastSpPath;
			setColor(COLOR::BLUE);
			std::cout << " 모델이 확인되었지만 ";
			setColor();
			std::cout << SpPath;
			setColor(COLOR::BLUE);
			std::cout << " 모델은 확인되지 않았습니다." << std::endl;

			std::cout << "그렇기 때문에 속도가 빠른 것으로 모두 적용합니다. 다만 정확도가 낮아질 수 있습니다." << std::endl;

			dlib::deserialize(FastSpPath) >> M_sp;
			dlib::deserialize(FastSpPath) >> M_fastsp;
		}

		if (alive_SpModel && alive_FastSpModel) {
			applyModel(SpPath, M_sp);
			applyModel(FastSpPath, M_fastsp);
		}
		applyModel(ResNetPath, M_resnet);
	}
	if (PreSetImage) {
		//사전에 GUI를 미리 이미지로 만들어서 함 이걸로 이미지 가져올 때 미리 보여주는 식으로
		//속도를 위해 고정 위치인 UI는 다른 곳에 미리 저장을 하고 업데이트 해야할 때 마다 저장한걸 끼워넣기
		GUIInitImage(preSetImage);

		//웹캠을 출력할 곳
		cv::Mat webcam(CamHeight, CamWidth, CV_8UC3);
		webcam = cv::Scalar(0x88, 0xC9, 0x03); //색 지정
		GUICon::putWebcamView(webcam, preSetImage); //그 색으로 웹캠뷰 채우는거

		//프리뷰 이미지 출력할 곳
		cv::Mat preImage((GUIWidth - GUIHeight) / 2 - 40, (GUIWidth - GUIHeight) / 2 - 40, CV_8UC3); //가로 전체에서 메인 카메라 표시 될 부분을 뺀거의 반에서 사이즈 조정 때문에 양옆 위아래 20씩 간격을 만들기 위해 40을 뺌
		preImage = cv::Scalar(0x00, 0x00, 0x00); //색 지정
		GUICon::putPreImageView(preImage, preSetImage); //그 색으로 프리뷰 채우는거

		frame = preSetImage; //메인 frame 처리 하는 곳으로 넘겨주는거

		//cv::imshow("Test", preSetImage); //frame 보여주는거

		//cv::waitKey(10); //키 기다리는거 이거 안하면 렉걸림
	}
	if (TransCelebVector) {
		//연예인 이미지를 벡터화 하는 것

		std::cout << std::endl;

		//폴더에 있는 이미지 부터 확인
		//dlib::image_window PI_showCelebImg; //연예인 얼굴을 직접 보기 위해
		std::vector<std::filesystem::path> celeb_paths = loadImagePath(CelebrityImagePath);

		int celebCount = 0; //연예인 몇명인지 이미지중 얼굴이 여러개도 있을 수 있기 때문에 그걸 걸러내기 위해 경로 갯수로 하는게 아님

		for (std::filesystem::path celeb_path : celeb_paths) {
			dlib::matrix<dlib::rgb_pixel> I_celeb_img;

			dlib::load_image(I_celeb_img, celeb_path.string());
			
			if (detector(I_celeb_img).size() == 1) { //사진에서 얼굴이 1개 감지되면
				setColor(COLOR::GREEN);
				std::cout << celeb_path << " 사진을 가져왔습니다.";
				setColor(COLOR::WHITE);
				std::cout << " 설정 된 연예인은 \"" << getFileName(celeb_path) << "\" 입니다." << std::endl;

				auto celeb_face = detector(I_celeb_img); //얼굴의 위치를 추출하기 위한 변수임

				dlib::matrix<dlib::rgb_pixel> I_face_chip; //사진에서 얼굴 부분만 추출한 사진을 가지고 있는 변수
				dlib::extract_image_chip(I_celeb_img, dlib::get_face_chip_details(M_sp(I_celeb_img, celeb_face[0]), 150, 0.25), I_face_chip); //얼굴 부분만 추출해주는 명령어

				if (LoadImageView) {
					//PI_showCelebImg.set_image(I_face_chip);
					if (ShowMainView) {
						//이미지와 같은 크기의 정사각형 배경을 만들고 색은 그 이미지의 평균색
						//그리고 그 배경을 웹캠 뷰에 맞게 리사이즈 시켜서 넣는다(그냥 함수로 바로 넣으면 됨)
						//만약 정사각형이면 이 과정을 패스

						if (I_celeb_img.nc() == I_celeb_img.nr())
							GUICon::putWebcamView(I_celeb_img, preSetImage);
						else {
							//이미지의 길이에 맞춰 배경 생성하고 색 넣기
							int squareLen = (I_celeb_img.nc() < I_celeb_img.nr() ? I_celeb_img.nr() : I_celeb_img.nc());
							
							cv::Mat originalViewImage(squareLen, squareLen, CV_8UC3);
							originalViewImage = cv::Scalar(0xFF, 0xFF, 0xFF);

							//배경 이미지 중앙에 사진 넣기
							//사진의 세로가 가로보다 짧을 경우 squareLen의 길이에서 반을 나누고 이미지 세로의 반 만큼 빼서 좌표 구하기						
							if (I_celeb_img.nr() < I_celeb_img.nc()) { //가로가 더 클 경우 같을 경우 비교 안하는건 위에서 이미 해서
								int ypos = (squareLen / 2) - (I_celeb_img.nr() / 2); //세로가 짧을 경우 정사각형의 한 변의 길에서 이미지 세로 반을 빼서 위치 구하는 것
								CPputImage(I_celeb_img, originalViewImage, cv::Rect(0, ypos, I_celeb_img.nc(), I_celeb_img.nr()));
							}
							else {
								int xpos = (squareLen / 2) - (I_celeb_img.nc() / 2); //위와 반대
								CPputImage(I_celeb_img, originalViewImage, cv::Rect(xpos, 0, I_celeb_img.nc(), I_celeb_img.nr()));
							}
							cv::cvtColor(originalViewImage, originalViewImage, cv::COLOR_RGB2BGR);
							GUICon::putWebcamView(originalViewImage, frame);
						}
					}
					GUICon::putPreImageView(I_face_chip, frame);
					cv::imshow("Test", frame);
					cv::waitKey(20);
					Sleep(LoadImageViewDelay);
				}

				I_celeb_faces_chip.push_back(I_face_chip);
				if (MakeExportImage) {
					I_celeb_faces.push_back(I_celeb_img);
					S_celeb_names.push_back(getFileName(celeb_path));
				}

				celebCount++;
			}
			else { //사진에서 얼굴이 여러개 감지되거나 감지 되지 않았을 때
				setColor(COLOR::RED);
				std::cout << celeb_path << " 경로의 사진에서 " << detector(I_celeb_img).size() << "명의 얼굴이 발견 되었습니다. 1명만 있도록 맞춰주세요." << std::endl;
				setColor(COLOR::GREEN);
			}
		}

		setColor(COLOR::AQUA);
		std::cout << celebCount << "명의 이미지를 가져왔습니다." << std::endl;
		setColor();

		//if (LoadImageView)
			//PI_showCelebImg.close_window();

		setColor(COLOR::YELLOW);
		std::cout << std::endl << "128개의 벡터 값으로 변환 중입니다." << std::endl;
		celeb_face_128_vector = M_resnet(I_celeb_faces_chip);

		setColor(COLOR::GREEN);
		std::cout << "완료" << std::endl << std::endl;
		setColor();
	}

	if (UseOpenCVGUI) {
		//GUI를 띄우는거

		//웹캠의 가로 세로도 다르기 때문에 잘라주는 것도 해야함

		cv::namedWindow("Test"); //윈도우 이름을 지정하는 것 같은데 확실하게는 모르지만 setMouseCallback을 쓸려면 써야함
		cv::setMouseCallback("Test", GUICon::buttonsCheck); //마우스의 정보를 보내주는 함수, Test라는 윈도우에서 마우스가 움직이거나 어떤 반응을 하면 buttonsCheck함수를 호출함

		cv::imshow("Test", frame);
		cv::waitKey();
	}

	system("pause");
}

void CPputImage(cv::Mat& I_image, cv::Mat& O_image, cv::Rect pos) {
	cv::resize(I_image, I_image, cv::Size(pos.width, pos.height));

	cv::Mat imageROI(O_image, pos);
	I_image.copyTo(imageROI);
}

void CPputImage(dlib::matrix<dlib::rgb_pixel> I_image, cv::Mat& O_image, cv::Rect pos) {
	cv::Mat image = dlib::toMat(I_image);
	
	cv::resize(image, image, cv::Size(pos.width, pos.height));

	cv::Mat imageROI(O_image, pos);
	image.copyTo(imageROI);
}

void CPputText(cv::Mat& O_image, cv::String text, cv::Point org, int ori, const char* fontName, int fontWeight, double fontScale, RGBScale textColor, RGBScale bkColor) {
	int fontSize = (int)(10 * fontScale);
	int width = O_image.cols;
	int height = fontSize * 3 / 2;

	HDC hdc = CreateCompatibleDC(NULL); //텍스트 이미지를 만들어두는 곳 같은거

	HBRUSH hBrush = CreateSolidBrush(bkColor.rgb); //채우는 방식인데 bkColor로 단색으로 채우는거

	//텍스트 이미지 크기 정하는거
	RECT rect;
	rect.left = rect.top = 0;
	rect.right = width;
	rect.bottom = height;

	//비트맵의 구조를 사전에 정의하는 것 크기나 색
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
	SelectObject(hdc, hbmp); //hdc에 적용? 하는 거

	FillRect(hdc, &rect, hBrush); //지정한 크기만큼 완전하게 채우는거 (다 채움)

	BITMAP bitmap;
	GetObject(hbmp, sizeof(BITMAP), &bitmap);

	//텍스트 이미지 만들 때 사용할 수 있는 폰트를 생성? 하는 그런거
	HFONT hFont = CreateFontA(
		fontSize,
		0,
		0,
		0,
		fontWeight,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET, //한국어나 일본어나 해주게 하는거 (아마)
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, //안티 에일리어싱을 켜주는거
		VARIABLE_PITCH | FF_ROMAN,
		fontName);
	SelectObject(hdc, hFont);
	SetTextColor(hdc, textColor.rgb);
	SetBkColor(hdc, bkColor.rgb);

	//계산을 위해 미리 텍스트의 사이즈 구하는거
	SIZE size;
	GetTextExtentPoint32A(hdc, text.c_str(), (int)text.length(), &size);

	TextOutA(hdc, 0, height / 3 * 1, text.c_str(), (int)text.length()); //이미지에 텍스트 적는거
	int posX = (ori == 0 ? org.x : (ori == 1 ? org.x - (size.cx / 2) : org.x - size.cx)); //기준 정하는거 0은 텍스트의 왼쪽 1은 텍스트의 중간 2는 텍스트의 오른쪽
	int posY = org.y - (size.cy / 2 + 5);

	//비트맵 사진을 OpenCV이미지에 삽입해주는거
	unsigned char* _tmp;
	unsigned char* _img;
	for (int y = 0; y < bitmap.bmHeight; y++) {
		if (posY + y >= 0 && posY + y < O_image.rows) {
			_img = O_image.data + (int)(3 * posX + (posY + y) * (((bitmap.bmBitsPixel / 8) * O_image.cols) & ~3));
			_tmp = (unsigned char*)(bitmap.bmBits) + (int)((bitmap.bmHeight - y - 1) * (((bitmap.bmBitsPixel / 8) * bitmap.bmWidth) & ~3));
			for (int x = 0; x < bitmap.bmWidth; x++) {
				if (x + posX >= O_image.cols)
					break;

				if (_tmp[0] != bkColor.b || _tmp[0] != bkColor.g || _tmp[0] != bkColor.r) { //텍스트 이미지의 배경 컬러는 없애기 위한 것, bgr 순서로 하는 이유는 Mat 이미지를 처음에 만들 때 BGR 순이여서
					_img[0] = (unsigned char)_tmp[0]; //B
					_img[1] = (unsigned char)_tmp[1]; //G
					_img[2] = (unsigned char)_tmp[2]; //R
				}
				_img += 3;
				_tmp += 3;
			}
		}
	}
}

void GUIInitImage(cv::Mat& O_image) {
	O_image = cv::Scalar(0x17, 0x17, 0x17); //배경 색을 약간 어두운 색으로 칠해주는 것
	cv::line(O_image, cv::Point(O_image.rows, 0), cv::Point(O_image.rows, O_image.rows), cv::Scalar(0x37, 0x00, 0xDA), 2); //카메라 보여질 부분과 정보 보여질 부분을 나눠주는 선
	cv::rectangle(O_image, cv::Rect(GUIHeight + 20, 180 + 20, ((GUIWidth - GUIHeight) / 2) - 40, ((GUIWidth - GUIHeight) / 2) - 40), cv::Scalar(0xED, 0xED, 0xED), 1 + 2, 4, 0); //프리뷰쪽 테투리 쳐주는거 1 + 2에서 2가 선 두께임


	CPputText(O_image, "Face Recognition", cv::Point(O_image.cols - ((O_image.cols - O_image.rows) / 2), 30), OriCenter, "맑은 고딕", FW_BOLD, 6, RGBScale(0xED, 0xED, 0xED), RGBScale(0x11, 0x11, 0x11));

	//랜드마크 버튼은 메뉴로 지정할 수 있도록 제작

	//버튼 들
	//라운드 버튼을 만들기가 어렵기 때문에 사진으로 대체하거나 각이 있는 사각형으로 해야할 듯
	
	//버튼이 자리할 수 있는 위치를 구한 후 그곳 안에서 여백을 생각하면서 버튼 위치를 지정해야함
	int buttonsMargin = 10; //총 버튼들의 왼쪽 오른쪽 끝 여백
	int buttonsPosXS = GUIHeight + buttonsMargin; //버튼들 위치 X 시작점
	int buttonsPosXE = GUIWidth - buttonsMargin; //버튼들 위치 X 끝점
	int buttonWidth = (buttonsPosXE - buttonsPosXS) / 3 - 5; //각 버튼이 차지할 가로 3은 버튼 갯수 5는 버튼 사이사이 간격
	
	//버튼 위치 지정 & RECT로 변환
	//GUICon::BR_capture = cv::Rect(buttonsPosXS,  ) //첫번째 버튼 위치 지정
	GUICon::Var::BR_capture = cv::Rect(CamWidth + 10, 0, 100, 100); //테스트 버튼
	GUICon::Var::BR_preview = cv::Rect(CamWidth + 10, 100 + 50, 100, 100); //테스트 버튼

	GUICon::cvRect2RECT(GUICon::Var::BR_capture, GUICon::Var::CBR_capture);
	GUICon::cvRect2RECT(GUICon::Var::BR_preview, GUICon::Var::CBR_preview);
	GUICon::cvRect2RECT(GUICon::Var::BR_export, GUICon::Var::CBR_export);

	//버튼 그리기
	cv::rectangle(O_image, GUICon::Var::BR_capture, cv::Scalar(0xFF, 0xFF, 0xFF)); //테스트 버튼
	cv::rectangle(O_image, GUICon::Var::BR_preview, cv::Scalar(0xFF, 0xFF, 0xFF)); //테스트 버튼


	//텍스트 들
}

bool modelisAlive(std::string& path) {
	setColor();
	std::cout << path;

	if (_access(path.c_str(), 0) != -1) {
		setColor(COLOR::GREEN);
		std::cout << " 모델을 찾았습니다." << std::endl;
		return 1;
	}
	else {
		setColor(COLOR::RED);
		std::cout << " 모델을 찾지 못했습니다." << std::endl;
		return 0;
	}
}

void useSearchModel(std::string& path, bool& alive_model) {
	char checkRight;

	setColor();
	std::cout << "모델의 위치를 변경하시겠습니까? \"Y\"입력";
	std::cin >> checkRight;

	if (checkRight == 'Y' || checkRight == 'y') {
		//모델 파일 위치를 재정의 하는거 만들기 (폴더 선택 창으로)
		//모델을 찾았으면 true로 바꾸고 취소를 눌렀으면 false로
		//path = "" 괄호 안에 재정의 된 경로 넣기
		alive_model = true;
	}
}

void applyModel(std::string& path, dlib::shape_predictor& model) {
	setColor();
	std::cout << path;
	setColor(COLOR::BLUE);
	std::cout << " 모델을 적용합니다." << std::endl;
	dlib::deserialize(path) >> model;
}

void applyModel(std::string& path, anet_type& model) {
	setColor();
	std::cout << path;
	setColor(COLOR::BLUE);
	std::cout << " 모델을 적용합니다." << std::endl;
	dlib::deserialize(path) >> model;
}

std::vector<std::filesystem::path> loadImagePath(std::string& folder) {
	std::vector<std::filesystem::path> path; //연예인 이미지의 경로를 여러개 저장하기 위해

	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(std::filesystem::current_path() / folder)) //폴더 경로에 있는 이미지들을 entry 변수에 하나씩 계속 넣어주며 반복함
		if (!entry.is_directory() && (entry.path().extension() == ".jpg" ||
			entry.path().extension() == ".jpeg" ||
			entry.path().extension() == ".png"))
			path.push_back(entry.path()); //폴더에 jpg, jpeg, png 확장자의 이미지가 있으면 path 벡터에 저장 

	return path;
}

std::string getFileName(std::filesystem::path& path) {
	return path.filename().string().substr(0, path.filename().string().find('.'));
}

void setColor(short textcolor, short background) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textcolor + background * 16);
}

void GUICon::buttonsCheck(int event, int x, int y, int flags, void* userData) {
	//std::cout << event << " " << x << " " << y << " " << flags << " " << userData << std::endl;

	/*
		event는 마우스로 어떤 짓을 한 것 클릭이나 스크롤 등등
		x, y는 마우스가 OpenCV윈도우 위에서 어디 위치에 있는지
		나머지는 안씀

		event
			1 : 왼쪽 마우스 눌름
			4 : 왼쪽 마우스 땜
			2 : 오른쪽 마우스 눌름
			5 : 오른쪽 마우스 땜
	*/

	//클릭한 상태에서 다른 곳에서 마우스를 땠을 때는 취소로 해서 인식이 안되게 해야하기 때문에 
	//특정 위치에서 1번하고 4번이 둘다 감지 됬을 때 그 버튼을 실행하게 해야함

	//mouse, pushButtonNum
	//event가 1이면 PtlnRect로 눌린 버튼이 있으면 그 버튼을 pushButtonNum에 저장하고 눌린 버튼이 없다면 pushButtonNum를 0으로
	//event가 4이면 pushButtonNum가 0이 아닐 때 pushButtonNum이 4번 눌렸을 때와 같으면 눌린 것으로 하고 같지 않거나 같으면 pushButtonNum를 0으로 
	
	if (event == 1) {
		//왼쪽 마우스 눌렀을 때

		GUICon::Var::mouse = POINT(x, y); //마우스 정보를 저장하는 것

		//PtlnRect는 마우스가 RECT의 위치 안에 있을 때 true를 내보냄
		
		//첫번째 버튼 안에 있을 때
		if (PtInRect(&GUICon::Var::CBR_capture, GUICon::Var::mouse)) //1번째 버튼의 위치에 마우스가 눌렸을 때
			GUICon::Var::pushButtonNum = 1;
		else if (PtInRect(&GUICon::Var::CBR_preview, GUICon::Var::mouse)) //2번째 버튼의 위치에 마우스가 눌렸을 때
			GUICon::Var::pushButtonNum = 2;
		else if (PtInRect(&GUICon::Var::CBR_export, GUICon::Var::mouse)) //3번째 버튼의 위치에 마우스가 눌렸을 때
			GUICon::Var::pushButtonNum = 3;
		else
			GUICon::Var::pushButtonNum = 0;
	}
	if (event == 4 && GUICon::Var::pushButtonNum != 0) { //왼쪽 마우스를 떼고 눌렸을 때 버튼이 있을 때
		//왼쪽 마우스 땠을 때

		GUICon::Var::mouse = POINT(x, y); //마우스 정보를 저장하는 것

		if (PtInRect(&GUICon::Var::CBR_capture, GUICon::Var::mouse)) { //1번 버튼에서 마우스가 때졌을 때
			if (GUICon::Var::pushButtonNum == 1) { //1번 버튼이 눌린게 확실해졌을 때
				std::cout << "1번 버튼 눌림" << std::endl;
			}
		}
		else if (PtInRect(&GUICon::Var::CBR_preview, GUICon::Var::mouse)) { //2번 버튼에서 마우스가 때졌을 때
			if (GUICon::Var::pushButtonNum == 2) { //2번 버튼이 눌린게 확실해졌을 때
				std::cout << "2번 버튼 눌림" << std::endl;
			}
		}
		else if (PtInRect(&GUICon::Var::CBR_export, GUICon::Var::mouse)) { //3번 버튼에서 마우스가 때졌을 때
			if (GUICon::Var::pushButtonNum == 3) { //3번 버튼이 눌린게 확실해졌을 때
				std::cout << "3번 버튼 눌림" << std::endl;
			}
		}
			
		GUICon::Var::pushButtonNum = 0; //눌리지 않은 것으로
	}

	//std::cout << GUICon::Var::pushButtonNum << std::endl; //테스트용
}

void GUICon::cvRect2RECT(cv::Rect& I_rect, RECT& O_rect) {
	//ChatGPT가 만들어줌 
	//CV::Rect를 Windef.h의 RECT로 변환시켜주는 명령어
	O_rect.left = I_rect.x;
	O_rect.top = I_rect.y;
	O_rect.right = I_rect.x + I_rect.width;
	O_rect.bottom = I_rect.y + I_rect.height;
}
