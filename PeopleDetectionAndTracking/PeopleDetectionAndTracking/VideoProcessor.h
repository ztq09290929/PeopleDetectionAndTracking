#ifndef _VIDEOPROCESSOR_
#define  _VIDEOPROCESSOR_
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>
#include <iostream>
#include <string>
#include <time.h>
#include "Cblob3.h"
#include "CImgproc.h"
#include "CColorAly.h"
using namespace cv;


class VideoProcessor
{
public:
	VideoProcessor();//构造函数，初始化列表赋初值
	~VideoProcessor();

	//void Init(GetInfoCallBackFun clsGetInfoCallBackFun, bool bLPR, bool bFace, char* pathName, int _height, int _width, int nResizeRatio);
	void Init(bool bLPR, bool bFace, char* pathName, int _height, int _width, int nResizeRatio);
	void GetObjectInfo(std::string& _objInfo);//以字符串形式返回当前帧物体信息
	//void Run(unsigned char* pBGR24, int _size, bool& _bObjectDistinguish, string& _strObjectInfo, int& _nObjectClass);//开始处理循环
	void Run(unsigned char* pBGR24, int _size, string& _strObjectInfo, int& _nObjectClass);//开始处理循环
	void onStop();
	string strObjectInfo;//目标特征信息（车牌字符或人脸存储路径）
	int nObjectClass;//目标类别（1为车，-1为人）

	cv::Mat		m_frame;//缩放之后的图像
private:
	int			m_height;//m_frame的高
	int			m_width;//m_frame的宽

	int			m_nResizeRatio;//缩放系数
	string		m_pathName;

	cv::Mat		m_foreImage;//前景图像


	cv::Mat		m_ori_image;//原始图像
	cv::Mat		m_element;//形态学结构元素

	CBlob m_CBlob;//跟踪类
	CImgproc m_CImgproc;//图像处理类
	//cv::Ptr<cv::BackgroundSubtractorMOG2> m_MOG2;//混合高斯类
	//Ptr<BackgroundSubtractorMOG2> m_MOG2;
	Ptr<BackgroundSubtractorKNN> m_KNN;
	cv::Mat m_TempImage;
};

#endif