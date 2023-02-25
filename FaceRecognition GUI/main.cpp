#include "stdafx.h"
#include "main.h"

#include <iostream>
#include <Windows.h>

int main(int argv, char* args[]) {
	//std::cout << argv << std::endl << args[0] << std::endl << args[1]; //파라미터 테스트

	std::vector<dlib::matrix<dlib::rgb_pixel>> I_celeb_faces; //연예인 원본 이미지
	std::vector<dlib::matrix<dlib::rgb_pixel>> I_celeb_faces_chip; //연예인 얼굴 이미지
	std::vector<std::string> S_celeb_names; //연예인 이름

	std::vector<dlib::matrix<float, 0, 1>> celeb_faces_128_vector; //128-D 의 벡터 값들

	cv::Mat frame(GUIHeight, GUIWidth, CV_8UC3); //미리 정한 가로 세로 대로 frame 만드는거
	cv::Mat preSetImage(GUIHeight, GUIWidth, CV_8UC3); //고정 GUI를 저장하는 곳
	cv::Mat I_WebCamFrame; //웹캠에서 이미지 가져온거 저장하는거
	cv::Mat I_captureImg; //캡쳐된 이미지를 저장하는 곳
	cv::Mat I_exportImage(MakeImageHeight, MakeImageWidth, CV_8UC3, cv::Scalar(0xFF,0xFF,0xFF)); //내보낼 이미지를 저장하는 곳
	cv::Mat logo; //세명컴고 로고가 저장되어있는 변수

	bool useFaceLandMark = true; //메인 웹캠뷰 쪽에서 얼굴을 인식해서 빨간 상자를 띄어주는걸 할지 말지
	
	cv::VideoCapture WebCam;
	int CamCount = 0; //카메라 갯수 

	cv::namedWindow(WinName); //윈도우 이름을 지정하는 것 같은데 확실하게는 모르지만 setMouseCallback을 쓸려면 써야함

	/*
		구현된 중요 기술들

		한글이나 영어를 컴퓨터의 폰트로 OpenCV 이미지에 넣는 것 (제일 어려웠음 일주일 이상 걸림)
		모델들이 있는지 판단하고 가져오는 것
		모델이 감지가 안되는 경우 가져올지 다른걸 사용할지 정하는거 (가져오는 기능 아직 미구현)
		특정 경로에 있는 사진들을 모두 가져와 얼굴이 1개인 것을 확인하고 벡터로 저장하는거
		카메라들 감지 하는 것
		원본 이미지를 정사각형에 표시할 때 이미지를 정사각형에 맞춰 축소하고 빈 공간을 채우는 것 (어려웠음)
		마우스가 눌러진 상태에서 다른 곳에서 마우스를 떼면 감지가 안되는 것으로 (어려웠음)
		크기 조절 해도 각자 위치에 있을 수 있도록 GUI 배치
		카메라의 가로가 크다면 세로 크기에 맞춰서 정사각형으로 자르고 세로가 크면 그 반대로 웹캠을 잘라주는 기능
		카메라에서 얼굴을 찾아서 사각형으로 어느 위치에 있는지 표시해주는 기능

	*/

	/*
		프리뷰 쪽에는 실시간으로 웹캠의 얼굴을 출력하는건 안함 그쪽은 결과창이기 때문에		

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
		if (Model::modelisAlive(ResNetPath)) alive_ResNetModel = true;
		else if (CheckModel) Model::useSearchModel(ResNetPath, alive_ResNetModel);
		else {
			setColor();
			std::cout << ResNetPath;
			setColor(COLOR::RED);
			std::cout << " 모델이 발견되지 않았기 때문에 프로그램을 종료합니다." << std::endl;
			exit(-1);
		}

		//빠른 모델이 없거나 느린 모델이 없으면 작동 방식
		//모델이 모두 있는지 없는지 확인 후 판단 하는 방식으로
		if (Model::modelisAlive(SpPath)) alive_SpModel = true;
		else if (CheckModel) Model::useSearchModel(SpPath, alive_SpModel);
		if (Model::modelisAlive(FastSpPath)) alive_FastSpModel = true;
		else if (CheckModel) Model::useSearchModel(FastSpPath, alive_FastSpModel);

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

			dlib::deserialize(SpPath) >> Model::M_sp;
			dlib::deserialize(SpPath) >> Model::M_fastsp;
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
			setColor();

			dlib::deserialize(FastSpPath) >> Model::M_sp;
			dlib::deserialize(FastSpPath) >> Model::M_fastsp;
		}

		if (alive_SpModel && alive_FastSpModel) {
			Model::applyModel(SpPath, Model::M_sp);
			Model::applyModel(FastSpPath, Model::M_fastsp);
		}
		Model::applyModel(ResNetPath, Model::M_resnet);
	}

	//이미지를 만드는걸 테스트 하는 거 이걸 이제 함수로 만들어서 연결하면 됨
	if(true) {
		//로고 이미지가 있는지 확인하고 있으면 가져오는 것
		if (ImageisAlive(logoPath))
			logo = cv::imread(logoPath);
		
		//테스트를 위한 이미지들을 세팅
		cv::Mat I_myTestImage, I_celebTestImage;
		I_myTestImage = cv::imread("./CelebrityImg/강태현.jpeg");
		I_celebTestImage = cv::imread("./CelebrityImg/개리.jpg");

		dlib::cv_image<dlib::bgr_pixel> I_celebDlibImage(I_celebTestImage), I_camDlibImage(I_myTestImage);

		//이미지 제작
		dlib::matrix<dlib::bgr_pixel> celeb_face_chip, cam_face_chip; //얼굴쪽 사진만 저장하기 위한 변수
		dlib::extract_image_chip(I_celebDlibImage, dlib::get_face_chip_details(Model::M_sp(I_celebDlibImage, Model::detector(I_celebDlibImage)[0]), 150, 0.6).rect, celeb_face_chip); //얼굴쪽 부분 사진만 따는 함수
		dlib::extract_image_chip(I_camDlibImage, dlib::get_face_chip_details(Model::M_sp(I_camDlibImage, Model::detector(I_camDlibImage)[0]), 150, 0.6).rect, cam_face_chip); //얼굴쪽 부분 사진만 따는 함수

		cv::Mat cv_celeb_face_chip = dlib::toMat(celeb_face_chip); //openCV의 Mat로 변경하는 거
		cv::Mat cv_cam_face_chip = dlib::toMat(cam_face_chip); //openCV의 Mat로 변경하는 거

		CPputImage(logo, I_exportImage, cv::Rect(0, 0, 535, 147)); //로고 집어넣는거
		CPputImage(cv_celeb_face_chip, I_exportImage, cv::Rect(MakeImageWidth / 2, MakeImageStartImageY, MakeImageWidth / 2, MakeImageWidth / 2)); //출력할 이미지에서 크게 오른쪽에 이미지 넣은거
		CPputImage(cv_cam_face_chip, I_exportImage, cv::Rect(0, MakeImageStartImageY, MakeImageWidth / 2, MakeImageWidth / 2)); //왼쪽꺼

		if (MakeImageUseOverlay) {
			//얼굴 이미지에서 작게 어느 위치에 얼굴이 있는지 표시하는거
			//어차피 위에서 이미지를 이미 삽입했으므로
			//해당 이미지에 사각형을 치고 바로 삽입할 위치에 삽입하는 방식으로

			drawFaceRectangle(cv_celeb_face_chip, cv_celeb_face_chip, Model::detector(celeb_face_chip)[0], MakeImageOverlayColor, true); //사각형 씌우는거
			drawFaceRectangle(cv_cam_face_chip, cv_cam_face_chip, Model::detector(cam_face_chip)[0], MakeImageOverlayColor, true); //사각형 씌우는거
		
			CPputImage(cv_celeb_face_chip, I_exportImage, cv::Rect(MakeImageWidth - MakeImageOverlaySize, MakeImageStartImageY + (MakeImageWidth / 2) - MakeImageOverlaySize, MakeImageOverlaySize, MakeImageOverlaySize)); //아래 작은 이미지에 배치하는거
			CPputImage(cv_cam_face_chip, I_exportImage, cv::Rect(0, MakeImageStartImageY + (MakeImageWidth / 2) - MakeImageOverlaySize, MakeImageOverlaySize, MakeImageOverlaySize)); //아래 작은 이미지에 배치하는거

			cv::rectangle(I_exportImage, cv::Rect(0, MakeImageStartImageY + (MakeImageWidth / 2) - MakeImageOverlaySize, MakeImageOverlaySize, MakeImageOverlaySize), cv::Scalar(0, 0, 0), 3, 4, 0); //구분선 작은거
			cv::rectangle(I_exportImage, cv::Rect(MakeImageWidth - MakeImageOverlaySize, MakeImageStartImageY + (MakeImageWidth / 2) - MakeImageOverlaySize, MakeImageOverlaySize, MakeImageOverlaySize), cv::Scalar(0, 0, 0), 3, 4, 0); //구분선 작은거
		}
		cv::rectangle(I_exportImage, cv::Rect(MakeImageWidth / 2, MakeImageStartImageY, MakeImageWidth / 2, MakeImageWidth / 2), cv::Scalar(0, 0, 0), 3, 4, 0); //구분선 큰거
		cv::rectangle(I_exportImage, cv::Rect(0, MakeImageStartImageY, MakeImageWidth / 2, MakeImageWidth / 2), cv::Scalar(0, 0, 0), 3, 4, 0); //구분선 큰거

		//현재 시간 측정
		std::chrono::system_clock::time_point nowCaptureDate = std::chrono::system_clock::now(); //현재 시간 구하기
		std::time_t nowCaptureDate_c = std::chrono::system_clock::to_time_t(nowCaptureDate);
		std::tm nowCaptureDate_tm = {};
		localtime_s(&nowCaptureDate_tm, &nowCaptureDate_c); //시간으로 변경하는거

		char nowCaptureDateStr[30];
		sprintf_s(nowCaptureDateStr, "%04d-%02d-%02d %s %02d:%02d:%02d",
			nowCaptureDate_tm.tm_year + 1900, nowCaptureDate_tm.tm_mon + 1, nowCaptureDate_tm.tm_mday,
			nowCaptureDate_tm.tm_hour < 12 ? "오전" : "오후",
			nowCaptureDate_tm.tm_hour % 12 == 0 ? 12 : nowCaptureDate_tm.tm_hour % 12,
			nowCaptureDate_tm.tm_min, nowCaptureDate_tm.tm_sec); //시간을 년-월-일 오전|오후 시:분:초 로 나타나게 하는 거 ChatGPT가 짜줌

		char celebCompare[20];
		sprintf_s(celebCompare, "%.1f%%", 87.5/*(1 - faceDistance.distance) * 100*/);
		CPputText(I_exportImage, celebCompare, cvPoint(MakeImageWidth, 95), 2, "맑은 고딕", FW_BOLD, 6, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //한글로 되게 바꿔야함 (완료)
		CPputText(I_exportImage, nowCaptureDateStr, cvPoint(MakeImageWidth, 0), 2, "맑은 고딕", FW_DEMIBOLD, 4, RGBScale(255, 0, 0), RGBScale(255, 255, 255));

		CPputText(I_exportImage, "나", cvPoint(MakeImageWidth / 4, 745), 1, "맑은 고딕", FW_BOLD, 6, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //CanvasWidth / 4 첫번째 사진의 중간
		CPputText(I_exportImage, "개리", cvPoint(MakeImageWidth / 4 * 3, 745), 1, "맑은 고딕", FW_BOLD, 6, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //CanvasWidth / 4 * 3 는 2번째 사진의 중간을 가르키기 위해

		cv::imshow("TestExportImg", I_exportImage);
		cv::waitKey();
	}

	if (UsePreSetImage) {
		//사전에 GUI를 미리 이미지로 만들어서 함 이걸로 이미지 가져올 때 미리 보여주는 식으로
		//속도를 위해 고정 위치인 UI는 다른 곳에 미리 저장을 하고 업데이트 해야할 때 마다 저장한걸 끼워넣기
		GUIInitImage(preSetImage);

		//웹캠을 출력할 곳 색으로 표시
		cv::Mat webcam(CamHeight, CamWidth, CV_8UC3);
		webcam = cv::Scalar(0x88, 0xC9, 0x03); //색 지정
		GUICon::putWebcamView(webcam, preSetImage); //그 색으로 웹캠뷰 채우는거

		//프리뷰 이미지 출력할 곳 색으로 표시
		cv::Mat preImage((GUIWidth - GUIHeight) / 2 - 40, (GUIWidth - GUIHeight) / 2 - 40, CV_8UC3); //가로 전체에서 메인 카메라 표시 될 부분을 뺀거의 반에서 사이즈 조정 때문에 양옆 위아래 20씩 간격을 만들기 위해 40을 뺌
		preImage = cv::Scalar(0x00, 0x00, 0x00); //색 지정
		GUICon::putPreImageView(preImage, preSetImage); //그 색으로 프리뷰 채우는거
		
		preSetImage.copyTo(frame); //메인 frame 처리 하는 곳으로 넘겨주는거
		//frame = preSetImage; //메인 frame 처리 하는 곳으로 넘겨주는거 <-- 이건 참조하는 그런 것 때문에 나중에 frame을 바꿔버리면 preSetImage도 같이 바뀜 그래서 못씀

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
			
			if (Model::detector(I_celeb_img).size() == 1) { //사진에서 얼굴이 1개 감지되면
				setColor(COLOR::GREEN);
				std::cout << celeb_path << " 사진을 가져왔습니다.";
				setColor(COLOR::WHITE);
				std::cout << " 설정 된 연예인은 \"" << getFileName(celeb_path) << "\" 입니다." << std::endl;

				auto celeb_face = Model::detector(I_celeb_img); //얼굴의 위치를 추출하기 위한 변수임

				dlib::matrix<dlib::rgb_pixel> I_face_chip; //사진에서 얼굴 부분만 추출한 사진을 가지고 있는 변수
				dlib::extract_image_chip(I_celeb_img, dlib::get_face_chip_details(Model::M_sp(I_celeb_img, celeb_face[0]), 150, 0.25), I_face_chip); //얼굴 부분만 추출해주는 명령어

				if (LoadImageView) {
					//PI_showCelebImg.set_image(I_face_chip);
					if (ShowMainView) {
						//이미지와 같은 크기의 정사각형 배경을 만들고 색은 그 이미지의 평균색
						//그리고 그 배경을 웹캠 뷰에 맞게 리사이즈 시켜서 넣는다(그냥 함수로 바로 넣으면 됨)
						//만약 정사각형이면 이 과정을 패스

						if (I_celeb_img.nc() == I_celeb_img.nr())
							GUICon::putWebcamView(I_celeb_img, frame);
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
					cv::imshow(WinName, frame);
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
				std::cout << celeb_path << " 경로의 사진에서 " << Model::detector(I_celeb_img).size() << "명의 얼굴이 발견 되었습니다. 1명만 있도록 맞춰주세요." << std::endl;
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
		celeb_faces_128_vector = Model::M_resnet(I_celeb_faces_chip);

		setColor(COLOR::GREEN);
		std::cout << "완료" << std::endl << std::endl;
		setColor();
		
		preSetImage.copyTo(frame); //메인 frame 처리 하는 곳으로 넘겨주는거
	}
	
	if (UseOpenCVGUI) {
		//GUI를 띄우는거
		
		//웹캠의 가로 세로도 다르기 때문에 잘라주는 것도 해야함

		cv::setMouseCallback(WinName, GUICon::buttonsCheck); //마우스의 정보를 보내주는 함수, Test라는 윈도우에서 마우스가 움직이거나 어떤 반응을 하면 buttonsCheck함수를 호출함

		int camera_track_val = 0; //카메라 설정 바의 현재 값 (기본 0)
		int pre_camera_track_val = -1; //카메라 설정 바의 바뀌기 이전 값 (-1은 값 범위에서 벗어나게 해야지 되서)
		cv::createTrackbar("Camera", WinName, &camera_track_val, CamCount - 1); //카메라 설정 바 만드는거

		while (true) {
			if (UseGetCalTime) {
				end_time = std::chrono::system_clock::now();
				get_cal_time_mill = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
				start_time = std::chrono::system_clock::now();
				
				std::cout << get_cal_time_mill.count() << std::endl;
			}

			preSetImage.copyTo(frame); //미리 준비된 이미지를 frame에 넣는거 frame은 표시할꺼 속도를 빠르게 하기위해서 미리 만든 걸 넣는거
			
			//카메라를 읽을 수 있는지 없는지, 만약 읽을 수 없으면 카메라를 변경함
			if (pre_camera_track_val != camera_track_val) { //카메라 트랙 바의 값이 바뀌면
				WebCam.open(camera_track_val); //웹캠을 트랙바에서 선택한 대로 열기
				pre_camera_track_val = camera_track_val; //이전 값을 현재 값으로 맞추기

				setColor(COLOR::GREEN);
				std::cout << camera_track_val << " 카메라로 변경했습니다." << std::endl;
				setColor();
			}
			
			if (WebCam.read(I_WebCamFrame)) { //웹캠에서 이미지를 읽을 수 있을 때
				isUseCapture = true; //캡쳐가 되고 있음

				//웹캠의 가로 세로 중 더 긴 것을 기준으로 정해서 사진을 잘라 정사각형으로 만들어 주는 것
				whCompareSquareCut(I_WebCamFrame);

				//웹캠에서 얼굴을 찾아서 사각형으로 표시해주기
				dlib::cv_image<dlib::bgr_pixel> I_calRecImg(I_WebCamFrame); //얼굴을 찾기 위해 Dlib의 이미지로 변경해주는거 매트릭스 형태임 Calculate Rectangle Image임
				auto faces = Model::detector(I_calRecImg); //매트릭스 이미지에서 얼굴들을 찾아서 faces로 저장하는 것
				int facesCount = faces.size(); //얼굴 개수를 저따가 넣는거

				//얼굴 횟수 텍스트 표시
				char S_facesCount[30]; //얼굴 몇개 있는지 텍스트 넣을 곳
				sprintf_s(S_facesCount, "얼굴 %d명 감지 되었습니다.", facesCount);
				CPputText(frame, S_facesCount, cv::Point(frame.cols - ((frame.cols - frame.rows) / 2), 110), OriCenter, "맑은 고딕", FW_BOLD, 3, (facesCount != 1 ? RGBScale(0xDA, 0x00, 0x37) : RGBScale(0xED, 0xED, 0xED)), BackgroundColor); //frame에 얼굴이 몇개 있는지 텍스트로 띄어주는 거 중간에 조건문은 1명이 아니면 빨간색으로 글자 띄우는거

				if (facesCount == 1)
					drawFaceRectangle(I_calRecImg, I_WebCamFrame, faces[0], FaceLandMarkColor);

				//메인 웹캠 뷰 쪽에 웹캠 사진 넣는거
				GUICon::putWebcamView(I_WebCamFrame, frame);

				//캡쳐 버튼이 눌리면 사진을 캡쳐하고 얼굴을 비교함
				if (useCapture) {
					capture_start_now = std::chrono::system_clock::now(); //현재의 시간을 저장
					get_capture_start_mill = std::chrono::duration_cast<std::chrono::milliseconds>(capture_start_now - capture_start); //현재 시간에서 캡쳐 시작 시간을 빼서 캡쳐 후의 시간을 측정해서 밀리세컨드로 저장

					if (get_capture_start_mill.count() >= 3000) { //캡쳐 후의 시간이 3초가 지나면 
						//캡쳐 버튼 눌르고 3초가 지났을 때
						if (facesCount == 1) { //웹캠에 얼굴이 1개만 있을 때
							std::cout << "캡쳐함" << std::endl;
							//GUICon::putWebcamViewText(frame, "CAPTURE!", RGBScale(0xDA, 0x00, 0x37));

							//현재 이미지 캡쳐
							WebCam.read(I_captureImg);
							whCompareSquareCut(I_captureImg);

							//cv::imshow("Test2", captureImg); //테스트용 이미지 표시

							//얼굴들을 계산해주는 곳
							setColor(COLOR::YELLOW);
							std::cout << "계산중" << std::endl;
							setColor();
							GUICon::putWebcamViewText(frame, "Cal..", RGBScale(0xDA, 0x00, 0x37));

							//계산을 따로 해주는 함수를 제작하고 이미지로 만드는걸 제작해서
							//계산을 바로 하고 이미지를 만들고 변수에 저장하고 있다가 
							//PREVIEW 버튼을 눌르면 미리 보여주는 창을 띄우고 EXPORT눌르면 내보내기가 되게

							std::vector<FaceDistance>faces_distance = compareFacesImage(I_captureImg, celeb_faces_128_vector); //이미지와 연예인 벡터 값을 넣어서 거리를 비교해서 정렬된 값을 가져와 저장하는 것

							//GUI에 순위를 표시해주는 것
							//예시들임
							std::cout << S_celeb_names[faces_distance[0].faceNum] << std::endl;
							std::cout << S_celeb_names[faces_distance[1].faceNum] << std::endl;
							std::cout << S_celeb_names[faces_distance[2].faceNum] << std::endl;
							std::cout << S_celeb_names[faces_distance[3].faceNum] << std::endl;

							useCapture = false; //모든 작업이 끝났기 때문에 캡쳐를 종료
						}
						else {
							setColor(COLOR::RED);
							std::cout << "웹캠에 얼굴이 감지되지 않아 캡쳐를 종료합니다." << std::endl;
							setColor();

							useCapture = false; //캡쳐할 때 얼굴이 감지 되지 않아서 캡쳐를 종료
						}
					}
					else { //3초가 지나지 않아서 웹캠 뷰로 3, 2, 1를 띄워주는 곳
						//3, 2, 1를 텍스트로 초마다 표시하게 하는 곳
						//std::cout << 3 - (get_capture_start_mill.count() / 1000) << std::endl; 
						cv::String capture_timer(std::to_string(3 - (get_capture_start_mill.count() / 1000))); //3초 타이머 구현, 3 2 1를 출력함
						GUICon::putWebcamViewText(frame, capture_timer);
					}
				}

				//테스트임
				//GUICon::putWebcamViewText(frame, "안녕");
			}
			else { //웹캠에서 이미지를 읽을 수 없을 때
				isUseCapture = false; //캡쳐가 되지 않고 있음
				useCapture = false; //이거 안하면 캡쳐 되고 있을 때 이미지를 읽을 수 없다가 다시 이미지를 읽으면 갑자기 캡쳐함

				setColor(COLOR::RED);
				std::cout << "웹캠에서 이미지를 읽을 수 없습니다." << std::endl;
				setColor();
			}

			cv::imshow(WinName, frame);
			
			char key = cv::waitKey(30);
			if (key == 27) {
				break;
			}
		}
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

	//메모리에서 삭제해주는거 이거 안하면 메모리 계속 사용함
	DeleteObject(hBrush);
	DeleteObject(hFont);
	DeleteObject(hbmp);
	DeleteObject(hdc);
}

void GUIInitImage(cv::Mat& O_image) {
	O_image = cv::Scalar(BackgroundColor.b, BackgroundColor.g, BackgroundColor.r); //배경 색을 약간 어두운 색으로 칠해주는 것
	cv::line(O_image, cv::Point(O_image.rows, 0), cv::Point(O_image.rows, O_image.rows), cv::Scalar(0x37, 0x00, 0xDA), 2); //카메라 보여질 부분과 정보 보여질 부분을 나눠주는 선
	cv::rectangle(O_image, cv::Rect(GUIHeight + 20, 180 + 20, ((GUIWidth - GUIHeight) / 2) - 40, ((GUIWidth - GUIHeight) / 2) - 40), cv::Scalar(0xED, 0xED, 0xED), 1 + 2, 4, 0); //프리뷰쪽 테투리 쳐주는거 1 + 2에서 2가 선 두께임


	CPputText(O_image, "Face Recognition", cv::Point(O_image.cols - ((O_image.cols - O_image.rows) / 2), 30), OriCenter, "맑은 고딕", FW_BOLD, 6, RGBScale(0xED, 0xED, 0xED), BackgroundColor);

	//랜드마크 버튼은 메뉴로 지정할 수 있도록 제작

	//버튼 들
	//라운드 버튼을 만들기가 어렵기 때문에 사진으로 대체하거나 각이 있는 사각형으로 해야할 듯
	
	//버튼이 자리할 수 있는 위치를 구한 후 그곳 안에서 여백을 생각하면서 버튼 위치를 지정해야함
	int buttonsMargin = 10; //총 버튼들의 왼쪽 오른쪽 아래 끝 여백
	int buttonsPosXS = GUIHeight + buttonsMargin; //버튼들 위치 X 시작점
	int buttonsPosXE = GUIWidth - buttonsMargin; //버튼들 위치 X 끝점
	
	int buttonWidth = (buttonsPosXE - buttonsPosXS) / 3 - 5; //각 버튼이 차지할 가로 3은 버튼 갯수 5는 버튼 사이사이 간격
	int buttonHeight = 50;
	int buttonY = GUIHeight - buttonHeight - buttonsMargin;

	//버튼 위치 지정 & RECT로 변환
	//GUICon::BR_capture = cv::Rect(buttonsPosXS,  ) //첫번째 버튼 위치 지정
	GUICon::Var::BR_capture = cv::Rect(CamWidth + 10, 0, 100, 100); //테스트 버튼
	GUICon::Var::BR_preview = cv::Rect(CamWidth + 10, 100 + 50, 100, 100); //테스트 버튼

	GUICon::Var::BR_capture = cv::Rect(buttonsPosXS, buttonY, buttonWidth, buttonHeight);
	GUICon::Var::BR_preview = cv::Rect(buttonsPosXS + (5 + 5) + buttonWidth, buttonY, buttonWidth, buttonHeight);
	GUICon::Var::BR_export = cv::Rect(buttonsPosXS + (5 + 5) + buttonWidth + (5 + 5) + buttonWidth, buttonY, buttonWidth, buttonHeight);

	GUICon::cvRect2RECT(GUICon::Var::BR_capture, GUICon::Var::CBR_capture);
	GUICon::cvRect2RECT(GUICon::Var::BR_preview, GUICon::Var::CBR_preview);
	GUICon::cvRect2RECT(GUICon::Var::BR_export, GUICon::Var::CBR_export);

	//버튼 그리기
	cv::rectangle(O_image, GUICon::Var::BR_capture, cv::Scalar(0xFF, 0xFF, 0xFF)); //테스트 버튼
	cv::rectangle(O_image, GUICon::Var::BR_preview, cv::Scalar(0xFF, 0xFF, 0xFF)); //테스트 버튼
	cv::rectangle(O_image, GUICon::Var::BR_export, cv::Scalar(0xFF, 0xFF, 0xFF));

	//버튼 텍스트 들
	CPputText(O_image, "CAPTURE", cv::Point(GUICon::Var::BR_capture.x + (GUICon::Var::BR_capture.width / 2), GUICon::Var::BR_capture.y + 13), 1, "맑은 고딕", FW_BOLD, 3, RGBScale(0xED, 0xED, 0xED), BackgroundColor);
	CPputText(O_image, "PREVIEW", cv::Point(GUICon::Var::BR_preview.x + (GUICon::Var::BR_preview.width / 2), GUICon::Var::BR_preview.y + 13), 1, "맑은 고딕", FW_BOLD, 3, RGBScale(0xED, 0xED, 0xED), BackgroundColor);
	CPputText(O_image, "EXPORT", cv::Point(GUICon::Var::BR_export.x + (GUICon::Var::BR_export.width / 2), GUICon::Var::BR_export.y + 13), 1, "맑은 고딕", FW_BOLD, 3, RGBScale(0xED, 0xED, 0xED), BackgroundColor);
}

bool Model::modelisAlive(std::string& path) {
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

void Model::useSearchModel(std::string& path, bool& alive_model) {
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

void Model::applyModel(std::string& path, dlib::shape_predictor& model) {
	setColor();
	std::cout << path;
	setColor(COLOR::BLUE);
	std::cout << " 모델을 적용합니다." << std::endl;
	dlib::deserialize(path) >> model;
}

void Model::applyModel(std::string& path, anet_type& model) {
	setColor();
	std::cout << path;
	setColor(COLOR::BLUE);
	std::cout << " 모델을 적용합니다." << std::endl;
	dlib::deserialize(path) >> model;
}

void drawFaceRectangle(dlib::cv_image<dlib::bgr_pixel> I_image, cv::Mat& O_image, dlib::rectangle face, cv::Scalar color, bool useFast) {
	dlib::chip_details faceChip; //얼굴에 위치에 대한 값이 저장되있는 변수

	if (useFast) //5개의 점을 찍어서 찾는 것 (빠름, 정확도 낮음)
		faceChip = dlib::get_face_chip_details(Model::M_fastsp(I_image, face), 150, 0.2); //찾은 얼굴의 위치를 저장하는 것 0.2는 사각형과 얼굴 내부 간격 같은거임
	else //68개의 점을 찍어서 찾는 것 (느림, 정확도 높음)
		faceChip = dlib::get_face_chip_details(Model::M_sp(I_image, face), 150, 0.2); //찾은 얼굴의 위치를 저장하는 것 0.2는 사각형과 얼굴 내부 간격 같은거임

	cv::rectangle(O_image, cv::Rect(faceChip.rect.left(), faceChip.rect.top(), faceChip.rect.width(), faceChip.rect.height()), color, 2, 4, 0); //내가 지정한 색으로 얼굴에 아까 구한 Rect 값으로 사각형을 그리는거임
}

void drawFaceRectangle(cv::Mat& I_image, cv::Mat& O_image, dlib::rectangle face, cv::Scalar color, bool useFast) {
	dlib::chip_details faceChip; //얼굴에 위치에 대한 값이 저장되있는 변수
	dlib::cv_image<dlib::bgr_pixel> faceImage(I_image);

	if (useFast) //5개의 점을 찍어서 찾는 것 (빠름, 정확도 낮음)
		faceChip = dlib::get_face_chip_details(Model::M_fastsp(faceImage, face), 150, 0.2); //찾은 얼굴의 위치를 저장하는 것 0.2는 사각형과 얼굴 내부 간격 같은거임
	else //68개의 점을 찍어서 찾는 것 (느림, 정확도 높음)
		faceChip = dlib::get_face_chip_details(Model::M_sp(faceImage, face), 150, 0.2); //찾은 얼굴의 위치를 저장하는 것 0.2는 사각형과 얼굴 내부 간격 같은거임

	cv::rectangle(O_image, cv::Rect(faceChip.rect.left(), faceChip.rect.top(), faceChip.rect.width(), faceChip.rect.height()), color, 2, 4, 0); //내가 지정한 색으로 얼굴에 아까 구한 Rect 값으로 사각형을 그리는거임
}

void whCompareSquareCut(cv::Mat& I_image) {
	if (I_image.cols >= I_image.rows) //이미지가 정사각형이거나 가로가 더 길때
		I_image = I_image(cv::Range(0, I_image.rows), cv::Range((I_image.cols - I_image.rows) / 2, (I_image.cols - I_image.rows) / 2 + I_image.rows));
	else //세로가 더 길때
		I_image = I_image(cv::Range((I_image.rows - I_image.cols) / 2, (I_image.rows - I_image.cols) / 2 + I_image.cols), cv::Range(0, I_image.cols));
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

std::vector<FaceDistance> compareFacesImage(cv::Mat& faceImg, std::vector<dlib::matrix<float, 0, 1>> faces_vector) {
	dlib::cv_image<dlib::bgr_pixel> I_cam_face(faceImg); //dlib로 비교하기 때문에 openCV에서 캡쳐한 이미지를 dlib 이미지로 변경
	std::vector<dlib::rectangle> compareFace = Model::detector(I_cam_face); //추출한 이미지에서 얼굴을 찾아서 얼굴 위치 저장

	dlib::matrix<dlib::rgb_pixel> I_cam_faces_chip; //얼굴 부분만 추출한 이미지를 저장하는 변수
	dlib::extract_image_chip(I_cam_face, dlib::get_face_chip_details(Model::M_sp(I_cam_face, compareFace[0]), 150, 0.25), I_cam_faces_chip); //이미지에서 얼굴 부분만 추출해서 저장 하는 함수 느려도 되는 작업이라 정교 모델로

	dlib::matrix<float, 0, 1> cam_face_128_vector = Model::M_resnet(I_cam_faces_chip); //얼굴의 이미지를 벡터로 변환

	std::vector<FaceDistance> faces_distance; //연예인과의 거리 순위들을 체크하기 위해 벡터를 사용
	for (int i = 0; i < faces_vector.size(); i++)
		faces_distance.push_back({ i, dlib::length(cam_face_128_vector - faces_vector[i]) }); //i는 몇번의 얼굴인지 다음껀 연예인과 캠 얼굴의 거리를 저장

	std::sort(faces_distance.begin(), faces_distance.end(), face_cmp); //비교된 얼굴들의 거리를 오름차순으로 정렬 거리가 가까울 수록 닮았기 때문에
	
	return faces_distance;
}

bool face_cmp(FaceDistance a, FaceDistance b) {
	return a.faceDistance < b.faceDistance;
}

void makeCompareImage(cv::Mat& I_camImage, cv::Mat& I_celebImage, FaceDistance& compareDistance, cv::Mat& logo) {

}

bool ImageisAlive(std::string& path) {
	setColor();
	std::cout << path;

	if (_access(path.c_str(), 0) != -1) {
		setColor(COLOR::GREEN);
		std::cout << " 이미지를 찾았습니다." << std::endl;
		return 1;
	}
	else {
		setColor(COLOR::RED);
		std::cout << " 이미지를 찾지 못했습니다." << std::endl;
		return 0;
	}
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
				if (isUseCapture) {
					std::cout << "캡쳐 버튼 눌림" << std::endl;
					useCapture = true;
					capture_start = std::chrono::system_clock::now();
				}
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

void GUICon::putWebcamViewText(cv::Mat& O_image, cv::String text, RGBScale color) {
	CPputText(O_image, text, cv::Point(CamWidth / 2, CamHeight / 2 - 70), OriCenter, "맑은 고딕", FW_BOLD, 15, color, RGBScale(0x00, 0x00, 0x00));
}