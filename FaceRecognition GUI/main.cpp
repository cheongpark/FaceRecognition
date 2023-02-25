#include "stdafx.h"
#include "main.h"

#include <iostream>
#include <Windows.h>

int main(int argv, char* args[]) {
	//std::cout << argv << std::endl << args[0] << std::endl << args[1]; //�Ķ���� �׽�Ʈ

	std::vector<dlib::matrix<dlib::rgb_pixel>> I_celeb_faces; //������ ���� �̹���
	std::vector<dlib::matrix<dlib::rgb_pixel>> I_celeb_faces_chip; //������ �� �̹���
	std::vector<std::string> S_celeb_names; //������ �̸�

	std::vector<dlib::matrix<float, 0, 1>> celeb_faces_128_vector; //128-D �� ���� ����

	cv::Mat frame(GUIHeight, GUIWidth, CV_8UC3); //�̸� ���� ���� ���� ��� frame ����°�
	cv::Mat preSetImage(GUIHeight, GUIWidth, CV_8UC3); //���� GUI�� �����ϴ� ��
	cv::Mat I_WebCamFrame; //��ķ���� �̹��� �����°� �����ϴ°�
	cv::Mat I_captureImg; //ĸ�ĵ� �̹����� �����ϴ� ��
	cv::Mat I_exportImage(MakeImageHeight, MakeImageWidth, CV_8UC3, cv::Scalar(0xFF,0xFF,0xFF)); //������ �̹����� �����ϴ� ��
	cv::Mat logo; //�����İ� �ΰ� ����Ǿ��ִ� ����

	bool useFaceLandMark = true; //���� ��ķ�� �ʿ��� ���� �ν��ؼ� ���� ���ڸ� ����ִ°� ���� ����
	
	cv::VideoCapture WebCam;
	int CamCount = 0; //ī�޶� ���� 

	cv::namedWindow(WinName); //������ �̸��� �����ϴ� �� ������ Ȯ���ϰԴ� ������ setMouseCallback�� ������ �����

	/*
		������ �߿� �����

		�ѱ��̳� ��� ��ǻ���� ��Ʈ�� OpenCV �̹����� �ִ� �� (���� ������� ������ �̻� �ɸ�)
		�𵨵��� �ִ��� �Ǵ��ϰ� �������� ��
		���� ������ �ȵǴ� ��� �������� �ٸ��� ������� ���ϴ°� (�������� ��� ���� �̱���)
		Ư�� ��ο� �ִ� �������� ��� ������ ���� 1���� ���� Ȯ���ϰ� ���ͷ� �����ϴ°�
		ī�޶�� ���� �ϴ� ��
		���� �̹����� ���簢���� ǥ���� �� �̹����� ���簢���� ���� ����ϰ� �� ������ ä��� �� (�������)
		���콺�� ������ ���¿��� �ٸ� ������ ���콺�� ���� ������ �ȵǴ� ������ (�������)
		ũ�� ���� �ص� ���� ��ġ�� ���� �� �ֵ��� GUI ��ġ
		ī�޶��� ���ΰ� ũ�ٸ� ���� ũ�⿡ ���缭 ���簢������ �ڸ��� ���ΰ� ũ�� �� �ݴ�� ��ķ�� �߶��ִ� ���
		ī�޶󿡼� ���� ã�Ƽ� �簢������ ��� ��ġ�� �ִ��� ǥ�����ִ� ���

	*/

	/*
		������ �ʿ��� �ǽð����� ��ķ�� ���� ����ϴ°� ���� ������ ���â�̱� ������		

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
		if (Model::modelisAlive(ResNetPath)) alive_ResNetModel = true;
		else if (CheckModel) Model::useSearchModel(ResNetPath, alive_ResNetModel);
		else {
			setColor();
			std::cout << ResNetPath;
			setColor(COLOR::RED);
			std::cout << " ���� �߰ߵ��� �ʾұ� ������ ���α׷��� �����մϴ�." << std::endl;
			exit(-1);
		}

		//���� ���� ���ų� ���� ���� ������ �۵� ���
		//���� ��� �ִ��� ������ Ȯ�� �� �Ǵ� �ϴ� �������
		if (Model::modelisAlive(SpPath)) alive_SpModel = true;
		else if (CheckModel) Model::useSearchModel(SpPath, alive_SpModel);
		if (Model::modelisAlive(FastSpPath)) alive_FastSpModel = true;
		else if (CheckModel) Model::useSearchModel(FastSpPath, alive_FastSpModel);

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

			dlib::deserialize(SpPath) >> Model::M_sp;
			dlib::deserialize(SpPath) >> Model::M_fastsp;
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

	//�̹����� ����°� �׽�Ʈ �ϴ� �� �̰� ���� �Լ��� ���� �����ϸ� ��
	if(true) {
		//�ΰ� �̹����� �ִ��� Ȯ���ϰ� ������ �������� ��
		if (ImageisAlive(logoPath))
			logo = cv::imread(logoPath);
		
		//�׽�Ʈ�� ���� �̹������� ����
		cv::Mat I_myTestImage, I_celebTestImage;
		I_myTestImage = cv::imread("./CelebrityImg/������.jpeg");
		I_celebTestImage = cv::imread("./CelebrityImg/����.jpg");

		dlib::cv_image<dlib::bgr_pixel> I_celebDlibImage(I_celebTestImage), I_camDlibImage(I_myTestImage);

		//�̹��� ����
		dlib::matrix<dlib::bgr_pixel> celeb_face_chip, cam_face_chip; //���� ������ �����ϱ� ���� ����
		dlib::extract_image_chip(I_celebDlibImage, dlib::get_face_chip_details(Model::M_sp(I_celebDlibImage, Model::detector(I_celebDlibImage)[0]), 150, 0.6).rect, celeb_face_chip); //���� �κ� ������ ���� �Լ�
		dlib::extract_image_chip(I_camDlibImage, dlib::get_face_chip_details(Model::M_sp(I_camDlibImage, Model::detector(I_camDlibImage)[0]), 150, 0.6).rect, cam_face_chip); //���� �κ� ������ ���� �Լ�

		cv::Mat cv_celeb_face_chip = dlib::toMat(celeb_face_chip); //openCV�� Mat�� �����ϴ� ��
		cv::Mat cv_cam_face_chip = dlib::toMat(cam_face_chip); //openCV�� Mat�� �����ϴ� ��

		CPputImage(logo, I_exportImage, cv::Rect(0, 0, 535, 147)); //�ΰ� ����ִ°�
		CPputImage(cv_celeb_face_chip, I_exportImage, cv::Rect(MakeImageWidth / 2, MakeImageStartImageY, MakeImageWidth / 2, MakeImageWidth / 2)); //����� �̹������� ũ�� �����ʿ� �̹��� ������
		CPputImage(cv_cam_face_chip, I_exportImage, cv::Rect(0, MakeImageStartImageY, MakeImageWidth / 2, MakeImageWidth / 2)); //���ʲ�

		if (MakeImageUseOverlay) {
			//�� �̹������� �۰� ��� ��ġ�� ���� �ִ��� ǥ���ϴ°�
			//������ ������ �̹����� �̹� ���������Ƿ�
			//�ش� �̹����� �簢���� ġ�� �ٷ� ������ ��ġ�� �����ϴ� �������

			drawFaceRectangle(cv_celeb_face_chip, cv_celeb_face_chip, Model::detector(celeb_face_chip)[0], MakeImageOverlayColor, true); //�簢�� ����°�
			drawFaceRectangle(cv_cam_face_chip, cv_cam_face_chip, Model::detector(cam_face_chip)[0], MakeImageOverlayColor, true); //�簢�� ����°�
		
			CPputImage(cv_celeb_face_chip, I_exportImage, cv::Rect(MakeImageWidth - MakeImageOverlaySize, MakeImageStartImageY + (MakeImageWidth / 2) - MakeImageOverlaySize, MakeImageOverlaySize, MakeImageOverlaySize)); //�Ʒ� ���� �̹����� ��ġ�ϴ°�
			CPputImage(cv_cam_face_chip, I_exportImage, cv::Rect(0, MakeImageStartImageY + (MakeImageWidth / 2) - MakeImageOverlaySize, MakeImageOverlaySize, MakeImageOverlaySize)); //�Ʒ� ���� �̹����� ��ġ�ϴ°�

			cv::rectangle(I_exportImage, cv::Rect(0, MakeImageStartImageY + (MakeImageWidth / 2) - MakeImageOverlaySize, MakeImageOverlaySize, MakeImageOverlaySize), cv::Scalar(0, 0, 0), 3, 4, 0); //���м� ������
			cv::rectangle(I_exportImage, cv::Rect(MakeImageWidth - MakeImageOverlaySize, MakeImageStartImageY + (MakeImageWidth / 2) - MakeImageOverlaySize, MakeImageOverlaySize, MakeImageOverlaySize), cv::Scalar(0, 0, 0), 3, 4, 0); //���м� ������
		}
		cv::rectangle(I_exportImage, cv::Rect(MakeImageWidth / 2, MakeImageStartImageY, MakeImageWidth / 2, MakeImageWidth / 2), cv::Scalar(0, 0, 0), 3, 4, 0); //���м� ū��
		cv::rectangle(I_exportImage, cv::Rect(0, MakeImageStartImageY, MakeImageWidth / 2, MakeImageWidth / 2), cv::Scalar(0, 0, 0), 3, 4, 0); //���м� ū��

		//���� �ð� ����
		std::chrono::system_clock::time_point nowCaptureDate = std::chrono::system_clock::now(); //���� �ð� ���ϱ�
		std::time_t nowCaptureDate_c = std::chrono::system_clock::to_time_t(nowCaptureDate);
		std::tm nowCaptureDate_tm = {};
		localtime_s(&nowCaptureDate_tm, &nowCaptureDate_c); //�ð����� �����ϴ°�

		char nowCaptureDateStr[30];
		sprintf_s(nowCaptureDateStr, "%04d-%02d-%02d %s %02d:%02d:%02d",
			nowCaptureDate_tm.tm_year + 1900, nowCaptureDate_tm.tm_mon + 1, nowCaptureDate_tm.tm_mday,
			nowCaptureDate_tm.tm_hour < 12 ? "����" : "����",
			nowCaptureDate_tm.tm_hour % 12 == 0 ? 12 : nowCaptureDate_tm.tm_hour % 12,
			nowCaptureDate_tm.tm_min, nowCaptureDate_tm.tm_sec); //�ð��� ��-��-�� ����|���� ��:��:�� �� ��Ÿ���� �ϴ� �� ChatGPT�� ¥��

		char celebCompare[20];
		sprintf_s(celebCompare, "%.1f%%", 87.5/*(1 - faceDistance.distance) * 100*/);
		CPputText(I_exportImage, celebCompare, cvPoint(MakeImageWidth, 95), 2, "���� ���", FW_BOLD, 6, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //�ѱ۷� �ǰ� �ٲ���� (�Ϸ�)
		CPputText(I_exportImage, nowCaptureDateStr, cvPoint(MakeImageWidth, 0), 2, "���� ���", FW_DEMIBOLD, 4, RGBScale(255, 0, 0), RGBScale(255, 255, 255));

		CPputText(I_exportImage, "��", cvPoint(MakeImageWidth / 4, 745), 1, "���� ���", FW_BOLD, 6, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //CanvasWidth / 4 ù��° ������ �߰�
		CPputText(I_exportImage, "����", cvPoint(MakeImageWidth / 4 * 3, 745), 1, "���� ���", FW_BOLD, 6, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //CanvasWidth / 4 * 3 �� 2��° ������ �߰��� ����Ű�� ����

		cv::imshow("TestExportImg", I_exportImage);
		cv::waitKey();
	}

	if (UsePreSetImage) {
		//������ GUI�� �̸� �̹����� ���� �� �̰ɷ� �̹��� ������ �� �̸� �����ִ� ������
		//�ӵ��� ���� ���� ��ġ�� UI�� �ٸ� ���� �̸� ������ �ϰ� ������Ʈ �ؾ��� �� ���� �����Ѱ� �����ֱ�
		GUIInitImage(preSetImage);

		//��ķ�� ����� �� ������ ǥ��
		cv::Mat webcam(CamHeight, CamWidth, CV_8UC3);
		webcam = cv::Scalar(0x88, 0xC9, 0x03); //�� ����
		GUICon::putWebcamView(webcam, preSetImage); //�� ������ ��ķ�� ä��°�

		//������ �̹��� ����� �� ������ ǥ��
		cv::Mat preImage((GUIWidth - GUIHeight) / 2 - 40, (GUIWidth - GUIHeight) / 2 - 40, CV_8UC3); //���� ��ü���� ���� ī�޶� ǥ�� �� �κ��� ������ �ݿ��� ������ ���� ������ �翷 ���Ʒ� 20�� ������ ����� ���� 40�� ��
		preImage = cv::Scalar(0x00, 0x00, 0x00); //�� ����
		GUICon::putPreImageView(preImage, preSetImage); //�� ������ ������ ä��°�
		
		preSetImage.copyTo(frame); //���� frame ó�� �ϴ� ������ �Ѱ��ִ°�
		//frame = preSetImage; //���� frame ó�� �ϴ� ������ �Ѱ��ִ°� <-- �̰� �����ϴ� �׷� �� ������ ���߿� frame�� �ٲ������ preSetImage�� ���� �ٲ� �׷��� ����

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
			
			if (Model::detector(I_celeb_img).size() == 1) { //�������� ���� 1�� �����Ǹ�
				setColor(COLOR::GREEN);
				std::cout << celeb_path << " ������ �����Խ��ϴ�.";
				setColor(COLOR::WHITE);
				std::cout << " ���� �� �������� \"" << getFileName(celeb_path) << "\" �Դϴ�." << std::endl;

				auto celeb_face = Model::detector(I_celeb_img); //���� ��ġ�� �����ϱ� ���� ������

				dlib::matrix<dlib::rgb_pixel> I_face_chip; //�������� �� �κи� ������ ������ ������ �ִ� ����
				dlib::extract_image_chip(I_celeb_img, dlib::get_face_chip_details(Model::M_sp(I_celeb_img, celeb_face[0]), 150, 0.25), I_face_chip); //�� �κи� �������ִ� ��ɾ�

				if (LoadImageView) {
					//PI_showCelebImg.set_image(I_face_chip);
					if (ShowMainView) {
						//�̹����� ���� ũ���� ���簢�� ����� ����� ���� �� �̹����� ��ջ�
						//�׸��� �� ����� ��ķ �信 �°� �������� ���Ѽ� �ִ´�(�׳� �Լ��� �ٷ� ������ ��)
						//���� ���簢���̸� �� ������ �н�

						if (I_celeb_img.nc() == I_celeb_img.nr())
							GUICon::putWebcamView(I_celeb_img, frame);
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
			else { //�������� ���� ������ �����ǰų� ���� ���� �ʾ��� ��
				setColor(COLOR::RED);
				std::cout << celeb_path << " ����� �������� " << Model::detector(I_celeb_img).size() << "���� ���� �߰� �Ǿ����ϴ�. 1�� �ֵ��� �����ּ���." << std::endl;
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
		celeb_faces_128_vector = Model::M_resnet(I_celeb_faces_chip);

		setColor(COLOR::GREEN);
		std::cout << "�Ϸ�" << std::endl << std::endl;
		setColor();
		
		preSetImage.copyTo(frame); //���� frame ó�� �ϴ� ������ �Ѱ��ִ°�
	}
	
	if (UseOpenCVGUI) {
		//GUI�� ���°�
		
		//��ķ�� ���� ���ε� �ٸ��� ������ �߶��ִ� �͵� �ؾ���

		cv::setMouseCallback(WinName, GUICon::buttonsCheck); //���콺�� ������ �����ִ� �Լ�, Test��� �����쿡�� ���콺�� �����̰ų� � ������ �ϸ� buttonsCheck�Լ��� ȣ����

		int camera_track_val = 0; //ī�޶� ���� ���� ���� �� (�⺻ 0)
		int pre_camera_track_val = -1; //ī�޶� ���� ���� �ٲ�� ���� �� (-1�� �� �������� ����� �ؾ��� �Ǽ�)
		cv::createTrackbar("Camera", WinName, &camera_track_val, CamCount - 1); //ī�޶� ���� �� ����°�

		while (true) {
			if (UseGetCalTime) {
				end_time = std::chrono::system_clock::now();
				get_cal_time_mill = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
				start_time = std::chrono::system_clock::now();
				
				std::cout << get_cal_time_mill.count() << std::endl;
			}

			preSetImage.copyTo(frame); //�̸� �غ�� �̹����� frame�� �ִ°� frame�� ǥ���Ҳ� �ӵ��� ������ �ϱ����ؼ� �̸� ���� �� �ִ°�
			
			//ī�޶� ���� �� �ִ��� ������, ���� ���� �� ������ ī�޶� ������
			if (pre_camera_track_val != camera_track_val) { //ī�޶� Ʈ�� ���� ���� �ٲ��
				WebCam.open(camera_track_val); //��ķ�� Ʈ���ٿ��� ������ ��� ����
				pre_camera_track_val = camera_track_val; //���� ���� ���� ������ ���߱�

				setColor(COLOR::GREEN);
				std::cout << camera_track_val << " ī�޶�� �����߽��ϴ�." << std::endl;
				setColor();
			}
			
			if (WebCam.read(I_WebCamFrame)) { //��ķ���� �̹����� ���� �� ���� ��
				isUseCapture = true; //ĸ�İ� �ǰ� ����

				//��ķ�� ���� ���� �� �� �� ���� �������� ���ؼ� ������ �߶� ���簢������ ����� �ִ� ��
				whCompareSquareCut(I_WebCamFrame);

				//��ķ���� ���� ã�Ƽ� �簢������ ǥ�����ֱ�
				dlib::cv_image<dlib::bgr_pixel> I_calRecImg(I_WebCamFrame); //���� ã�� ���� Dlib�� �̹����� �������ִ°� ��Ʈ���� ������ Calculate Rectangle Image��
				auto faces = Model::detector(I_calRecImg); //��Ʈ���� �̹������� �󱼵��� ã�Ƽ� faces�� �����ϴ� ��
				int facesCount = faces.size(); //�� ������ ������ �ִ°�

				//�� Ƚ�� �ؽ�Ʈ ǥ��
				char S_facesCount[30]; //�� � �ִ��� �ؽ�Ʈ ���� ��
				sprintf_s(S_facesCount, "�� %d�� ���� �Ǿ����ϴ�.", facesCount);
				CPputText(frame, S_facesCount, cv::Point(frame.cols - ((frame.cols - frame.rows) / 2), 110), OriCenter, "���� ���", FW_BOLD, 3, (facesCount != 1 ? RGBScale(0xDA, 0x00, 0x37) : RGBScale(0xED, 0xED, 0xED)), BackgroundColor); //frame�� ���� � �ִ��� �ؽ�Ʈ�� ����ִ� �� �߰��� ���ǹ��� 1���� �ƴϸ� ���������� ���� ���°�

				if (facesCount == 1)
					drawFaceRectangle(I_calRecImg, I_WebCamFrame, faces[0], FaceLandMarkColor);

				//���� ��ķ �� �ʿ� ��ķ ���� �ִ°�
				GUICon::putWebcamView(I_WebCamFrame, frame);

				//ĸ�� ��ư�� ������ ������ ĸ���ϰ� ���� ����
				if (useCapture) {
					capture_start_now = std::chrono::system_clock::now(); //������ �ð��� ����
					get_capture_start_mill = std::chrono::duration_cast<std::chrono::milliseconds>(capture_start_now - capture_start); //���� �ð����� ĸ�� ���� �ð��� ���� ĸ�� ���� �ð��� �����ؼ� �и�������� ����

					if (get_capture_start_mill.count() >= 3000) { //ĸ�� ���� �ð��� 3�ʰ� ������ 
						//ĸ�� ��ư ������ 3�ʰ� ������ ��
						if (facesCount == 1) { //��ķ�� ���� 1���� ���� ��
							std::cout << "ĸ����" << std::endl;
							//GUICon::putWebcamViewText(frame, "CAPTURE!", RGBScale(0xDA, 0x00, 0x37));

							//���� �̹��� ĸ��
							WebCam.read(I_captureImg);
							whCompareSquareCut(I_captureImg);

							//cv::imshow("Test2", captureImg); //�׽�Ʈ�� �̹��� ǥ��

							//�󱼵��� ������ִ� ��
							setColor(COLOR::YELLOW);
							std::cout << "�����" << std::endl;
							setColor();
							GUICon::putWebcamViewText(frame, "Cal..", RGBScale(0xDA, 0x00, 0x37));

							//����� ���� ���ִ� �Լ��� �����ϰ� �̹����� ����°� �����ؼ�
							//����� �ٷ� �ϰ� �̹����� ����� ������ �����ϰ� �ִٰ� 
							//PREVIEW ��ư�� ������ �̸� �����ִ� â�� ���� EXPORT������ �������Ⱑ �ǰ�

							std::vector<FaceDistance>faces_distance = compareFacesImage(I_captureImg, celeb_faces_128_vector); //�̹����� ������ ���� ���� �־ �Ÿ��� ���ؼ� ���ĵ� ���� ������ �����ϴ� ��

							//GUI�� ������ ǥ�����ִ� ��
							//���õ���
							std::cout << S_celeb_names[faces_distance[0].faceNum] << std::endl;
							std::cout << S_celeb_names[faces_distance[1].faceNum] << std::endl;
							std::cout << S_celeb_names[faces_distance[2].faceNum] << std::endl;
							std::cout << S_celeb_names[faces_distance[3].faceNum] << std::endl;

							useCapture = false; //��� �۾��� ������ ������ ĸ�ĸ� ����
						}
						else {
							setColor(COLOR::RED);
							std::cout << "��ķ�� ���� �������� �ʾ� ĸ�ĸ� �����մϴ�." << std::endl;
							setColor();

							useCapture = false; //ĸ���� �� ���� ���� ���� �ʾƼ� ĸ�ĸ� ����
						}
					}
					else { //3�ʰ� ������ �ʾƼ� ��ķ ��� 3, 2, 1�� ����ִ� ��
						//3, 2, 1�� �ؽ�Ʈ�� �ʸ��� ǥ���ϰ� �ϴ� ��
						//std::cout << 3 - (get_capture_start_mill.count() / 1000) << std::endl; 
						cv::String capture_timer(std::to_string(3 - (get_capture_start_mill.count() / 1000))); //3�� Ÿ�̸� ����, 3 2 1�� �����
						GUICon::putWebcamViewText(frame, capture_timer);
					}
				}

				//�׽�Ʈ��
				//GUICon::putWebcamViewText(frame, "�ȳ�");
			}
			else { //��ķ���� �̹����� ���� �� ���� ��
				isUseCapture = false; //ĸ�İ� ���� �ʰ� ����
				useCapture = false; //�̰� ���ϸ� ĸ�� �ǰ� ���� �� �̹����� ���� �� ���ٰ� �ٽ� �̹����� ������ ���ڱ� ĸ����

				setColor(COLOR::RED);
				std::cout << "��ķ���� �̹����� ���� �� �����ϴ�." << std::endl;
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

	//�޸𸮿��� �������ִ°� �̰� ���ϸ� �޸� ��� �����
	DeleteObject(hBrush);
	DeleteObject(hFont);
	DeleteObject(hbmp);
	DeleteObject(hdc);
}

void GUIInitImage(cv::Mat& O_image) {
	O_image = cv::Scalar(BackgroundColor.b, BackgroundColor.g, BackgroundColor.r); //��� ���� �ణ ��ο� ������ ĥ���ִ� ��
	cv::line(O_image, cv::Point(O_image.rows, 0), cv::Point(O_image.rows, O_image.rows), cv::Scalar(0x37, 0x00, 0xDA), 2); //ī�޶� ������ �κа� ���� ������ �κ��� �����ִ� ��
	cv::rectangle(O_image, cv::Rect(GUIHeight + 20, 180 + 20, ((GUIWidth - GUIHeight) / 2) - 40, ((GUIWidth - GUIHeight) / 2) - 40), cv::Scalar(0xED, 0xED, 0xED), 1 + 2, 4, 0); //�������� ������ ���ִ°� 1 + 2���� 2�� �� �β���


	CPputText(O_image, "Face Recognition", cv::Point(O_image.cols - ((O_image.cols - O_image.rows) / 2), 30), OriCenter, "���� ���", FW_BOLD, 6, RGBScale(0xED, 0xED, 0xED), BackgroundColor);

	//���帶ũ ��ư�� �޴��� ������ �� �ֵ��� ����

	//��ư ��
	//���� ��ư�� ����Ⱑ ��Ʊ� ������ �������� ��ü�ϰų� ���� �ִ� �簢������ �ؾ��� ��
	
	//��ư�� �ڸ��� �� �ִ� ��ġ�� ���� �� �װ� �ȿ��� ������ �����ϸ鼭 ��ư ��ġ�� �����ؾ���
	int buttonsMargin = 10; //�� ��ư���� ���� ������ �Ʒ� �� ����
	int buttonsPosXS = GUIHeight + buttonsMargin; //��ư�� ��ġ X ������
	int buttonsPosXE = GUIWidth - buttonsMargin; //��ư�� ��ġ X ����
	
	int buttonWidth = (buttonsPosXE - buttonsPosXS) / 3 - 5; //�� ��ư�� ������ ���� 3�� ��ư ���� 5�� ��ư ���̻��� ����
	int buttonHeight = 50;
	int buttonY = GUIHeight - buttonHeight - buttonsMargin;

	//��ư ��ġ ���� & RECT�� ��ȯ
	//GUICon::BR_capture = cv::Rect(buttonsPosXS,  ) //ù��° ��ư ��ġ ����
	GUICon::Var::BR_capture = cv::Rect(CamWidth + 10, 0, 100, 100); //�׽�Ʈ ��ư
	GUICon::Var::BR_preview = cv::Rect(CamWidth + 10, 100 + 50, 100, 100); //�׽�Ʈ ��ư

	GUICon::Var::BR_capture = cv::Rect(buttonsPosXS, buttonY, buttonWidth, buttonHeight);
	GUICon::Var::BR_preview = cv::Rect(buttonsPosXS + (5 + 5) + buttonWidth, buttonY, buttonWidth, buttonHeight);
	GUICon::Var::BR_export = cv::Rect(buttonsPosXS + (5 + 5) + buttonWidth + (5 + 5) + buttonWidth, buttonY, buttonWidth, buttonHeight);

	GUICon::cvRect2RECT(GUICon::Var::BR_capture, GUICon::Var::CBR_capture);
	GUICon::cvRect2RECT(GUICon::Var::BR_preview, GUICon::Var::CBR_preview);
	GUICon::cvRect2RECT(GUICon::Var::BR_export, GUICon::Var::CBR_export);

	//��ư �׸���
	cv::rectangle(O_image, GUICon::Var::BR_capture, cv::Scalar(0xFF, 0xFF, 0xFF)); //�׽�Ʈ ��ư
	cv::rectangle(O_image, GUICon::Var::BR_preview, cv::Scalar(0xFF, 0xFF, 0xFF)); //�׽�Ʈ ��ư
	cv::rectangle(O_image, GUICon::Var::BR_export, cv::Scalar(0xFF, 0xFF, 0xFF));

	//��ư �ؽ�Ʈ ��
	CPputText(O_image, "CAPTURE", cv::Point(GUICon::Var::BR_capture.x + (GUICon::Var::BR_capture.width / 2), GUICon::Var::BR_capture.y + 13), 1, "���� ���", FW_BOLD, 3, RGBScale(0xED, 0xED, 0xED), BackgroundColor);
	CPputText(O_image, "PREVIEW", cv::Point(GUICon::Var::BR_preview.x + (GUICon::Var::BR_preview.width / 2), GUICon::Var::BR_preview.y + 13), 1, "���� ���", FW_BOLD, 3, RGBScale(0xED, 0xED, 0xED), BackgroundColor);
	CPputText(O_image, "EXPORT", cv::Point(GUICon::Var::BR_export.x + (GUICon::Var::BR_export.width / 2), GUICon::Var::BR_export.y + 13), 1, "���� ���", FW_BOLD, 3, RGBScale(0xED, 0xED, 0xED), BackgroundColor);
}

bool Model::modelisAlive(std::string& path) {
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

void Model::useSearchModel(std::string& path, bool& alive_model) {
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

void Model::applyModel(std::string& path, dlib::shape_predictor& model) {
	setColor();
	std::cout << path;
	setColor(COLOR::BLUE);
	std::cout << " ���� �����մϴ�." << std::endl;
	dlib::deserialize(path) >> model;
}

void Model::applyModel(std::string& path, anet_type& model) {
	setColor();
	std::cout << path;
	setColor(COLOR::BLUE);
	std::cout << " ���� �����մϴ�." << std::endl;
	dlib::deserialize(path) >> model;
}

void drawFaceRectangle(dlib::cv_image<dlib::bgr_pixel> I_image, cv::Mat& O_image, dlib::rectangle face, cv::Scalar color, bool useFast) {
	dlib::chip_details faceChip; //�󱼿� ��ġ�� ���� ���� ������ִ� ����

	if (useFast) //5���� ���� �� ã�� �� (����, ��Ȯ�� ����)
		faceChip = dlib::get_face_chip_details(Model::M_fastsp(I_image, face), 150, 0.2); //ã�� ���� ��ġ�� �����ϴ� �� 0.2�� �簢���� �� ���� ���� ��������
	else //68���� ���� �� ã�� �� (����, ��Ȯ�� ����)
		faceChip = dlib::get_face_chip_details(Model::M_sp(I_image, face), 150, 0.2); //ã�� ���� ��ġ�� �����ϴ� �� 0.2�� �簢���� �� ���� ���� ��������

	cv::rectangle(O_image, cv::Rect(faceChip.rect.left(), faceChip.rect.top(), faceChip.rect.width(), faceChip.rect.height()), color, 2, 4, 0); //���� ������ ������ �󱼿� �Ʊ� ���� Rect ������ �簢���� �׸��°���
}

void drawFaceRectangle(cv::Mat& I_image, cv::Mat& O_image, dlib::rectangle face, cv::Scalar color, bool useFast) {
	dlib::chip_details faceChip; //�󱼿� ��ġ�� ���� ���� ������ִ� ����
	dlib::cv_image<dlib::bgr_pixel> faceImage(I_image);

	if (useFast) //5���� ���� �� ã�� �� (����, ��Ȯ�� ����)
		faceChip = dlib::get_face_chip_details(Model::M_fastsp(faceImage, face), 150, 0.2); //ã�� ���� ��ġ�� �����ϴ� �� 0.2�� �簢���� �� ���� ���� ��������
	else //68���� ���� �� ã�� �� (����, ��Ȯ�� ����)
		faceChip = dlib::get_face_chip_details(Model::M_sp(faceImage, face), 150, 0.2); //ã�� ���� ��ġ�� �����ϴ� �� 0.2�� �簢���� �� ���� ���� ��������

	cv::rectangle(O_image, cv::Rect(faceChip.rect.left(), faceChip.rect.top(), faceChip.rect.width(), faceChip.rect.height()), color, 2, 4, 0); //���� ������ ������ �󱼿� �Ʊ� ���� Rect ������ �簢���� �׸��°���
}

void whCompareSquareCut(cv::Mat& I_image) {
	if (I_image.cols >= I_image.rows) //�̹����� ���簢���̰ų� ���ΰ� �� �涧
		I_image = I_image(cv::Range(0, I_image.rows), cv::Range((I_image.cols - I_image.rows) / 2, (I_image.cols - I_image.rows) / 2 + I_image.rows));
	else //���ΰ� �� �涧
		I_image = I_image(cv::Range((I_image.rows - I_image.cols) / 2, (I_image.rows - I_image.cols) / 2 + I_image.cols), cv::Range(0, I_image.cols));
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

std::vector<FaceDistance> compareFacesImage(cv::Mat& faceImg, std::vector<dlib::matrix<float, 0, 1>> faces_vector) {
	dlib::cv_image<dlib::bgr_pixel> I_cam_face(faceImg); //dlib�� ���ϱ� ������ openCV���� ĸ���� �̹����� dlib �̹����� ����
	std::vector<dlib::rectangle> compareFace = Model::detector(I_cam_face); //������ �̹������� ���� ã�Ƽ� �� ��ġ ����

	dlib::matrix<dlib::rgb_pixel> I_cam_faces_chip; //�� �κи� ������ �̹����� �����ϴ� ����
	dlib::extract_image_chip(I_cam_face, dlib::get_face_chip_details(Model::M_sp(I_cam_face, compareFace[0]), 150, 0.25), I_cam_faces_chip); //�̹������� �� �κи� �����ؼ� ���� �ϴ� �Լ� ������ �Ǵ� �۾��̶� ���� �𵨷�

	dlib::matrix<float, 0, 1> cam_face_128_vector = Model::M_resnet(I_cam_faces_chip); //���� �̹����� ���ͷ� ��ȯ

	std::vector<FaceDistance> faces_distance; //�����ΰ��� �Ÿ� �������� üũ�ϱ� ���� ���͸� ���
	for (int i = 0; i < faces_vector.size(); i++)
		faces_distance.push_back({ i, dlib::length(cam_face_128_vector - faces_vector[i]) }); //i�� ����� ������ ������ �����ΰ� ķ ���� �Ÿ��� ����

	std::sort(faces_distance.begin(), faces_distance.end(), face_cmp); //�񱳵� �󱼵��� �Ÿ��� ������������ ���� �Ÿ��� ����� ���� ��ұ� ������
	
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
		std::cout << " �̹����� ã�ҽ��ϴ�." << std::endl;
		return 1;
	}
	else {
		setColor(COLOR::RED);
		std::cout << " �̹����� ã�� ���߽��ϴ�." << std::endl;
		return 0;
	}
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
				if (isUseCapture) {
					std::cout << "ĸ�� ��ư ����" << std::endl;
					useCapture = true;
					capture_start = std::chrono::system_clock::now();
				}
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

void GUICon::putWebcamViewText(cv::Mat& O_image, cv::String text, RGBScale color) {
	CPputText(O_image, text, cv::Point(CamWidth / 2, CamHeight / 2 - 70), OriCenter, "���� ���", FW_BOLD, 15, color, RGBScale(0x00, 0x00, 0x00));
}