#include "screen/function.h"

RGBScale::RGBScale(int r, int g, int b) {
	this->r = r;
	this->g = g;
	this->b = b;
}

void putImage(cv::Mat Image, cv::Mat& outImage, cv::Rect pos) {
	cv::Mat imageROI(outImage, pos);
	Image.copyTo(imageROI);
}

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
void putImage(matrix<bgr_pixel> Image, cv::Mat& outImage, cv::Rect pos) {
	cv::Mat Img = toMat(Image), imageROI(outImage, pos);
	Img.copyTo(imageROI);
}
*/

/*
void getScreenSize(int& width, int& height) {
	RECT DesktopScreen;

	HWND hwnd = GetDesktopWindow();
	GetWindowRect(hwnd, DesktopScreen);

	width = DesktopScreen.right;
	height = DesktopScreen.bottom;
}
*/