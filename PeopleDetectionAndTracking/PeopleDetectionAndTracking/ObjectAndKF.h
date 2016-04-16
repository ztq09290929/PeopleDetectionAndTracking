#pragma once
#include <opencv2/opencv.hpp>
#include <opencv/cv.h> 
#include <vector>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "DetectionDef.h"
#include "CColorAly.h"
//#include "FaceDetection_Interface.h"//人脸识别
//#include "LPR_Interface.h" //车牌识别

using namespace std;
using namespace cv;


class ObjectAndKF//物体跟踪类
{
public:
	ObjectAndKF() :m_objID(-1)
	{
		m_colorH.resize(2);//一个物体颜色分上半段和下半段
		//m_allClassifyResults = 0;
	}
	//	ObjectAndKF& operator = (const ObjectAndKF&);

	vector<cv::Point> m_vecCenters;	//存储该物体的所有轨迹点
	cv::Rect m_rect;				//存储最后一个轨迹点对应的最小外接矩形
	vector<cv::Point> m_Contours;	//存储最后一个轨迹点对应的轮廓
	int m_objID;					//存储物体的id号
	//int m_class;					//当前帧人车分类，-1表示非车，1表示车
	vector<Vec3d> m_colorH;			//该物体的颜色，分为上半段和下半段，枚举变量

	//int m_allClassifyResults;       //将所有帧人车分类结果累加求和,负表示非车，正表示车
	cv::Mat m_perfect_color_image;  //最佳的彩色ROI图像
	cv::Mat m_perfect_color_big_image;  //最佳的彩色ROI图像
	cv::Mat m_perfect_bin_image;	//最佳的前景ROI

	void Init(GetInfoCallBackFun clsGetInfoCallBackFun, string strPictureFolder, bool bLPR, bool bFace);					//初始化卡尔曼滤波器，需要传入m_vecCenters中的第一个点
	cv::Point Predict();			//使用卡尔曼滤波器预测下一点的位置，返回下一点
	void Correct(int num);			//使用图像中真实的中心点修正滤波器,num为m_vecCenters的索引

	void UpdatePerfectImages(const cv::Mat& _binImg, cv::Mat& _colorImg, cv::Mat& _ori_image,int nResizeRatio);//更新最佳图像
	//void GetFinalOBjInfo(bool& _bObjectDistinguish, string& ObjectInfo, int& _nObjectClass);			//当物体离开画面时，将信息输出
	void GetFinalOBjInfo( string& ObjectInfo, int& _nObjectClass);			//当物体离开画面时，将信息输出
private:
	string m_output_filepath;//当物体离开画面时，将信息输出到这里
	string m_output_imagepath; //当物体离开画面时,输出最佳图像
	string m_output_faceimgpath;//当人离开时，输出其头部图像
	string m_output_lprimgpath;//当人离开时，输出其头部图像

	bool m_bLPR;
	bool m_bFace;
	GetInfoCallBackFun m_clsGetInfoCallBackFun;

	const int stateNum = 4;
	const int measureNum = 2;
	KalmanFilter KF;
	Mat state; //state(x,y,detaX,detaY)  
	Mat processNoise;
	Mat measurement;    //measurement(x,y) 

	

};