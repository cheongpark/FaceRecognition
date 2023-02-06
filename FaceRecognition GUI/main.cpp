#include "stdafx.h"
#include "main.h"

#include <iostream>
#include <Windows.h>


int main(int argv, char* args[]) {
	//std::cout << argv << std::endl << args[0] << std::endl << args[1]; //�Ķ���� �׽�Ʈ

	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector(); //�������� �� ���� ã���ִ°���
	dlib::shape_predictor M_sp, M_fastsp; //�󱼿� ���� �� �� �κ��� Ȯ���ϴ°�
	anet_type M_resnet; //�� �̹����� ������ 128���� ���� ������ �ٲ��ִ� ��

	std::vector<dlib::matrix<dlib::rgb_pixel>> I_celeb_faces;
	std::vector<dlib::matrix<dlib::rgb_pixel>> I_celeb_faces_chip;
	std::vector<std::string> S_celeb_names;

	std::vector<dlib::matrix<float, 0, 1>> celeb_face_128_vector;

	/*
		�̸� ��Ģ
		�̹��� : I_����
		������ �̹��� : PI_����
		���� : V_����
		������ ���� : PV_����
		String : S_����
		�� : M_����
		 
		�غ���
		1. �� ������ ���� Ž����� ã�� �ϴ� ��� ���帯
		2. �޸𸮸� �Ƴ��� ���� �̹��� ��θ� ������ �� �̸��� Ȯ���ڸ� �����ͼ� ���߿� ���� �ٲ� �� �⺻ �̹��� ��ο� �̸��� Ȯ���ڸ� �����ؼ� ������ �� �ְԵ� �ٲ㺸��

		1. ī�޶� Ȯ��
		2. ���� �ҷ�����
		3. ������ �̹����� ���� Ȯ���ϰ� ����ȭ
	*/

	if (CheckCam) {
		//ī�޶� �� �ִ��� Ȯ��, ���� �������� �ʰų� ���ų� �߰ߵ��� ������ �������� �ϰų� ���α׷��� ���ų� ��� �ϱ�
	}
	//�� �ε�
	if (UseModelLoad) {
		bool alive_SpModel = false, alive_FastSpModel = false, alive_ResNetModel = false;

		//ResNet �𵨺��� �ҷ��� ���� �߿��ϱ� ������ ������ ����
		if (modelisAlive(ResNetPath)) alive_ResNetModel = true;
		else if (CheckModel) useSearchModel(ResNetPath, alive_ResNetModel);
		else {
			setColor();
			std::cout << ResNetPath;
			setColor(COLOR::RED);
			std::cout << " ���� �߰ߵ��� �ʾұ� ������ ���α׷��� �����մϴ�." << std::endl;
			exit(-1);
		}

		//���� ���� ���ų� ���� ���� ������ �۵� ���
		//���� ��� �ִ��� ������ Ȯ�� �� �Ǵ� �ϴ� �������
		if (modelisAlive(SpPath)) alive_SpModel = true;
		else if (CheckModel) useSearchModel(SpPath, alive_SpModel);
		if (modelisAlive(FastSpPath)) alive_FastSpModel = true;
		else if (CheckModel) useSearchModel(FastSpPath, alive_FastSpModel);

		std::cout << std::endl;

		//-----�Ǵ�-----
		//�ϳ��� ���� �� �� ��뷮 ������ �� �ϳ��� �����ϰ� �ؾ��ϴµ� �׷����� �ڵ带 ���� �����ϰ� �� �� �־ ���߿� ������ �� ������ �ϴ°ɷ�
		//���� �𵨸� ���� ��
		if (!alive_SpModel && !alive_FastSpModel) {
			setColor(COLOR::RED);
			std::cout << " ���� ��� �߰ߵ��� �ʾұ� ������ ���α׷��� �����մϴ�." << std::endl;
			exit(-1);
		}
		if (alive_SpModel && !alive_FastSpModel) {
			setColor();
			std::cout << SpPath;
			setColor(COLOR::BLUE);
			std::cout << " ���� Ȯ�εǾ����� ";
			setColor();
			std::cout << FastSpPath;
			setColor(COLOR::BLUE);
			std::cout << " ���� Ȯ�ε��� �ʾҽ��ϴ�." << std::endl;

			std::cout << "�׷��� ������ �ӵ��� ���� ������ ��� �����մϴ�." << std::endl;

			dlib::deserialize(SpPath) >> M_sp;
			dlib::deserialize(SpPath) >> M_fastsp;
		}
		if (!alive_SpModel && alive_FastSpModel) {
			setColor();
			std::cout << FastSpPath;
			setColor(COLOR::BLUE);
			std::cout << " ���� Ȯ�εǾ����� ";
			setColor();
			std::cout << SpPath;
			setColor(COLOR::BLUE);
			std::cout << " ���� Ȯ�ε��� �ʾҽ��ϴ�." << std::endl;

			std::cout << "�׷��� ������ �ӵ��� ���� ������ ��� �����մϴ�. �ٸ� ��Ȯ���� ������ �� �ֽ��ϴ�." << std::endl;

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
		//������ �̹����� ����ȭ �ϴ� ��

		std::cout << std::endl;

		//������ �ִ� �̹��� ���� Ȯ��
		dlib::image_window PI_showCelebImg; //������ ���� ���� ���� ����
		std::vector<std::filesystem::path> celeb_paths = loadImagePath(CelebrityImagePath);

		int celebCount = 0; //������ ������� �̹����� ���� �������� ���� �� �ֱ� ������ �װ� �ɷ����� ���� ��� ������ �ϴ°� �ƴ�

		for (std::filesystem::path celeb_path : celeb_paths) {
			dlib::matrix<dlib::rgb_pixel> I_celeb_img;
			
			dlib::load_image(I_celeb_img, celeb_path.string());
			
			if (detector(I_celeb_img).size() == 1) { //�������� ���� 1�� �����Ǹ�
				setColor(COLOR::GREEN);
				std::cout << celeb_path << " ������ �����Խ��ϴ�.";
				setColor(COLOR::WHITE);
				std::cout << " ���� �� �������� \"" << getFileName(celeb_path) << "\" �Դϴ�." << std::endl;

				auto celeb_face = detector(I_celeb_img); //���� ��ġ�� �����ϱ� ���� ������

				dlib::matrix<dlib::rgb_pixel> I_face_chip; //�������� �� �κи� ������ ������ ������ �ִ� ����
				dlib::extract_image_chip(I_celeb_img, dlib::get_face_chip_details(M_sp(I_celeb_img, celeb_face[0]), 150, 0.25), I_face_chip); //�� �κи� �������ִ� ��ɾ�

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
			else { //�������� ���� ������ �����ǰų� ���� ���� �ʾ��� ��
				setColor(COLOR::RED);
				std::cout << celeb_path << " ����� �������� " << detector(I_celeb_img).size() << "���� ���� �߰� �Ǿ����ϴ�. 1�� �ֵ��� �����ּ���." << std::endl;
				setColor(COLOR::GREEN);
			}
		}

		setColor(COLOR::AQUA);
		std::cout << celebCount << "���� �̹����� �����Խ��ϴ�." << std::endl;
		setColor();

		if (LoadImageView)
			PI_showCelebImg.close_window();

		setColor(COLOR::YELLOW);
		std::cout << std::endl << "128���� ���� ������ ��ȯ ���Դϴ�." << std::endl;
		celeb_face_128_vector = M_resnet(I_celeb_faces_chip);
		
		setColor(COLOR::GREEN);
		std::cout << "�Ϸ�" << std::endl << std::endl;
		setColor();
	}
	system("pause");
}

bool modelisAlive(std::string& path) {
	setColor();
	std::cout << path;

	if (_access(path.c_str(), 0) != -1) {
		setColor(COLOR::GREEN);
		std::cout << " ���� ã�ҽ��ϴ�." << std::endl;
		return 1;
	}
	else {
		setColor(COLOR::RED);
		std::cout << " ���� ã�� ���߽��ϴ�." << std::endl;
		return 0;
	}
}

void useSearchModel(std::string& path, bool& alive_model) {
	char checkRight;

	setColor();
	std::cout << "���� ��ġ�� �����Ͻðڽ��ϱ�? \"Y\"�Է�";
	std::cin >> checkRight;

	if (checkRight == 'Y' || checkRight == 'y') {
		//�� ���� ��ġ�� ������ �ϴ°� ����� (���� ���� â����)
		//���� ã������ true�� �ٲٰ� ��Ҹ� �������� false��
		//path = "" ��ȣ �ȿ� ������ �� ��� �ֱ�
		alive_model = true;
	}
}

void applyModel(std::string& path, dlib::shape_predictor& model) {
	setColor();
	std::cout << path;
	setColor(COLOR::BLUE);
	std::cout << " ���� �����մϴ�." << std::endl;
	dlib::deserialize(path) >> model;
}

void applyModel(std::string& path, anet_type& model) {
	setColor();
	std::cout << path;
	setColor(COLOR::BLUE);
	std::cout << " ���� �����մϴ�." << std::endl;
	dlib::deserialize(path) >> model;
}

std::vector<std::filesystem::path> loadImagePath(std::string& folder) {
	std::vector<std::filesystem::path> path; //������ �̹����� ��θ� ������ �����ϱ� ����

	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(std::filesystem::current_path() / folder)) //���� ��ο� �ִ� �̹������� entry ������ �ϳ��� ��� �־��ָ� �ݺ���
		if (!entry.is_directory() && (entry.path().extension() == ".jpg" ||
			entry.path().extension() == ".jpeg" ||
			entry.path().extension() == ".png"))
			path.push_back(entry.path()); //������ jpg, jpeg, png Ȯ������ �̹����� ������ path ���Ϳ� ���� 

	return path;
}

std::string getFileName(std::filesystem::path& path) {
	return path.filename().string().substr(0, path.filename().string().find('.'));
}

void setColor(short textcolor, short background) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textcolor + background * 16);
}