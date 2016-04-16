#include "stdafx.h"
#include "DetectionDef.h"

string ColorEnum2String(ObjectColor _color)
{
	string returnName = "ERROR";
	switch (_color)
	{
	case OBJECT_COLOR_RED:
		returnName = "RED";
		return returnName;
		break;
	case OBJECT_COLOR_ORANGE:
		returnName = "ORANGE";
		return returnName;
		break;
	case OBJECT_COLOR_YELLOW:
		returnName = "YELLOW";
		return returnName;
		break;
	case OBJECT_COLOR_GREEN:
		returnName = "GREEN";
		return returnName;
		break;
	case OBJECT_COLOR_BLUE:
		returnName = "BLUE";
		return returnName;
		break;
	case OBJECT_COLOR_CYAN:
		returnName = "CYAN";
		return returnName;
		break;
	case OBJECT_COLOR_PURPLE:
		returnName = "PURPLE";
		return returnName;
		break;
	case OBJECT_COLOR_BLACK:
		returnName = "BLACK";
		return returnName;
		break;
	case OBJECT_COLOR_WHITE:
		returnName = "WHITE";
		return returnName;
		break;
	case OBJECT_COLOR_GRAY:
		returnName = "GRAY";
		return returnName;
		break;
	default:
		return returnName;
		break;
	}
}

CPara::CPara()
{
	CBlob_TrackPointNumThresh = 15;//物体轨迹点少于一定数目认为是干扰
	CBlob_ContoursRectNumThresh = 10;//轮廓筛选，去除长宽小于4*resize_Ratio*4*resize_Ratio的轮廓
	CBlob_DistanceThresh = 50; //两帧中心点最小距离阈值
	CBlob_DistanceThreshKal = 96; //应用卡尔曼滤波预估时，预估点和当前帧中心点最小距离阈值
	m_nResizeRatio = 2;//缩放系数
}

void CPara::Init(int TrackPointNumThresh, int ContoursRectNumThresh, int DistanceThresh, int DistanceThreshKal, int ResizeRatio)
{
	CBlob_TrackPointNumThresh = TrackPointNumThresh;//物体轨迹点少于一定数目认为是干扰
	CBlob_ContoursRectNumThresh = ContoursRectNumThresh;//轮廓筛选，去除长宽小于4*resize_Ratio*4*resize_Ratio的轮廓
	CBlob_DistanceThresh = DistanceThresh; //两帧中心点最小距离阈值
	CBlob_DistanceThreshKal = DistanceThreshKal; //应用卡尔曼滤波预估时，预估点和当前帧中心点最小距离阈值
	m_nResizeRatio = ResizeRatio;//缩放系数
}

CPara::~CPara()
{
	CBlob_TrackPointNumThresh = 15;//物体轨迹点少于一定数目认为是干扰
	CBlob_ContoursRectNumThresh = 10;//轮廓筛选，去除长宽小于4*resize_Ratio*4*resize_Ratio的轮廓
	CBlob_DistanceThresh = 50; //两帧中心点最小距离阈值
	CBlob_DistanceThreshKal = 96; //应用卡尔曼滤波预估时，预估点和当前帧中心点最小距离阈值
	m_nResizeRatio = 2;//缩放系数
}