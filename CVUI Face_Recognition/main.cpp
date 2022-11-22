#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <windows.h>

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define WINDOW_NAME	"Face Recognition (연예인 닮음 찾기)"

struct RGBScale
{
	int r;
	int g;
	int b;
	int rgb = RGB(r, g, b);
	
	RGBScale(int r, int g, int b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}
};

void putImage(cv::Mat Image, cv::Mat& outImage, cv::Rect pos) {
	cv::Mat imageROI(outImage, pos);
	Image.copyTo(imageROI);
}

//void putImage(matrix<bgr_pixel> Image, cv::Mat& outImage, cv::Rect pos) {
//	cv::Mat Img = toMat(Image), imageROI(outImage, pos);
//	Img.copyTo(imageROI);
//}

void _putText(cv::Mat& img, const cv::String& text, const cv::Point& org, int ori, const char* fontname, int fontWeight, double fontScale, bool anti, RGBScale textcolor, RGBScale bkcolor) {
	int fontSize = (int)(10 * fontScale);
	int width = img.cols;
	int height = fontSize * 3 / 2;

	HDC hdc = ::CreateCompatibleDC(NULL);

	//채우는 방법? 인 듯, 색을 이걸로 정하면 되는 듯
	HBRUSH hBrush = CreateSolidBrush(bkcolor.rgb);

	//처음 큰 비트맵의 색을 채우기 위해 크기를 지정한 것임
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

	//텍스트 배경 외의 색이 원래 검은색인데 채우는거임
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
		OUT_DEFAULT_PRECIS, //SHIFTJIS_CHARSET -> DEFAULT_CHARSET으로 변경
		CLIP_DEFAULT_PRECIS,
		(anti ? ANTIALIASED_QUALITY : DEFAULT_QUALITY), //안티를 허용하면 해주고 아니면 안해주고
		VARIABLE_PITCH | FF_ROMAN,
		fontname);
	::SelectObject(hdc, hFont);
	::SetTextColor(hdc, textcolor.rgb);
	::SetBkColor(hdc, bkcolor.rgb);

	//넓이 높이 구하기
	SIZE size;
	GetTextExtentPoint32A(hdc, text.c_str(), (int)text.length(), &size);

	::TextOutA(hdc, 0, height / 3 * 1, text.c_str(), (int)text.length());
	int posX = (ori == 0 ? org.x : (ori == 1 ? org.x - (size.cx / 2) : org.x - size.cx)); //기준이 왼쪽이면 그대로, 중간이면 기준점 - 넓이 / 2, 오른쪽이면 기준점 - 넓이
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

				if (_tmp[0] != bkcolor.b || _tmp[1] != bkcolor.g || _tmp[2] != bkcolor.r) { //순서를 bgr를 써서 bgr로 한거, rgb 순서이면 그걸로 해야함
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

/*
void getScreenSize(int& width, int& height) {
	RECT DesktopScreen;

	HWND hwnd = GetDesktopWindow();
	GetWindowRect(hwnd, DesktopScreen);

	width = DesktopScreen.right;
	height = DesktopScreen.bottom;
}
*/

int main()
{
	int ScreenW = 1280, ScreenH = 720;
	//getScreenSize(ScreenX, ScreenY);
	//std::cout << ScreenX << " " << ScreenY << std::endl;


	//opencv 윈도우 이름 정하는거
	cvui::init(WINDOW_NAME);

	double scaling = 1.0;
	double currentScaling = -1;

	int LRMargin = 20;
	int Margin = 30;

	bool useLand = true;

	cv::Mat frame(ScreenH, ScreenW, CV_8UC3);

	while (true) {
		//배경 컬러 지정
		frame = cv::Scalar(0x11, 0x11, 0x11);

		//메인 제목 
		//cvui::text(frame, 1112, 43, "Face Recognition", 0.96);
		_putText(frame, "Face Recognition", cv::Point(ScreenW - ((ScreenW - ScreenH) / 2), 30), 1, "맑은 고딕", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x11, 0x11, 0x11));

		int faceNum = 1;
		char faceText[40];
		sprintf_s(faceText, "얼굴 %d명 감지 되었습니다.", faceNum);
		_putText(frame, faceText, cv::Point(ScreenH + LRMargin, 100), 0, "맑은 고딕", FW_BOLD, 3, true, (faceNum >= 2 ? RGBScale(0xDA, 0x00, 0x37): RGBScale(0xED, 0xED, 0xED)), RGBScale(0x11, 0x11, 0x11));


		cvui::checkbox(frame, ScreenW - (200 + LRMargin), 100, "", &useLand, RGBScale(0xED, 0xED, 0xED).rgb, 1);


		// Buttons will return true if they were clicked, which makes
		// handling clicks a breeze.
		if (cvui::button(frame, std::lround(scaling * 50), std::lround(scaling * 60), "Colored Button", scaling * cvui::DEFAULT_FONT_SCALE, 0xa05050)) {
			std::cout << "Button clicked" << std::endl;
		}

		// If you do not specify the button width/height, the size will be
		// automatically adjusted to properly house the label.
		cvui::button(frame, std::lround(scaling * 200), std::lround(scaling * 70), "Button with large label", scaling * cvui::DEFAULT_FONT_SCALE);

		// You can tell the width and height you want
		cvui::button(frame, std::lround(scaling * 410), std::lround(scaling * 70), std::lround(scaling * 15), std::lround(scaling * 15), "x", scaling * cvui::DEFAULT_FONT_SCALE);

		cv::Mat tempImg(ScreenH, ScreenH, CV_8UC3);
		tempImg = cv::Scalar(0x3D, 0x9F, 0x4E);
		putImage(tempImg, frame, cv::Rect(0, 0, ScreenH, ScreenH));
		if (true) {
			_putText(frame, "웹캠 연결 없음", cv::Point(ScreenH / 2, ScreenH / 2), 1, "맑은 고딕", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x4E, 0x9F, 0x3D));
		}
		
		cv::line(frame, cv::Point(ScreenH + 1, 0), cv::Point(ScreenH + 1, ScreenH), cv::Scalar(0xED, 0xED, 0xED), 2);

		// The trackbar component can be used to create scales.
		// It works with all numerical types (including chars).
		//cvui::trackbar(frame, std::lround(scaling * 420), std::lround(scaling * 110), std::lround(scaling * 150), &trackbarValue, 0., 50., 1, "%.1Lf", 0, 1.0, scaling * cvui::DEFAULT_FONT_SCALE);

		// Checkboxes also accept a pointer to a variable that controls
		// the state of the checkbox (checked or not). cvui::checkbox() will
		// automatically update the value of the boolean after all
		// interactions, but you can also change it by yourself. Just
		// do "checked = true" somewhere and the checkbox will change
		// its appearance.
		//cvui::checkbox(frame, std::lround(scaling * 200), std::lround(scaling * 190), "Checkbox", &checked, 0x000000, scaling * cvui::DEFAULT_FONT_SCALE);
		//cvui::checkbox(frame, std::lround(scaling * 200), std::lround(scaling * 220), "A checked checkbox", &checked2, 0x000000, scaling * cvui::DEFAULT_FONT_SCALE);

		// This function must be called *AFTER* all UI components. It does
		// all the behind the scenes magic to handle mouse clicks, etc.
		cvui::update();

		// Show everything on the screen
		cv::imshow(WINDOW_NAME, frame);

		// Check if ESC key was pressed
		if (cv::waitKey(20) == 27) {
			break;
		}
	}

	return 0;
}