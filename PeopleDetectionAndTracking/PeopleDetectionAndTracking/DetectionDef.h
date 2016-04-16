#ifndef DETECTIONDEF_H_
#define DETECTIONDEF_H_

#include <string>
using namespace std;
///颜色
enum ObjectColor
{
	OBJECT_COLOR_RED = 0,
	OBJECT_COLOR_ORANGE = 1,
	OBJECT_COLOR_YELLOW = 2,
	OBJECT_COLOR_GREEN = 3,
	OBJECT_COLOR_CYAN = 4,//青色
	OBJECT_COLOR_BLUE = 5,
	OBJECT_COLOR_PURPLE = 6,
	OBJECT_COLOR_WHITE = 7,
	OBJECT_COLOR_BLACK = 8,
	OBJECT_COLOR_GRAY = 9,

};

string ColorEnum2String(ObjectColor _color);

///参数类，所有参数都存储在这里
class CPara
{
public:
	unsigned short CBlob_TrackPointNumThresh ;//物体轨迹点少于一定数目认为是干扰
	unsigned short CBlob_ContoursRectNumThresh ;//轮廓筛选，去除长宽小于20*20的轮廓
	unsigned short CBlob_DistanceThresh ; //两帧中心点最小距离阈值
	unsigned short CBlob_DistanceThreshKal ; //应用卡尔曼滤波预估时，预估点和当前帧中心点最小距离阈值

	unsigned short m_nResizeRatio;//缩放系数
public:
	CPara();//构造函数，初始化列表赋初值
	void Init(int TrackPointNumThresh, int ContoursRectNumThresh, int DistanceThresh, int DistanceThreshKal, int ResizeRatio);
	~CPara();

};

#endif