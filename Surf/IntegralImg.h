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

	int Width;		//ͼƬ�Ŀ�
	int Height;		//ͼƬ�ĸ�
	Mat Original;	//ԭʼͼƬ
	Mat Integral;	//����ͼ��
	
	float AreaSum(int x,int y,int dx,int dy);

};

