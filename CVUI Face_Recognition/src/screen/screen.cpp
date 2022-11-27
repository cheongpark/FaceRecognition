#include "screen/screen.h"
#include "screen/cvui.h"

RGBScale::RGBScale(int r, int g, int b) {
	this->r = r;
	this->g = g;
	this->b = b;

	this->rgb = RGB(r, g, b);
}

void screenInit(int width, int height) {
	//opencv ������ �̸� ���ϴ°�
	cvui::init(WINDOW_NAME);

	cv::VideoCapture cap;
	camSetting(cap, height, height, 1);

	int LRMargin = 20;
	int Margin = 30;

	bool useLand = true;

	cv::Mat frame(height, width, CV_8UC3);
	while (true) {
		frameRender(frame, cap, LRMargin, Margin, useLand);
		cvui::update();

		cv::imshow(WINDOW_NAME, frame);

		if (cv::waitKey(20) == 27) {
			break;
		}
	}
}

void frameRender(cv::Mat &frame, cv::VideoCapture &cap, const int &LRMargin, const int &Margin, bool &useLand) {
	//ScreenW = frame.cols, ScreenH = frame.rows
	//std::cout << frame.cols << " " << frame.rows << std::endl;
	
	//��� �÷� ����
	frame = cv::Scalar(0x11, 0x11, 0x11);
	
	//���� ����
	_putText(frame, "Face Recognition", cv::Point(frame.cols - ((frame.cols - frame.rows) / 2), 30), 1, "���� ���", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x11, 0x11, 0x11));

	int faceNum = 1;
	char faceText[40];
	sprintf_s(faceText, "�� %d�� ���� �Ǿ����ϴ�.", faceNum);
	_putText(frame, faceText, cv::Point(frame.rows + LRMargin, 100), 0, "���� ���", FW_BOLD, 3, true, (faceNum >= 2 ? RGBScale(0xDA, 0x00, 0x37) : RGBScale(0xED, 0xED, 0xED)), RGBScale(0x11, 0x11, 0x11));

	cvui::checkbox(frame, frame.cols - (200 + LRMargin), 100, "", &useLand, RGBScale(0xED, 0xED, 0xED).rgb, 10);
	/*
	if (cvui::button(frame, std::lround(scaling * 50), std::lround(scaling * 60), "Colored Button", scaling * cvui::DEFAULT_FONT_SCALE, 0xa05050)) {
		std::cout << "Button clicked" << std::endl;
	}

	cvui::button(frame, std::lround(scaling * 200), std::lround(scaling * 70), "Button with large label", scaling * cvui::DEFAULT_FONT_SCALE);

	cvui::button(frame, std::lround(scaling * 410), std::lround(scaling * 70), std::lround(scaling * 15), std::lround(scaling * 15), "x", scaling * cvui::DEFAULT_FONT_SCALE);

	*/
	
	cv::Mat camImg(frame.rows, frame.rows, CV_8UC3);
	capFaceImage(camImg, cap);


	cv::resize(camImg, camImg, cv::Size(frame.rows, frame.rows));

	std::cout << "check" << std::endl;

	cv::imshow("test", camImg);

	//putImage(camImg, frame, cv::Rect(0, 0, frame.rows, frame.rows));
	//if (true) {
	//	_putText(frame, "��ķ ���� ����", cv::Point(frame.rows / 2, frame.rows / 2), 1, "���� ���", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x4E, 0x9F, 0x3D));
	//}

	cv::line(frame, cv::Point(frame.rows + 1, 0), cv::Point(frame.rows + 1, frame.rows), cv::Scalar(0xED, 0xED, 0xED), 2);
}

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

void webcamViewerMessage(std::string& text) {
	_putText(frame, "��ķ ���� ����", cv::Point(frame.rows / 2, frame.rows / 2), 1, "���� ���", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x4E, 0x9F, 0x3D));
}
/*
int main()
{
	int ScreenW = 1280, ScreenH = 720;
	//getScreenSize(ScreenX, ScreenY);
	//std::cout << ScreenX << " " << ScreenY << std::endl;


	//opencv ������ �̸� ���ϴ°�
	cvui::init(WINDOW_NAME);

	double scaling = 1.0;
	double currentScaling = -1;

	int LRMargin = 20;
	int Margin = 30;

	bool useLand = true;

	cv::Mat frame(ScreenH, ScreenW, CV_8UC3);

	while (true) {
		//��� �÷� ����
		frame = cv::Scalar(0x11, 0x11, 0x11);

		//���� ����
		//cvui::text(frame, 1112, 43, "Face Recognition", 0.96);
		_putText(frame, "Face Recognition", cv::Point(ScreenW - ((ScreenW - ScreenH) / 2), 30), 1, "���� ���", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x11, 0x11, 0x11));

		int faceNum = 1;
		char faceText[40];
		sprintf_s(faceText, "�� %d�� ���� �Ǿ����ϴ�.", faceNum);
		_putText(frame, faceText, cv::Point(ScreenH + LRMargin, 100), 0, "���� ���", FW_BOLD, 3, true, (faceNum >= 2 ? RGBScale(0xDA, 0x00, 0x37) : RGBScale(0xED, 0xED, 0xED)), RGBScale(0x11, 0x11, 0x11));


		cvui::checkbox(frame, ScreenW - (200 + LRMargin), 100, "d", &useLand, RGBScale(0xED, 0xED, 0xED).rgb, 10);


		if (cvui::button(frame, std::lround(scaling * 50), std::lround(scaling * 60), "Colored Button", scaling * cvui::DEFAULT_FONT_SCALE, 0xa05050)) {
			std::cout << "Button clicked" << std::endl;
		}

		cvui::button(frame, std::lround(scaling * 200), std::lround(scaling * 70), "Button with large label", scaling * cvui::DEFAULT_FONT_SCALE);

		cvui::button(frame, std::lround(scaling * 410), std::lround(scaling * 70), std::lround(scaling * 15), std::lround(scaling * 15), "x", scaling * cvui::DEFAULT_FONT_SCALE);

		cv::Mat tempImg(ScreenH, ScreenH, CV_8UC3);
		tempImg = cv::Scalar(0x3D, 0x9F, 0x4E);
		putImage(tempImg, frame, cv::Rect(0, 0, ScreenH, ScreenH));
		if (true) {
			_putText(frame, "��ķ ���� ����", cv::Point(ScreenH / 2, ScreenH / 2), 1, "���� ���", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x4E, 0x9F, 0x3D));
		}

		cv::line(frame, cv::Point(ScreenH + 1, 0), cv::Point(ScreenH + 1, ScreenH), cv::Scalar(0xED, 0xED, 0xED), 2);

		cvui::update();

		cv::imshow(WINDOW_NAME, frame);

		if (cv::waitKey(20) == 27) {
			break;
		}
	}

	return 0;
}
*/

