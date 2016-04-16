#ifndef _CCOLORALY_
#define  _CCOLORALY_
#include <opencv2/opencv.hpp>
#include <vector>
#include "DetectionDef.h"
using namespace cv;
using namespace std;

class CColorAly
{
public:

	void GetMaxRGB(cv::Rect _rect, vector<cv::Point> _Contours, cv::Mat& _colorROI, cv::Point3i& _colorRGB);//寻找RBG每个通道数量最多的颜色值返回
	void GetAverageRGB(cv::Rect _rect, vector<cv::Point> _Contours, cv::Mat& _colorROI, cv::Point3i& _colorRGB);//寻找RBG各个通道的平均值返回
	//void GetAverageHSV(cv::Rect _rect, cv::Mat& _colorROI, vector<ObjectColor> & _color, int _class);//传入长度为2的颜色向量，还有物体类别，0为非车，1为车
	void GetAverageHSV(cv::Rect _rect, cv::Mat& _colorROI,const cv::Mat& _binROI, vector<ObjectColor> & _color, int _class);//传入长度为2的颜色向量，还有物体类别，-1为非车，1为车
	void GetAverageHSV2(cv::Mat& _colorROI, const cv::Mat& _binROI, vector<ObjectColor> & _color, int _class);//传入长度为2的颜色向量，还有物体类别，-1为非车，1为车

	//输出RGB均值
	void GetAverageHSV3(cv::Mat& _colorROI, const cv::Mat& _binROI, vector<Vec3d> & _color, int _class);
private:
	enum ObjectColor color;
};

#endif /