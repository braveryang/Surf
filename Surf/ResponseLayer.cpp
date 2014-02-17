/************************************************************************/
/* FileName:	ResponseLayer.cpp											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#include "ResponseLayer.h"


ResponseLayer::ResponseLayer(IntegralImg * img, int octave, int interval)
{
	this->Step=(int)pow(2.0,octave-1);
	this->Width=img->Width/this->Step;
	this->Height=img->Height/this->Step;
	this->Lobe=(int)pow(2.0,octave)*interval+1;
	this->Lobe2=this->Lobe*2-1;
	this->Size=3*this->Lobe;
	this->Border=this->Size/2;
	this->Count=this->Size*this->Size;
	this->Octave=octave;
	this->Interval=interval;
	this->Data=new Mat(this->Height,this->Width,CV_32FC1);
	this->LapData=new Mat(this->Height,this->Width,CV_32FC1);
	this->BuildLayerData(img);
}

void ResponseLayer::BuildLayerData(IntegralImg * img)
{
	float inverse_area = 1.0/this->Count;
	float Dxx,Dyy,Dxy;

	for(int r=0,x=0;x<Height;r+=this->Step,x+=1)
	{
		for(int c=0,y=0;y<Width;c+=this->Step,y+=1)
		{
			Dxx=img->AreaSum(r-Lobe+1,c-Border,Lobe2,Size)-img->AreaSum(r-Lobe+1,c-Lobe/2,Lobe2,Lobe)*3;
			Dyy=img->AreaSum(r-Border,c-Lobe+1,Size,Lobe2)-img->AreaSum(r-Lobe/2,c-Lobe+1,Lobe,Lobe2)*3;
			Dxy=img->AreaSum(r-Lobe,c+1,Lobe,Lobe)+img->AreaSum(r+1,c-Lobe,Lobe,Lobe)
				-img->AreaSum(r-Lobe,c-Lobe,Lobe,Lobe)-img->AreaSum(r+1,c+1,Lobe,Lobe);
			Dxx*=inverse_area;
			Dyy*=inverse_area;
			Dxy*=inverse_area;

			this->Data->at<float>(x,y)=(Dxx * Dyy - 0.81f * Dxy * Dxy);
			this->LapData->at<float>(x,y) = (Dxx + Dyy >= 0 ? 1 : 0);
		}
	}
}

float ResponseLayer::GetResponse(int x,int y, int step)
{
	int scale = step/this->Step;
	//std::cout<<this->Data->at<float>((x*scale),(y*scale))<<std::endl;
	return this->Data->at<float>((x*scale),(y*scale));
}

float ResponseLayer::GetLaplacian(int x,int y,int step)
{
	int scale = step/this->Step;
	return this->LapData->at<float>((x*scale),(y*scale));
}

ResponseLayer::~ResponseLayer(void)
{
}
