/************************************************************************/
/* FileName:	ResponseLayer.h											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#pragma once
#include "IntegralImg.h"
class ResponseLayer
{
public:
	//本层图像的宽度
	int Width;
	//本层图像的高度
	int Height;
	//模板作用的步长
	int Step;
	//模板的长度的1/3
	int Lobe;
	//Lobe*2-1
	int Lobe2;
	//模板的长度一半，边框
	int Border;
	//模板长度
	int Size;
	//模板元素个数
	int Count;
	//金字塔级数
	int Octave;
	//金字塔层数
	int Interval;
	//高斯卷积后的图片
	Mat * Data;
	//Laplacian矩阵
	Mat * LapData;

	ResponseLayer(IntegralImg * img, int octave, int interval);
	~ResponseLayer(void);

	void BuildLayerData(IntegralImg * img);
	float GetResponse(int x,int y, int step);
	float GetLaplacian(int x,int y,int step);
};

