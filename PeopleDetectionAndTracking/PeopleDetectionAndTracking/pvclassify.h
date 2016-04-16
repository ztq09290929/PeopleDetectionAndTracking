#ifndef _PV_CLASSIFY_
#define _PV_CLASSIFY_
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "ObjectAndKF.h"
using namespace std;
using namespace cv;

class PVClassify{
public:

	void ImportClassify0();//从指定路径将分类器导入m_classify0中
	int GetObjClassify(const vector<cv::Point>& _Contours, const cv::Rect& _rect);//获取某个轮廓的各个特征,并进行分类,分类结果-1为非机动车，1为机动车
	void GetSamples(ObjectAndKF& _obj);//将某一物体当前帧的6个特征输出到文件中
private:
	string m_classify_filepath = "../data/classifys/classify.txt";//分类器txt存储的相对路径,该分类器训练正确率99.1%
	string m_output_samples_filepath = "../data/ClassifyTrainer/dataSet.txt";//提取样本导出txt文件的路径

	vector<vector<double>> m_classify0;//存储人车adaboost分类器，每个样本6个特征，分类结果-1为非机动车，1为机动车,m_classify0第一维是每个弱分类器决策树，第二维是决策树的各个参数

	void GetObjRatio1323(const vector<cv::Point>& _contours, const cv::Rect& _rect, double & _ratio13, double& _ratio23);//获取物体高度与轮廓的1/3和2/3处宽度的比例
};
#endif