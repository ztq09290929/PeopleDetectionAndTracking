#pragma once
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc_c.h>
#include <list> 
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "ObjectAndKF.h"
#include "CColorAly.h"
#include "DetectionDef.h"
//#include "pvclassify.h"

using namespace std;
using namespace cv;

class CBlob//物体查找类
{
public:
	CBlob();	//在构造函数中从txt导入人车分类器

	//void Init(GetInfoCallBackFun clsGetInfoCallBackFun, bool bLPR, bool bFace, string strPathName, int TrackPointNumThresh, int ContoursRectNumThresh, int DistanceThresh, int DistanceThreshKal, int nResizeRatio);
	void Init(bool bLPR, bool bFace, string strPathName, int TrackPointNumThresh, int ContoursRectNumThresh, int DistanceThresh, int DistanceThreshKal, int nResizeRatio);

	CColorAly m_CColorAly;//颜色分析类
	CPara m_para;//参数类
	//PVClassify m_pvclassify;//人车分类器

	void BlobDetecter(const cv::Mat& _binImg, cv::Mat& _outputImg);	//寻找前景物体中的较大轮廓，求其外接矩形和中心点还有轮廓
	//void ClassifyCenters(const cv::Mat& _binImg, cv::Mat& _colorImg, cv::Mat& _ori_image, bool& _bObjectDistinguish, string& _strObjectInfo, int& _nObjectClass);//当前帧的矩形中心归类到各个物体
	void ClassifyCenters(const cv::Mat& _binImg, cv::Mat& _colorImg, cv::Mat& _ori_image, string& _strObjectInfo, int& _nObjectClass);//当前帧的矩形中心归类到各个物体
	void DrawPaths(cv::Mat& _outputImg);							//画出轨迹

	int GetObjNum();		//返回当前画面中的目标个数（只包含有id的）
	void GetObjInfo(std::string& _objInfo);//以字符串形式返回当前帧物体信息
private:
	void FindNearstPointKal(cv::Point pLast, int& id);		//利用卡尔曼滤波方法，预估下一点位置，并在附近寻找下一点
	void FindNearstPoint(cv::Point pLast, int& id);			//利用一般方法求取某一点的附近的点

	list<ObjectAndKF> m_listObjects;		//用一个链表存储不同的物体，其中每一个ObjectAndKF对象就是一个物体，每一个物体都需要有个卡尔曼滤波器
	list<ObjectAndKF> m_listVanishObj;		//用来存储在画面中消失的物体，在画面四周消失的物体不存储

	vector<cv::Point> m_centers;			//每一帧找到的矩形中心点都存在这里
	vector<cv::Rect> m_rects;				//每一帧找到的外接矩形存在这里
	std::vector<std::vector<cv::Point>> m_goodContours;//每一帧筛选的轮廓存在这
	//vector<int> m_PorV;						//每一个轮廓的分类结果，-1表示非机动车，1表示机动车
	vector<vector<Vec3d>> m_colorsRGB;        //每一个轮廓的主要RBG值，每个轮廓有上半段和下半段两个颜色值

	long m_objCount;//统计物体总数

	string m_strPathName = "";

	int m_nResizeRatio = 0;

	GetInfoCallBackFun m_clsGetInfoCallBackFun;

	bool m_bLPR;
	bool m_bFace;

};
