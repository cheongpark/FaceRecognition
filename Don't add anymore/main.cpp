//dlib::get_face_chip_details(sp(celeb_overlay, detector(celeb_overlay)[0]), OverlayImgSize, 0.25) �̰� OverlayImgSize �κ��� �ƹ��ų� ������ �ȵ� ������ �̹����� �Է��Ѱ� * �Է��Ѱ���

#include "stdafx.h"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define WINDOW_NAME	"Face Recognition (������ ���� ã��)"

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

//�б� ��Ʈ�� 1280 * 720
//��ī 640 * 480

#define WEBCAMW 640
#define WEBCAMH 480 //���� ũ���� �Է¹ް� �������� ©��� �� ��

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

	//ä��� ���? �� ��, ���� �̰ɷ� ���ϸ� �Ǵ� ��
	HBRUSH hBrush = CreateSolidBrush(bkcolor.rgb);

	//ó�� ū ��Ʈ���� ���� ä��� ���� ũ�⸦ ������ ����
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

	//�ؽ�Ʈ ��� ���� ���� ���� �������ε� ä��°���
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
		OUT_DEFAULT_PRECIS, //SHIFTJIS_CHARSET -> DEFAULT_CHARSET���� ����
		CLIP_DEFAULT_PRECIS,
		(anti ? ANTIALIASED_QUALITY : DEFAULT_QUALITY), //��Ƽ�� ����ϸ� ���ְ� �ƴϸ� �����ְ�
		VARIABLE_PITCH | FF_ROMAN,
		fontname);
	SelectObject(hdc, hFont);
	SetTextColor(hdc, textcolor.rgb);
	SetBkColor(hdc, bkcolor.rgb);

	//���� ���� ���ϱ�
	SIZE size;
	GetTextExtentPoint32A(hdc, text.c_str(), (int)text.length(), &size);

	TextOutA(hdc, 0, height / 3 * 1, text.c_str(), (int)text.length());
	int posX = (ori == 0 ? org.x : (ori == 1 ? org.x - (size.cx / 2) : org.x - size.cx)); //������ �����̸� �״��, �߰��̸� ������ - ���� / 2, �������̸� ������ - ����
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

				if (_tmp[0] != bkcolor.b || _tmp[1] != bkcolor.g || _tmp[2] != bkcolor.r) { //������ bgr�� �Ἥ bgr�� �Ѱ�, rgb �����̸� �װɷ� �ؾ���
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

	dlib::matrix<dlib::bgr_pixel> logo; //���߿� �̹��� ����� �� �����İ� �ΰ���
	std::string logoPath = "logo/logo.jpg";
	if (MakeExportImage) {
		if (_access(spPath.c_str(), 0) != -1) {
			dlib::load_image(logo, logoPath);
			setColor(COLOR::GREEN);
			std::cout << logoPath << " ������ ã�ҽ��ϴ�." << std::endl;
			setColor();
		}
		else {
			setColor(COLOR::RED);
			std::cout << logoPath << " ������ ã�� ���߽��ϴ�." << std::endl;
			setColor();
			ErrorExit();
		}
	}

	//�� �ε�
	//ķ�� �������� ��ҵ��� ������
	//ķ ���� - ī�޶� ���� ������ �Ʒ��� �ִ°���
	//ķ��Ҹ� �����ͼ� ���ؼ� �����ϰ� ǥ��

	//�� �ε�
	{
		{
			if (_access(spPath.c_str(), 0) != -1) {
				setColor(COLOR::GREEN);
				std::cout << spPath << " ���� ã�ҽ��ϴ�." << std::endl;
				setColor();
			}
			else {
				setColor(COLOR::RED);
				std::cout << spPath << " ���� ã�� ���߽��ϴ�." << std::endl;
				setColor();
				ErrorExit();
			}

			if (_access(fastspPath.c_str(), 0) != -1) {
				setColor(COLOR::GREEN);
				std::cout << fastspPath << " ���� ã�ҽ��ϴ�." << std::endl;
				setColor();
			}
			else {
				setColor(COLOR::RED);
				std::cout << fastspPath << " ���� ã�� ���߽��ϴ�." << std::endl;
				setColor();
				ErrorExit();
			}

			if (_access(ResNetPath.c_str(), 0) != -1) {
				setColor(COLOR::GREEN);
				std::cout << ResNetPath << " ���� ã�ҽ��ϴ�." << std::endl;
				setColor();
			}
			else {
				setColor(COLOR::RED);
				std::cout << ResNetPath << " ���� ã�� ���߽��ϴ�." << std::endl;
				setColor();
				ErrorExit();
			}
		}
		dlib::deserialize(spPath) >> sp;
		dlib::deserialize(fastspPath) >> fastsp;
		dlib::deserialize(ResNetPath) >> net;
	}
	//�̹��� �ε� �� ����ȭ
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
					std::cout << celeb_path[celeb] << " ������ �����Խ��ϴ�.";
					setColor(COLOR::WHITE);
					std::cout << " ���� �� �������� \"" << get_filename(celeb_path[celeb]) << "\" �Դϴ�." << std::endl;

					auto celeb_face = detector(img);

					dlib::matrix<dlib::rgb_pixel> face_chip;
					dlib::extract_image_chip(img, dlib::get_face_chip_details(sp(img, celeb_face[0]), 150, 0.25), face_chip);

					if (loadImageView) {
						showCelebImg.set_image(face_chip);
						Sleep(loadImageViewDelay);
					}

					if (MakeExportImage) //���߿� �̹��� ���� ������ �� ������
						celeb_faces.push_back(img);
					celeb_faces_chip.push_back(std::move(face_chip));
					celeb_names.push_back(get_filename(celeb_path[celeb]));

					celebCount++;
				}
				catch (size_t num) {
					setColor(COLOR::RED);
					std::cout << celeb_path[celeb] << " ����� �������� " << detector(img).size() << "���� ���� �߰� �Ǿ����ϴ�. 1�� �ֵ��� �����ּ���." << std::endl;
					setColor(COLOR::DARK_WHITE);
				}
			}
			setColor(COLOR::AQUA);
			std::cout << celebCount << "���� �̹����� �����Խ��ϴ�." << std::endl;
			setColor();

			if (loadImageView)
				showCelebImg.close_window();

			setColor(COLOR::YELLOW);
			std::cout << std::endl << "128���� ���� ������ ��ȯ ���Դϴ�." << std::endl;
			Celebface_descriptors = net(celeb_faces_chip);
			setColor(COLOR::GREEN);

			std::cout << "�Ϸ�" << std::endl << std::endl;
			setColor();
		}
	}
	
	//ķ�� �������� ��ҵ��� ������ 
	{
		//opencv ������ �̸� ���ϴ°�
		cvui::init(WINDOW_NAME);

		cv::VideoCapture cap;
		cap.open(camNum);

		if (!cap.isOpened()) { //��ķ ã�� ���ϸ�
			setColor(COLOR::RED);
			std::cout << "Error: ��ķ�� ã�� ���߽��ϴ�. \a" << std::endl;
			setColor();
		}
		else {
			setColor(COLOR::GREEN);
			std::cout << "Success: ��ķ�� ã�ҽ��ϴ�." << std::endl;
			setColor();
		}

		//�̰� �� ī�޶��� ���� ũ���� �����ϰ� �ű⿡�� ©��� ��
		//���⼱ �� �κ� ���� �ȵ�
		cap.set(cv::CAP_PROP_FRAME_WIDTH, WEBCAMW); //��ķ�� ���� ����� ������
		cap.set(cv::CAP_PROP_FRAME_HEIGHT, WEBCAMH); //��ķ�� ���� ����� �����Ԥ�

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

		cv::Mat CaptureImg(WEBCAMH, WEBCAMW, CV_8UC3); //���簢�� ĸ���ϸ� �� ������ �ѱ沨
		cv::Mat camImg(WEBCAMH, WEBCAMW, CV_8UC3); //�ϴ� ���⿡�� 16:9�� ���߰� ���߿� ���簢������ �ٲܲ���

		cv::Mat frame(SCREENH, SCREENW, CV_8UC3);

		frame = cv::Scalar(0x11, 0x11, 0x11);
		_putText(frame, "Face Recognition", cv::Point(frame.cols - ((frame.cols - frame.rows) / 2), 30), 1, "���� ���", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x11, 0x11, 0x11));
		cv::line(frame, cv::Point(frame.rows + 1, 0), cv::Point(frame.rows + 1, frame.rows), cv::Scalar(0xED, 0xED, 0xED), 2);
		//������ �̸� �ϰ� �Ź� ���� �� ��ġ�� �����°� �ƴ� �͵� �κп��� ��� ���� ��� ������Ʈ �ǰ� �ϴ� ��.

		//int frameCnt = 0;
		//float timeElapsed = 0.0f;

		while (true) {
			std::cout << "check" << std::endl;

			{ //fps ���
				//frameCnt++;
				//if((mTimer))
			}

			//������ ������
			{
				//cols�� ���� , rows�� ����
				//��� �÷� ����
				//frame = cv::Scalar(0x11, 0x11, 0x11);
				cv::rectangle(frame, cv::Rect(cv::Point2i(frame.rows + 1 + 2, 30 + 60), cv::Point2i(frame.cols, frame.rows)), cv::Scalar(0x11, 0x11, 0x11), -1);

				//���� ����
				//_putText(frame, "Face Recognition", cv::Point(frame.cols - ((frame.cols - frame.rows) / 2), 30), 1, "���� ���", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x11, 0x11, 0x11));

				if (true) { //ī�޶� �о�� �Ҷ�
					if (!cap.read(camImg)) {
						setColor(12);
						std::cout << "��ķ���� �̹����� ���� �� �����ϴ�." << std::endl;
						goto webcamEnd; //��Ʈ������ ���� �� ������ �ǳʶٱ�
					}
					
					//���⼭ �̸� �ٲٴ� ������ ȭ�鿡�� ǥ�õ��� �ʴ� �κе� �����ϱ� ����
					camImg = camImg(cv::Range(0, WEBCAMH), cv::Range(WEBCAMCUT, WEBCAMCUT + WEBCAMH));

					dlib::cv_image<dlib::bgr_pixel> cimg(camImg); //�̹����� ��Ʈ���� ���·� ����
					auto face = detector(cimg);

					char faceText[30];
					sprintf_s(faceText, "�� %d�� ���� �Ǿ����ϴ�.", face.size());
					_putText(frame, faceText, cv::Point(frame.rows + LRMargin, 100), 0, "���� ���", FW_BOLD, 3, true, (face.size() >= 2 ? RGBScale(0xDA, 0x00, 0x37) : RGBScale(0xED, 0xED, 0xED)), RGBScale(0x11, 0x11, 0x11));

					if (face.size() >= 2) {
						setColor(12);
						std::cout << "���� ������ �ֳ׿�." << std::endl;
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
				
				if (true) { //ī�޶� �о�� �Ҷ�
					cv::resize(camImg, camImg, cv::Size(frame.rows, frame.rows));
					//cv::imshow("test", camImg);

					putImage(camImg, frame, cv::Rect(0, 0, frame.rows, frame.rows));
					//if (true) {
					//	_putText(frame, "��ķ ���� ����", cv::Point(frame.rows / 2, frame.rows / 2), 1, "���� ���", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x4E, 0x9F, 0x3D));
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

						//ĸ���� �� captureImg�� �ѱ�� ���� ������ ��� �Ѱܼ� ����� �ڿ� �ٽ� �����̰� �� ����
						cap.read(CaptureImg);
						CaptureImg = CaptureImg(cv::Range(0, WEBCAMH), cv::Range(WEBCAMCUT, WEBCAMCUT + WEBCAMH));
						//cv::imshow("test2", CaptureImg);

						if (Calculate_Comparison) {
							if (usecapture) {
								setColor(COLOR::YELLOW);
								std::cout << "��� ��" << std::endl;

								dlib::cv_image<dlib::bgr_pixel> Myface(CaptureImg);

								auto my_face = detector(Myface);

								dlib::matrix<dlib::rgb_pixel> face_chip;
								extract_image_chip(Myface, dlib::get_face_chip_details(sp(Myface, my_face[0]), 150, 0.25), face_chip); //150�� ���ϴ°Ŷ� dlib���� �⺻���� ��õ�ϴ°� ������

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
								std::cout << "���� ���� ���� " << celeb_names[faceLowDistance.faceNum] << ", �Ÿ� : " << faceLowDistance.distance << std::endl;
								setColor();
								//�׽�Ʈ�� �����ϴ°���
								dlib::save_jpeg(face_chip, "face_chip.jpg");
								dlib::save_jpeg(Myface, "My_Face.jpg");

								//�̹��� �̻ڰ� ���� ����ϴ� ��
								if (MakeExportImage) {
									cv::Mat outImg(CanvasHeight, CanvasWidth, CV_8UC3, cv::Scalar(255, 255, 255)); //Scalar�� ä�����

									dlib::matrix<dlib::bgr_pixel> celeb_face_chip, my_face_chip;
									extract_image_chip(celeb_faces[faceLowDistance.faceNum], dlib::get_face_chip_details(sp(celeb_faces[faceLowDistance.faceNum], detector(celeb_faces[faceLowDistance.faceNum])[0]), 150, 0.6).rect, celeb_face_chip); //get_face_chip_details �� �� ��ġ, ���μ��� ������(���簢��), ������� ������ ���ڰ� ������ ���� ���� ���Ե� rect�� �ϴ°� ȸ�� ���� ���ֱ� ����
									extract_image_chip(Myface, dlib::get_face_chip_details(sp(Myface, detector(Myface)[0]), 150, 0.6).rect, my_face_chip);
									//������ �κ��� ��ü �̹������� �� �κ��� CanvasWidth / 2 ���� ũ�� �߸� �� ����
									//�߻� ���� �̹������� ������� �Ÿ��� �ָ� �ڸ��� �Ǵµ� �� �Ÿ��� ������ �Ѿ������ �Ѿ���� ���·� �ڸ��� ������ �̷� �Ǵ°���

									cv::Mat cv_celeb_face_chip = dlib::toMat(celeb_face_chip);
									cv::Mat cv_my_face_chip = dlib::toMat(my_face_chip);
									cv::Mat cv_logo = dlib::toMat(logo);

									cv::resize(cv_celeb_face_chip, cv_celeb_face_chip, cv::Size(CanvasWidth / 2, CanvasWidth / 2));
									cv::resize(cv_my_face_chip, cv_my_face_chip, cv::Size(CanvasWidth / 2, CanvasWidth / 2));
									cv::resize(cv_logo, cv_logo, cv::Size(535, 147));

									putImage(cv_logo, outImg, cv::Rect(0, 0, 535, 147));

									putImage(cv_my_face_chip, outImg, cv::Rect(0, ImageStartY, CanvasWidth / 2, CanvasWidth / 2)); //Rect���� ���� ���ϰ� ũ�� ������ Rect(x1, y1, width, height)
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

									cv::rectangle(outImg, cv::Rect(0, ImageStartY, CanvasWidth / 2, CanvasWidth / 2), cv::Scalar(0, 0, 0), 3, 4, 0); //cv::copyMakeBorder �̰� �� �� �׵θ� ä������ ���� �̹��� ũ�Ⱑ �޶����� �ȵ�
									cv::rectangle(outImg, cv::Rect(CanvasWidth / 2, ImageStartY, CanvasWidth / 2, CanvasWidth / 2), cv::Scalar(0, 0, 0), 3, 4, 0); //3�� �β���

									//���� �Ʒ� �ؽ�Ʈ �ٲٴ°� �ϼ�
									//�ѱ� �ؽ�Ʈ
									char celebCompare[20];
									sprintf_s(celebCompare, "%.1f%%", (1 - faceLowDistance.distance) * 100);
									_putText(outImg, celebCompare, cvPoint(CanvasWidth, 95), 2, "���� ���", FW_BOLD, 6, true, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //�ѱ۷� �ǰ� �ٲ���� (�Ϸ�)
									_putText(outImg, "2022-11-08 ���� 03:16:32", cvPoint(CanvasWidth, 0), 2, "���� ���", FW_DEMIBOLD, 4, true, RGBScale(255, 0, 0), RGBScale(255, 255, 255));

									_putText(outImg, "��", cvPoint(CanvasWidth / 4, 745), 1, "���� ���", FW_BOLD, 6, true, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //CanvasWidth / 4 ù��° ������ �߰�
									_putText(outImg, celeb_names[faceLowDistance.faceNum], cvPoint(CanvasWidth / 4 * 3, 745), 1, "���� ���", FW_BOLD, 6, true, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //CanvasWidth / 4 * 3 �� 2��° ������ �߰��� ����Ű�� ����

									//cv::imshow("oImg", outImg);

									//cv::waitKey(); //�̰� ���ϸ� ���ɸ� Ű ��ٸ��� ��
									//cv::destroyAllWindows(); //�̹��� ���� ���°ǰ���

									cv::imwrite("01.jpg", outImg);
								}
							}
							else {
								setColor(COLOR::RED);
								std::cout << "���� �������� �ʾҽ��ϴ�." << std::endl;
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
						_putText(frame, CaptureCount, cv::Point(frame.rows / 2, frame.rows / 2), 1, "���� ���", FW_BOLD, 10, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x4E, 0x9F, 0x3D));

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
	

	
    //ķ ����
    
}