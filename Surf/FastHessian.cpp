/************************************************************************/
/* FileName:	FastHessian.cpp											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#include "FastHessian.h"
using namespace std;

FastHessian::FastHessian(IntegralImg iImg, int octaves, int intervals, float threshold)
	:Octaves(octaves),Intervals(intervals),Img(iImg),Threshold(threshold)
{
	GeneratePyramid();
}

void FastHessian::GeneratePyramid()
{
	for(int o=1;o<=Octaves;o++)
	{
		for(int i=1;i<=Intervals;i++)
		{
			int size=3*((int)pow(2.0,o)*i+1);
			if(!this->Pyramid.count(size))
			{
				this->Pyramid[size]=new ResponseLayer(&Img,o,i);
				//cv::imshow("d",abs((*(Pyramid[size])->Data)*100));
				//cv::waitKey();
			}
		}
	}
}

void FastHessian::GetIPoints()
{
	// Clear the vector of exisiting IPoints
	this->IPoints.clear();

	// Get the response layers
	ResponseLayer *b, *m, *t;
	//对Octave循环
	for (int o = 1; o <= this->Octaves; ++o) 
	{
		//一个Octave中两个Interval的Size的差值
		int step = (int)(3*pow(2.0,o));
		//计算当前层所需要的算子的Size
		int size = step+3;
		//该算子的作用步长
		int s=(int)pow(2.0,o-1);
		//卷积后图片的宽度
		int width=this->Img.Width/s;
		//卷积后图片的长度
		int height=this->Img.Height/s;

		//对Interval循环
		for (int i = 1; i <= this->Intervals-2; ++i)
		{

			b = this->Pyramid[size];			//最底层
			m = this->Pyramid[size+step];		//中间层
			t = this->Pyramid[size+2*step];		//最高层

			//计算Border，在Border内的像素不记录为关键点
			//这里的Border计算有些疑惑，为何要除以Step？
			int border=(t->Border+1)/(t->Step);

			//遍历所有的点，寻找符合极大抑制的点
			//OpenSurf里面遍历了所有包含Border在内的点
			//这里直接忽略那些点，从第一个有意义像素开始
			for (int r = border+1; r < height-border; ++r)
			{
				for (int c = border+1; c < width-border; ++c)
				{
					//判断中间层的中间元素是否是其周围26个元素中最大的
					if (IsExtremum(r, c, s, t, m, b))
					{
						//进行亚像素级别的差值，寻找亚像素级别的特征点
						InterpolateExtremum(r, c, s, t, m, b);
						//cout<<'('<<r<<','<<c<<')'<<endl;
					}
				}
			}
			//上升一个层
			size+=step;
		}
	}
	//ShowIPoint();
}

//在图片上画出关键点
void FastHessian::ShowIPoint()
{
	for(int i=0;i<this->IPoints.size();i++)
	{
		cv::circle(this->Img.Original,cv::Point(this->IPoints[i].x,this->IPoints[i].y),3,cv::Scalar(255,0,0,100));
	}
	imshow("d",this->Img.Original);
}

//极大值抑制
bool FastHessian::IsExtremum(int r,int c,int step,ResponseLayer * t,ResponseLayer * m,ResponseLayer * b)
{
	// check the candidate point in the middle layer is above thresh 
	float candidate = m->GetResponse(r, c, step);
	if (candidate < this->Threshold) 
	return 0; 

	for (int rr = -1; rr <=1; ++rr)
	{
		for (int cc = -1; cc <=1; ++cc)
		{
			// if any response in 3x3x3 is greater candidate not maximum
			if (
			t->GetResponse(r+rr, c+cc, step) >= candidate ||							//与顶层9个元素比较
			((rr != 0 || cc != 0) && m->GetResponse(r+rr, c+cc, step) >= candidate) ||	//与中间层8个元素比较
			b->GetResponse(r+rr, c+cc, step) >= candidate								//与底层9个元素比较
			) 
			return 0;
		}
	}
	return 1;
}

//亚像素求解
void FastHessian::InterpolateExtremum(int r, int c,int step, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b)
{
	// get the step distance between filters
	// check the middle filter is mid way between top and bottom
	int filterStep = (m->Size - b->Size);
	assert(filterStep > 0 && t->Size - m->Size == m->Size - b->Size);
 
	// Get the offsets to the actual location of the extremum
	double xi = 0, xr = 0, xc = 0;
	InterpolateStep(r, c, step, t, m, b, &xi, &xr, &xc );

	// If point is sufficiently close to the actual extremum
	if( fabs( xi ) < 0.5f  &&  fabs( xr ) < 0.5f  &&  fabs( xc ) < 0.5f )
	{
		IPoint p;
		p.x=static_cast<float>((c+xc)*step);
		p.y=static_cast<float>((r+xr)*step);
		p.scale=static_cast<float>((0.1333f)*(m->Size+xi*filterStep));
		p.laplacian=static_cast<int>(m->GetLaplacian(r,c,step));
		this->IPoints.push_back(p);
	}
}

//用泰勒展开求解极值点
void FastHessian::InterpolateStep(int r, int c,int step, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b, 
                                  double* xi, double* xr, double* xc )
{
  Mat dD, H, H_inv, X;

  dD = Deriv3D( r, c, step, t, m, b );
  //cout<<dD<<endl;
  H = Hessian3D( r, c, step, t, m, b );
  //cout<<H<<endl;
  invert( H, H_inv, CV_SVD );
  //cout<<H_inv<<endl;
  gemm( H_inv, dD, -1, NULL, 0, X, 0 );

  *xc = X.at<double>(0,0);
  *xr = X.at<double>(1,0);
  *xi = X.at<double>(2,0);
}

//计算一阶导数
Mat FastHessian::Deriv3D(int r,int c,int step,ResponseLayer *t, ResponseLayer *m, ResponseLayer *b)
{
	double dx,dy,ds;
	dx = (m->GetResponse(r, c + 1, step) - m->GetResponse(r, c - 1, step)) / 2.0;
	dy = (m->GetResponse(r + 1, c, step) - m->GetResponse(r - 1, c, step)) / 2.0;
	ds = (t->GetResponse(r, c, step) - b->GetResponse(r, c, step)) / 2.0;

	//构造一阶导数
	Mat dI = (Mat_<double>(3,1) << dx,dy,ds);

	return dI;
}

//计算二阶导数
Mat FastHessian::Hessian3D(int r, int c, int step, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b)
{
  double v, dxx, dyy, dss, dxy, dxs, dys;

  v = m->GetResponse(r, c, step);
  dxx = m->GetResponse(r, c + 1, step) + m->GetResponse(r, c - 1, step) - 2 * v;
  dyy = m->GetResponse(r + 1, c, step) + m->GetResponse(r - 1, c, step) - 2 * v;
  dss = t->GetResponse(r, c, step) + b->GetResponse(r, c, step) - 2 * v;
  dxy = ( m->GetResponse(r + 1, c + 1, step) - m->GetResponse(r + 1, c - 1, step) - 
          m->GetResponse(r - 1, c + 1, step) + m->GetResponse(r - 1, c - 1, step) ) / 4.0;
  dxs = ( t->GetResponse(r, c + 1, step) - t->GetResponse(r, c - 1, step) - 
          b->GetResponse(r, c + 1, step) + b->GetResponse(r, c - 1, step) ) / 4.0;
  dys = ( t->GetResponse(r + 1, c, step) - t->GetResponse(r - 1, c, step) - 
          b->GetResponse(r + 1, c, step) + b->GetResponse(r - 1, c, step) ) / 4.0;
  
  //构造Hessian矩阵
  Mat H = (Mat_<double>(3,3) << 
	  dxx,dxy,dxs,
	  dxy,dyy,dys,
	  dxs,dys,dss);

  return H;
}

FastHessian::~FastHessian(void)
{
}
