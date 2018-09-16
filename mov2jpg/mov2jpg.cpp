// mov2jpg.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include "nv12ConstImage320x240.h"

#define CONVERTMOIVE 1
#define YUV2PNG 0
#define CONVERTTOYUV 0
#define IMGTOYUV 0
#define VIDEO2YUV 0

using namespace std;
using namespace cv;



char BWhead[1078];
int Read_Image(char* file, unsigned char* data)
{
	FILE *fp = NULL; /* 需要注意 */

	fp = fopen(file, "rb");

	if (NULL == fp)
	{
		return -1; /* 要返回错误代码 */
	}

	fread(BWhead, 1, 1078, fp);

	//	fseek(fp,1078L,SEEK_SET);

	fread(data, 1, 640 * 480*1, fp);
	//	image_read(fp,image);

	fclose(fp);

	return 0;
}


//根据目录，查找所有图片
void readDirFileName(string myPath, vector<string>& myFiles,vector<string>& filesName)
{

	WIN32_FIND_DATA findData;
	HANDLE hFindFile;
	hFindFile=::FindFirstFile(myPath.c_str(),&findData);
	if(hFindFile!=INVALID_HANDLE_VALUE)
	{
		do
		{
			if(findData.cFileName[0]=='.')
				continue;

			string theName=myPath;
			theName.replace(theName.find("*.*"),3,findData.cFileName);

			myFiles.push_back(theName);
			filesName.push_back(findData.cFileName);

		}while(::FindNextFile(hFindFile,&findData));
	}
}


uchar RGBtoY(uchar R, uchar G, uchar B)
{
	return (uchar)((0.257 * R) + (0.504 * G) + (0.098 * B) + 16);
}

uchar RGBtoU(uchar R, uchar G, uchar B)
{
	return (uchar)(-(0.148 * R) - (0.291 * G) + (0.439 * B) + 128);
}

uchar RGBtoV(uchar R, uchar G, uchar B)
{
	return (uchar)((0.439 * R) - (0.368 * G) - (0.071 * B) + 128);
}

void rgb2nv12(IplImage* frame, uchar* pNV12Frame)//(uchar* rgb, signed long width, signed long height, int is_bottom_up)
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
	if ((frame->width % 4) || (frame->height % 4))
	{
		fprintf(stderr, "Error: The Width and Height must be multiple of Four");
		//return NULL;
	}

	unDataPixels = frame->width * frame->height;
	// Y plane + UV plane
	unFrameBytes = (unsigned long)(unDataPixels + (unDataPixels / 2));

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


	for (y = 0; y < frame->height; y += 2)
	{

		pRGBeven = (uchar*)frame->imageData + ((y + 0) * (frame->width * 3));
		pRGBodd = (uchar*)frame->imageData + ((y + 1) * (frame->width * 3));


		// process even Y pixels and all U, V pixels
		for (x = 0; x < frame->width; x += 2)
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
		pRGBodd = (uchar*)frame->imageData + ((y + 1) * (frame->width * 3));
		//}

		// process odd Y pixels 
		for (x = 0; x < frame->width; ++x)
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
#define pi2 3.141592654
#define LIMIT_V(v) ((v)-(int)((v)/pi2)*2*pi2)

#define LIMIT_V2(v) ((v)>0?((v)-(int)((v+pi2)/(2*pi2))*2*pi2):((v)-(int)((v-pi2)/(2*pi2))*2*pi2))


int main()
{


	double a = LIMIT_V2(-2.630098);

	double df = LIMIT_V2(1223.4);
	double fefe = cos(1223.4) - cos(df);

	//FileNode xx;
	//String dd, xxd;
	//cv::read(xx, dd, xxd);


	int fefefefe = 0;

	//ofstream out("out.txt");

	//Mat dddd = imread("00000003.jpg");

	//Mat eeee;

	//resize(dddd, eeee, Size(320, 240));

	//const int ee = 320 * 240 * 1.5;

	//uchar  frameData[ee];

	//rgb2nv12(&(IplImage)eeee, frameData);

	////uchar* one = (uchar*)(dddd.data);

	//for (size_t i = 0; i < ee; i++)
	//{
	//	char dd[20];
	//	sprintf(dd, "%d", frameData[i]);
	//	out << dd << ",";

	//}
	//out.close();


	//unsigned char datat[640*480*1];

	//Read_Image("2.bmp", datat);

	//Mat imgd = Mat(Size(640, 480), CV_8U, datat);

	//Read_Image("3.bmp", datat);


	//imshow("fef", imgd);
	//waitKey(0);

	//IplImage * img = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);

	//memcpy(img->imageData, datat, sizeof(unsigned char)* 640 * 480 * 1);

	//cv::imshow("ff", (cv::Mat)img);
	//cv::waitKey(0);

	
	//int eee = sizeof(myManNV12ImageData);

	//Mat ddd = Mat::zeros(Size(320, 240), CV_8UC1);

	//memcpy(ddd.data, myManNV12ImageData, sizeof(unsigned char)* 320 * 240 );

	//imshow("aaa",ddd);
	//waitKey(0);




#if CONVERTMOIVE
	CvCapture *capture = cvCreateFileCapture("E:\\lwg\\手势识别自研\\data\\VID_19700102_000517.mp4");
	int num = 1;

	int skip = 1000;

	while (1){

		Mat image = cvQueryFrame(capture);

		resize(image, image, Size(640, 480), 0, 0, CV_INTER_LINEAR);
		char xx[100];
		sprintf_s(xx, "%08d.jpg", num);
		num++;
		cout << xx;
		imwrite(xx, image);
		imshow("image", image);
		waitKey(3);

	}

#elif YUV2PNG

	//CvCapture *capture = cvCreateFileCapture("E:\\lwg\\视频转图片usingCv\\mov2jpg\\mov2jpg\\图像追踪（2）.mp4");
	int num = 1;


	vector<string> myFiles;
	vector<string> filesName;
	readDirFileName("E:\\lwg\\adb\\f01\\img_data\\*.*", myFiles, filesName);

	string outFile = "E:\\lwg\\adb\\f01\\data\\cam0\\data\\";

	Size imgSz = Size(528,384);
	//Size imgSz = Size(1056, 768);

	for (int i = 0; i < myFiles.size(); i++){

		Mat image = Mat::zeros(imgSz,CV_8UC1);

		string name = filesName[i].substr(0, filesName[i].size() - 4);

		FILE *fp = NULL;

		if ((fp = fopen(myFiles[i].c_str(), "rb+")) == NULL)
		{
			printf("open file %s error\n", myFiles[i].c_str());
			return -1;
		}

		fread((uchar*)image.data, imgSz.width*imgSz.height*sizeof(char), 1, fp);
		fclose(fp);

		//save png
		char xx[100];
		sprintf_s(xx, (outFile + "%s.png").c_str(), name.c_str());
		num++;
		cout << xx<<endl;
		imwrite(xx, image);
		imshow("image", image);
		waitKey(3);


	}


#elif CONVERTTOYUV
    //convert to yuv
	//CvCapture *capture = cvCreateFileCapture("E:\\lwg\\视频转图片usingCv\\mov2jpg\\mov2jpg\\图像追踪（2）.mp4");
	int num = 1;


	vector<string> myFiles;
	vector<string> filesName;
	readDirFileName("E:\\lwg\\SLAM\\data\\cam0\\data\\*.*", myFiles, filesName);

	string outFile = ".\\dstImg\\";




	for (int i = 0; i < myFiles.size();i++){

		Mat image = imread(myFiles[i],0);

		resize(image, image, Size(640, 480), 0, 0, CV_INTER_LINEAR);

		string name=filesName[i].substr(0, filesName[i].size() - 4);

		//char xx[100];
		//sprintf_s(xx, (outFile + "%s.bmp").c_str(), name.c_str());
		//num++;
		//cout << xx;
		//imwrite(xx, image);
		//imshow("image", image);
		//waitKey(3);

		//save yuv

	
		char xx[100];
		sprintf_s(xx, (outFile + "%s.yuv").c_str(), name.c_str());
		num++;

		ofstream out(xx, std::ios::out | ios::binary);
		out.write((char*)image.data, image.cols*image.rows*sizeof(char));//把字符串str1全部写到yyy.yyy中  
		out.close();

		imshow("image", image);
		waitKey(3);


	}

#elif IMGTOYUV


Mat img = imread("1.jpg");
int imgWidth = 2688;
int imgHeight = 1520;

Mat dst;
resize(img, dst, Size(imgWidth, imgHeight));

uchar *yuvImgData = (uchar*)malloc(imgWidth * imgHeight * 3 / 2);
rgb2nv12(&(IplImage)dst, yuvImgData);


ofstream out("out1.yuv", std::ios::out | ios::binary);
out.write((char*)yuvImgData, imgWidth * imgHeight * 3 / 2 * sizeof(char));//把字符串str1全部写到yyy.yyy中  
out.close();


#elif VIDEO2YUV

	CvCapture *capture = cvCreateFileCapture("D:\\Slo-Mo_20180110_151022.mp4");
	int num = 1;

	int skip = 1000;

	ofstream out("test_120.yuv", std::ios::out | ios::binary);
	while (1){

		Mat image = cvQueryFrame(capture);

		resize(image, image, Size(640, 480), 0, 0, CV_INTER_LINEAR);

		cvtColor(image, image, CV_RGB2GRAY);
		//char xx[100];
		//sprintf_s(xx, "%08d.jpg", num);
		//num++;
		//cout << xx;
		//imwrite(xx, image);


		out.write((char*)image.data, 640 * 480 * sizeof(char));//把字符串str1全部写到yyy.yyy中  



		imshow("image", image);
		waitKey(3);


	}

	out.close();



#endif


	return 0;
}

