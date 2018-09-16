#include "stdafx.h"
#include "ImageFileHandle.h"


ImageFileHandle::ImageFileHandle()
{
}


ImageFileHandle::~ImageFileHandle()
{
}


//����Ŀ¼����������ͼƬ
void ImageFileHandle::readDirFileName(std::string myPath, std::vector<std::string>& filesName,std::string fileEnd)
{
	//windows
	#if IS_WINDOW
	myPath.append("/*."+fileEnd);
		WIN32_FIND_DATA findData;
		HANDLE hFindFile;
		hFindFile = ::FindFirstFile(myPath.c_str(), &findData);
		if (hFindFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (findData.cFileName[0] == '.')
					continue;

				filesName.push_back(findData.cFileName);
			} while (::FindNextFile(hFindFile, &findData));
		}
	#else


	#endif
}
