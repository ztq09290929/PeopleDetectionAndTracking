#include "stdafx.h"
#include "Cblob3.h"

///以下为物体查找类---------------------------------------------------------------------------------------------------------------

//构造函数，初始化计数器为0并且导入分类器
CBlob::CBlob()
{
	m_objCount = 0;
	//m_pvclassify.ImportClassify0();
	
}

//void CBlob::Init(GetInfoCallBackFun clsGetInfoCallBackFun, bool bLPR, bool bFace, string strPathName, int TrackPointNumThresh, int ContoursRectNumThresh, int DistanceThresh, int DistanceThreshKal, int nResizeRatio)
void CBlob::Init(bool bLPR, bool bFace, string strPathName, int TrackPointNumThresh, int ContoursRectNumThresh, int DistanceThresh, int DistanceThreshKal, int nResizeRatio)
{
	//m_clsGetInfoCallBackFun = clsGetInfoCallBackFun;
	m_bLPR = bLPR;
	m_bFace = bFace;
	m_nResizeRatio = nResizeRatio;
	m_strPathName = strPathName;
	m_para.Init(TrackPointNumThresh, ContoursRectNumThresh, DistanceThresh, DistanceThreshKal, nResizeRatio);
}

int CBlob::GetObjNum()
{
	int count = 0;
	for (auto it = m_listObjects.begin(); it != m_listObjects.end();++it)
	{
		if (it->m_objID != -1)
		{
			++count;
		}
	}
	return count;
}

void CBlob::BlobDetecterUseRect(const cv::Mat& _binImg, cv::Mat& _colorImg,std::vector<cv::Rect>& _input_rects)
{
	m_centers.clear();
	m_rects.clear();
	m_colorsRGB.clear();

	for (auto it = _input_rects.begin(); it != _input_rects.end(); ++it)
	{
		m_rects.push_back(*it);

		cv::Point center;		
		center.x = it->x + it->width / 2;//矩形的中心点
		center.y = it->y + it->height / 2;
		m_centers.push_back(center);//存入外接矩形的中心点

		vector<Vec3d> colorRGB;
		m_CColorAly.GetAverageHSV3(_colorImg(*it), _binImg(*it), colorRGB, -1);
		m_colorsRGB.push_back(colorRGB);
	}
}

void CBlob::BlobDetecter(const cv::Mat& _binImg, cv::Mat& _colorImg)
{
	cv::Mat src = _binImg.clone();
	//cv::Mat srcROI = src(cv::Rect(0, src.rows * 3 / 10, src.cols, src.rows * 7 / 10));//切掉图像的边缘5%
	//cv::Mat outROI = _outputImg(cv::Rect(0, _outputImg.rows * 3 / 10, _outputImg.cols, _outputImg.rows * 7 / 10));//为了坐标方便，输出图像也要提取ROI

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(src, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	//m_goodContours.clear();//处理每一帧图像，都要清空之前的点
	m_centers.clear();
	m_rects.clear();
	//m_PorV.clear();
	m_colorsRGB.clear();
	//进行筛选，只保留矩形框大于30*30的轮廓，中心点和矩形框
	for (auto i = contours.begin(); i != contours.end(); ++i)
	{
		cv::Rect temRect = cv::boundingRect(*i);//求轮廓的外接矩形
		//如果轮廓长宽大于阈值，且切掉画面上部分30%
		if (temRect.width > m_para.CBlob_ContoursRectNumThresh && temRect.height > m_para.CBlob_ContoursRectNumThresh && temRect.y > _binImg.rows * 3 / 10)
		//if (temRect.width > m_para.CBlob_ContoursRectNumThresh && temRect.height > m_para.CBlob_ContoursRectNumThresh )
		{
			//m_goodContours.push_back(*i);
			m_rects.push_back(temRect);
			cv::Point center;
			int xSum(0), ySum(0);
			for (auto j = i->begin(); j != i->end(); ++j)
			{
				xSum += j->x;
				ySum += j->y;
			}
			center.x = (int)(xSum / i->size());//轮廓的中心点
			center.y = (int)(ySum / i->size());
			//center.x = temRect.x + temRect.width / 2;//矩形的中心点
			//center.y = temRect.y + temRect.height / 2;
			m_centers.push_back(center);//存入外接矩形的中心点
		
			//m_PorV.push_back(m_pvclassify.GetObjClassify(*i, temRect));

			vector<Vec3d> colorRGB;
			m_CColorAly.GetAverageHSV3( _colorImg(temRect), _binImg(temRect), colorRGB,-1);
			m_colorsRGB.push_back(colorRGB);
		}
	}

	//m_centers,m_goodContours,m_rects中元素个数应一致
	//cout<<"当前帧的中心点个数 = "<<m_centers.size()<<endl;
}

//寻找m_centers中与pLast最近的一点，返回其索引号
void CBlob::FindNearstPoint(cv::Point pLast, int& id)
{
	double dist = 0;
	double minDist = 10000;
	int count = 0;
	//	if (m_centers.empty())cout << "m_centers为空" << endl;
	for (auto i = m_centers.begin(); i != m_centers.end(); ++i)//首先遍历当前帧矩形中心点集，提取与输入点的最小距离
	{
		dist = sqrt((pLast.x - (i->x))*(pLast.x - (i->x)) + (pLast.y - (i->y))*(pLast.y - (i->y)));

		if (dist < minDist )
		{
			minDist = dist;
			id = count;
		}
		++count;
	}
	if (minDist < m_para.CBlob_DistanceThresh)//如果输入点与中心点集最小距离满足要求，则将相应位置的标记置为0，表示该点已被用过
	{
	}
	else//如果距离太大，或者根本没有矩形中心点，即当前帧没有物体
	{
		id = -1;
	}
}
void CBlob::FindNearstPointKal(cv::Point pLast, int& id)
{
	double dist = 0;
	double minDist = 10000;
	int count = 0;
	//	if (m_centers.empty())cout << "m_centers为空" << endl;
	for (auto i = m_centers.begin(); i != m_centers.end(); ++i)
	{
		dist = sqrt((pLast.x - (i->x))*(pLast.x - (i->x)) + (pLast.y - (i->y))*(pLast.y - (i->y)));

		if (dist < minDist)
		{
			minDist = dist;
			id = count;
		}
		++count;
	}
	if (minDist < m_para.CBlob_DistanceThreshKal)//与上一函数功能相同，只是输入的点为卡尔曼滤波的预测点，在该预测点附近查找
	{
	}
	else
	{
		id = -1;
	}
}

//void CBlob::ClassifyCenters(const cv::Mat& _binImg, cv::Mat& _colorImg, cv::Mat& _ori_image, bool& _bObjectDistinguish, string& _strObjectInfo,int& _nObjectClass)
void CBlob::ClassifyCenters(const cv::Mat& _binImg, cv::Mat& _colorImg, cv::Mat& _ori_image, string& _strObjectInfo, int& _nObjectClass)
{
	/*_bObjectDistinguish = false;*/
	//_strLP = "";

	//cv::Mat outROI = _outputImg(cv::Rect(0, _outputImg.rows * 3 / 10, _outputImg.cols, _outputImg.rows * 7 / 10));
	if (m_listObjects.empty())//如果存储物体的链表为空，首先用当前帧的矩形中心点集去初始化各个物体,并存矩形框
	{
		for (unsigned int i = 0; i != m_centers.size(); ++i)//每一点都是一个新的物体
		{
			ObjectAndKF obj1;
			obj1.m_vecCenters.push_back(cv::Point(m_centers.at(i).x, m_centers.at(i).y));
			obj1.m_rect = m_rects.at(i);
			//obj1.m_Contours = m_goodContours.at(i);
			//obj1.m_class = m_PorV.at(i);
			obj1.m_colorH = m_colorsRGB.at(i);
			//obj1.m_allClassifyResults += obj1.m_class; //累计类别
			obj1.UpdatePerfectImages(_binImg, _colorImg, _ori_image,m_nResizeRatio);//更新最佳图像

			obj1.Init(m_clsGetInfoCallBackFun, m_strPathName, m_bLPR,m_bFace);//存入该物体的第一个中心点位置后，初始化其卡尔曼滤波器
			m_listObjects.push_back(obj1);
		}
	}

	else//如果链表中有元素，则代表上一帧有物体
	{

		for (auto it = m_listObjects.begin(); it != m_listObjects.end();)//遍历链表中的每一个物体对象
		{
			if (it->m_vecCenters.size() ==1)//如果该物体对象只保存了一个位置点，不能使用卡尔曼滤波
			{
				cv::Point pLast = it->m_vecCenters.back();//拿到列表当前obj的最后一个点
				int id;
				FindNearstPoint(pLast, id);//寻找和这一点最近的当前帧中的点，认为是下一点

				if (id == -1)//如果没找到，则认为物体已经消失，则删除链表中的该项
				{
					//如果物体不在图像边缘，而且物体的id号不为-1，则认为是暂时消失
					//if (it->m_objID != -1 && pLast.x > _outputImg.cols / 10 && pLast.x<_outputImg.cols * 9 / 10 && pLast.y>_outputImg.rows / 10 && pLast.y < _outputImg.rows * 9 / 10)
					//{
					//	m_listVanishObj.push_back(*it);
					//	it = m_listObjects.erase(it);
					//}
					//else//否则认为是移动出了画面
					//{
						it = m_listObjects.erase(it);
					//}
				}
				else//如果找到了，则将第二点的坐标存入向量，并将对应矩形框更新，并且用其修正卡尔曼滤波器，并且从当前帧中删除已经使用了的中心点
				{
					it->m_vecCenters.push_back(m_centers.at(id));
					it->m_rect = m_rects.at(id);
					//it->m_Contours = m_goodContours.at(id);
					//it->m_class = m_PorV.at(id);
					//it->m_colorH = m_colorsRGB.at(id);
					//it->m_allClassifyResults += it->m_class; //累计类别
					it->UpdatePerfectImages(_binImg, _colorImg, _ori_image, m_para.m_nResizeRatio);//更新最佳图像

					m_centers.erase(m_centers.begin() + id);//在当前帧的中心点集和外接矩形集删除已经使用过的点和矩形
					m_rects.erase(m_rects.begin() + id);
					//m_goodContours.erase(m_goodContours.begin() + id);
					//m_PorV.erase(m_PorV.begin() + id);
					m_colorsRGB.erase(m_colorsRGB.begin() + id);

					it->Predict();
					it->Correct(1);
					++it;
				}
			}
			else//如果该物体对象存着2个以上的位置点，则可以使用卡尔曼滤波器
			{
				cv::Point prePoint = it->Predict();//预估出当前帧这一点的位置

//    			cv::circle(outROI, prePoint, 5, cv::Scalar(0, 255, 255), 3, 8);//画出预估的点

				int id;
				FindNearstPointKal(prePoint, id);//在预估的位置附近寻找真实的点的位置

				if (id == -1)//如果没找到，则认为物体已经消失，则删除链表中的该项
				{
					//如果物体不在图像边缘，而且物体的id号不为-1，即物体有编号，则认为是暂时消失，若id为-1，此时物体还被当作是干扰
					//if (it->m_objID != -1 && prePoint.x > _outputImg.cols / 10 && prePoint.x<_outputImg.cols * 9 / 10 && prePoint.y>_outputImg.rows / 10 && prePoint.y < _outputImg.rows * 9 / 10)
					//{
					//	m_listVanishObj.push_back(*it);
					//	it = m_listObjects.erase(it);
					//}
					//else//否则认为是移动出了画面
					//{
					if (it->m_objID != -1)
					{
						//it->GetFinalOBjInfo(_bObjectDistinguish, _strObjectInfo, _nObjectClass); //物体离开画面，则输出其最佳图像和信息
						it->GetFinalOBjInfo(_strObjectInfo, _nObjectClass); //物体离开画面，则输出其最佳图像和信息
					}
					it = m_listObjects.erase(it);
					//}
				}
				else//如果找到了，则将这点的坐标存入向量，并将对应矩形框更新，并且用其修正卡尔曼滤波器
				{
					it->m_vecCenters.push_back(m_centers.at(id));
					it->m_rect = m_rects.at(id);
					//it->m_Contours = m_goodContours.at(id);
					//it->m_class = m_PorV.at(id);
					//it->m_colorH = m_colorsRGB.at(id);
					//it->m_allClassifyResults += it->m_class; //累计类别
					it->UpdatePerfectImages(_binImg, _colorImg, _ori_image, m_para.m_nResizeRatio);//更新最佳图像

					m_centers.erase(m_centers.begin() + id);//在当前帧的中心点集和外接矩形集删除已经使用过的点和矩形
					m_rects.erase(m_rects.begin() + id);
					//m_goodContours.erase(m_goodContours.begin() + id);
					//m_PorV.erase(m_PorV.begin() + id);
					m_colorsRGB.erase(m_colorsRGB.begin() + id);

					it->Correct((int)it->m_vecCenters.size()-1);
					//若物体轨迹足够长，则赋予其一个id号，否则认为是干扰，还不给分配编号
					if (it->m_vecCenters.size() > m_para.CBlob_TrackPointNumThresh && it->m_objID == -1)
					{
						time_t now = 0;
						//it->m_objID = (long)time(&now) + (++m_objCount);
						it->m_objID = (++m_objCount);
					}
					//生成人车分类样本数据
					//if (it->m_objID != -1)
					//{
					//	m_pvclassify.GetSamples(*it);
					//}

					++it;
				}
			}
		}

		for (unsigned int i = 0; i < m_centers.size(); ++i)//对于中心点集中剩下的点，认为他们是本帧中新出现的物体的中心点，亦或是之前短暂消失的物体
		{
			ObjectAndKF obj2;//则创建新的物体对象
			obj2.m_vecCenters.push_back(cv::Point(m_centers[i].x,m_centers[i].y));
			obj2.m_rect = m_rects[i];
			//obj2.m_Contours = m_goodContours[i];
			//obj2.m_class = m_PorV[i];
			obj2.m_colorH = m_colorsRGB[i];
			//obj2.m_allClassifyResults += obj2.m_class; //累计类别
			obj2.UpdatePerfectImages(_binImg, _colorImg, _ori_image, m_para.m_nResizeRatio);//更新最佳图像

			obj2.Init(m_clsGetInfoCallBackFun, m_strPathName, m_bLPR, m_bFace);
			m_listObjects.push_back(obj2);//将该对象存入链表

		}
	}

	//cout << "list的大小 = " << m_listObjects.size() << endl;
	//cout << "vinash的大小 = " << m_listVanishObj.size() << endl;
}

void CBlob::DrawPaths(cv::Mat& _outputImg)
{
	//由于坐标原因，画图时也应取出ROI
	//cv::Mat outROI = _outputImg(cv::Rect(0, _outputImg.rows* 3/10, _outputImg.cols , _outputImg.rows * 7 / 10));
	for (auto lisIt = m_listObjects.begin(); lisIt != m_listObjects.end(); ++lisIt)//画出链表中的各点，每个物体的点各自连成直线
	{
		if (lisIt->m_vecCenters.size() >m_para.CBlob_TrackPointNumThresh)//筛选去少于10个轨迹点的物体，认为是干扰
		{
			for (unsigned int i = 0; i < lisIt->m_vecCenters.size(); ++i)
			{
				if (i>0)
				{
					cv::line(_outputImg, lisIt->m_vecCenters.at(i - 1), lisIt->m_vecCenters.at(i), cv::Scalar(0, 0, 255), 2, 8);//每一帧，将一个物体所有轨迹点连成线
				}
			}
			cv::circle(_outputImg, lisIt->m_vecCenters.at(lisIt->m_vecCenters.size() - 1), 4, cv::Scalar(0, 0, 255), 3, 8);//画出物体中心点

			cv::rectangle(_outputImg, lisIt->m_rect, cv::Scalar(0, 255, 0), 2, 8);//画出各物体的外接矩形

			//std::vector<std::vector<cv::Point>> conts;
			//conts.push_back(lisIt->m_Contours);
			//cv::drawContours(outROI, conts, -1, cv::Scalar(255, 0, 0), 2, 8);

			ostringstream text;
			ostringstream textRGB;
			//if (lisIt->m_class == -1)
			if (true)
			{
				text <<"P"<< lisIt->m_objID << "(" << lisIt->m_vecCenters.at(lisIt->m_vecCenters.size() - 1).x << "," << lisIt->m_vecCenters.at(lisIt->m_vecCenters.size() - 1).y << ")";
				cv::Size textsize = getTextSize(text.str(), CV_FONT_HERSHEY_SIMPLEX, 0.5, 2, 0);
				cv::putText(_outputImg, text.str(), cvPoint(lisIt->m_rect.x + lisIt->m_rect.width, lisIt->m_rect.y - 3), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0), 2, 8);
				textRGB << "(" << "(" << lisIt->m_colorH.at(0)[0] << "," << lisIt->m_colorH.at(0)[1] << "," << lisIt->m_colorH.at(0)[2] << ")" << "," << "(" << lisIt->m_colorH.at(1)[0] << "," << lisIt->m_colorH.at(1)[1] << "," << lisIt->m_colorH.at(1)[2] << ")" << ")";
				textsize = getTextSize(textRGB.str(), CV_FONT_HERSHEY_SIMPLEX, 0.5, 2, 0);
				cv::putText(_outputImg, textRGB.str(), cvPoint(lisIt->m_rect.x + lisIt->m_rect.width, lisIt->m_rect.y + lisIt->m_rect.height + textsize.height + 3), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2, 8);
			}
			else
			{
				text <<"V"<< lisIt->m_objID << "(" << lisIt->m_vecCenters.at(lisIt->m_vecCenters.size() - 1).x << "," << lisIt->m_vecCenters.at(lisIt->m_vecCenters.size() - 1).y << ")";
				cv::Size textsize = getTextSize(text.str(), CV_FONT_HERSHEY_SIMPLEX, 0.5, 2, 0);
				cv::putText(_outputImg, text.str(), cvPoint(lisIt->m_rect.x + lisIt->m_rect.width, lisIt->m_rect.y - 3), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 2, 8);
				textRGB << "(" << lisIt->m_colorH.at(0)[0] << "," << lisIt->m_colorH.at(0)[1] << "," << lisIt->m_colorH.at(0)[2] << ")";
				//textRGB << "(" << ColorEnum2String(lisIt->m_colorH.at(0)) << "," << ColorEnum2String(lisIt->m_colorH.at(1)) << ")";
				textsize = getTextSize(textRGB.str(), CV_FONT_HERSHEY_SIMPLEX, 0.5, 2, 0);
				cv::putText(_outputImg, textRGB.str(), cvPoint(lisIt->m_rect.x + lisIt->m_rect.width, lisIt->m_rect.y + lisIt->m_rect.height + textsize.height + 3), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2, 8);
			}
		}
	}

	//标出当前帧总目标数
	ostringstream numText;
	numText << "Num: " << GetObjNum();
	cv::Size numTextsize = getTextSize(numText.str(), CV_FONT_HERSHEY_COMPLEX, 0.9, 2, 0);
	cv::putText(_outputImg, numText.str(), cvPoint(5, _outputImg.rows - 5), CV_FONT_HERSHEY_COMPLEX, 0.9, cv::Scalar(255, 0, 0), 2, 8);
}

void CBlob::GetObjInfo(string& _objInfo)//以字符串形式返回当前帧物体信息
{
	ostringstream _objInfoStream;
	_objInfoStream << "objectnum:" << GetObjNum() << ";";
	int tempCount = 0;
	for (auto it = m_listObjects.begin(); it != m_listObjects.end(); ++it)
	{
		if (it->m_objID != -1)
		{
			++tempCount;
			_objInfoStream << "ID" << setfill('0') << setw(3) << tempCount << ":" << "object" << setfill('0') << setw(5) << it->m_objID << ";";
			_objInfoStream << "Type" << setfill('0') << setw(3) << tempCount << ":" << -1 << ";";
			_objInfoStream << "UpperPart" << setfill('0') << setw(3) << tempCount << ":" << "(" << it->m_colorH.at(0)[0] << "," << it->m_colorH.at(0)[1] << "," << it->m_colorH.at(0)[2] << ")" << ";";
			_objInfoStream << "LowerPart" << setfill('0') << setw(3) << tempCount << ":" << "(" << it->m_colorH.at(1)[0] << "," << it->m_colorH.at(1)[1] << "," << it->m_colorH.at(1)[2] << ")" << ";";
			_objInfoStream << "X" << setfill('0') << setw(3) << tempCount << ":" << ((it->m_vecCenters.back().x) * m_nResizeRatio) << ";";
			_objInfoStream << "Y" << setfill('0') << setw(3) << tempCount << ":" << ((it->m_vecCenters.back().y) * m_nResizeRatio) << ";";
		}
	}
	_objInfo = _objInfoStream.str();
}