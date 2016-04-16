#include "stdafx.h"
#include "pvclassify.h"

void PVClassify::ImportClassify0()
{
	//下面从txt导入分类器，存入二维向量
	ifstream fr;
	fr.open(m_classify_filepath, ios::in);
	if (!fr.is_open())
	{
		cout << "打开分类器文件错误！" << endl; exit(1);
	}

	string line;
	while (getline(fr, line))
	{
		stringstream ss(line);
		string sub_str;

		vector<double> classifys;
		while (getline(ss, sub_str, '\t'))
		{
			if (sub_str == "\r")
			{
			}
			else if (sub_str == "lt")//小于用-1表示
			{
				classifys.push_back(-1.0);
			}
			else if (sub_str == "gt")//大于用+1表示
			{
				classifys.push_back(1.0);
			}
			else
			{
				double val = atof(sub_str.c_str());
				classifys.push_back(val);
			}
		}
		m_classify0.push_back(classifys);
	}
}

int PVClassify::GetObjClassify(const vector<cv::Point>& _Contours, const cv::Rect& _rect)
{
	//对于一个物体的轮廓，获取一个6维特征向量
	vector<double> features;
	//double contours_length = cv::arcLength(_Contours, true);//轮廓长度
	//double contours_area = cv::contourArea(_Contours);//轮廓面积
	//_features.push_back(_features.at(1) / (_features.at(0)*_features.at(0)));//圆形度
	//_features.push_back(_features.at(1) / (_rect.height*_rect.width));//占空比
	//_features.push_back((double)_rect.height / _rect.width);//长宽比
	////计算惯性主轴方向
	//cv::Moments mm;
	//mm = cv::moments(_Contours);
	//double theta1 = atan(2 * mm.mu11 / (mm.mu20 - mm.mu02));
	//double theta2 = atan(2 * mm.mu11 / (mm.mu20 - mm.mu02)) + CV_PI / 2;
	//double theta = (2 * (mm.mu20 - mm.mu02)*cos(2 * theta1) + 4 * mm.mu11*sin(2 * theta1)) > 0 ? theta1 : theta2;
	//_features.push_back(theta);
	double contours_length = cv::arcLength(_Contours, true);//轮廓长度
	double contours_area = cv::contourArea(_Contours);//轮廓面积
	double ratio13(0), ratio23(0);//物体高度与1/3和2/3处轮廓宽度的比例，这是第一、二个特征
	GetObjRatio1323(_Contours, _rect, ratio13, ratio23);
	//特征1,1/3高宽比
	features.push_back(ratio13);

	//特征2,2/3高宽比
	features.push_back(ratio23);

	//特征3，圆形度
	features.push_back((contours_length * contours_length) / contours_area);

	//特征4，占空比
	features.push_back(contours_area / (_rect.height * _rect.width));

	//特征5，外接矩形高宽比
	features.push_back((double)_rect.height / _rect.width);

	//特征6，惯性主轴方向
	cv::Moments mm;
	mm = cv::moments(_Contours);
	double theta1 = atan(2 * mm.mu11 / (mm.mu20 - mm.mu02));
	double theta2 = atan(2 * mm.mu11 / (mm.mu20 - mm.mu02)) + CV_PI / 2;
	double theta = (2 * (mm.mu20 - mm.mu02)*cos(2 * theta1) + 4 * mm.mu11*sin(2 * theta1)) > 0 ? theta1 : theta2;
	features.push_back(theta);

	//使用特征向量和分类器进行分类
	double classifyResult = 0;
	for (unsigned int i = 0; i < m_classify0.size(); ++i)
	{
		double subResult = 1.0;
		if (m_classify0[i][1] < 0)//小于号
		{
			if (features[(unsigned int)m_classify0[i][0]] <= m_classify0[i][2])
			{
				subResult = -1.0;
			}
		}
		else//大于号
		{
			if (features[(unsigned int)m_classify0[i][0]] > m_classify0[i][2])
			{
				subResult = -1.0;
			}
		}

		classifyResult += subResult*m_classify0[i][3];
	}

	if (classifyResult < 0)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

void PVClassify::GetSamples(ObjectAndKF& _obj)
{
	vector<double> features;
	double contours_length = cv::arcLength(_obj.m_Contours, true);//轮廓长度
	double contours_area = cv::contourArea(_obj.m_Contours);//轮廓面积
	double ratio13(0), ratio23(0);//物体高度与1/3和2/3处轮廓宽度的比例，这是第一、二个特征
	GetObjRatio1323(_obj.m_Contours, _obj.m_rect, ratio13, ratio23);
	//特征1,1/3高宽比
	features.push_back(ratio13);

	//特征2,2/3高宽比
	features.push_back(ratio23);

	//特征3，圆形度
	features.push_back((contours_length * contours_length)/contours_area);

	//特征4，占空比
	features.push_back(contours_area / (_obj.m_rect.height * _obj.m_rect.width));

	//特征5，外接矩形高宽比
	features.push_back((double)_obj.m_rect.height / _obj.m_rect.width);

	//特征6，惯性主轴方向
	cv::Moments mm;
	mm = cv::moments(_obj.m_Contours);
	double theta1 = atan(2 * mm.mu11 / (mm.mu20 - mm.mu02));
	double theta2 = atan(2 * mm.mu11 / (mm.mu20 - mm.mu02)) + CV_PI / 2;
	double theta = (2 * (mm.mu20 - mm.mu02)*cos(2 * theta1) + 4 * mm.mu11*sin(2 * theta1)) > 0 ? theta1 : theta2;
	features.push_back(theta);
	features.push_back(_obj.m_objID);

	//将这一帧的样本特征输出到文件
	ofstream outfile(m_output_samples_filepath, ios::app);
	outfile << features[0] << "	";
	outfile << features[1] << "	";
	outfile << features[2] << "	";
	outfile << features[3] << "	";
	outfile << features[4] << "	";
	outfile << features[5] << "	";
	outfile << features[6] << "	"<< endl;
	outfile.close();
}

void PVClassify::GetObjRatio1323(const vector<cv::Point>& _contours, const cv::Rect& _rect, double & _ratio13, double& _ratio23)
{
	int ycoord13(0), ycoord23(0);//存放1/3,2/3高度处的y坐标
	ycoord13 = _rect.y + _rect.height / 3;
	ycoord23 = _rect.y + _rect.height * 2 / 3;

	vector<int> xcoord13s;//获取y坐标位于1/3和2/3高度的轮廓上所有点的x坐标
	vector<int> xcoord23s;
	for (auto it = _contours.begin(); it != _contours.end(); ++it)
	{
		if (it->y == ycoord13)
		{
			xcoord13s.push_back(it->x);
		}
		if (it->y == ycoord23)
		{
			xcoord23s.push_back(it->x);
		}
	}

	int min_x(100000), max_x(0);
	for (unsigned int i = 0; i < xcoord13s.size(); ++i)
	{
		if (xcoord13s[i] < min_x)
		{
			min_x = xcoord13s[i];
		}
		if (xcoord13s[i]>max_x)
		{
			max_x = xcoord13s[i];
		}
	}
	if ((max_x - min_x) < 0.1)
	{
		_ratio13 = 0;
	}
	else
	{
		_ratio13 = fabs((double)_rect.height / (max_x - min_x));
	}

	min_x = 100000;
	max_x = 0;
	for (unsigned int i = 0; i < xcoord23s.size(); ++i)
	{
		if (xcoord23s[i] < min_x)
		{
			min_x = xcoord23s[i];
		}
		if (xcoord23s[i]>max_x)
		{
			max_x = xcoord23s[i];
		}
	}
	if ((max_x - min_x) < 0.1)
	{
		_ratio23 = 0;
	}
	else
	{
		_ratio23 = fabs((double)_rect.height / (max_x - min_x));
	}
}