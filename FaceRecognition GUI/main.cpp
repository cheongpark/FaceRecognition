#include "stdafx.h"
#include "main.h"

#include <iostream>
#include <Windows.h>


int main(int argv, char* args[]) {
	//std::cout << argv << std::endl << args[0] << std::endl << args[1]; //�Ķ���� �׽�Ʈ

	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector(); //�������� �� ���� ã���ִ°���
	dlib::shape_predictor M_sp, M_fastsp; //�󱼿� ���� �� �� �κ��� Ȯ���ϴ°�
	anet_type M_resnet; //�� �̹����� ������ 128���� ���� ������ �ٲ��ִ� ��

	std::vector<dlib::matrix<dlib::rgb_pixel>> I_celeb_faces; //������ ���� �̹���
	std::vector<dlib::matrix<dlib::rgb_pixel>> I_celeb_faces_chip; //������ �� �̹���
	std::vector<std::string> S_celeb_names; //������ �̸�

	std::vector<dlib::matrix<float, 0, 1>> celeb_face_128_vector; //128-D �� ���� ����

	cv::Mat frame(GUIHeight, GUIWidth, CV_8UC3); //�̸� ���� ���� ���� ��� frame ����°�
	cv::Mat preSetImage(GUIHeight, GUIWidth, CV_8UC3); //���� GUI�� �����ϴ� ��

	/*
		cols ����
		rows ����
		
		Rect �μ��� (x, y, width, height)�� �� ��� ���������� ���� ���� ���̸� �Է��ϸ� ��.

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
		3. ī�޶� �̸� Ȯ���ϴ°Ŵ� DirectShow �� ����ؾ� �ϰ� ������ �ٸ� �� �ֱ� ������ ���߿� ����

		1. ī�޶� Ȯ��
		2. ���� �ҷ�����
		3. ������ �̹����� ���� Ȯ���ϰ� ����ȭ
	*/

	if (CheckCam) {
		//ī�޶� �� �ִ��� Ȯ��, ���� �������� �ʰų� ���ų� �߰ߵ��� ������ �������� �ϰų� ���α׷��� ���ų� ��� �ϱ�

		//ī�޶� ���� Ȯ���ϱ�
		cv::VideoCapture cap;
		int CamCount = 0; //ī�޶� ���� 

		setColor(COLOR::GREEN);
		while (true) {
			cap.open(CamCount);
			if (cap.isOpened()) std::cout << CamCount++ << " ���� ī�޶� ���� �Ǿ����ϴ�." << std::endl;
			else break;
		}
		setColor();
		std::cout << "ī�޶� " << CamCount << "�� �߰ߵǾ����ϴ�." << std::endl;

		//�̷��� �ؼ� ī�޶� ��ȣ�� Ȯ���ϰ� OpenCV �޴����� ī�޶� ��ȣ ������ ���� �ϴ� ��
	}
	if (UseModelLoad) {
		//�� �ε�
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
	if (PreSetImage) {
		//������ GUI�� �̸� �̹����� ���� �� �̰ɷ� �̹��� ������ �� �̸� �����ִ� ������
		//�ӵ��� ���� ���� ��ġ�� UI�� �ٸ� ���� �̸� ������ �ϰ� ������Ʈ �ؾ��� �� ���� �����Ѱ� �����ֱ�
		GUIInitImage(preSetImage);

		//��ķ�� ����� ��
		cv::Mat webcam(CamHeight, CamWidth, CV_8UC3);
		webcam = cv::Scalar(0x88, 0xC9, 0x03);
		GUICon::putWebcamView(webcam, preSetImage);

		//������ �̹��� ����� ��
		cv::Mat preImage((GUIWidth - GUIHeight) / 2 - 40, (GUIWidth - GUIHeight) / 2 - 40, CV_8UC3); //���� ��ü���� ���� ī�޶� ǥ�� �� �κ��� ������ �ݿ��� ������ ���� ������ �翷 ���Ʒ� 20�� ������ ����� ���� 40�� ��
		preImage = cv::Scalar(0x00, 0x00, 0x00);
		GUICon::putPreImageView(preImage, preSetImage);

		//cv::imshow("Test", preSetImage); //frame �����ִ°�

		//cv::waitKey(10); //Ű ��ٸ��°� �̰� ���ϸ� ���ɸ�
	}
	if (TransCelebVector) {
		//������ �̹����� ����ȭ �ϴ� ��

		std::cout << std::endl;

		//������ �ִ� �̹��� ���� Ȯ��
		//dlib::image_window PI_showCelebImg; //������ ���� ���� ���� ����
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
					//PI_showCelebImg.set_image(I_face_chip);
					GUICon::putPreImageView(I_face_chip, preSetImage);
					cv::imshow("Test", preSetImage);
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
			else { //�������� ���� ������ �����ǰų� ���� ���� �ʾ��� ��
				setColor(COLOR::RED);
				std::cout << celeb_path << " ����� �������� " << detector(I_celeb_img).size() << "���� ���� �߰� �Ǿ����ϴ�. 1�� �ֵ��� �����ּ���." << std::endl;
				setColor(COLOR::GREEN);
			}
		}

		setColor(COLOR::AQUA);
		std::cout << celebCount << "���� �̹����� �����Խ��ϴ�." << std::endl;
		setColor();

		//if (LoadImageView)
			//PI_showCelebImg.close_window();

		setColor(COLOR::YELLOW);
		std::cout << std::endl << "128���� ���� ������ ��ȯ ���Դϴ�." << std::endl;
		celeb_face_128_vector = M_resnet(I_celeb_faces_chip);

		setColor(COLOR::GREEN);
		std::cout << "�Ϸ�" << std::endl << std::endl;
		setColor();
	}

	if (UseOpenCVGUI) {
		//GUI�� ���°�

		//��ķ�� ���� ���ε� �ٸ��� ������ �߶��ִ� �͵� �ؾ���
		

		
		
		
	}

	system("pause");
}

void CPputImage(cv::Mat& I_image, cv::Mat& O_image, cv::Rect pos) {
	cv::resize(I_image, I_image, cv::Size(pos.width, pos.height));

	cv::Mat imageROI(O_image, pos);
	I_image.copyTo(imageROI);
}

void CPputText(cv::Mat& O_image, cv::String text, cv::Point org, int ori, const char* fontName, int fontWeight, double fontScale, RGBScale textColor, RGBScale bkColor) {
	int fontSize = (int)(10 * fontScale);
	int width = O_image.cols;
	int height = fontSize * 3 / 2;

	HDC hdc = CreateCompatibleDC(NULL); //�ؽ�Ʈ �̹����� �����δ� �� ������

	HBRUSH hBrush = CreateSolidBrush(bkColor.rgb); //ä��� ����ε� bkColor�� �ܻ����� ä��°�

	//�ؽ�Ʈ �̹��� ũ�� ���ϴ°�
	RECT rect;
	rect.left = rect.top = 0;
	rect.right = width;
	rect.bottom = height;

	//��Ʈ���� ������ ������ �����ϴ� �� ũ�⳪ ��
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
	SelectObject(hdc, hbmp); //hdc�� ����? �ϴ� ��

	FillRect(hdc, &rect, hBrush); //������ ũ�⸸ŭ �����ϰ� ä��°� (�� ä��)

	BITMAP bitmap;
	GetObject(hbmp, sizeof(BITMAP), &bitmap);

	//�ؽ�Ʈ �̹��� ���� �� ����� �� �ִ� ��Ʈ�� ����? �ϴ� �׷���
	HFONT hFont = CreateFontA(
		fontSize,
		0,
		0,
		0,
		fontWeight,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET, //�ѱ�� �Ϻ�� ���ְ� �ϴ°� (�Ƹ�)
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, //��Ƽ ���ϸ������ ���ִ°�
		VARIABLE_PITCH | FF_ROMAN,
		fontName);
	SelectObject(hdc, hFont);
	SetTextColor(hdc, textColor.rgb);
	SetBkColor(hdc, bkColor.rgb);

	//����� ���� �̸� �ؽ�Ʈ�� ������ ���ϴ°�
	SIZE size;
	GetTextExtentPoint32A(hdc, text.c_str(), (int)text.length(), &size);

	TextOutA(hdc, 0, height / 3 * 1, text.c_str(), (int)text.length()); //�̹����� �ؽ�Ʈ ���°�
	int posX = (ori == 0 ? org.x : (ori == 1 ? org.x - (size.cx / 2) : org.x - size.cx)); //���� ���ϴ°� 0�� �ؽ�Ʈ�� ���� 1�� �ؽ�Ʈ�� �߰� 2�� �ؽ�Ʈ�� ������
	int posY = org.y - (size.cy / 2 + 5);

	//��Ʈ�� ������ OpenCV�̹����� �������ִ°�
	unsigned char* _tmp;
	unsigned char* _img;
	for (int y = 0; y < bitmap.bmHeight; y++) {
		if (posY + y >= 0 && posY + y < O_image.rows) {
			_img = O_image.data + (int)(3 * posX + (posY + y) * (((bitmap.bmBitsPixel / 8) * O_image.cols) & ~3));
			_tmp = (unsigned char*)(bitmap.bmBits) + (int)((bitmap.bmHeight - y - 1) * (((bitmap.bmBitsPixel / 8) * bitmap.bmWidth) & ~3));
			for (int x = 0; x < bitmap.bmWidth; x++) {
				if (x + posX >= O_image.cols)
					break;

				if (_tmp[0] != bkColor.b || _tmp[0] != bkColor.g || _tmp[0] != bkColor.r) { //�ؽ�Ʈ �̹����� ��� �÷��� ���ֱ� ���� ��, bgr ������ �ϴ� ������ Mat �̹����� ó���� ���� �� BGR ���̿���
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
	O_image = cv::Scalar(0x17, 0x17, 0x17); //��� ���� �ణ ��ο� ������ ĥ���ִ� ��
	cv::line(O_image, cv::Point(O_image.rows, 0), cv::Point(O_image.rows, O_image.rows), cv::Scalar(0x37, 0x00, 0xDA), 2); //ī�޶� ������ �κа� ���� ������ �κ��� �����ִ� ��
	cv::rectangle(O_image, cv::Rect(GUIHeight + 20, 180 + 20, ((GUIWidth - GUIHeight) / 2) - 40, ((GUIWidth - GUIHeight) / 2) - 40), cv::Scalar(0xED, 0xED, 0xED), 1 + 2, 4, 0); //�������� ������ ���ִ°� 1 + 2���� 2�� �� �β���


	CPputText(O_image, "Face Recognition", cv::Point(O_image.cols - ((O_image.cols - O_image.rows) / 2), 30), OriCenter, "���� ���", FW_BOLD, 6, RGBScale(0xED, 0xED, 0xED), RGBScale(0x11, 0x11, 0x11));
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