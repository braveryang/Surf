/************************************************************************/
/* FileName:	IntegralImg.h											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#pragma once
#include <opencv2\opencv.hpp>
using namespace cv;
class IntegralImg
{
public:
	IntegralImg();
	IntegralImg(Mat img);
	~IntegralImg(void);

	int Width;		//图片的宽
	int Height;		//图片的高
	Mat Original;	//原始图片
	Mat Integral;	//积分图像
	
	float AreaSum(int x,int y,int dx,int dy);

};

