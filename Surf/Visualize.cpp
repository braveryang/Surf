/************************************************************************/
/* FileName:	Visualize.cpp											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#include "Visualize.h"

using namespace cv;
using namespace std;

Visualize::Visualize(void)
{
}

//画出关键点
void Visualize::DrawIPoint(char * name, cv::Mat img, std::vector<IPoint> ipts)
{
	cout<<"Surf Found: "<<ipts.size()<<" Interest Points."<<endl;
	for(int i=0;i<ipts.size();i++)
	{
		int scale = static_cast<int>(ipts[i].scale * 2.5);
		if(ipts[i].laplacian == 0)
		{
			cv::circle(img,ipts[i],scale,cv::Scalar(255,0,0));
		}
		else if(ipts[i].laplacian == 1)
		{
			cv::circle(img,ipts[i],scale,cv::Scalar(0,0,255));
		}
		int r1 = fRound(ipts[i].y);
		int c1 = fRound(ipts[i].x);
		int c2 = fRound(scale * cos(ipts[i].orientation)) + c1;
		int r2 = fRound(scale * sin(ipts[i].orientation)) + r1;
		cv::line(img,cv::Point(c1, r1), cv::Point(c2, r2), cv::Scalar(0, 255, 0));
	}
	imshow(name,img);
}

//一次画出所有匹配的点对
void Visualize::DrawMatch(cv::Mat img1, cv::Mat img2, std::vector< std::pair<IPoint,IPoint> > matches)
{
	int Width = img1.cols + img2.cols;
	int Height = max(img1.rows,img2.rows);
	//Mat Conbine=Mat::zeros(Height,Width,CV_8UC3);
	//img1.copyTo(Conbine(Rect(0, 0, img1.cols, img1.rows)));
	//img2.copyTo(Conbine(Rect(img1.cols,0,img2.cols,img2.rows)));
	//imshow("d",Conbine);
	for (unsigned int i = 0; i < matches.size(); ++i)
	{
		circle(img1,matches[i].first,4,Scalar(255,0,0));
		circle(img2,matches[i].second,4,Scalar(255,0,0));
  
		const int & w = img1.cols;
		line(img1,matches[i].first,Point(matches[i].second.x+w,matches[i].second.y), Scalar(255,255,255),1);
		line(img2,Point(matches[i].first.x-w,matches[i].first.y),matches[i].second, Scalar(255,255,255),1);
	}

	imshow("1",img1);
	imshow("2",img2);
}

//一步一步画出匹配的点对
void Visualize::DrawMatchStep(cv::Mat img1, cv::Mat img2, std::vector< std::pair<IPoint,IPoint> > matches)
{
	int Width = img1.cols + img2.cols;
	int Height = max(img1.rows,img2.rows);
	//Mat Conbine=Mat::zeros(Height,Width,CV_8UC3);
	//img1.copyTo(Conbine(Rect(0, 0, img1.cols, img1.rows)));
	//img2.copyTo(Conbine(Rect(img1.cols,0,img2.cols,img2.rows)));
	//imshow("d",Conbine);
	for (unsigned int i = 0; i < matches.size(); ++i)
	{
		circle(img1,matches[i].first,4,Scalar(255,0,0));
		circle(img2,matches[i].second,4,Scalar(255,0,0));
  
		const int & w = img1.cols;
		line(img1,matches[i].first,Point(matches[i].second.x+w,matches[i].second.y), Scalar(255,255,255),1);
		line(img2,Point(matches[i].first.x-w,matches[i].first.y),matches[i].second, Scalar(255,255,255),1);
		imshow("1", img1);
		imshow("2",img2);
		waitKey(1000);
	}


}

//! Round float to nearest integer
inline int Visualize::fRound(float flt)
{
  return (int) floor(flt+0.5f);
}

Visualize::~Visualize(void)
{
}
