/************************************************************************/
/* FileName:	IntegralImg.cpp											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#include "IntegralImg.h"

//初始化积分图像
IntegralImg::IntegralImg(Mat img)
{
	this->Original=img;
	cv::integral(this->Original,this->Integral);
	this->Width=img.cols;
	this->Height=img.rows;
}


IntegralImg::~IntegralImg(void)
{

}

//计算积分图像 A-B-C+D
float IntegralImg::AreaSum(int x,int y,int dx,int dy)
{
	int r1;
	int c1;
	int r2;
	int c2;
	r1 = std::min(x,Height);
	c1 = std::min(y,Width);
	r2 = std::min(x+dx,Height);
	c2 = std::min(y+dy,Width);
	r1 = std::max(r1,0);
	c1 = std::max(c1,0);
	r2 = std::max(r2,0);
	c2 = std::max(c2,0);
	double A = this->Integral.at<double>(r1,c1);
	double B = this->Integral.at<double>(r2,c1);
	double C = this->Integral.at<double>(r1,c2);
	double D = this->Integral.at<double>(r2,c2);
	return (float)std::max(0.0,A+D-B-C);
}