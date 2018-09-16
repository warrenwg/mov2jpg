#pragma once
#include <iostream>
#include <vector>

#define IS_WINDOW 1
#define IS_LINUX 0
#define USE_OPENCV 1

#if IS_WINDOW
#include <Windows.h>
#include <io.h>
#include <direct.h>
#endif

#if USE_OPENCV
#include <opencv2\opencv.hpp>
#endif


enum IMAGE_FORMAT
{
	IMG_RGB=0,
	IMG_BGR,
	IMG_NV12

};

class ImageFileHandle
{
public:
	ImageFileHandle();
	~ImageFileHandle();
	void readImageFile(char* fileName,char* data,int& width,int& height,int& channel,int& format);
	void writeImageFile(char* data,int width,int height,int channel, char* fileName);

	void ImageFileHandle::readDirFileName(std::string myPath, std::vector<std::string>& filesName, std::string fileEnd);
};

