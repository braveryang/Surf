/************************************************************************/
/* FileName:	Validate.cpp											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#include "Validate.h"

using namespace cv;
using namespace std;

//这个类主要用来验证关键点的正确性

//filename指的是homography矩阵的文件名
Validate::Validate(char * filename)
{
	strcpy(this->HomographyMatrixFileName,filename);
	this->H = this->LoadHomographyMatrix(this->HomographyMatrixFileName);
}

//加载Homography矩阵
Mat Validate::LoadHomographyMatrix(char * filename)
{
	ifstream ifs(filename);
	Mat H(3,3,CV_32FC1);
	ifs >>H.at<float>(0,0)>>H.at<float>(0,1)>>H.at<float>(0,2)
		>>H.at<float>(1,0)>>H.at<float>(1,1)>>H.at<float>(1,2)
		>>H.at<float>(2,0)>>H.at<float>(2,1)>>H.at<float>(2,2);
	return H;
}

//计算关键点的误差
//参数GroundTruth为输出，表示正确的关键点位置
std::vector<float> Validate::Compare(vector< pair<IPoint,IPoint> > &matches, vector<IPoint> & GroundTruth)
{
	std::vector<float> error;
	for(int i = 0;i<matches.size();i++)
	{
		Mat p1 = (Mat_<float>(3,1) << matches[i].first.x, matches[i].first.y, 1);
		Mat p2 = this->H*p1;
		IPoint p3;
		p3.x = p2.at<float>(0,0)/p2.at<float>(2,0);
		p3.y = p2.at<float>(1,0)/p2.at<float>(2,0);
		GroundTruth.push_back(p3);
		error.push_back((matches[i].second.x-p3.x)*(matches[i].second.x-p3.x)+(matches[i].second.y-p3.y)*(matches[i].second.y-p3.y));
	}
	return error;
}

//预测单应矩阵
Mat Validate::CalculateHomographyMatrix(vector< pair<IPoint,IPoint> > &matches)
{
	std::vector<Point2f> plist1;   
	std::vector<Point2f> plist2;
	for(int i=0;i<matches.size();i++)
	{
		plist1.push_back(matches[i].first);
		plist2.push_back(matches[i].second);
	}
	return cv::findHomography(plist1,plist2);
}

//测试函数
void Validate::Test()
{
	Mat img1 = imread("1.jpg");
	Mat img2 = imread("2.jpg");
	Mat img3;
	Mat H = this->LoadHomographyMatrix("HMatrix.txt");
	Mat p1 = (Mat_<float>(3,1) << 100.f, 100.f, 1.f);
	Mat p2 = H*p1;
	cout<<p1<<endl<<p2<<endl;
	/*
	circle(img1,
		Point(
		static_cast<int>(p1.at<float>(0,0)),
		static_cast<int>(p1.at<float>(1,0))),
		10,Scalar(255,0,0));
	circle(img2,
		Point(
		static_cast<int>(p2.at<float>(0,0)),
		static_cast<int>(p2.at<float>(1,0))),
		10,Scalar(255,0,0));
		*/
	imshow("1",img1);
	imshow("2",img2);
	cv::warpPerspective(img1,img3,H,img1.size());
	imshow("3",img3);
	imwrite("3.jpg",img3);
}

Validate::~Validate(void)
{
}
