#include "stdafx.h"
#include "main.h"

#include <iostream>
#include <Windows.h>


int main(int argv, char* args[]) {
	//std::cout << argv << std::endl << args[0] << std::endl << args[1]; //파라미터 테스트

	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector(); //사진에서 얼굴 갯수 찾아주는거임
	dlib::shape_predictor M_sp, M_fastsp; //얼굴에 점을 찍어서 얼굴 부분을 확인하는거
	anet_type M_resnet; //얼굴 이미지를 넣으면 128개의 벡터 값으로 바꿔주는 모델

	std::vector<dlib::matrix<dlib::rgb_pixel>> I_celeb_faces;
	std::vector<dlib::matrix<dlib::rgb_pixel>> I_celeb_faces_chip;
	std::vector<std::string> S_celeb_names;

	std::vector<dlib::matrix<float, 0, 1>> celeb_face_128_vector;

	/*
		이름 규칙
		이미지 : I_변수
		프리뷰 이미지 : PI_변수
		비디오 : V_변수
		프리뷰 비디오 : PV_변수
		String : S_변수
		모델 : M_변수
		 
		해볼꺼
		1. 모델 파일을 파일 탐색기로 찾게 하는 기능 만드릭
		2. 메모리를 아끼기 위해 이미지 경로를 저장할 때 이름과 확장자만 가져와서 나중에 어디로 바꿀 때 기본 이미지 경로와 이름과 확장자를 결합해서 가져올 수 있게도 바꿔보기

		1. 카메라 확인
		2. 모델을 불러오기
		3. 연예인 이미지의 얼굴을 확인하고 벡터화
	*/

	if (CheckCam) {
		//카메라가 잘 있는지 확인, 만약 잘찍히지 않거나 없거나 발견되지 않으면 사진으로 하거나 프로그램을 끄거나 등등 하기
	}
	//모델 로드
	if (UseModelLoad) {
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
	if (TransCelebVector) {
		//연예인 이미지를 벡터화 하는 것

		std::cout << std::endl;

		//폴더에 있는 이미지 부터 확인
		dlib::image_window PI_showCelebImg; //연예인 얼굴을 직접 보기 위해
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
					PI_showCelebImg.set_image(I_face_chip);
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

		if (LoadImageView)
			PI_showCelebImg.close_window();

		setColor(COLOR::YELLOW);
		std::cout << std::endl << "128개의 벡터 값으로 변환 중입니다." << std::endl;
		celeb_face_128_vector = M_resnet(I_celeb_faces_chip);
		
		setColor(COLOR::GREEN);
		std::cout << "완료" << std::endl << std::endl;
		setColor();
	}
	system("pause");
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