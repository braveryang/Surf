/************************************************************************/
/* FileName:	IPoint.h											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#pragma once
#include <opencv2\opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

class IPoint:public Point2f
{
public:
	IPoint(void);
	~IPoint(void);
	//float x;
	//float y;
	float dx;
	float dy;
	float scale;
	float orientation;
	float laplacian;
	float descriptor[64];

	float operator-(const IPoint &rhs);
	static void GetMatches(vector<IPoint> &ipts1, vector<IPoint> &ipts2, vector< pair<IPoint,IPoint> > &matches);
};

