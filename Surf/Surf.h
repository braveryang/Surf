/************************************************************************/
/* FileName:	Surf.h											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#pragma once
#include <opencv2\opencv.hpp>
#include <vector>
#include "IPoint.h"

using namespace std;
using namespace cv;

class Surf
{
public:
	Surf(void);
	~Surf(void);

	vector<IPoint> GetAllFeatures(Mat img);
};

