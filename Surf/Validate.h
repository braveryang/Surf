/************************************************************************/
/* FileName:	Validate.h											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#pragma once
#include <opencv2\opencv.hpp>
#include <vector>
#include <fstream>
#include "IPoint.h"

class Validate
{
public:
	char HomographyMatrixFileName[100];
	cv::Mat H;

	Validate(char * filename);
	~Validate(void);

	cv::Mat LoadHomographyMatrix(char * filename);
	std::vector<float> Compare(std::vector< std::pair<IPoint,IPoint> > &matches, std::vector<IPoint> & plist3);
	cv::Mat Validate::CalculateHomographyMatrix(std::vector< std::pair<IPoint,IPoint> > &matches);
	void Test();
};

