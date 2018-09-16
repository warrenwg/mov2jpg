#pragma once
#include <opencv2/opencv.hpp>

class yuvCode
{
public:
	yuvCode();
	~yuvCode();

	uchar RGBtoY(uchar R, uchar G, uchar B);
	uchar RGBtoU(uchar R, uchar G, uchar B);
	uchar RGBtoV(uchar R, uchar G, uchar B);
	void rgb2nv12(cv::Mat frame, uchar* pNV12Frame);
	void nv12trgb(uchar * image, CvSize _imgSize, IplImage * colorImage, CvRect validRect);
	void halveYUV420(uchar* data, uchar* yuv, int imageWidth, int imageHeight);
	void cutYUV420(uchar* data, uchar* yuv, int imageWidth, int imageHeight, int cutRate);
	void halveYUV420(uchar* data, uchar* yuv, int imageWidth, int imageHeight, int rectX, int rectY, int rectW, int rectH);

};

