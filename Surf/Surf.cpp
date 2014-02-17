/************************************************************************/
/* FileName:	Surf.cpp											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#include "Surf.h"
#include "FastHessian.h"
#include "IntegralImg.h"
#include "ResponseLayer.h"
#include "SurfDescriptor.h"
#include "time.h"

Surf::Surf(void)
{
}

vector<IPoint> Surf::GetAllFeatures(Mat img)
{
	clock_t start;
	clock_t end;

	start = clock();
	IntegralImg IImg(img);
	end = clock();
	std::cout<< "IntegralImg took: " << float(end - start) / CLOCKS_PER_SEC  << " seconds" << std::endl;

	start = clock();
	FastHessian fh(IImg,4,4,0.0001);
	fh.GetIPoints();
	end = clock();
	std::cout<< "FastHessian took: " << float(end - start) / CLOCKS_PER_SEC  << " seconds" << std::endl;

	start = clock();
	SurfDescriptor sd(IImg,fh.IPoints);
	sd.GetOrientation();
	sd.GetDescriptor();
	end = clock();
	std::cout<< "Descriptor took: " << float(end - start) / CLOCKS_PER_SEC  << " seconds" << std::endl;

	return fh.IPoints;
}

Surf::~Surf(void)
{
}
