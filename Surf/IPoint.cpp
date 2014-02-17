/************************************************************************/
/* FileName:	IPoint.cpp											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#include "IPoint.h"
#include "math.h"

using namespace std;

IPoint::IPoint()
{

}

//! Gets the distance in descriptor space between Ipoints
float IPoint::operator-(const IPoint &rhs)
{
	float sum=0.f;
	for(int i=0; i < 64; ++i)
		sum += (this->descriptor[i] - rhs.descriptor[i])*(this->descriptor[i] - rhs.descriptor[i]);
	return sqrt(sum);
}

//! Populate IpPairVec with matched ipts 
void IPoint::GetMatches(vector<IPoint> &ipts1, vector<IPoint> &ipts2, vector< pair<IPoint,IPoint> > &matches)
{
  float dist, d1, d2;
  IPoint *match;

  matches.clear();

  for(unsigned int i = 0; i < ipts1.size(); i++) 
  {
    d1 = d2 = FLT_MAX;

    for(unsigned int j = 0; j < ipts2.size(); j++) 
    {
      dist = ipts1[i] - ipts2[j];  

      if(dist<d1) // if this feature matches better than current best
      {
        d2 = d1;
        d1 = dist;
        match = &ipts2[j];
      }
      else if(dist<d2) // this feature matches better than second best
      {
        d2 = dist;
      }
    }

    // If match has a d1:d2 ratio < 0.65 ipoints are a match
    if(d1/d2 < 0.65) 
    { 
      // Store the change in position
      ipts1[i].dx = match->x - ipts1[i].x; 
      ipts1[i].dy = match->y - ipts1[i].y;
      matches.push_back(std::make_pair(ipts1[i], *match));
    }
  }
}

IPoint::~IPoint(void)
{
}
