/************************************************************************/
/* FileName:	SurfDescriptor.h											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#pragma once
#include <opencv2\opencv.hpp>
#include <vector>
#include "IPoint.h"
#include "IntegralImg.h"

class SurfDescriptor
{
public:
	IntegralImg &Img;
	std::vector<IPoint> &IPoints;

	void GetOrientation();
	void GetDescriptor();

	float gaussian(int x, int y, float sig);
	float gaussian(float x, float y, float sig);
	float haarX(int row, int column, int s);
	float haarY(int row, int column, int s);
	float getAngle(float X, float Y);
	float RotateX(float x, float y ,float si, float co);
	float RotateY(float x, float y ,float si, float co);
	int fRound(float flt);
	void DrawOrientation();

	SurfDescriptor(IntegralImg &img, std::vector<IPoint> &iPoints);
	~SurfDescriptor(void);
};

