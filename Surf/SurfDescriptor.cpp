/************************************************************************/
/* FileName:	SurfDescriptor.cpp											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#include "SurfDescriptor.h"

using namespace std;

//-------------------------------------------------------
//! SURF priors (these need not be done at runtime)
const float pi = 3.14159f;

//! lookup table for 2d gaussian (sigma = 2.5) where (0,0) is top left and (6,6) is bottom right
const float gauss25 [7][7] = {
  0.02546481,	0.02350698,	0.01849125,	0.01239505,	0.00708017,	0.00344629,	0.00142946,
  0.02350698,	0.02169968,	0.01706957,	0.01144208,	0.00653582,	0.00318132,	0.00131956,
  0.01849125,	0.01706957,	0.01342740,	0.00900066,	0.00514126,	0.00250252,	0.00103800,
  0.01239505,	0.01144208,	0.00900066,	0.00603332,	0.00344629,	0.00167749,	0.00069579,
  0.00708017,	0.00653582,	0.00514126,	0.00344629,	0.00196855,	0.00095820,	0.00039744,
  0.00344629,	0.00318132,	0.00250252,	0.00167749,	0.00095820,	0.00046640,	0.00019346,
  0.00142946,	0.00131956,	0.00103800,	0.00069579,	0.00039744,	0.00019346,	0.00008024
};

//-------------------------------------------------------

SurfDescriptor::SurfDescriptor(IntegralImg &img, std::vector<IPoint> &iPoints):Img(img),IPoints(iPoints)
{

}


SurfDescriptor::~SurfDescriptor(void)
{
}

//我主要提取了有旋转不变性的特征

//提取当前关键点在附近区域的主方向
void SurfDescriptor::GetOrientation()
{
	for(int i=0;i<this->IPoints.size();i++)
	{
		const int pCount=109;
		IPoint &p = IPoints[i];
		float gauss=0.f;
		int s=fRound(p.scale),r=fRound(p.y),c=fRound(p.x);
		float resX[pCount],resY[pCount],Ang[pCount];
		int id[]={6,5,4,3,2,1,0,1,2,3,4,5,6};

		int idx=0;

		//计算6倍scale的区域的haar特征
		for(int i=-6;i<=6;i++)
		{
			for(int j=-6;j<=6;j++)
			{
				if(i*i+j*j<36)
				{
					//用4倍scale的haar特征提取x y方向上的梯度特征
					//为何是4倍sigma？
					gauss = gauss25[id[i+6]][id[j+6]];
					resX[idx] = gauss * haarX(r+j*s, c+i*s, 4*s);
					resY[idx] = gauss * haarY(r+j*s, c+i*s, 4*s);
					//计算当前点的方向特征
					Ang[idx] = getAngle(resX[idx], resY[idx]);
					idx++;
				}
			}
		}

		//计算主方向
		float sumX=0.f, sumY=0.f;
		float maxX=0.f, maxY=0.f;
		float max=0.f, orientation=0.f;
		float ang1=0.f, ang2=0.f;
		
		//计算pi/3扇形的特征点
		//步长为0.15
		float pi3=pi/3.0f;
		for(ang1=0; ang1<2*pi;ang1+=0.15f)
		{
			ang2 = (ang1+pi3>2*pi ? ang1-5.0f*pi3 : ang1+pi3);
			sumX = sumY =0.f;
			for(int k=0;k<pCount;k++)
			{
				const float & ang = Ang[k];
				if(ang1<ang2 && ang1<ang && ang<ang2)
				{
					sumX+=resX[k];
					sumY+=resY[k];
				}
				//居然在or上面少了一对括号
				else if(ang1>ang2 && 
					((0<ang && ang<ang2) || (ang1<ang && ang<2*pi)))
				{
					sumX+=resX[k];
					sumY+=resY[k];
				}
			}

			//找到主方向，也就是模最大的方向
			if(sumX*sumX + sumY*sumY > max)
			{
				max=sumX*sumX + sumY*sumY;
				maxX=sumX;
				maxY=sumY;
			}
		}

		p.orientation = getAngle(maxX,maxY);
	}
}

//画出主方向特征
void SurfDescriptor::DrawOrientation()
{
	int r1,c1,c2,r2;
	for(int i=0;i<this->IPoints.size();i++)
	{
		r1 = fRound(IPoints[i].y);
		c1 = fRound(IPoints[i].x);
		c2 = fRound(10 * cos(IPoints[i].orientation)) + c1;
		r2 = fRound(10 * sin(IPoints[i].orientation)) + r1;
		cv::line(this->Img.Original,cv::Point(c1, r1), cv::Point(c2, r2), cv::Scalar(0, 255, 0));
	}
	imshow("d",this->Img.Original);
}

//根据主方向的值，提取4*4*4=64维的Haar特征
void SurfDescriptor::GetDescriptor()
{
	//OpenSURF 在这个函数写得让人很难理解
	//不像是为了效率，反正写得很纠结
	//由于将特征点附近区域划分成4*4的子区域
	//那么特征点是某个子区域的中心点
	int o[]={-7, -2, 3, 8};
	//int so[]={-2, -1, 0, 1, 2};

	for(int t=0;t<this->IPoints.size();t++)
	{
		IPoint &p = IPoints[t];
		float scale = p.scale;
		float * desp = p.descriptor;
		int x = fRound(p.x);
		int y = fRound(p.y);
		float co = cos(p.orientation);
		float si = sin(p.orientation);
		float cx = -0.5f, cy = 0.f; //Subregion centers for the 4x4 gaussian weighting
		int count = 0;
		float len = 0.f;

		for(int i=0;i<4;i++)
		{
			cx += 1.f;
			cy = -0.5f;
			for(int j=0;j<4;j++)
			{
				int xs = fRound(RotateX(scale*o[i],scale*o[j],si,co) + x);
				int ys = fRound(RotateY(scale*o[i],scale*o[j],si,co) + y);
				float dx=0.f,dy=0.f,mdx=0.f,mdy=0.f;

				cy += 1.f;
				for(int k=o[i]-5;k<=o[i]+3;k++)
				{
					for(int l=o[j]-5;l<=o[j]+3;l++)
					{
						int sample_x = fRound(RotateX(scale*k,scale*l,si,co) + x);
						int sample_y = fRound(RotateY(scale*k,scale*l,si,co) + y);

						//为何是2.5*scale，文章里面写的是3.3*scale
						float gauss_s1 = gaussian(xs-sample_x,ys-sample_y,2.5f*scale);
						float rx = haarX(sample_y,sample_x,2*fRound(scale));
						float ry = haarY(sample_y,sample_x,2*fRound(scale));

						float rrx = gauss_s1*RotateX(rx,ry,si,co);
						float rry = gauss_s1*RotateY(rx,ry,si,co);

						dx += rrx;
						dy += rry;
						mdx += fabs(rrx);
						mdy += fabs(rry);
					}
				}

				float gauss_s2 = gaussian(cx-2.f,cy-2.f,1.5f);

				desp[count++] = dx*gauss_s2;
				desp[count++] = dy*gauss_s2;
				desp[count++] = mdx*gauss_s2;
				desp[count++] = mdy*gauss_s2;

				len += (dx*dx + dy*dy + mdx*mdx + mdy*mdy) * gauss_s2*gauss_s2;
			}
		}

		len = sqrt(len);
		for(int i = 0; i < 64; ++i)
			desp[i] /= len;

	}

}

//以下为辅助函数
//根据角度，旋转坐标
inline float SurfDescriptor::RotateX(float x, float y ,float si, float co)
{
	return -x*si+y*co;
}

inline float SurfDescriptor::RotateY(float x, float y ,float si, float co)
{
	return x*co+y*si;
}

//! Round float to nearest integer
inline int SurfDescriptor::fRound(float flt)
{
  return (int) floor(flt+0.5f);
}
//-------------------------------------------------------

//! Calculate the value of the 2d gaussian at x,y
inline float SurfDescriptor::gaussian(int x, int y, float sig)
{
  return (1.0f/(2.0f*pi*sig*sig)) * exp( -(x*x+y*y)/(2.0f*sig*sig));
}

//-------------------------------------------------------

//! Calculate the value of the 2d gaussian at x,y
inline float SurfDescriptor::gaussian(float x, float y, float sig)
{
  return 1.0f/(2.0f*pi*sig*sig) * exp( -(x*x+y*y)/(2.0f*sig*sig));
}

//-------------------------------------------------------

//! Calculate Haar wavelet responses in x direction
inline float SurfDescriptor::haarX(int row, int column, int s)
{
	return Img.AreaSum(row-s/2, column, s, s/2) 
		- Img.AreaSum(row-s/2, column-s/2, s, s/2);
}

//-------------------------------------------------------

//! Calculate Haar wavelet responses in y direction
inline float SurfDescriptor::haarY(int row, int column, int s)
{
	return Img.AreaSum(row, column-s/2, s/2, s) 
		- Img.AreaSum(row-s/2, column-s/2, s/2, s);
}

//-------------------------------------------------------

//! Get the angle from the +ve x-axis of the vector given by (X Y)
float SurfDescriptor::getAngle(float X, float Y)
{
  if(X > 0 && Y >= 0)
    return atan(Y/X);

  if(X < 0 && Y >= 0)
    return pi - atan(-Y/X);

  if(X < 0 && Y < 0)
    return pi + atan(Y/X);

  if(X > 0 && Y < 0)
    return 2*pi - atan(-Y/X);

  return 0;
}