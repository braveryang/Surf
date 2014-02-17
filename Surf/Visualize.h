/************************************************************************/
/* FileName:	Visualize.h											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#pragma once
#include <opencv2\opencv.hpp>
#include <vector>
#include "IPoint.h"
class Visualize
{
public:
	Visualize(void);
	~Visualize(void);

	void DrawIPoint(char * name, cv::Mat img, std::vector<IPoint> ipts);
	void DrawMatch(cv::Mat img1, cv::Mat img2, std::vector< std::pair<IPoint,IPoint> > matches);
	void DrawMatchStep(cv::Mat img1, cv::Mat img2, std::vector< std::pair<IPoint,IPoint> > matches);
	int fRound(float flt);
};

