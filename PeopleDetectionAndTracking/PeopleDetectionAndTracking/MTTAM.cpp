// YV12TORGB24.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "MTTAM.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "Vfw.h"
#include "avcodec.h"
#include "avformat.h"
#include "swscale.h"

#ifdef __cplusplus
}
#endif

bool YV12ToBGR24(unsigned char* pYUV,unsigned char* pBGR24,int width,int height)
{
    if (width < 1 || height < 1 || pYUV == NULL || pBGR24 == NULL)
	{
		return false;
	}

    AVPicture pFrameYUV,pFrameBGR;
    
    avpicture_fill(&pFrameYUV,pYUV,PIX_FMT_YUV420P,width,height);

    //U,V互换
    uint8_t * ptmp=pFrameYUV.data[1];
    pFrameYUV.data[1]=pFrameYUV.data[2];
    pFrameYUV.data [2]=ptmp;

    avpicture_fill(&pFrameBGR,pBGR24,PIX_FMT_BGR24,width,height);

    struct SwsContext* imgCtx = NULL;
    imgCtx = sws_getContext(width,height,PIX_FMT_YUV420P,width,height,PIX_FMT_BGR24,SWS_BILINEAR,0,0,0);

    if (imgCtx != NULL)
	{
        sws_scale(imgCtx,pFrameYUV.data,pFrameYUV.linesize,0,height,pFrameBGR.data,pFrameBGR.linesize);
        if(imgCtx)
		{
            sws_freeContext(imgCtx);
            imgCtx = NULL;
		}
        return true;
    }
    else
	{
        sws_freeContext(imgCtx);
        imgCtx = NULL;
        return false;
    }
}

//bool onMTTAMInit(GetInfoCallBackFun clsGetInfoCallBackFun, bool bLPR, bool bFace, char* pathName, int _height, int _width, int nResizeRatio)
//{
//	m_pclsVideoProcessor = new VideoProcessor();
//
//	if(m_pclsVideoProcessor != NULL)
//	{
//		m_pclsVideoProcessor->Init(clsGetInfoCallBackFun, bLPR, bFace, pathName, _height, _width, nResizeRatio);
//		
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}

bool onMTTAMInit(GetInfoCallBackFun clsGetInfoCallBackFun, bool bLPR, bool bFace, char* pathName, int _height, int _width, int nResizeRatio)
{

	m_clsVideoProcessor.Init(clsGetInfoCallBackFun, bLPR, bFace, pathName, _height, _width, nResizeRatio);

	return true;
}

//bool onMTTAMInputFrame(unsigned char* pBGR24, int _size, char* strObjectInfo, int& nNum)
//{
//
//	if (m_pclsVideoProcessor != NULL)
//	{
//
//		m_pclsVideoProcessor->Run(pBGR24, _size, m_pclsVideoProcessor->strObjectInfo, m_pclsVideoProcessor->nObjectClass);
//
//		//*strObjectInfo="objectnum:5;ID01:object01;Type01:1;UpperPart01:red;LowerPart01:blue;X01:137;Y01:145;ID02:object02;Type02:0;UpperPart02:red;LowerPart02:blue;X02:147;Y02:146;ID03:object03;Type03:0;UpperPart03:red;LowerPart03:blue;X03:147;Y03:146;ID04:object04;Type04:0;UpperPart04:red;LowerPart04:blue;X04:147;Y04:146;ID05:object05;Type05:0;UpperPart05:red;LowerPart05:blue;X05:147;Y05:146;";
//
//		//weigang delete 20160118
//		m_pclsVideoProcessor->GetObjectInfo(m_strObjInfo);
//	
//		m_strObjInfo.copy(strObjectInfo, m_strObjInfo.length(), 0);
//
//		nNum = m_strObjInfo.length();
//
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}


bool onMTTAMInputFrame(unsigned char* pBGR24, int _size, char* strObjectInfo, int& nNum)
{

	m_clsVideoProcessor.Run(pBGR24, _size, m_clsVideoProcessor.strObjectInfo, m_clsVideoProcessor.nObjectClass);

	//*strObjectInfo="objectnum:5;ID01:object01;Type01:1;UpperPart01:red;LowerPart01:blue;X01:137;Y01:145;ID02:object02;Type02:0;UpperPart02:red;LowerPart02:blue;X02:147;Y02:146;ID03:object03;Type03:0;UpperPart03:red;LowerPart03:blue;X03:147;Y03:146;ID04:object04;Type04:0;UpperPart04:red;LowerPart04:blue;X04:147;Y04:146;ID05:object05;Type05:0;UpperPart05:red;LowerPart05:blue;X05:147;Y05:146;";
	
	//weigang delete 20160118
	m_clsVideoProcessor.GetObjectInfo(m_strObjInfo);

	m_strObjInfo.copy(strObjectInfo, m_strObjInfo.length(), 0);

	nNum = m_strObjInfo.length();

	return true;

}

bool onMTTAMStop()
{
	//if (m_pclsVideoProcessor != NULL)
	//{
	//	//m_pclsVideoProcessor->onStop();
	//	delete m_pclsVideoProcessor;

	//	m_pclsVideoProcessor = NULL;
	//}
	return true;
}
