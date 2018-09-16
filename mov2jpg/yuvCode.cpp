#include "yuvCode.h"
#include <iostream>


yuvCode::yuvCode()
{
}


yuvCode::~yuvCode()
{
}


//
// Created by vsoon on 2018/9/6.
//




uchar yuvCode::RGBtoY(uchar R, uchar G, uchar B)
{
	return (uchar)((0.257 * R) + (0.504 * G) + (0.098 * B) + 16);
}

uchar yuvCode::RGBtoU(uchar R, uchar G, uchar B)
{
	return (uchar)(-(0.148 * R) - (0.291 * G) + (0.439 * B) + 128);
}

uchar yuvCode::RGBtoV(uchar R, uchar G, uchar B)
{
	return (uchar)((0.439 * R) - (0.368 * G) - (0.071 * B) + 128);
}

void yuvCode::rgb2nv12(cv::Mat frame, uchar* pNV12Frame)//(uchar* rgb, signed long width, signed long height, int is_bottom_up)
{
	signed long	unDataPixels, unFrameBytes, x, y;
	// rgb data vars
	uchar 	*pRGBeven, *pRGBodd;
	uchar 	R, G, B;
	// nv12 data vars
	uchar	*pYPlane, *pUVPlane;
	uchar 	Y, U, V;
	uchar	U00, U01, U10, U11, V00, V01, V10, V11;
	//uchar 	*pNV12Frame = NULL;

	// check out the rgb data size conditions
	if ((frame.cols % 4) || (frame.rows % 4))
	{
		fprintf(stderr, "Error: The Width and Height must be multiple of Four");
		//return NULL;
	}

	unDataPixels = frame.cols * frame.rows;
	// Y plane + UV plane
	unFrameBytes = (unsigned long)(unDataPixels + (unDataPixels / 2));
	//fprintf(stderr, " Raw NV12 Frame Size: %d\n", (int)unFrameBytes);

	// allocate memory for NV12 frame data
	//if (!(pNV12Frame = (uchar*)malloc(unFrameBytes)))
	//{
	//	fprintf(stderr, "Error: Out of memory");
	//return NULL;
	//}
	memset(pNV12Frame, 0x00, unFrameBytes);

	pYPlane = pNV12Frame;
	pUVPlane = pNV12Frame + unDataPixels;

	// the algorithm below converts nv12 frame to implement
	// the following on-screen line scheme:
	//
	//          Y00   Y01  Y02   Y03  ... ...
	//           U00,V00    U01,V01     ...
	//          Y10   Y11  Y12   Y13  ... ...
	//
	// in other words, it processes the input rgb data by
	// pair of its lines (even & odd at time) to get U & V
	// color values from 4 neighbor pixels.


	for (y = 0; y < frame.rows; y += 2)
	{
		// set current even and odd line pointers
		//if (is_bottom_up)
		//{
		//	pRGBeven = frame.data + ((frame.rows - y - 1) * (frame.cols * 3));
		//	pRGBodd = frame.data + ((frame.rows - y - 2) * (frame.cols * 3));
		//}
		//else
		//{
		pRGBeven = frame.data + ((y + 0) * (frame.cols * 3));
		pRGBodd = frame.data + ((y + 1) * (frame.cols * 3));
		//}

		// process even Y pixels and all U, V pixels
		for (x = 0; x < frame.cols; x += 2)
		{
			B = *pRGBeven; 	++pRGBeven;
			G = *pRGBeven;	++pRGBeven;
			R = *pRGBeven;  ++pRGBeven;

			Y = RGBtoY(R, G, B);
			U00 = RGBtoU(R, G, B);
			V00 = RGBtoV(R, G, B);

			*pYPlane = Y;  ++pYPlane;

			B = *pRGBeven; 	++pRGBeven;
			G = *pRGBeven;	++pRGBeven;
			R = *pRGBeven;  ++pRGBeven;

			Y = RGBtoY(R, G, B);
			U01 = RGBtoU(R, G, B);
			V01 = RGBtoV(R, G, B);

			*pYPlane = Y;  ++pYPlane;

			B = *pRGBodd; 	++pRGBodd;
			G = *pRGBodd;	++pRGBodd;
			R = *pRGBodd;	++pRGBodd;

			U10 = RGBtoU(R, G, B);
			V10 = RGBtoV(R, G, B);

			B = *pRGBodd; 	++pRGBodd;
			G = *pRGBodd;	++pRGBodd;
			R = *pRGBodd;	++pRGBodd;

			U11 = RGBtoU(R, G, B);
			V11 = RGBtoV(R, G, B);

			U = (uchar)((U00 + U01 + U10 + U11) / 4);
			V = (uchar)((V00 + V01 + V10 + V11) / 4);

			*pUVPlane = U;	++pUVPlane;
			*pUVPlane = V;	++pUVPlane;
		}

		// set current odd line pointer back
		//if (is_bottom_up)
		//{
		//	pRGBodd = frame.data + ((frame.rows - y - 2) * (frame.cols * 3));
		//}
		//else
		//{
		pRGBodd = frame.data + ((y + 1) * (frame.cols * 3));
		//}

		// process odd Y pixels
		for (x = 0; x < frame.cols; ++x)
		{
			B = *pRGBodd; 	++pRGBodd;
			G = *pRGBodd;	++pRGBodd;
			R = *pRGBodd;	++pRGBodd;

			Y = RGBtoY(R, G, B);

			*pYPlane = Y;  ++pYPlane;
		}

	} // end of for(y)

	//return pNV12Frame;
}



void yuvCode::nv12trgb(uchar * image, CvSize _imgSize, IplImage * colorImage, CvRect validRect){

	memset(colorImage->imageData, 0, colorImage->width*colorImage->height * 3 * sizeof(uchar));

	int line, col;
	int y, u, v, yy, vr, ug, vg, ub;
	int r, g, b;
	uchar *py, *pu, *pv;


	for (size_t i = validRect.y, row = 0; i < validRect.height + validRect.y; i++, row++)
	{

		py = image + i*_imgSize.width + validRect.x;
		pu = image + _imgSize.width*_imgSize.height + (i / 2)*_imgSize.width + (validRect.x / 2) * 2;
		pv = pu + 1;
		y = *py++;
		yy = y << 8;
		u = *pu - 128;
		ug = 88 * u;
		ub = 454 * u;
		v = *pv - 128;
		vg = 183 * v;
		vr = 359 * v;

		uchar* onecolorImage = (uchar*)(colorImage->imageData + colorImage->widthStep*row);

		for (size_t j = validRect.x, col = 0; j < validRect.width + validRect.x; j++, col++)
		{

			r = (yy + vr) >> 8;
			g = (yy - ug - vg) >> 8;
			b = (yy + ub) >> 8;
			if (r < 0)   r = 0;
			if (r > 255) r = 255;
			if (g < 0)   g = 0;
			if (g > 255) g = 255;
			if (b < 0)   b = 0;
			if (b > 255) b = 255;

			onecolorImage[col * 3] = b;
			onecolorImage[col * 3 + 1] = g;
			onecolorImage[col * 3 + 2] = r;

			//oneOrgY[col] = g;

			y = *py++;
			yy = y << 8;
			if (j & 1) {
				pu += 2;
				pv = pu + 1;
				u = *pu - 128;
				ug = 88 * u;
				ub = 454 * u;
				v = *pv - 128;
				vg = 183 * v;
				vr = 359 * v;
			}
		}
	}



}


//yuv420 hal
void yuvCode::halveYUV420(uchar* data, uchar* yuv, int imageWidth, int imageHeight) {

	// halve yuma
	int i = 0;
	for (int y = 0; y < imageHeight; y += 2) {
		for (int x = 0; x < imageWidth; x += 2) {
			yuv[i] = data[y * imageWidth + x];
			i++;
		}
	}
	// halve U and V color components
	for (int y = 0; y < imageHeight / 2; y += 2) {
		for (int x = 0; x < imageWidth; x += 4) {
			yuv[i] = data[(imageWidth * imageHeight) + (y * imageWidth) + x];
			i++;
			yuv[i] = data[(imageWidth * imageHeight) + (y * imageWidth) + (x + 1)];
			i++;
		}
	}
}

//yuv420 hal
void yuvCode::cutYUV420(uchar* data, uchar* yuv, int imageWidth, int imageHeight, int cutRate){

	// halve yuma
	int i = 0;
	for (int y = 0; y < imageHeight; y += cutRate) {
		for (int x = 0; x < imageWidth; x += cutRate) {
			yuv[i] = data[y * imageWidth + x];
			i++;
		}
	}
	// halve U and V color components
	int widthCutStep = cutRate * 2;
	for (int y = 0; y < imageHeight / 2; y += cutRate) {
		for (int x = 0; x < imageWidth; x += widthCutStep) {
			yuv[i] = data[(imageWidth * imageHeight) + (y * imageWidth) + x];
			i++;
			yuv[i] = data[(imageWidth * imageHeight) + (y * imageWidth) + (x + 1)];
			i++;
		}
	}
}


//yuv420 hal and roi
void yuvCode::halveYUV420(uchar* data, uchar* yuv, int imageWidth, int imageHeight, int rectX, int rectY, int rectW, int rectH) {


	rectX = std::min(std::max(rectX, 0), imageWidth);
	rectY = std::min(std::max(rectY, 0), imageHeight);

	int rightX = rectX + rectW;
	int rightY = rectY + rectH;

	rightX = std::min(std::max(rightX, 0), imageWidth);
	rightY = std::min(std::max(rightY, 0), imageHeight);

	if (rectW == 0 || rectH == 0){
		rectX = 0;
		rectY = 0;
		rightX = imageWidth;
		rightY = imageHeight;
	}

	int halImageWidth = imageWidth / 2;
	int halImageHeight = imageHeight / 2;

	// halve yuma

	int i = 0;
	for (int y = rectY; y < rightY; y += 2) {
		for (int x = rectX; x < rightX; x += 2) {
			yuv[(y >> 1)*halImageWidth + (x >> 1)] = data[y * imageWidth + x];
		}
	}
	int offset = (imageWidth*imageHeight) >> 2;
	// halve U and V color components
	for (int y = rectY; y < rightY / 2; y += 2) {
		for (int x = rectX; x < rightX; x += 4) {
			yuv[offset + (y >> 1)*halImageWidth + (x >> 1)] = data[(imageWidth * imageHeight) + (y * imageWidth) + x];
			yuv[offset + (y >> 1)*halImageWidth + (x >> 1) + 1] = data[(imageWidth * imageHeight) + (y * imageWidth) + (x + 1)];
		}
	}


	/*
	int halImageWidth=imageWidth/2;
	int halImageHeight=imageHeight/2;

	// halve yuma
	int i = 0;
	for (int y = 0; y < imageHeight; y+=2) {
	for (int x = 0; x < imageWidth; x+=2) {
	yuv[(y>>1)*halImageWidth+(x>>1)] = data[y * imageWidth + x];
	}
	}
	int offset=(imageWidth*imageHeight)>>2;
	// halve U and V color components
	for (int y = 0; y < imageHeight / 2; y+=2) {
	for (int x = 0; x < imageWidth; x += 4) {
	yuv[offset+(y>>1)*halImageWidth+(x>>1)] = data[(imageWidth * imageHeight) + (y * imageWidth) + x];
	yuv[offset+(y>>1)*halImageWidth+(x>>1)+1] = data[(imageWidth * imageHeight) + (y * imageWidth) + (x + 1)];
	}
	}
	*/
}
