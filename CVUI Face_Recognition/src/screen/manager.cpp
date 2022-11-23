#include "screen/manager.h"

Screen::Screen(int width, int height) {
	this->ScreenW = width;
	this->ScreenH = height;
}

/*
void Screen::getScreen() {
	std::cout << this->ScreenW << " " << this->ScreenH << std::endl;
}
*/

/*
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
		_putText(frame, faceText, cv::Point(ScreenH + LRMargin, 100), 0, "맑은 고딕", FW_BOLD, 3, true, (faceNum >= 2 ? RGBScale(0xDA, 0x00, 0x37) : RGBScale(0xED, 0xED, 0xED)), RGBScale(0x11, 0x11, 0x11));


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
			_putText(frame, "웹캠 연결 없음", cv::Point(ScreenH / 2, ScreenH / 2), 1, "맑은 고딕", FW_BOLD, 6, true, RGBScale(0xED, 0xED, 0xED), RGBScale(0x4E, 0x9F, 0x3D));
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

