// mov2jpg.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "ImageFileHandle.h"


#define CONVERTMOIVE 1
#define YUV2PNG 0
#define CONVERTTOYUV 0
#define IMGTOYUV 0
#define VIDEO2YUV 0

using namespace std;
using namespace cv;






#define FILE2FILE 1

#define MOV2FILE 0


#define SAVE_BGR 1



int main()
{

	ImageFileHandle tImageFileHandle;

#if FILE2FILE
	std::string filePath = "./hand_300/";
	std::vector<std::string> myFiles;
	tImageFileHandle.readDirFileName(filePath, myFiles,"jpg");

	for (int i = 0; i <myFiles.size(); i++){

		std::string imagePath = filePath + myFiles[i];

		cv::Mat frame=cv::imread(imagePath);


#if SAVE_BGR
		std::string bgr_path = filePath + "/bgr/";
		if (_access(bgr_path.c_str(), 0)){
			//create folder
			_mkdir(bgr_path.c_str());
		}

		char imagename[256] = { 0 };
		char* suffix = NULL;
		strcpy(imagename, (char*)myFiles[i].c_str());
		suffix = strrchr(imagename, '.');
		if (suffix != NULL){
			*suffix = '\0';
		}
		sprintf(imagename, "%s.bgr", imagename);

		std::string savename = bgr_path + imagename;
		FILE* pFile = fopen(savename.c_str(), "wb+");
		if (pFile != NULL)
		{
			fwrite(frame.data, frame.cols*frame.rows * frame.channels(), 1, pFile);
			fclose(pFile);
		}

		char filename[256] = { 0 };
		sprintf(filename, "{\"%s\",%d,%d},", imagename, frame.cols, frame.rows);
		savename = bgr_path + "/imageList.txt";
		pFile = fopen(savename.c_str(), "a + ");
		if (pFile != NULL)
		{
			fputs(filename, pFile);
			fclose(pFile);
		}

#endif

		cv::imshow("frame", frame);
		cv::waitKey(3);
	}


#elif MOV2FILE



#endif 



	return 0;
}

