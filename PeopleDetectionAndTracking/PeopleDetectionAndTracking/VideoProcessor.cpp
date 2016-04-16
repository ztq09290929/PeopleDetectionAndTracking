#include "stdafx.h"
#include "VideoProcessor.h"

using namespace cv;

VideoProcessor::VideoProcessor()
{
	
	//m_MOG2 =  cv::createBackgroundSubtractorMOG2(500, 20, true);//开启了阴影检测
	//m_MOG2->setNMixtures(5);//使用五个模型

	m_KNN = cv::createBackgroundSubtractorKNN(500, 300, true);//开启了阴影检测

	m_nResizeRatio = 1;//默认缩放系数为1
}

VideoProcessor::~VideoProcessor()
{
	/*if (m_MOG2 != NULL)
	{
		delete m_MOG2;
	}*/
}

void VideoProcessor::onStop()
{
	/*if (m_MOG2 != NULL)
	{
		delete m_MOG2;
	}*/

}

//void VideoProcessor::Init(GetInfoCallBackFun clsGetInfoCallBackFun, bool bLPR, bool bFace, char* pathName, int _height, int _width, int nResizeRatio)
void VideoProcessor::Init( bool bLPR, bool bFace, char* pathName, int _height, int _width, int nResizeRatio)
{
	//weigang  20160112
	m_pathName = pathName;

	m_nResizeRatio = nResizeRatio;

	m_height = _height;

	m_width = _width;

	m_TempImage = cv::Mat(m_height, m_width, CV_8UC3);

	//m_CBlob.Init(clsGetInfoCallBackFun, bLPR, bFace, m_pathName, 15, 20 / m_nResizeRatio, 50, 96, m_nResizeRatio);
	m_CBlob.Init(bLPR, bFace, m_pathName, 15, 20 / m_nResizeRatio, 50, 96, m_nResizeRatio);
}

void VideoProcessor::GetObjectInfo(std::string& _objInfo)
{
	m_CBlob.GetObjInfo(_objInfo);
}

//void VideoProcessor::Run(unsigned char* pBGR24, int _size, bool& _bObjectDistinguish, string& _strObjectInfo, int& _nObjectClass)
void VideoProcessor::Run(unsigned char* pBGR24, int _size, string& _strObjectInfo, int& _nObjectClass)
{
	//weigang add 20160120
	
	memmove(m_TempImage.data, pBGR24, _size);

	m_frame = m_TempImage.clone();
	//weigang add 20160120

	m_ori_image = m_frame.clone();//存储原始图像

	cv::resize(m_frame, m_frame, cv::Size(m_frame.cols / m_nResizeRatio, m_frame.rows / m_nResizeRatio));//调整图像为原来1/2
	
	cv::GaussianBlur(m_frame, m_frame, cv::Size(3, 3), 0, 0);


	//应用混合高斯得到前景图像,学习率0.005
	//m_MOG2->apply(m_frame, m_foreImage, 0.01);
	m_KNN->apply(m_frame, m_foreImage, 0.01);
	//反色，由于阴影部分的像素值为127，若想保留阴影，则阈值小于127，若想去除阴影，则阈值大于127
	cv::threshold(m_foreImage, m_foreImage, 130, 255, THRESH_BINARY);
	cv::medianBlur(m_foreImage, m_foreImage, 3);
	m_element = cv::getStructuringElement(MORPH_RECT, cv::Size(5, 5), cv::Point(-1, -1));
	cv::morphologyEx(m_foreImage, m_foreImage, cv::MORPH_CLOSE, m_element);
	m_CImgproc.FillHole(m_foreImage, m_foreImage);

	m_CBlob.BlobDetecter(m_foreImage, m_frame);
	//m_CBlob.ClassifyCenters(m_foreImage, m_frame, m_ori_image, _bObjectDistinguish, _strObjectInfo, _nObjectClass);
	m_CBlob.ClassifyCenters(m_foreImage, m_frame, m_ori_image, _strObjectInfo, _nObjectClass);
			
	m_CBlob.DrawPaths(m_frame);

	
	cv::imshow("InputImage", m_frame);
	cv::imshow("OutputFront", m_foreImage);
	
	cv::waitKey(1);
	
}

