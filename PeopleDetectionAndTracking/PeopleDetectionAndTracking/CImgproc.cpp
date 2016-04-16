#include "stdafx.h"
#include "CImgproc.h"

void CImgproc::FillHole(const Mat srcBw, Mat &dstBw)//Ìî³ä¿Õ¶´
{
	Size m_Size = srcBw.size();
	Mat Temp = Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcBw.type());//ÑÓÕ¹Í¼Ïñ  
	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));

	cv::floodFill(Temp, Point(0, 0), Scalar(255));

	Mat cutImg;//²Ã¼ôÑÓÕ¹µÄÍ¼Ïñ  
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);

	dstBw = srcBw | (~cutImg);
}

void CImgproc::AddEdge(const Mat srcFw, const Mat srcBw, Mat &dstBw)
{
	cv::Mat backImgEdge;
	cv::Mat frontImgEdge;
	cv::Canny(srcFw, frontImgEdge, 150, 255, 3);
	cv::Canny(srcBw, backImgEdge, 150, 255, 3);
	cv::Mat chaImg(backImgEdge.size(), CV_8UC1);
	cv::absdiff(backImgEdge, frontImgEdge, chaImg);
	//imshow("backImgEdge", backImgEdge);
	//imshow("frontImgEdge", frontImgEdge);
	//imshow("chaImg", chaImg);
	dstBw = dstBw | chaImg;


}