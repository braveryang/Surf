/************************************************************************/
/* FileName:	ResponseLayer.h											*/
/* Author:		Pang Liang												*/
/* Dependence:	OpenCV 2.4                    				            */
/* Date:        2013.5		                                            */
/************************************************************************/
#pragma once
#include "IntegralImg.h"
class ResponseLayer
{
public:
	//����ͼ��Ŀ��
	int Width;
	//����ͼ��ĸ߶�
	int Height;
	//ģ�����õĲ���
	int Step;
	//ģ��ĳ��ȵ�1/3
	int Lobe;
	//Lobe*2-1
	int Lobe2;
	//ģ��ĳ���һ�룬�߿�
	int Border;
	//ģ�峤��
	int Size;
	//ģ��Ԫ�ظ���
	int Count;
	//����������
	int Octave;
	//����������
	int Interval;
	//��˹������ͼƬ
	Mat * Data;
	//Laplacian����
	Mat * LapData;

	ResponseLayer(IntegralImg * img, int octave, int interval);
	~ResponseLayer(void);

	void BuildLayerData(IntegralImg * img);
	float GetResponse(int x,int y, int step);
	float GetLaplacian(int x,int y,int step);
};

