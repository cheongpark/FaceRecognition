#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <windows.h>

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define WINDOW_NAME	"Face Recognition (연예인 닮음 찾기)"

struct Pos
{
	int x;
	int y;

	Pos(int x, int y) {
		this->x = x;
		this->y = y;
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

void _putText(cv::Mat& img, const cv::String& text, const cv::Point& org, int ori, const char* fontname, int fontWeight, double fontScale, bool anti, int color, int bkcolor) {
    int fontSize = (int)(10 * fontScale);
    int width = img.cols;
    int height = fontSize * 3 / 2;

    HDC hdc = ::CreateCompatibleDC(NULL);

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

	//FillRect(hdc, RECT(width, height), ) //hdc의 width, height의 모든 픽셀을 이 함수 호출할 때 마지막 값 bkcolor로 설정하기

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
    ::SetTextColor(hdc, color);
	::SetBkColor(hdc, bkcolor);

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

                //if (_tmp[0] == 0 && _tmp[1] == 0 && _tmp[2] == 0) //원래는 이거인데 배경색인 하얀색을 제거하기 위해 함
				if (_tmp[0] != bkcolor || _tmp[1] != bkcolor || _tmp[2] != bkcolor) {
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
	
	cv::Mat frame(ScreenH, ScreenW, CV_8UC3);

	while (true) {
		//배경 컬러 지정
		frame = cv::Scalar(17, 17, 17);

		//메인 제목 
		//cvui::text(frame, 1112, 43, "Face Recognition", 0.96);
		_putText(frame, "Face Recognition", cv::Point(ScreenW - ((ScreenW - ScreenH) / 2), 30), 1, "맑은 고딕", FW_BOLD, 6, true, RGB(0xED, 0xED, 0xED), RGB(17, 17, 17));

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
		tempImg = cv::Scalar(255, 0, 255);
		putImage(tempImg, frame, cv::Rect(0, 0, ScreenH, ScreenH));
		cv::rectangle(frame, cv::Rect(1, 1, ScreenH - 1, ScreenH - 1), cv::Scalar(255, 255, 255), 3, 4, 0);

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