#include "stdafx.h"
#include "ObjectAndKF.h"

///以下为物体跟踪类------------------------------------------------------------------------------------------------------------------
//ObjectAndKF& ObjectAndKF::operator = (const ObjectAndKF& obj)
//{
//	KF = obj.KF;
//	state = obj.state;
//	processNoise = obj.processNoise;
//	measurement = obj.measurement;
//	m_objID = obj.m_objID;
//	m_vecCenters = obj.m_vecCenters;
//	m_rect = obj.m_rect;
//	return *this;
//
//}


void ObjectAndKF::Init(GetInfoCallBackFun clsGetInfoCallBackFun, string strPictureFolder, bool bLPR, bool bFace)
{
	m_output_filepath = strPictureFolder+"/output.txt";//当物体离开画面时，将信息输出到这里
	m_output_imagepath = strPictureFolder+"/"; //当物体离开画面时,输出最佳图像
	m_output_faceimgpath = strPictureFolder+"/face/";//当人离开时，输出其头部图像
	m_output_lprimgpath = strPictureFolder + "/lpr/";//当人离开时，输出其头部图像

	m_bLPR = bLPR;
	m_bFace = bFace;

	m_clsGetInfoCallBackFun = clsGetInfoCallBackFun;

	KF = KalmanFilter(stateNum, measureNum, 0);
	state = Mat(stateNum, 1, CV_32FC1); //state(x,y,detaX,detaY)  
	processNoise = Mat(stateNum, 1, CV_32F);
	measurement = Mat::zeros(measureNum, 1, CV_32F);    //measurement(x,y)  

	randn(state, Scalar::all(0), Scalar::all(0.1));//随机生成一个矩阵，期望是0，标准差为0.1;  
	state.at<float>(0) = (float)m_vecCenters.at(0).x;//付给第一帧的初始位置坐标x和y
	state.at<float>(1) = (float)m_vecCenters.at(0).y;

	KF.transitionMatrix = (Mat_<float>(4, 4) <<
		1, 0, 1, 0,
		0, 1, 0, 1,
		0, 0, 1, 0,
		0, 0, 0, 1);//元素导入矩阵，按行;  
	//setIdentity: 缩放的单位对角矩阵;  
	//!< measurement matrix (H) 观测模型  
	setIdentity(KF.measurementMatrix);

	//!< process noise covariance matrix (Q)  
	// wk 是过程噪声，并假定其符合均值为零，协方差矩阵为Qk(Q)的多元正态分布;  
	setIdentity(KF.processNoiseCov, Scalar::all(1e-5));

	//!< measurement noise covariance matrix (R)  
	//vk 是观测噪声，其均值为零，协方差矩阵为Rk,且服从正态分布;  
	setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));

	//!< priori error estimate covariance matrix (P'(k)): P'(k)=A*P(k-1)*At + Q)*/  A代表F: transitionMatrix  
	//预测估计协方差矩阵;  
	setIdentity(KF.errorCovPost, Scalar::all(1));

	//!< corrected state (x(k)): x(k)=x'(k)+K(k)*(z(k)-H*x'(k))  
	//initialize post state of kalman filter at random   
	randn(KF.statePost, Scalar::all(0), Scalar::all(0.1));
	KF.statePost.at<float>(0) = (float)m_vecCenters.at(0).x;//付给第一帧的初始位置坐标x和y
	KF.statePost.at<float>(1) = (float)m_vecCenters.at(0).y;
}

cv::Point ObjectAndKF::Predict()
{
	Point statePt = Point((int)KF.statePost.at<float>(0), (int)KF.statePost.at<float>(1));

	//2.kalman prediction     
	Mat prediction = KF.predict();
	Point predictPt = Point((int)prediction.at<float>(0), (int)prediction.at<float>(1));

	return predictPt;
}

void ObjectAndKF::Correct(int num)
{
	//3.update measurement  
	measurement.at<float>(0) = (float)m_vecCenters.at(num).x;
	measurement.at<float>(1) = (float)m_vecCenters.at(num).y;

	//4.update  
	KF.correct(measurement);
}

void ObjectAndKF::UpdatePerfectImages(const cv::Mat& _binImg, cv::Mat& _colorImg, cv::Mat& _ori_image, int nResizeRatio)
{
	//将ROI区域还原到原始高清分辨率的图像中
	cv::Rect rect_big;
	//rect_big.x = m_rect.x * 2 <= _ori_image.cols ? m_rect.x * 2 : _ori_image.cols;//防止越界
	//rect_big.y = m_rect.y * 2 <= _ori_image.rows ? m_rect.y * 2 : _ori_image.rows;
	//rect_big.width = rect_big.x + m_rect.width * 2 <= _ori_image.cols ? m_rect.width * 2 : _ori_image.cols - rect_big.x;
	//rect_big.height = rect_big.y + m_rect.height * 2 <= _ori_image.rows ? m_rect.height * 2 : _ori_image.rows - rect_big.y;

	rect_big.x = m_rect.x * nResizeRatio <= _ori_image.cols ? m_rect.x * nResizeRatio : _ori_image.cols;//防止越界
	rect_big.y = m_rect.y * nResizeRatio <= _ori_image.rows ? m_rect.y * nResizeRatio : _ori_image.rows;
	rect_big.width = rect_big.x + m_rect.width * nResizeRatio <= _ori_image.cols ? m_rect.width * nResizeRatio : _ori_image.cols - rect_big.x;
	rect_big.height = rect_big.y + m_rect.height * nResizeRatio <= _ori_image.rows ? m_rect.height * nResizeRatio : _ori_image.rows - rect_big.y;

	if (m_perfect_color_big_image.empty())
	{
		m_perfect_color_big_image = _ori_image(rect_big).clone();
		m_perfect_color_image = _colorImg(m_rect).clone();
		m_perfect_bin_image = _binImg(m_rect).clone();
	}
	if ((_binImg(m_rect).rows * _binImg(m_rect).cols) > (m_perfect_bin_image.rows * m_perfect_bin_image.cols))
	{
		m_perfect_color_big_image = _ori_image(rect_big).clone();
		m_perfect_color_image = _colorImg(m_rect).clone();
		m_perfect_bin_image = _binImg(m_rect).clone();
	}
}

//void ObjectAndKF::GetFinalOBjInfo(bool& _bObjectDistinguish, string& _strObjectInfo, int& _nObjectClass)
void ObjectAndKF::GetFinalOBjInfo( string& _strObjectInfo, int& _nObjectClass)
{
	char* strInfo;
	int nNum;
	int nObjectType;

	string obj_class;
	int obj_class2;

	obj_class = "people";
	obj_class2 = -1;
	_nObjectClass = -1;
	//ofstream outfile(m_output_filepath, ios::app);
	//outfile << setfill('0') << setw(5) << m_objID << "	" << obj_class << "	" << ColorEnum2String(m_colorH.at(0)) << "	" << ColorEnum2String(m_colorH.at(1)) << endl;
	//outfile.close();

	CColorAly cColorAly;
	vector<Vec3d> colorRGB;
	cColorAly.GetAverageHSV3(m_perfect_color_image, m_perfect_bin_image, colorRGB, obj_class2);//利用最佳帧提取人或者车的HSV

	//weigang 20160118 

	//假设是人，输出人脸图像
	//if (obj_class2 == -1 && m_bFace)
	//{
	//	FaceDetection_Interface m_faceDetection;
	//	Mat faceImg;
	//	//下面两行测试人脸识别用
	//	/*Mat FDtest = imread("FDtestImg.jpg");
	//	if (m_faceDetection.getFace(FDtest, faceImg) == 1)*/
	//	if (m_faceDetection.getFace(m_perfect_color_big_image, faceImg) == 1)
	//	{
	//		ostringstream faceimage_name;
	//		ostringstream faceimage_name_ID;
	//		faceimage_name << m_output_faceimgpath << m_objID << "_" << obj_class << ".bmp";
	//		cv::imwrite(faceimage_name.str(), faceImg);

	//		/*_bObjectDistinguish = true;*/
	//		faceimage_name_ID << "ID:" << "object" << m_objID << ";" << "PathName:" << m_output_faceimgpath << m_objID << "_" << obj_class << ".bmp;";
	//		_strObjectInfo = faceimage_name_ID.str();

	//		m_clsGetInfoCallBackFun(_strObjectInfo.c_str(), _strObjectInfo.length(), _nObjectClass);
	//		//0：表示输出的返回值信息为保存的人脸路径（格式示例:"PathName:..\output\2-people.bmp;"）
	//	}
	//}
	////如果是车辆，判断有无车牌
	//else if (obj_class2 == 1 && m_bLPR)
	//{
	//	std::string LP;
	//	LprInterface m_clsLprInterface;
	//	if (m_clsLprInterface.GetLPR(m_perfect_color_big_image, LP) == true)
	//	{
	//		cout << "DLL 输出车牌："<< LP << endl;
	//		ostringstream lpr_name;
	//		ostringstream lpr_name_ID;
	//		lpr_name << m_output_lprimgpath << m_objID << "_" << LP << ".bmp";
	//		cv::imwrite(lpr_name.str(), m_perfect_color_big_image);

	//		//_bObjectDistinguish = true;
	//		lpr_name_ID << "ID:" << "object" << m_objID << ";" << "PathName:" << m_output_lprimgpath << m_objID << "_" << LP << ".bmp;";
	//		_strObjectInfo = lpr_name_ID.str() + "LicensePlate:" + LP + ";";
	//		
	//		m_clsGetInfoCallBackFun(_strObjectInfo.c_str(), _strObjectInfo.length(), _nObjectClass);
	//		//1：表示输出的返回值信息为车牌信息（格式示例 : "PathName:..\output\2-苏E 8HG64.bmp;LicensePlate:苏E 8HG64;"）
	//	}
	//	else
	//	{
	//		cout << "车牌未识别！" << endl;
	//	}
	//}

	//输出perfect image
	ostringstream image_name;
	image_name << m_output_imagepath << m_objID << "-" << obj_class << "-" << "(" << colorRGB.at(0)[0] << "," << colorRGB.at(0)[1] << "," << colorRGB.at(0)[2] << ")" << "_" << "(" << colorRGB.at(1)[0] << "," << colorRGB.at(1)[1] << "," << colorRGB.at(1)[2] << ")" << ".bmp";
	cv::imwrite(image_name.str(), m_perfect_color_big_image);
}



/**************************C++回调函数:test*************************************/
//typedef int (WINAPI *PFCALLBACK)(int Param1, int Param2);
//
//int WINAPI CBFunc(int Param1, int Param2);
//
//PFCALLBACK   gCallBack = 0;
//void WINAPI TestCallBack(PFCALLBACK Func)
//{
//	if (Func == NULL)return;
//
//	gCallBack = Func;
//
//	return;
//}
/*************************************************************************/