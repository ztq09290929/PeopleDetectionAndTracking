#include "stdafx.h"
#include "VideoProcessor.h"

int main()
{
	//GetInfoCallBackFun clsGetInfoCallBackFun;
	char * path = " ";
	bool bDistin;
	int objClass;
	string objInfo;
	VideoProcessor videoProcessor;

	cv::VideoCapture VCcapture;

	//VCcapture.open("../VideoProcessor/images/视频2.mp4");
	VCcapture.open("../PeopleDetectionAndTracking/images/视频4-1-4m.mp4");
	if (!VCcapture.isOpened())
	{
		std::cout << "视频打开失败！" << std::endl;
		return -1;
	}

	cv::Mat temp_frame;
	VCcapture.set(CAP_PROP_POS_FRAMES,50);
	VCcapture >> temp_frame;

	//GetInfoCallBackFun clsGetInfoCallBackFun;
	//videoProcessor.Init(clsGetInfoCallBackFun, 1080, 1920, 3, 0, 540, 1905, 540, 900, 0, 900, 1020);
	//videoProcessor.Init(clsGetInfoCallBackFun, false, false, path,temp_frame.rows, temp_frame.cols, 3);
	videoProcessor.Init(false, false, path, temp_frame.rows, temp_frame.cols, 5);

	while (VCcapture.read(temp_frame))
	{
		videoProcessor.Run(temp_frame.data, temp_frame.rows * temp_frame.cols * 3, objInfo,objClass);
	}

	return 0;

}