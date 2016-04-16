#include "stdafx.h"
#include "CColorAly.h"


void CColorAly::GetMaxRGB(cv::Rect _rect, vector<cv::Point> _Contours, cv::Mat& _colorROI, cv::Point3i& _colorRGB)
{
	cv::Mat ROI = _colorROI(_rect).clone();
	vector<cv::Point> ROIContours;
	for (auto it = _Contours.begin(); it != _Contours.end(); ++it)
	{
		cv::Point tem;
		tem.x = it->x - _rect.x;
		tem.y = it->y - _rect.y;
		ROIContours.push_back(tem);
	}

	vector<cv::Point3i> m_counts(256);
	for (int i = 0; i < ROI.rows; ++i)
	{
		Vec3b* imgData = ROI.ptr<Vec3b>(i);
		for (int j = 0; j < ROI.cols; ++j)
		{
			//if (cv::pointPolygonTest(ROIContours, cv::Point(j, i), 0) == 1)//如果点在轮廓里，才统计,耗费大量时间
			//{
				++(m_counts.at(imgData[j][0]).x);
				++(m_counts.at(imgData[j][1]).y);
				++(m_counts.at(imgData[j][2]).z);
			//}
		}
	}

	int countB(0), countG(0), countR(0);
	uchar idB(0), idG(0), idR(0);
	for (unsigned int k = 0; k < m_counts.size(); ++k)
	{
		if (m_counts[k].x > countB)
		{
			countB = m_counts[k].x;
			idB = (uchar)k;
		}
		if (m_counts[k].y > countG)
		{
			countG = m_counts[k].y;
			idG = (uchar)k;
		}
		if (m_counts[k].z > countR)
		{
			countR = m_counts[k].z;
			idR = (uchar)k;
		}
	}

	_colorRGB.x = idR;
	_colorRGB.y = idG;
	_colorRGB.z = idB;
}

void CColorAly::GetAverageRGB(cv::Rect _rect, vector<cv::Point> _Contours, cv::Mat& _colorROI, cv::Point3i& _colorRGB)
{
	cv::Mat ROI = _colorROI(_rect).clone();
	vector<double> sums(3);
	sums[0] = 0;
	sums[1] = 0;
	sums[2] = 0;

	for (int i = 0; i < ROI.rows; ++i)
	{
		Vec3b* imgData = ROI.ptr<Vec3b>(i);
		for (int j = 0; j < ROI.cols; ++j)
		{
			sums[0] += imgData[j][0];
			sums[1] += imgData[j][1];
			sums[2] += imgData[j][2];
		}
	}

	int num = ROI.rows*ROI.cols;

	_colorRGB.x = (uchar)(sums[2]/num);
	_colorRGB.y = (uchar)(sums[1] / num);
	_colorRGB.z = (uchar)(sums[0] / num);
}

//void CColorAly::GetAverageHSV(cv::Rect _rect, cv::Mat& _colorROI, const cv::Mat& _binROI, vector<ObjectColor> & _color, int _class)//传入长度为2的颜色向量，还有物体类别，0为非车，1为车
//{
//	Mat image = _colorROI.clone();
//	Mat binImage = _binROI.clone();
//	Mat ROI_HSV;
//	Mat ROI = image(_rect);
//	Mat binROI = binImage(_rect);
//	//Mat *RGB[2];
//	std::vector<cv::Mat> sbgr(ROI.channels());//通道分离用
//	std::vector<cv::Mat> mbgr(ROI.channels());//通道合成用
//	split(ROI, sbgr);
//	equalizeHist(sbgr[0], mbgr[0]);//三个通道分别直方图均衡化
//	equalizeHist(sbgr[1], mbgr[1]);
//	equalizeHist(sbgr[2], mbgr[2]);
//
//	merge(mbgr, ROI);//三个通道重新合成
//	cvtColor(ROI, ROI_HSV, COLOR_BGR2HSV);//此时H 0-180;
//	sbgr.clear();
//	split(ROI_HSV, sbgr);//分割出H通道
//	double sum_V = 0;
//	int pixelCounter = 0;//统计轮廓内像素点个数，计算平均的V用
//	int colorCount[6] = { 0 };//统计各个颜色点个数，红黄绿青蓝紫
//	if (_class == 0)//是人的话
//	{
//		//人上半身
//		for (int i = 0; i < (int)sbgr[0].rows/2; i++)
//		{
//			uchar* p0 = sbgr[0].ptr<uchar>(i);
//			uchar* p2 = sbgr[2].ptr<uchar>(i);
//			uchar* bin = binROI.ptr<uchar>(i);
//			for (int j = 0; j < sbgr[0].cols; j++)
//			{
//				if (bin[j] != 0)
//				{
//					pixelCounter++;
//					sum_V += p2[j];
//					p0[j] = p0[j] * 2;
//					if ((p0[j] >= 0 && p0[j] < 30) || (p0[j] >= 330 && p0[j] <= 360))
//						colorCount[0]++;//红色
//					else if (p0[j] >= 30 && p0[j] < 90)
//						colorCount[1]++;//黄色
//					else if (p0[j] >= 90 && p0[j] < 150)
//						colorCount[2]++;//绿色
//					else if (p0[j] >= 150 && p0[j] < 210)
//						colorCount[3]++;//青
//					else if (p0[j] >= 210 && p0[j] < 270)
//						colorCount[4]++;//蓝
//					else if (p0[j] >= 270 && p0[j] < 330)
//						colorCount[5]++;//紫
//				}
//			}
//		}
//		double  value_V = sum_V / pixelCounter;//计算平均的V值
//		int maxTem = 0;
//		int flag = 0;
//		if (value_V > 221 && value_V <= 255)
//		{
//			_color[0] = OBJECT_COLOR_WHITE;
//		}
//		else if (value_V >= 0 && value_V <= 46)
//		{
//			_color[0] = OBJECT_COLOR_BLACK;
//		}
//		else
//		{
//			maxTem = colorCount[0];
//			flag = 0;
//			for (int i = 1; i < 6;i++)//寻找那种颜色点最多
//			{
//				if (maxTem<colorCount[i])
//				{
//				    maxTem = colorCount[i];
//					flag = i;
//				}
//			}
//			switch (flag)
//			{
//			case 0:
//				_color[0] = OBJECT_COLOR_RED;
//				break;
//			case 1:
//				_color[0] = OBJECT_COLOR_YELLOW;
//				break;
//			case 2:
//				_color[0] = OBJECT_COLOR_GREEN;
//				break;
//			case 3:
//				_color[0] = OBJECT_COLOR_CYAN;
//				break;
//			case 4:
//				_color[0] = OBJECT_COLOR_BLUE;
//				break;
//			case 5:
//				_color[0] = OBJECT_COLOR_PURPLE;
//				break;
//			default:
//				_color[0] = OBJECT_COLOR_WHITE;
//				break;
//			}
//		}
//		//人下半身
//		pixelCounter = 0;//计数器清零
//		flag = 0;
//		sum_V = 0;
//		for (int i = 0; i < 6;i++)
//		{
//			colorCount[i] = 0;
//		}
//		for (int i = (int)sbgr[0].rows / 2; i < sbgr[0].rows; i++)
//		{
//			uchar* p0 = sbgr[0].ptr<uchar>(i);
//			uchar* p2 = sbgr[2].ptr<uchar>(i);
//			uchar* bin = binROI.ptr<uchar>(i);
//			for (int j = 0; j < sbgr[0].cols; j++)
//			{
//				if (bin[j] != 0)
//				{
//					pixelCounter++;
//					sum_V += p2[j];
//					p0[j] = p0[j] * 2;
//					if ((p0[j] >= 0 && p0[j] < 30) || (p0[j] >= 330 && p0[j] <= 360))
//						colorCount[0]++;//红色
//					else if (p0[j] >= 30 && p0[j] < 90)
//						colorCount[1]++;//黄色
//					else if (p0[j] >= 90 && p0[j] < 150)
//						colorCount[2]++;//绿色
//					else if (p0[j] >= 150 && p0[j] < 210)
//						colorCount[3]++;//青
//					else if (p0[j] >= 210 && p0[j] < 270)
//						colorCount[4]++;//蓝
//					else if (p0[j] >= 270 && p0[j] < 330)
//						colorCount[5]++;//紫
//				}
//			}
//		}
//		value_V = sum_V / pixelCounter;//计算平均的V值
//		maxTem = 0;
//		if (value_V > 221 && value_V <= 255)
//		{
//			_color[1] = OBJECT_COLOR_WHITE;
//		}
//		else if (value_V >= 0 && value_V <= 46)
//		{
//			_color[1] = OBJECT_COLOR_BLACK;
//		}
//		else
//		{
//			maxTem = colorCount[0];
//			flag = 0;
//			for (int i = 1; i < 6; i++)//寻找那种颜色点最多
//			{
//				if (maxTem < colorCount[i])
//				{
//					maxTem = colorCount[i];
//					flag = i;
//				}
//			}
//			switch (flag)
//			{
//			case 0:
//				_color[1] = OBJECT_COLOR_RED;
//				break;
//			case 1:
//				_color[1] = OBJECT_COLOR_YELLOW;
//				break;
//			case 2:
//				_color[1] = OBJECT_COLOR_GREEN;
//				break;
//			case 3:
//				_color[1] = OBJECT_COLOR_CYAN;
//				break;
//			case 4:
//				_color[1] = OBJECT_COLOR_BLUE;
//				break;
//			case 5:
//				_color[1] = OBJECT_COLOR_PURPLE;
//				break;
//			default:
//				_color[1] = OBJECT_COLOR_WHITE;
//				break;
//			}
//		}
//	}
//	else//class==1是车
//	{
//		for (int i = 0; i < sbgr[0].rows; i++)
//		{
//			uchar* p0 = sbgr[0].ptr<uchar>(i);
//			uchar* p2 = sbgr[2].ptr<uchar>(i);
//			uchar* bin = binROI.ptr<uchar>(i);
//			for (int j = 0; j < sbgr[0].cols; j++)
//			{
//				if (bin[j] != 0)
//				{
//					pixelCounter++;
//					sum_V += p2[j];
//					p0[j] = p0[j] * 2;
//					if ((p0[j] >= 0 && p0[j] < 30) || (p0[j] >= 330 && p0[j] <= 360))
//						colorCount[0]++;//红色
//					else if (p0[j] >= 30 && p0[j] < 90)
//						colorCount[1]++;//黄色
//					else if (p0[j] >= 90 && p0[j] < 150)
//						colorCount[2]++;//绿色
//					else if (p0[j] >= 150 && p0[j] < 210)
//						colorCount[3]++;//青
//					else if (p0[j] >= 210 && p0[j] < 270)
//						colorCount[4]++;//蓝
//					else if (p0[j] >= 270 && p0[j] < 330)
//						colorCount[5]++;//紫
//				}
//			}
//		}
//		double  value_V = sum_V / pixelCounter;//计算平均的V值
//		if (value_V > 221 && value_V <= 255)
//		{
//			_color[0] = OBJECT_COLOR_WHITE;
//			_color[1] = OBJECT_COLOR_WHITE;
//		}
//		else if (value_V >= 0 && value_V <= 46)
//		{
//			_color[0] = OBJECT_COLOR_BLACK;
//			_color[1] = OBJECT_COLOR_BLACK;
//		}
//		else
//		{
//			int maxTem = colorCount[0];
//			int flag = 0;
//			for (int i = 1; i < 6; i++)//寻找那种颜色点最多
//			{
//				if (maxTem < colorCount[i])
//				{
//					maxTem = colorCount[i];
//					flag = i;
//				}
//			}
//			switch (flag)
//			{
//			case 0:
//				_color[0] = OBJECT_COLOR_RED;
//				_color[1] = OBJECT_COLOR_RED;
//				break;
//			case 1:
//				_color[0] = OBJECT_COLOR_YELLOW;
//				_color[1] = OBJECT_COLOR_YELLOW;
//				break;
//			case 2:
//				_color[0] = OBJECT_COLOR_GREEN;
//				_color[1] = OBJECT_COLOR_GREEN;
//				break;
//			case 3:
//				_color[0] = OBJECT_COLOR_CYAN;
//				_color[1] = OBJECT_COLOR_CYAN;
//				break;
//			case 4:
//				_color[0] = OBJECT_COLOR_BLUE;
//				_color[1] = OBJECT_COLOR_BLUE;
//				break;
//			case 5:
//				_color[0] = OBJECT_COLOR_PURPLE;
//				_color[1] = OBJECT_COLOR_PURPLE;
//				break;
//			default:
//				_color[0] = OBJECT_COLOR_WHITE;
//				_color[1] = OBJECT_COLOR_WHITE;
//				break;
//			}
//		}
//	}
//}

void CColorAly::GetAverageHSV(cv::Rect _rect, cv::Mat& _colorROI, const cv::Mat& _binROI, vector<ObjectColor> & _color, int _class)//传入长度为2的颜色向量，还有物体类别，0为非车，1为车
{
	int whiteGrayThreshold = 125;//HSV白灰两色亮度V区分值，原理为221
	int blackGrayThresholdV = 46;//HSV黑灰两色亮度V区分值，原理为46
	Mat binImage = _binROI.clone(); 
	Mat image = _colorROI.clone();
	Mat ROI_HSV;
	Mat ROI = image(_rect);
	Mat binROI = binImage(_rect);
	
	std::vector<cv::Mat> sbgr(ROI.channels());//通道分离用
	std::vector<cv::Mat> mbgr(ROI.channels());//通道合成用
	//split(ROI, sbgr);
	//equalizeHist(sbgr[0], mbgr[0]);//三个通道分别直方图均衡化
	//equalizeHist(sbgr[1], mbgr[1]);
	//equalizeHist(sbgr[2], mbgr[2]);
	//merge(mbgr, ROI);//三个通道重新合成

	cvtColor(ROI, ROI_HSV, COLOR_BGR2HSV);//此时H 0-180;
	sbgr.clear();
	split(ROI_HSV, sbgr);//分割出H通道

	int colorCount[10] = { 0 };//统计每个颜色像素点个数，顺序为  红橙黄绿青蓝紫白黑

	if (_class == -1)//判断是人
	{
		//统计人上半部分的颜色
		for (int i = 0; i < (int)(sbgr[0].rows / 2); i++)
		{
			uchar* p0 = sbgr[0].ptr<uchar>(i);
			uchar* p1 = sbgr[1].ptr<uchar>(i);
			uchar* p2 = sbgr[2].ptr<uchar>(i);
			uchar* bin = binROI.ptr<uchar>(i);
			for (int j = 0; j < sbgr[0].cols; j++)
			{
				if (bin[j] != 0)
				{
					if ((p0[j] >= 0 && p0[j] <= 10 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255) || (p0[j] >= 156 && p0[j] <= 180 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255))
					{
						colorCount[0]+=2;//红色red
					}
					else if (p0[j] >= 11 && p0[j] <= 25 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[1]+=2;//橙色orange
					}
					else if (p0[j] >= 26 && p0[j] <= 34 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[2]+=2;//黄色yellow
					}
					else if (p0[j] >= 35 && p0[j] <= 77 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[3]+=2;//绿色green
					}
					else if (p0[j] >= 78 && p0[j] <= 99 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[4]+=2;//青色cyan
					}
					else if (p0[j] >= 100 && p0[j] <= 124 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[5]+=2;//蓝色blue
					}
					else if (p0[j] >= 125 && p0[j] <= 155 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[6]+=2;//紫色purple
					}
					else if (p0[j] >= 0 && p0[j] <= 180 && p1[j] >= 0 && p1[j] <= 30 && p2[j] >= whiteGrayThreshold && p2[j] <= 255)
					{
						colorCount[7]++;//白色white
					}
					else if (p0[j] >= 0 && p0[j] <= 180 && p1[j] >= 0 && p1[j] <= 255 && p2[j] >= 0 && p2[j] <= blackGrayThresholdV)
					{
						colorCount[8]++;//黑色black
					}
					else
					{
						colorCount[9]++;//灰色gray;
					}
				}
			}

		}
		int maxTemp = colorCount[0];
		int flag = 0;
		for (int i = 1; i < 10; i++)
		{
			if (maxTemp <= colorCount[i])
			{
				maxTemp = colorCount[i];
				flag = i;
			}
		}
		switch (flag)
		{
		case 0:
			_color[0] = OBJECT_COLOR_RED;
			break;
		case 1:
			_color[0] = OBJECT_COLOR_ORANGE;
			break;
		case 2:
			_color[0] = OBJECT_COLOR_YELLOW;
			break;
		case 3:
			_color[0] = OBJECT_COLOR_GREEN;
			break;
		case 4:
			_color[0] = OBJECT_COLOR_CYAN;
			break;
		case 5:
			_color[0] = OBJECT_COLOR_BLUE;
			break;
		case 6:
			_color[0] = OBJECT_COLOR_PURPLE;
			break;
		case 7:
			_color[0] = OBJECT_COLOR_WHITE;
			break;
		case 8:
			_color[0] = OBJECT_COLOR_BLACK;
			break;
		case 9:
			_color[0] = OBJECT_COLOR_GRAY;
			break;
		default:
			_color[0] = OBJECT_COLOR_WHITE;
			break;
		}


		//判断物体下半部分
		for (int i = 0; i < 10; i++)
		{
			colorCount[i] = 0;
		}
		maxTemp = 0;
		for (int i = (int)(sbgr[0].rows / 2); i < sbgr[0].rows; i++)
		{
			uchar* p0 = sbgr[0].ptr<uchar>(i);
			uchar* p1 = sbgr[1].ptr<uchar>(i);
			uchar* p2 = sbgr[2].ptr<uchar>(i);
			uchar* bin = binROI.ptr<uchar>(i);
			for (int j = 0; j < sbgr[0].cols; j++)
			{
				if (bin[j] != 0)
				{
					if ((p0[j] >= 0 && p0[j] <= 10 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255) || (p0[j] >= 156 && p0[j] <= 180 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255))
					{
						colorCount[0] += 2;//红色red
					}
					else if (p0[j] >= 11 && p0[j] <= 25 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[1] += 2;//橙色orange
					}
					else if (p0[j] >= 26 && p0[j] <= 34 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[2] += 2;//黄色yellow
					}
					else if (p0[j] >= 35 && p0[j] <= 77 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[3] += 2;//绿色green
					}
					else if (p0[j] >= 78 && p0[j] <= 99 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[4] += 2;//青色cyan
					}
					else if (p0[j] >= 100 && p0[j] <= 124 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[5] += 2;//蓝色blue
					}
					else if (p0[j] >= 125 && p0[j] <= 155 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[6] += 2;//紫色purple
					}
					else if (p0[j] >= 0 && p0[j] <= 180 && p1[j] >= 0 && p1[j] <= 30 && p2[j] >= whiteGrayThreshold && p2[j] <= 255)
					{
						colorCount[7]++;//白色white
					}
					else if (p0[j] >= 0 && p0[j] <= 180 && p1[j] >= 0 && p1[j] <= 255 && p2[j] >= 0 && p2[j] <= blackGrayThresholdV)
					{
						colorCount[8]++;//黑色black
					}
					else
					{
						colorCount[9]++;//灰色gray;
					}
				}

			}

		}
		maxTemp = colorCount[0];
		flag = 0;
		for (int i = 1; i < 10; i++)
		{
			if (maxTemp <= colorCount[i])
			{
				maxTemp = colorCount[i];
				flag = i;
			}
		}
		switch (flag)
		{
		case 0:
			_color[1] = OBJECT_COLOR_RED;
			break;
		case 1:
			_color[1] = OBJECT_COLOR_ORANGE;
			break;
		case 2:
			_color[1] = OBJECT_COLOR_YELLOW;
			break;
		case 3:
			_color[1] = OBJECT_COLOR_GREEN;
			break;
		case 4:
			_color[1] = OBJECT_COLOR_CYAN;
			break;
		case 5:
			_color[1] = OBJECT_COLOR_BLUE;
			break;
		case 6:
			_color[1] = OBJECT_COLOR_PURPLE;
			break;
		case 7:
			_color[1] = OBJECT_COLOR_WHITE;
			break;
		case 8:
			_color[1] = OBJECT_COLOR_BLACK;
			break;
		case 9:
			_color[1] = OBJECT_COLOR_GRAY;
			break;
		default:
			_color[1] = OBJECT_COLOR_WHITE;
			break;
		}
	}
	else//车的时候 class==2
	{
		for (int i = 0; i < sbgr[0].rows; i++)
		{
			uchar* p0 = sbgr[0].ptr<uchar>(i);
			uchar* p1 = sbgr[1].ptr<uchar>(i);
			uchar* p2 = sbgr[2].ptr<uchar>(i);
			uchar* bin = binROI.ptr<uchar>(i);
			for (int j = 0; j < sbgr[0].cols; j++)
			{
				if (bin[j] != 0)
				{
					if ((p0[j] >= 0 && p0[j] <= 10 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255) || (p0[j] >= 156 && p0[j] <= 180 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255))
					{
						colorCount[0] += 2;//红色red
					}
					else if (p0[j] >= 11 && p0[j] <= 25 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[1] += 2;//橙色orange
					}
					else if (p0[j] >= 26 && p0[j] <= 34 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[2] += 2;//黄色yellow
					}
					else if (p0[j] >= 35 && p0[j] <= 77 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[3] += 2;//绿色green
					}
					else if (p0[j] >= 78 && p0[j] <= 99 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[4] += 2;//青色cyan
					}
					else if (p0[j] >= 100 && p0[j] <= 124 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[5] += 2;//蓝色blue
					}
					else if (p0[j] >= 125 && p0[j] <= 155 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[6] += 2;//紫色purple
					}
					else if (p0[j] >= 0 && p0[j] <= 180 && p1[j] >= 0 && p1[j] <= 30 && p2[j] >= whiteGrayThreshold && p2[j] <= 255)
					{
						colorCount[7]++;//白色white
					}
					else if (p0[j] >= 0 && p0[j] <= 180 && p1[j] >= 0 && p1[j] <= 255 && p2[j] >= 0 && p2[j] <= blackGrayThresholdV)
					{
						colorCount[8]++;//黑色black
					}
					else
					{
						colorCount[9]++;//灰色black;
					}
				}
			}

		}
		int maxTemp = colorCount[0];
		int flag = 0;
		for (int i = 1; i < 10; i++)
		{
			if (maxTemp <= colorCount[i])
			{
				maxTemp = colorCount[i];
				flag = i;
			}
		}
		switch (flag)
		{
		case 0:
			_color[0] = OBJECT_COLOR_WHITE;
			_color[1] = OBJECT_COLOR_WHITE;
			break;
		case 8:
			_color[0] = OBJECT_COLOR_BLACK;
			_color[1] = OBJECT_COLOR_BLACK;
			break;
		case 9:
			_color[0] = OBJECT_COLOR_GRAY;
			_color[1] = OBJECT_COLOR_GRAY;
			break;
		default:
			_color[0] = OBJECT_COLOR_RED;
			_color[1] = OBJECT_COLOR_RED;
			break;
		case 1:
			_color[0] = OBJECT_COLOR_ORANGE;
			_color[1] = OBJECT_COLOR_ORANGE;
			break;
		case 2:
			_color[0] = OBJECT_COLOR_YELLOW;
			_color[1] = OBJECT_COLOR_YELLOW;
			break;
		case 3:
			_color[0] = OBJECT_COLOR_GREEN;
			_color[1] = OBJECT_COLOR_GREEN;
			break;
		case 4:
			_color[0] = OBJECT_COLOR_CYAN;
			_color[1] = OBJECT_COLOR_CYAN;
			break;
		case 5:
			_color[0] = OBJECT_COLOR_BLUE;
			_color[1] = OBJECT_COLOR_BLUE;
			break;
		case 6:
			_color[0] = OBJECT_COLOR_PURPLE;
			_color[1] = OBJECT_COLOR_PURPLE;
			break;
		case 7:
			_color[0] = OBJECT_COLOR_WHITE;
			_color[1] = OBJECT_COLOR_WHITE;
			break;
		}
	}
};

void CColorAly::GetAverageHSV2(cv::Mat& _colorROI, const cv::Mat& _binROI, vector<ObjectColor> & _color, int _class)//传入长度为2的颜色向量，还有物体类别，0为非车，1为车
{
	int whiteGrayThreshold = 125;//HSV白灰两色亮度V区分值，原理为221
	int blackGrayThresholdV = 46;//HSV黑灰两色亮度V区分值，原理为46
	Mat binImage = _binROI.clone();
	Mat image = _colorROI.clone();
	Mat ROI_HSV;
	Mat ROI = image;
	Mat binROI = binImage;

	std::vector<cv::Mat> sbgr(ROI.channels());//通道分离用
	std::vector<cv::Mat> mbgr(ROI.channels());//通道合成用
	//split(ROI, sbgr);
	//equalizeHist(sbgr[0], mbgr[0]);//三个通道分别直方图均衡化
	//equalizeHist(sbgr[1], mbgr[1]);
	//equalizeHist(sbgr[2], mbgr[2]);
	//merge(mbgr, ROI);//三个通道重新合成

	cvtColor(ROI, ROI_HSV, COLOR_BGR2HSV);//此时H 0-180;
	sbgr.clear();
	split(ROI_HSV, sbgr);//分割出H通道

	int colorCount[10] = { 0 };//统计每个颜色像素点个数，顺序为  红橙黄绿青蓝紫白黑

	if (_class == -1)//判断是人
	{
		//统计人上半部分的颜色
		for (int i = 0; i < (int)(sbgr[0].rows / 2); i++)
		{
			uchar* p0 = sbgr[0].ptr<uchar>(i);
			uchar* p1 = sbgr[1].ptr<uchar>(i);
			uchar* p2 = sbgr[2].ptr<uchar>(i);
			uchar* bin = binROI.ptr<uchar>(i);
			for (int j = 0; j < sbgr[0].cols; j++)
			{
				if (bin[j] != 0)
				{
					if ((p0[j] >= 0 && p0[j] <= 10 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255) || (p0[j] >= 156 && p0[j] <= 180 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255))
					{
						colorCount[0] += 2;//红色red
					}
					else if (p0[j] >= 11 && p0[j] <= 25 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[1] += 2;//橙色orange
					}
					else if (p0[j] >= 26 && p0[j] <= 34 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[2] += 2;//黄色yellow
					}
					else if (p0[j] >= 35 && p0[j] <= 77 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[3] += 2;//绿色green
					}
					else if (p0[j] >= 78 && p0[j] <= 99 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[4] += 2;//青色cyan
					}
					else if (p0[j] >= 100 && p0[j] <= 124 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[5] += 2;//蓝色blue
					}
					else if (p0[j] >= 125 && p0[j] <= 155 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[6] += 2;//紫色purple
					}
					else if (p0[j] >= 0 && p0[j] <= 180 && p1[j] >= 0 && p1[j] <= 30 && p2[j] >= whiteGrayThreshold && p2[j] <= 255)
					{
						colorCount[7]++;//白色white
					}
					else if (p0[j] >= 0 && p0[j] <= 180 && p1[j] >= 0 && p1[j] <= 255 && p2[j] >= 0 && p2[j] <= blackGrayThresholdV)
					{
						colorCount[8]++;//黑色black
					}
					else
					{
						colorCount[9]++;//灰色gray;
					}
				}
			}

		}
		int maxTemp = colorCount[0];
		int flag = 0;
		for (int i = 1; i < 10; i++)
		{
			if (maxTemp <= colorCount[i])
			{
				maxTemp = colorCount[i];
				flag = i;
			}
		}
		switch (flag)
		{
		case 0:
			_color[0] = OBJECT_COLOR_RED;
			break;
		case 1:
			_color[0] = OBJECT_COLOR_ORANGE;
			break;
		case 2:
			_color[0] = OBJECT_COLOR_YELLOW;
			break;
		case 3:
			_color[0] = OBJECT_COLOR_GREEN;
			break;
		case 4:
			_color[0] = OBJECT_COLOR_CYAN;
			break;
		case 5:
			_color[0] = OBJECT_COLOR_BLUE;
			break;
		case 6:
			_color[0] = OBJECT_COLOR_PURPLE;
			break;
		case 7:
			_color[0] = OBJECT_COLOR_WHITE;
			break;
		case 8:
			_color[0] = OBJECT_COLOR_BLACK;
			break;
		case 9:
			_color[0] = OBJECT_COLOR_GRAY;
			break;
		default:
			_color[0] = OBJECT_COLOR_WHITE;
			break;
		}


		//判断物体下半部分
		for (int i = 0; i < 10; i++)
		{
			colorCount[i] = 0;
		}
		maxTemp = 0;
		for (int i = (int)(sbgr[0].rows / 2); i < sbgr[0].rows; i++)
		{
			uchar* p0 = sbgr[0].ptr<uchar>(i);
			uchar* p1 = sbgr[1].ptr<uchar>(i);
			uchar* p2 = sbgr[2].ptr<uchar>(i);
			uchar* bin = binROI.ptr<uchar>(i);
			for (int j = 0; j < sbgr[0].cols; j++)
			{
				if (bin[j] != 0)
				{
					if ((p0[j] >= 0 && p0[j] <= 10 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255) || (p0[j] >= 156 && p0[j] <= 180 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255))
					{
						colorCount[0] += 2;//红色red
					}
					else if (p0[j] >= 11 && p0[j] <= 25 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[1] += 2;//橙色orange
					}
					else if (p0[j] >= 26 && p0[j] <= 34 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[2] += 2;//黄色yellow
					}
					else if (p0[j] >= 35 && p0[j] <= 77 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[3] += 2;//绿色green
					}
					else if (p0[j] >= 78 && p0[j] <= 99 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[4] += 2;//青色cyan
					}
					else if (p0[j] >= 100 && p0[j] <= 124 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[5] += 2;//蓝色blue
					}
					else if (p0[j] >= 125 && p0[j] <= 155 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[6] += 2;//紫色purple
					}
					else if (p0[j] >= 0 && p0[j] <= 180 && p1[j] >= 0 && p1[j] <= 30 && p2[j] >= whiteGrayThreshold && p2[j] <= 255)
					{
						colorCount[7]++;//白色white
					}
					else if (p0[j] >= 0 && p0[j] <= 180 && p1[j] >= 0 && p1[j] <= 255 && p2[j] >= 0 && p2[j] <= blackGrayThresholdV)
					{
						colorCount[8]++;//黑色black
					}
					else
					{
						colorCount[9]++;//灰色gray;
					}
				}

			}

		}
		maxTemp = colorCount[0];
		flag = 0;
		for (int i = 1; i < 10; i++)
		{
			if (maxTemp <= colorCount[i])
			{
				maxTemp = colorCount[i];
				flag = i;
			}
		}
		switch (flag)
		{
		case 0:
			_color[1] = OBJECT_COLOR_RED;
			break;
		case 1:
			_color[1] = OBJECT_COLOR_ORANGE;
			break;
		case 2:
			_color[1] = OBJECT_COLOR_YELLOW;
			break;
		case 3:
			_color[1] = OBJECT_COLOR_GREEN;
			break;
		case 4:
			_color[1] = OBJECT_COLOR_CYAN;
			break;
		case 5:
			_color[1] = OBJECT_COLOR_BLUE;
			break;
		case 6:
			_color[1] = OBJECT_COLOR_PURPLE;
			break;
		case 7:
			_color[1] = OBJECT_COLOR_WHITE;
			break;
		case 8:
			_color[1] = OBJECT_COLOR_BLACK;
			break;
		case 9:
			_color[1] = OBJECT_COLOR_GRAY;
			break;
		default:
			_color[1] = OBJECT_COLOR_WHITE;
			break;
		}
	}
	else//车的时候 class==2
	{
		for (int i = 0; i < sbgr[0].rows; i++)
		{
			uchar* p0 = sbgr[0].ptr<uchar>(i);
			uchar* p1 = sbgr[1].ptr<uchar>(i);
			uchar* p2 = sbgr[2].ptr<uchar>(i);
			uchar* bin = binROI.ptr<uchar>(i);
			for (int j = 0; j < sbgr[0].cols; j++)
			{
				if (bin[j] != 0)
				{
					if ((p0[j] >= 0 && p0[j] <= 10 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255) || (p0[j] >= 156 && p0[j] <= 180 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255))
					{
						colorCount[0] += 2;//红色red
					}
					else if (p0[j] >= 11 && p0[j] <= 25 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[1] += 2;//橙色orange
					}
					else if (p0[j] >= 26 && p0[j] <= 34 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[2] += 2;//黄色yellow
					}
					else if (p0[j] >= 35 && p0[j] <= 77 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[3] += 2;//绿色green
					}
					else if (p0[j] >= 78 && p0[j] <= 99 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[4] += 2;//青色cyan
					}
					else if (p0[j] >= 100 && p0[j] <= 124 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[5] += 2;//蓝色blue
					}
					else if (p0[j] >= 125 && p0[j] <= 155 && p1[j] >= 43 && p1[j] <= 255 && p2[j] >= 46 && p2[j] <= 255)
					{
						colorCount[6] += 2;//紫色purple
					}
					else if (p0[j] >= 0 && p0[j] <= 180 && p1[j] >= 0 && p1[j] <= 30 && p2[j] >= whiteGrayThreshold && p2[j] <= 255)
					{
						colorCount[7]++;//白色white
					}
					else if (p0[j] >= 0 && p0[j] <= 180 && p1[j] >= 0 && p1[j] <= 255 && p2[j] >= 0 && p2[j] <= blackGrayThresholdV)
					{
						colorCount[8]++;//黑色black
					}
					else
					{
						colorCount[9]++;//灰色black;
					}
				}
			}

		}
		int maxTemp = colorCount[0];
		int flag = 0;
		for (int i = 1; i < 10; i++)
		{
			if (maxTemp <= colorCount[i])
			{
				maxTemp = colorCount[i];
				flag = i;
			}
		}
		switch (flag)
		{
		case 0:
			_color[0] = OBJECT_COLOR_WHITE;
			_color[1] = OBJECT_COLOR_WHITE;
			break;
		case 8:
			_color[0] = OBJECT_COLOR_BLACK;
			_color[1] = OBJECT_COLOR_BLACK;
			break;
		case 9:
			_color[0] = OBJECT_COLOR_GRAY;
			_color[1] = OBJECT_COLOR_GRAY;
			break;
		default:
			_color[0] = OBJECT_COLOR_RED;
			_color[1] = OBJECT_COLOR_RED;
			break;
		case 1:
			_color[0] = OBJECT_COLOR_ORANGE;
			_color[1] = OBJECT_COLOR_ORANGE;
			break;
		case 2:
			_color[0] = OBJECT_COLOR_YELLOW;
			_color[1] = OBJECT_COLOR_YELLOW;
			break;
		case 3:
			_color[0] = OBJECT_COLOR_GREEN;
			_color[1] = OBJECT_COLOR_GREEN;
			break;
		case 4:
			_color[0] = OBJECT_COLOR_CYAN;
			_color[1] = OBJECT_COLOR_CYAN;
			break;
		case 5:
			_color[0] = OBJECT_COLOR_BLUE;
			_color[1] = OBJECT_COLOR_BLUE;
			break;
		case 6:
			_color[0] = OBJECT_COLOR_PURPLE;
			_color[1] = OBJECT_COLOR_PURPLE;
			break;
		case 7:
			_color[0] = OBJECT_COLOR_WHITE;
			_color[1] = OBJECT_COLOR_WHITE;
			break;
		}
	}
};

//输出RGB均值
void CColorAly::GetAverageHSV3(cv::Mat& _colorROI, const cv::Mat& _binROI, vector<Vec3d> & _color, int _class)//传入长度为2的颜色向量，还有物体类别，0为非车，1为车
{
	Mat ROI_HSV;
	Mat ROI = _colorROI.clone();
	Mat binROI = _binROI.clone();
	std::vector<cv::Mat> sbgr(ROI.channels());//通道分离用
	std::vector<cv::Mat> mbgr(ROI.channels());//通道合成用
	//split(ROI, sbgr);
	//equalizeHist(sbgr[0], mbgr[0]);//三个通道分别直方图均衡化
	//equalizeHist(sbgr[1], mbgr[1]);
	//equalizeHist(sbgr[2], mbgr[2]);
	//merge(mbgr, ROI);//三个通道重新合成
	cvtColor(ROI, ROI_HSV, COLOR_BGR2HSV);//此时H 0-180;
	sbgr.clear();
	split(ROI_HSV, sbgr);//分割出HSV通道

	double _averageHSV_top[3];//存储人上半部分HSV三个通道平均值,顺序为HSV
	double _averageHSV_bottom[3];//存储人下半部分HSV三个通道平均值
	double _averageHSV_car[3];//存储车身HSV三个通道平均值

	double H_Sum[2] = { 0, 0 };//1、人三个通道的所有点的HSV值得和，分上下半身，其中_Sum[0]为上半身；2、是车则只用_Sum[0]
	double S_Sum[2] = { 0, 0 };
	double V_Sum[2] = { 0, 0 };
	int ForgPointSum[2] = { 0, 0 };//1、人：统计人上下半身前景点的个数，ForgPointSum[0]为上半身;2、车：只用ForgPointSum[0]

	if (_class == -1)//判断是人
	{
		//统计人上半部分的颜色
		for (int i = 0; i < (int)(sbgr[0].rows / 2); i++)
		{
			uchar* p0 = sbgr[0].ptr<uchar>(i);
			uchar* p1 = sbgr[1].ptr<uchar>(i);
			uchar* p2 = sbgr[2].ptr<uchar>(i);
			uchar* bin = binROI.ptr<uchar>(i);
			for (int j = 0; j < sbgr[0].cols; j++)
			{
				if (bin[j] != 0)
				{
					H_Sum[0] += p0[j];
					S_Sum[0] += p1[j];
					V_Sum[0] += p2[j];
					ForgPointSum[0]++;
				}
			}
		}
		//统计人下半部分的颜色
		for (int i = (int)(sbgr[0].rows / 2); i < sbgr[0].rows; i++)
		{
			uchar* p0 = sbgr[0].ptr<uchar>(i);
			uchar* p1 = sbgr[1].ptr<uchar>(i);
			uchar* p2 = sbgr[2].ptr<uchar>(i);
			uchar* bin = binROI.ptr<uchar>(i);
			for (int j = 0; j < sbgr[0].cols; j++)
			{
				if (bin[j] != 0)
				{
					H_Sum[1] += p0[j];
					S_Sum[1] += p1[j];
					V_Sum[1] += p2[j];
					ForgPointSum[1]++;
				}
			}
		}
		//求上下身HSV平均值
		_averageHSV_top[0] = H_Sum[0] / ForgPointSum[0];
		_averageHSV_top[1] = S_Sum[0] / ForgPointSum[0];
		_averageHSV_top[2] = V_Sum[0] / ForgPointSum[0];
		_averageHSV_bottom[0] = H_Sum[1] / ForgPointSum[1];
		_averageHSV_bottom[1] = S_Sum[1] / ForgPointSum[1];
		_averageHSV_bottom[2] = V_Sum[1] / ForgPointSum[1];
		Vec3d temp1,temp2;//push上下半身hsv
		temp1[0] = _averageHSV_top[0];
		temp1[1] = _averageHSV_top[1];
		temp1[2] = _averageHSV_top[2];
		temp2[0] = _averageHSV_bottom[0];
		temp2[1] = _averageHSV_bottom[1];
		temp2[2] = _averageHSV_bottom[2];
		_color.push_back(temp1);
		_color.push_back(temp2);
	}
	else//车的时候 class==2
	{
		for (int i = 0; i < sbgr[0].rows; i++)
		{
			uchar* p0 = sbgr[0].ptr<uchar>(i);
			uchar* p1 = sbgr[1].ptr<uchar>(i);
			uchar* p2 = sbgr[2].ptr<uchar>(i);
			uchar* bin = binROI.ptr<uchar>(i);
			for (int j = 0; j < sbgr[0].cols; j++)
			{
				if (bin[j] != 0)
				{
					H_Sum[0] += p0[j];
					S_Sum[0] += p1[j];
					V_Sum[0] += p2[j];
					ForgPointSum[0]++;
				}
			}
		}
		//计算车身平均HSV值
		_averageHSV_car[0] = H_Sum[0] / ForgPointSum[0];
		_averageHSV_car[1] = S_Sum[0] / ForgPointSum[0];
		_averageHSV_car[2] = V_Sum[0] / ForgPointSum[0];

		Vec3d temp1;
		temp1[0] = _averageHSV_car[0];
		temp1[1] = _averageHSV_car[1];
		temp1[2] = _averageHSV_car[2];
		_color.push_back(temp1);
		_color.push_back(temp1);
	}
};