// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 YV12TORGB24_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// YV12TORGB24_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef MTTAM_EXPORTS
#define MTTAM_API __declspec(dllexport)
#else
#define MTTAM_API __declspec(dllimport)
#endif
#include "VideoProcessor.h"
#include <opencv2/opencv.hpp>




//		1.上层把解码后的每帧RGB数据传给分析DLL
//      2.DLL分析每帧图像并传出分析结果（以前的功能，保持不变），将原始目标图像保留在自身内存中。
//      3.目标消失时根据目标的系列视频完成目标归类校验，确定目标最终属性（最初的方式）
//      4.对判断为人的目标比较选择其出现在整个视频中目标区域最大时的图片，将该图片上的原始目标区域抠取出来保存为图片。并在分析函数的参数最后增加一项（bool bObjectDistinguish），该值为真表示有人脸取出或车牌识别出。
//      5.对判断为车的目标将在原始图片中抠出的图片传给车牌识别程序进行车牌识别，如果识别出车牌信息，也在分析函数的参数最后增加一项（bool bObjectDistinguish），该值为真表示有人脸取出或车牌识别出。
//      6.上层一旦检测到bool bObjectDistinguish为真即调用标识别信息导出函数导出目标信息；
//      7.增加一项目标识别信息导出函数，该函数输出参数（int nObjectType)，0：表示输出的返回值信息为保存的人脸路径（格式示例:"PathName:..\output\2-people.bmp;"），1：表示输出的返回值信息为车牌信息（格式示例:"PathName:..\output\2-苏E 8HG64.bmp;LicensePlate:苏E 8HG64;"）。

//VideoProcessor*		m_pclsVideoProcessor = NULL;
VideoProcessor			m_clsVideoProcessor;
std::string				m_strObjInfo;

MTTAM_API bool		YV12ToBGR24(unsigned char* pYUV,unsigned char* pBGR24,int width,int height);

MTTAM_API bool		onMTTAMInit(GetInfoCallBackFun clsGetInfoCallBackFun, bool bLPR, bool bFace, char* pathName, int _height, int _width, int nResizeRatio);

MTTAM_API bool		onMTTAMInputFrame(unsigned char* pBGR24, int _size, char* strObjectInfo, int& nNum);
																																							
MTTAM_API bool		onMTTAMStop();
