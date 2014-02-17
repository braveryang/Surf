#pragma once
#include "IntegralImg.h"
#include "IPoint.h"
#include "ResponseLayer.h"
#include <vector>
#include <map>
/************************************************************************/
/* FileName:	FastHessian.h											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
class FastHessian
{
public:
	
	IntegralImg Img;
	int Octaves;
	int Intervals;
	float Threshold;
	std::map<int,ResponseLayer *> Pyramid;
	std::vector<IPoint> IPoints;

	FastHessian(IntegralImg iImg,int octaves,int intervals,float threshold);
	~FastHessian(void);

	void GeneratePyramid();
	bool IsExtremum(int r,int c,
		int step,ResponseLayer * t,ResponseLayer * m,ResponseLayer * b);
	void GetIPoints();
	void ShowIPoint();
	Mat Deriv3D(int r,int c,int step,
		ResponseLayer *t, ResponseLayer *m, ResponseLayer *b);
	Mat Hessian3D(int r, int c, int step,
		ResponseLayer *t, ResponseLayer *m, ResponseLayer *b);
	void InterpolateStep(int r, int c,int step,
		ResponseLayer *t, ResponseLayer *m, ResponseLayer *b, 
        double* xi, double* xr, double* xc );
	void InterpolateExtremum(int r, int c,int step, 
		ResponseLayer *t, ResponseLayer *m, ResponseLayer *b);
};

