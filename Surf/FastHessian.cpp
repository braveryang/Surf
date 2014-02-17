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
	//��Octaveѭ��
	for (int o = 1; o <= this->Octaves; ++o) 
	{
		//һ��Octave������Interval��Size�Ĳ�ֵ
		int step = (int)(3*pow(2.0,o));
		//���㵱ǰ������Ҫ�����ӵ�Size
		int size = step+3;
		//�����ӵ����ò���
		int s=(int)pow(2.0,o-1);
		//�����ͼƬ�Ŀ��
		int width=this->Img.Width/s;
		//�����ͼƬ�ĳ���
		int height=this->Img.Height/s;

		//��Intervalѭ��
		for (int i = 1; i <= this->Intervals-2; ++i)
		{

			b = this->Pyramid[size];			//��ײ�
			m = this->Pyramid[size+step];		//�м��
			t = this->Pyramid[size+2*step];		//��߲�

			//����Border����Border�ڵ����ز���¼Ϊ�ؼ���
			//�����Border������Щ�ɻ�Ϊ��Ҫ����Step��
			int border=(t->Border+1)/(t->Step);

			//�������еĵ㣬Ѱ�ҷ��ϼ������Ƶĵ�
			//OpenSurf������������а���Border���ڵĵ�
			//����ֱ�Ӻ�����Щ�㣬�ӵ�һ�����������ؿ�ʼ
			for (int r = border+1; r < height-border; ++r)
			{
				for (int c = border+1; c < width-border; ++c)
				{
					//�ж��м����м�Ԫ���Ƿ�������Χ26��Ԫ��������
					if (IsExtremum(r, c, s, t, m, b))
					{
						//���������ؼ���Ĳ�ֵ��Ѱ�������ؼ����������
						InterpolateExtremum(r, c, s, t, m, b);
						//cout<<'('<<r<<','<<c<<')'<<endl;
					}
				}
			}
			//����һ����
			size+=step;
		}
	}
	//ShowIPoint();
}

//��ͼƬ�ϻ����ؼ���
void FastHessian::ShowIPoint()
{
	for(int i=0;i<this->IPoints.size();i++)
	{
		cv::circle(this->Img.Original,cv::Point(this->IPoints[i].x,this->IPoints[i].y),3,cv::Scalar(255,0,0,100));
	}
	imshow("d",this->Img.Original);
}

//����ֵ����
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
			t->GetResponse(r+rr, c+cc, step) >= candidate ||							//�붥��9��Ԫ�رȽ�
			((rr != 0 || cc != 0) && m->GetResponse(r+rr, c+cc, step) >= candidate) ||	//���м��8��Ԫ�رȽ�
			b->GetResponse(r+rr, c+cc, step) >= candidate								//��ײ�9��Ԫ�رȽ�
			) 
			return 0;
		}
	}
	return 1;
}

//���������
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

//��̩��չ����⼫ֵ��
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

//����һ�׵���
Mat FastHessian::Deriv3D(int r,int c,int step,ResponseLayer *t, ResponseLayer *m, ResponseLayer *b)
{
	double dx,dy,ds;
	dx = (m->GetResponse(r, c + 1, step) - m->GetResponse(r, c - 1, step)) / 2.0;
	dy = (m->GetResponse(r + 1, c, step) - m->GetResponse(r - 1, c, step)) / 2.0;
	ds = (t->GetResponse(r, c, step) - b->GetResponse(r, c, step)) / 2.0;

	//����һ�׵���
	Mat dI = (Mat_<double>(3,1) << dx,dy,ds);

	return dI;
}

//������׵���
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
  
  //����Hessian����
  Mat H = (Mat_<double>(3,3) << 
	  dxx,dxy,dxs,
	  dxy,dyy,dys,
	  dxs,dys,dss);

  return H;
}

FastHessian::~FastHessian(void)
{
}
