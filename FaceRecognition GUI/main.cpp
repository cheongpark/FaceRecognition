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

	bool useFaceLandMark = true; //���� ��ķ�� �ʿ��� ���� �ν��ؼ� ���� ���ڸ� ����ִ°� ���� ����

	/*
		������ �߿� �����

		�ѱ��̳� ��� ��ǻ���� ��Ʈ�� OpenCV �̹����� �ִ� �� (���� ������� ������ �̻� �ɸ�)
		�𵨵��� �ִ��� �Ǵ��ϰ� �������� ��
		���� ������ �ȵǴ� ��� �������� �ٸ��� ������� ���ϴ°� (�������� ��� ���� �̱���)
		Ư�� ��ο� �ִ� �������� ��� ������ ���� 1���� ���� Ȯ���ϰ� ���ͷ� �����ϴ°�
		ī�޶�� ���� �ϴ� ��
		���� �̹����� ���簢���� ǥ���� �� �̹����� ���簢���� ���� ����ϰ� �� ������ ä��� �� (�������)
		���콺�� ������ ���¿��� �ٸ� ������ ���콺�� ���� ������ �ȵǴ� ������ (�������)
	*/

	/*
		cols ����
		rows ����

		//���帶ũ ��ư�� �޴��� ������ �� �ֵ��� ����
		
		cv::Rect �μ��� (x, y, width, height)�� �� ��� ���������� ���� ���� ���̸� �Է��ϸ� ��.

		�̸� ��Ģ
		�̹��� : I_���� //Image
		������ �̹��� : PI_���� //Preview Image
		���� : V_���� //Video
		������ ���� : PV_���� //Preview Video
		String : S_���� //String
		�� : M_���� //Model
		��ư Rect : BR_���� //Button Rect
		��ư ��ġ �񱳿� Rect : CBR_���� //Compare Button Rect
		 
		�غ���
		1. �� ������ ���� Ž����� ã�� �ϴ� ��� ���帯
		2. �޸𸮸� �Ƴ��� ���� �̹��� ��θ� ������ �� �̸��� Ȯ���ڸ� �����ͼ� ���߿� ���� �ٲ� �� �⺻ �̹��� ��ο� �̸��� Ȯ���ڸ� �����ؼ� ������ �� �ְԵ� �ٲ㺸��
		3. ī�޶� �̸� Ȯ���ϴ°Ŵ� DirectShow �� ����ؾ� �ϰ� ������ �ٸ� �� �ֱ� ������ ���߿� ����

		1. ī�޶� Ȯ��
		2. ���� �ҷ�����
		3. ������ �̹����� ���� Ȯ���ϰ� ����ȭ
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
		webcam = cv::Scalar(0x88, 0xC9, 0x03); //�� ����
		GUICon::putWebcamView(webcam, preSetImage); //�� ������ ��ķ�� ä��°�

		//������ �̹��� ����� ��
		cv::Mat preImage((GUIWidth - GUIHeight) / 2 - 40, (GUIWidth - GUIHeight) / 2 - 40, CV_8UC3); //���� ��ü���� ���� ī�޶� ǥ�� �� �κ��� ������ �ݿ��� ������ ���� ������ �翷 ���Ʒ� 20�� ������ ����� ���� 40�� ��
		preImage = cv::Scalar(0x00, 0x00, 0x00); //�� ����
		GUICon::putPreImageView(preImage, preSetImage); //�� ������ ������ ä��°�

		frame = preSetImage; //���� frame ó�� �ϴ� ������ �Ѱ��ִ°�

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
					if (ShowMainView) {
						//�̹����� ���� ũ���� ���簢�� ����� ����� ���� �� �̹����� ��ջ�
						//�׸��� �� ����� ��ķ �信 �°� �������� ���Ѽ� �ִ´�(�׳� �Լ��� �ٷ� ������ ��)
						//���� ���簢���̸� �� ������ �н�

						if (I_celeb_img.nc() == I_celeb_img.nr())
							GUICon::putWebcamView(I_celeb_img, preSetImage);
						else {
							//�̹����� ���̿� ���� ��� �����ϰ� �� �ֱ�
							int squareLen = (I_celeb_img.nc() < I_celeb_img.nr() ? I_celeb_img.nr() : I_celeb_img.nc());
							
							cv::Mat originalViewImage(squareLen, squareLen, CV_8UC3);
							originalViewImage = cv::Scalar(0xFF, 0xFF, 0xFF);

							//��� �̹��� �߾ӿ� ���� �ֱ�
							//������ ���ΰ� ���κ��� ª�� ��� squareLen�� ���̿��� ���� ������ �̹��� ������ �� ��ŭ ���� ��ǥ ���ϱ�						
							if (I_celeb_img.nr() < I_celeb_img.nc()) { //���ΰ� �� Ŭ ��� ���� ��� �� ���ϴ°� ������ �̹� �ؼ�
								int ypos = (squareLen / 2) - (I_celeb_img.nr() / 2); //���ΰ� ª�� ��� ���簢���� �� ���� �濡�� �̹��� ���� ���� ���� ��ġ ���ϴ� ��
								CPputImage(I_celeb_img, originalViewImage, cv::Rect(0, ypos, I_celeb_img.nc(), I_celeb_img.nr()));
							}
							else {
								int xpos = (squareLen / 2) - (I_celeb_img.nc() / 2); //���� �ݴ�
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

		cv::namedWindow("Test"); //������ �̸��� �����ϴ� �� ������ Ȯ���ϰԴ� ������ setMouseCallback�� ������ �����
		cv::setMouseCallback("Test", GUICon::buttonsCheck); //���콺�� ������ �����ִ� �Լ�, Test��� �����쿡�� ���콺�� �����̰ų� � ������ �ϸ� buttonsCheck�Լ��� ȣ����

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

	//���帶ũ ��ư�� �޴��� ������ �� �ֵ��� ����

	//��ư ��
	//���� ��ư�� ����Ⱑ ��Ʊ� ������ �������� ��ü�ϰų� ���� �ִ� �簢������ �ؾ��� ��
	
	//��ư�� �ڸ��� �� �ִ� ��ġ�� ���� �� �װ� �ȿ��� ������ �����ϸ鼭 ��ư ��ġ�� �����ؾ���
	int buttonsMargin = 10; //�� ��ư���� ���� ������ �� ����
	int buttonsPosXS = GUIHeight + buttonsMargin; //��ư�� ��ġ X ������
	int buttonsPosXE = GUIWidth - buttonsMargin; //��ư�� ��ġ X ����
	int buttonWidth = (buttonsPosXE - buttonsPosXS) / 3 - 5; //�� ��ư�� ������ ���� 3�� ��ư ���� 5�� ��ư ���̻��� ����
	
	//��ư ��ġ ���� & RECT�� ��ȯ
	//GUICon::BR_capture = cv::Rect(buttonsPosXS,  ) //ù��° ��ư ��ġ ����
	GUICon::Var::BR_capture = cv::Rect(CamWidth + 10, 0, 100, 100); //�׽�Ʈ ��ư
	GUICon::Var::BR_preview = cv::Rect(CamWidth + 10, 100 + 50, 100, 100); //�׽�Ʈ ��ư

	GUICon::cvRect2RECT(GUICon::Var::BR_capture, GUICon::Var::CBR_capture);
	GUICon::cvRect2RECT(GUICon::Var::BR_preview, GUICon::Var::CBR_preview);
	GUICon::cvRect2RECT(GUICon::Var::BR_export, GUICon::Var::CBR_export);

	//��ư �׸���
	cv::rectangle(O_image, GUICon::Var::BR_capture, cv::Scalar(0xFF, 0xFF, 0xFF)); //�׽�Ʈ ��ư
	cv::rectangle(O_image, GUICon::Var::BR_preview, cv::Scalar(0xFF, 0xFF, 0xFF)); //�׽�Ʈ ��ư


	//�ؽ�Ʈ ��
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

void GUICon::buttonsCheck(int event, int x, int y, int flags, void* userData) {
	//std::cout << event << " " << x << " " << y << " " << flags << " " << userData << std::endl;

	/*
		event�� ���콺�� � ���� �� �� Ŭ���̳� ��ũ�� ���
		x, y�� ���콺�� OpenCV������ ������ ��� ��ġ�� �ִ���
		�������� �Ⱦ�

		event
			1 : ���� ���콺 ����
			4 : ���� ���콺 ��
			2 : ������ ���콺 ����
			5 : ������ ���콺 ��
	*/

	//Ŭ���� ���¿��� �ٸ� ������ ���콺�� ���� ���� ��ҷ� �ؼ� �ν��� �ȵǰ� �ؾ��ϱ� ������ 
	//Ư�� ��ġ���� 1���ϰ� 4���� �Ѵ� ���� ���� �� �� ��ư�� �����ϰ� �ؾ���

	//mouse, pushButtonNum
	//event�� 1�̸� PtlnRect�� ���� ��ư�� ������ �� ��ư�� pushButtonNum�� �����ϰ� ���� ��ư�� ���ٸ� pushButtonNum�� 0����
	//event�� 4�̸� pushButtonNum�� 0�� �ƴ� �� pushButtonNum�� 4�� ������ ���� ������ ���� ������ �ϰ� ���� �ʰų� ������ pushButtonNum�� 0���� 
	
	if (event == 1) {
		//���� ���콺 ������ ��

		GUICon::Var::mouse = POINT(x, y); //���콺 ������ �����ϴ� ��

		//PtlnRect�� ���콺�� RECT�� ��ġ �ȿ� ���� �� true�� ������
		
		//ù��° ��ư �ȿ� ���� ��
		if (PtInRect(&GUICon::Var::CBR_capture, GUICon::Var::mouse)) //1��° ��ư�� ��ġ�� ���콺�� ������ ��
			GUICon::Var::pushButtonNum = 1;
		else if (PtInRect(&GUICon::Var::CBR_preview, GUICon::Var::mouse)) //2��° ��ư�� ��ġ�� ���콺�� ������ ��
			GUICon::Var::pushButtonNum = 2;
		else if (PtInRect(&GUICon::Var::CBR_export, GUICon::Var::mouse)) //3��° ��ư�� ��ġ�� ���콺�� ������ ��
			GUICon::Var::pushButtonNum = 3;
		else
			GUICon::Var::pushButtonNum = 0;
	}
	if (event == 4 && GUICon::Var::pushButtonNum != 0) { //���� ���콺�� ���� ������ �� ��ư�� ���� ��
		//���� ���콺 ���� ��

		GUICon::Var::mouse = POINT(x, y); //���콺 ������ �����ϴ� ��

		if (PtInRect(&GUICon::Var::CBR_capture, GUICon::Var::mouse)) { //1�� ��ư���� ���콺�� ������ ��
			if (GUICon::Var::pushButtonNum == 1) { //1�� ��ư�� ������ Ȯ�������� ��
				std::cout << "1�� ��ư ����" << std::endl;
			}
		}
		else if (PtInRect(&GUICon::Var::CBR_preview, GUICon::Var::mouse)) { //2�� ��ư���� ���콺�� ������ ��
			if (GUICon::Var::pushButtonNum == 2) { //2�� ��ư�� ������ Ȯ�������� ��
				std::cout << "2�� ��ư ����" << std::endl;
			}
		}
		else if (PtInRect(&GUICon::Var::CBR_export, GUICon::Var::mouse)) { //3�� ��ư���� ���콺�� ������ ��
			if (GUICon::Var::pushButtonNum == 3) { //3�� ��ư�� ������ Ȯ�������� ��
				std::cout << "3�� ��ư ����" << std::endl;
			}
		}
			
		GUICon::Var::pushButtonNum = 0; //������ ���� ������
	}

	//std::cout << GUICon::Var::pushButtonNum << std::endl; //�׽�Ʈ��
}

void GUICon::cvRect2RECT(cv::Rect& I_rect, RECT& O_rect) {
	//ChatGPT�� ������� 
	//CV::Rect�� Windef.h�� RECT�� ��ȯ�����ִ� ��ɾ�
	O_rect.left = I_rect.x;
	O_rect.top = I_rect.y;
	O_rect.right = I_rect.x + I_rect.width;
	O_rect.bottom = I_rect.y + I_rect.height;
}
