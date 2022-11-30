#include "stdafx.h"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define WINDOW_NAME	"Face Recognition (������ ���� ã��)"

#define SCREENW 1280
#define SCREENH 720

#define WEBCAMW 1280
#define WEBCAMH 720 //���� ũ���� �Է¹ް� �������� ©��� �� ��

#define WEBCAMCUT (WEBCAMW - WEBCAMH) / 2

#define camNum 0

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

	HDC hdc = ::CreateCompatibleDC(NULL);

	//ä��� ���? �� ��, ���� �̰ɷ� ���ϸ� �Ǵ� ��
	HBRUSH hBrush = CreateSolidBrush(bkcolor.rgb);

	//ó�� ū ��Ʈ���� ���� ä��� ���� ũ�⸦ ������ ����
	//https://stackoverflow.com/questions/53175915/creating-a-new-bitmap-with-given-dimensions-filled-with-given-color
	RECT rect;
	rect.left = rect.top = 0;
	rect.right = width;
	rect.bottom = height;

	BITMAPINFOHEADER header;
	::ZeroMemory(&header, sizeof(BITMAPINFOHEADER));
	header.biSize = sizeof(BITMAPINFOHEADER);
	header.biWidth = width;
	header.biHeight = height;
	header.biPlanes = 1;
	header.biBitCount = 24;
	BITMAPINFO bitmapInfo;
	bitmapInfo.bmiHeader = header;
	HBITMAP hbmp = ::CreateDIBSection(NULL, (LPBITMAPINFO)&bitmapInfo, DIB_RGB_COLORS, NULL, NULL, 0);
	::SelectObject(hdc, hbmp);

	//�ؽ�Ʈ ��� ���� ���� ���� �������ε� ä��°���
	FillRect(hdc, &rect, hBrush);

	BITMAP  bitmap;
	::GetObject(hbmp, sizeof(BITMAP), &bitmap);

	HFONT hFont = ::CreateFontA(
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
	::SelectObject(hdc, hFont);
	::SetTextColor(hdc, textcolor.rgb);
	::SetBkColor(hdc, bkcolor.rgb);

	//���� ���� ���ϱ�
	SIZE size;
	GetTextExtentPoint32A(hdc, text.c_str(), (int)text.length(), &size);

	::TextOutA(hdc, 0, height / 3 * 1, text.c_str(), (int)text.length());
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

	::DeleteObject(hFont);
	::DeleteObject(hbmp);
	::DeleteDC(hdc);
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

	std::string spPath = "Dlib_Model/shape_predictor_68_face_landmarks_GTX.dat";
	std::string fastspPath = "Dlib_Model/shape_predictor_5_face_landmarks.dat";

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
		}
		dlib::deserialize(spPath) >> sp;
		dlib::deserialize(fastspPath) >> fastsp;
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
		cap.set(cv::CAP_PROP_FRAME_HEIGHT, WEBCAMH); //��ķ�� ���� ����� ������

		//cv::Mat Test;
		//cap.read(Test);
		//cv::imshow("test", Test);

		int LRMargin = 20;
		int Margin = 30;

		bool useLand = true;

		cv::Mat camImg(WEBCAMH, WEBCAMW, CV_8UC3); //�ϴ� ���⿡�� 16:9�� ���߰� ���߿� ���簢������ �ٲܲ���

		cv::Mat frame(SCREENH, SCREENW, CV_8UC3);
		while (true) {
			std::cout << "check" << std::endl;
			//������ ������
			{
				//cols�� ���� , rows�� ����
				//��� �÷� ����
				frame = cv::Scalar(0x11, 0x11, 0x11);

				//���� ����
				_putText(frame, "Face Recognition", cv::Point(frame.cols - ((frame.cols - frame.rows) / 2), 30), 1, "���� ���", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x11, 0x11, 0x11));

				
				cvui::checkbox(frame, frame.cols - (200 + LRMargin), 100, "", &useLand, RGBScale(0xED, 0xED, 0xED).rgb, 10);
				if (cvui::button(frame, frame.rows + LRMargin, frame.rows - 100, "Capture", 0.5, 0xFF3333)) {
					setColor(COLOR::PURPLE);
					std::cout << "Capture" << std::endl;
					setColor();

					//3�� ���� ��Ĭ ��� �ϱ� 
				}

				if (true) { //ī�޶� �о�� �Ҷ�
					if (!cap.read(camImg)) {
						setColor(12);
						std::cout << "��ķ���� �̹����� ���� �� �����ϴ�." << std::endl;
						goto webcamEnd; //��Ʈ������ ���� �� ������ �ǳʶٱ�
					}

					dlib::cv_image<dlib::bgr_pixel> cimg(camImg); //�̹����� ��Ʈ���� ���·� ����
					auto face = detector(cimg);

					char faceText[30];
					sprintf_s(faceText, "�� %d�� ���� �Ǿ����ϴ�.", face.size());
					_putText(frame, faceText, cv::Point(frame.rows + LRMargin, 100), 0, "���� ���", FW_BOLD, 3, true, (face.size() >= 2 ? RGBScale(0xDA, 0x00, 0x37) : RGBScale(0xED, 0xED, 0xED)), RGBScale(0x11, 0x11, 0x11));

					if (face.size() >= 2) {
						setColor(12);
						std::cout << "���� ������ �ֳ׿�." << std::endl;
						setColor();
						goto webcamEnd;
					}
					else if (face.size() == 0)
						goto webcamEnd;

					auto faceRect = dlib::get_face_chip_details(fastsp(cimg, face[0]), 1, 0.2).rect;

					cv::rectangle(camImg, cv::Rect(faceRect.left(), faceRect.top(), faceRect.width(), faceRect.height()), cv::Scalar(0, 0, 255), 1, 4, 0);
				}

				webcamEnd:
				camImg = camImg(cv::Range(0, WEBCAMH), cv::Range(WEBCAMCUT, WEBCAMCUT + WEBCAMH));
				cv::resize(camImg, camImg, cv::Size(frame.rows, frame.rows));
				//cv::imshow("test", camImg);

				putImage(camImg, frame, cv::Rect(0, 0, frame.rows, frame.rows));
				//if (true) {
				//	_putText(frame, "��ķ ���� ����", cv::Point(frame.rows / 2, frame.rows / 2), 1, "���� ���", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x4E, 0x9F, 0x3D));
				//}

				cv::line(frame, cv::Point(frame.rows + 1, 0), cv::Point(frame.rows + 1, frame.rows), cv::Scalar(0xED, 0xED, 0xED), 2);
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