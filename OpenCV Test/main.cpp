#include "stdafx.h"


#define CanvasWidth 1196
#define CanvasHeight 803

#define OverlayImgSize 220

#define OverlayThickness 3
#define OverlayColor rgb_pixel(255, 0, 0)

#define ImageStartY 147

#define POS_LEFT 0
#define POS_CENTER 1 
#define POS_RIGHT 2

#define USE_OVERLAY true

using namespace std;
using namespace dlib;

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

void putImage(cv::Mat Image, cv::Mat& outImage, cv::Rect pos) {
    cv::Mat imageROI(outImage, pos);
    Image.copyTo(imageROI);
}

void putImage(matrix<bgr_pixel> Image, cv::Mat& outImage, cv::Rect pos) {
    cv::Mat faceImg = toMat(Image), imageROI(outImage, pos);
    faceImg.copyTo(imageROI);
}

//int fontWeight 추가 여러개의 굵기나 값들을 사용할려고
/*void _putText(cv::Mat& img, const cv::String& text, const cv::Point& org, const char* fontname, int fontWeight, double fontScale, cv::Scalar color) {
    //https://jitaku.work/it/category/image-processing/opencv/write_japanese/ //현재 이 코드
    //https://scorpion140309.blog.fc2.com/blog-entry-163.html
    //위 링크로 한글로 쓰는 방법 알아보기 (완료) SHIFTJIS_CHARSET -> DEFAULT_CHARSET
    int fontSize = (int)(10 * fontScale); // 10は適当 
    int width = img.cols;
    int height = fontSize * 3 / 2; // 高さはフォントサイズの1.5倍

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

    BITMAP  bitmap;
    ::GetObject(hbmp, sizeof(BITMAP), &bitmap);

    int back_color = 0x99;
    int memSize = (((bitmap.bmBitsPixel / 8) * width) & ~3) * height;
    std::memset(bitmap.bmBits, back_color, memSize);

    HFONT hFont = ::CreateFontA(
        fontSize, 0, 0, 0, fontWeight, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, //SHIFTJIS_CHARSET -> DEFAULT_CHARSET으로 변경
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        VARIABLE_PITCH | FF_ROMAN, fontname);
    ::SelectObject(hdc, hFont);

    ::TextOutA(hdc, 0, height / 3 * 1, text.c_str(), (int)text.length());

    int posX = org.x;
    int posY = org.y - fontSize + (10 * fontScale / 2.5); // + (10 * fontScale / 2.5) 추가

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
                if (_tmp[0] == 0 && _tmp[1] == 0 && _tmp[2] == 0) {
                    _img[0] = (unsigned char)color.val[0];
                    _img[1] = (unsigned char)color.val[1];
                    _img[2] = (unsigned char)color.val[2];
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
*/

//fontWeight는 폰트 두께, ori는 삽입을 어디 기준으로 할지 왼쪽 중간 오른쪽 중, anti는 안티 에일리어싱을 할지
void _putText(cv::Mat& img, const cv::String& text, const cv::Point& org, const int ori, const char* fontname, const int fontWeight, const double fontScale, const bool anti, const RGBScale textcolor, const RGBScale bkcolor) {
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


full_object_detection oneFaceDetect(frontal_face_detector detector, shape_predictor shape, matrix<rgb_pixel> img) {
    auto face = detector(img);
    if (face.size() != 1) {
        cout << "얼굴이 여러개 입니다.";
        exit(0);
    }
    return shape(img, face[0]);
}

full_object_detection oneFaceDetect(frontal_face_detector detector, shape_predictor shape, matrix<bgr_pixel> img) {
    auto face = detector(img);
    if (face.size() != 1) {
        cout << "얼굴이 여러개 입니다.";
        exit(0);
    }
    return shape(img, face[0]);
}

int main() {
    frontal_face_detector detector = get_frontal_face_detector();

    shape_predictor sp;
    deserialize("shape_predictor_68_face_landmarks_GTX.dat") >> sp;

    cv::Mat oImg(CanvasHeight, CanvasWidth, CV_8UC3, cv::Scalar(255, 255, 255)); //Scalar는 채우기임


    matrix<rgb_pixel> celeb_img, my_img, logo;
    load_image(celeb_img, "0.jpg");
    load_image(my_img, "1.jpg");
    load_image(logo, "logo.jpg");

    matrix<bgr_pixel> celeb_face_chip, my_face_chip; //rgb로 하면 색 반전됨 cv로 변환하면서 바뀌나봄

    extract_image_chip(celeb_img, get_face_chip_details(oneFaceDetect(detector, sp, celeb_img), CanvasWidth, 0.8).rect, celeb_face_chip); //get_face_chip_details 는 얼굴 위치, 가로세로 사이즈(정사각형), 어느정도 나올지 숫자가 높아질 수록 많이 포함됨 rect로 하는건 회전 값을 안주기 위해
    extract_image_chip(my_img, get_face_chip_details(oneFaceDetect(detector, sp, my_img), CanvasWidth, 0.8).rect, my_face_chip);

    matrix<bgr_pixel> celeb_resize_img(CanvasWidth / 2, CanvasWidth / 2),
        my_resize_img(CanvasWidth / 2, CanvasWidth / 2),
        resize_logo(147, 535); //리사이즈 해줄 때 크기 정하기

    resize_image(celeb_face_chip, celeb_resize_img); //앞에 있는걸 뒤에 에다가 딱 맞춰서 넣음
    resize_image(my_face_chip, my_resize_img);
    resize_image(logo, resize_logo);

    //cout << celeb_resize_img.nc() << " " << celeb_resize_img.nr() << endl; //n은 모르고 cols 하고 rows 같음
    //image_window win(logo);


    putImage(resize_logo, oImg, cv::Rect(0, 0, 535, 147));

    putImage(celeb_resize_img, oImg, cv::Rect(0, ImageStartY, CanvasWidth / 2, CanvasWidth / 2)); //Rect에서 왼쪽 위하고 크기 지정임 Rect(x1, y1, width, height)
    putImage(my_resize_img, oImg, cv::Rect(CanvasWidth / 2, ImageStartY, CanvasWidth / 2, CanvasWidth / 2));

    if (USE_OVERLAY) {
        matrix<bgr_pixel> celeb_overlay = celeb_face_chip, my_overlay = my_face_chip;
        matrix<bgr_pixel> celeb_overlay_resize(OverlayImgSize, OverlayImgSize), my_overlay_resize(OverlayImgSize, OverlayImgSize);

        draw_rectangle(celeb_overlay, get_face_chip_details(oneFaceDetect(detector, sp, celeb_overlay), CanvasWidth, 0.25).rect, OverlayColor, OverlayThickness);
        draw_rectangle(my_overlay, get_face_chip_details(oneFaceDetect(detector, sp, my_overlay), CanvasWidth, 0.25).rect, OverlayColor, OverlayThickness);

        resize_image(celeb_overlay, celeb_overlay_resize);
        resize_image(my_overlay, my_overlay_resize);

        putImage(celeb_overlay_resize, oImg, cv::Rect(0, 525, OverlayImgSize, OverlayImgSize));
        putImage(my_overlay_resize, oImg, cv::Rect(CanvasWidth - OverlayImgSize, 525, OverlayImgSize, OverlayImgSize));

        cv::rectangle(oImg, cv::Rect(0, 525, OverlayImgSize, OverlayImgSize), cv::Scalar(0, 0, 0), 3, 4, 0);
        cv::rectangle(oImg, cv::Rect(CanvasWidth - OverlayImgSize, 525, OverlayImgSize, OverlayImgSize), cv::Scalar(0, 0, 0), 3, 4, 0);
    }

    cv::rectangle(oImg, cv::Rect(0, ImageStartY, CanvasWidth / 2, CanvasWidth / 2), cv::Scalar(0, 0, 0), 3, 4, 0); //cv::copyMakeBorder 이건 밖 만 테두리 채워져서 사진 이미지 크기가 달라져서 안됨
    cv::rectangle(oImg, cv::Rect(CanvasWidth / 2, ImageStartY, CanvasWidth / 2, CanvasWidth / 2), cv::Scalar(0, 0, 0), 3, 4, 0); //3은 두께임

    //한글 텍스트
    _putText(oImg, "89.7% 마동석", cvPoint(CanvasWidth, 95), 2, "맑은 고딕", FW_BOLD, 6, true, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //한글로 되게 바꿔야함 (완료)
    _putText(oImg, "2022-11-08 오후 03:16:32", cvPoint(CanvasWidth, 0), 2, "맑은 고딕", FW_DEMIBOLD, 4, true, RGBScale(255, 0, 0), RGBScale(255, 255, 255));

    _putText(oImg, "나", cvPoint(CanvasWidth / 4, 745), 1, "맑은 고딕", FW_BOLD, 6, true, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //CanvasWidth / 4 첫번째 사진의 중간
    _putText(oImg, "마동석", cvPoint(CanvasWidth / 4 * 3, 745), 1, "맑은 고딕", FW_BOLD, 6, true, RGBScale(0, 0, 0), RGBScale(255, 255, 255)); //CanvasWidth / 4 * 3 는 2번째 사진의 중간을 가르키기 위해

    cv::imshow("oImg", oImg);

    cv::waitKey(); //이거 안하면 렉걸림 키 기다리는 듯
    cv::destroyAllWindows(); //이미지 띄운거 끄는건가봄

    cv::imwrite("01.jpg", oImg);


    cin.get();
}