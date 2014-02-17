/************************************************************************/
/* FileName:	main.cpp											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#include <iostream>
#include <opencv2\opencv.hpp>
#include "IntegralImg.h"
#include "ResponseLayer.h"
#include "FastHessian.h"
#include "SurfDescriptor.h"
#include "Visualize.h"
#include "Validate.h"
#include "Surf.h"

#include "time.h"

using namespace std;
using namespace cv;

Mat ReadFloatImg(char * szFilename)
{
	Mat iImg = imread(szFilename,0);
	Mat fImg;
	iImg.convertTo(fImg,CV_32FC1);
	fImg/=255.0;
	return fImg;
}

void StaticMatch(char * imgname1, char * imgname2, char * Hname)
{
	Surf surf;

	Mat colorImg1 = imread(imgname1);
	Mat img1 = ReadFloatImg(imgname1);
	Mat colorImg2 = imread(imgname2);
	Mat img2 = ReadFloatImg(imgname2);
	Visualize v;

	cout<<"Get Features from Image 1:"<<endl;
	vector<IPoint> ips1 = surf.GetAllFeatures(img1);
	//v.DrawIPoint("img1",colorImg1,ips1);
	cout<<endl;

	cout<<"Get Features from Image 2:"<<endl;
	vector<IPoint> ips2 = surf.GetAllFeatures(img2);
	//v.DrawIPoint("img2",colorImg2,ips2);
	cout<<endl;

	vector< pair<IPoint,IPoint> > matches;
	IPoint::GetMatches(ips1,ips2,matches);
	cout<<"Match points: "<<matches.size()<<endl;

	v.DrawMatch(colorImg1,colorImg2,matches);

	int Width = img1.cols + img2.cols;
	int Height = max(img1.rows,img2.rows);
	Mat Conbine=Mat::zeros(Height,Width,CV_8UC3);
	colorImg1.copyTo(Conbine(Rect(0, 0, colorImg1.cols, colorImg1.rows)));
	colorImg2.copyTo(Conbine(Rect(colorImg1.cols,0,colorImg2.cols,colorImg2.rows)));
	imwrite(String(Hname).append(".jpg"),Conbine);

	Validate va(Hname);
	vector<IPoint> groundTruth;
	vector<float> error = va.Compare(matches,groundTruth);

	int right = 0;
	for(int i=0;i<error.size();i++)
	{
		if(error[i]<4)
		{
			right+=1;
		}
	}
	cout<<"Repeatability: "<<(float)right/error.size()<<endl;

	Mat HMatrix = va.CalculateHomographyMatrix(matches);

	cout<<"Homography Matrix:\n"<<HMatrix;

	ofstream fout("ipairs.txt");
	for(int i=0;i<matches.size();i++)
	{
		fout<<matches[i].first.x<<"\t"<<matches[i].first.y<<"\t"<<matches[i].second.x<<"\t"<<matches[i].second.y<<"\n";
	}
	fout.close();

	ofstream mout("HMatrix.txt");
	mout<<HMatrix;
	mout.close();

	waitKey();

}

int main()
{
	//Validate va(""); va.Test(); waitKey();
	StaticMatch("1.jpg","2.jpg","HMatrix");
	//StaticMatch("img1.png","img2.png","H1to2p");
	//StaticMatch("img1.png","img3.png","H1to3p");
	//StaticMatch("img1.png","img4.png","H1to4p");
	//StaticMatch("img1.png","img5.png","H1to5p");
	//StaticMatch("img1.png","img6.png","H1to6p");
	return 0;
}

	//imshow("s",IImg.Integral);
	//cout<<IImg.AreaSum(IImg.Height-4,IImg.Width-4,3,3)<<endl;
	//ResponseLayer rl1(&IImg,3,1);
	//imshow("d1",abs(*rl1.Data)*100);
	//ResponseLayer rl2(&IImg,3,2);
	//imshow("d2",abs(*rl2.Data)*100);
	//ResponseLayer rl3(&IImg,3,3);
	//imshow("d3",abs(*rl3.Data)*100);