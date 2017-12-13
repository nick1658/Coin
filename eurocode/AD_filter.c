
#include "def.h"



//The median value of the average
//��λֵƽ���˲�������
U16 filterMedianAverageSample (U16 *adValueBuf, int sampleNum)
{
	int i, j;
	U16 temp = 0, sum = 0;
	
	for (j = 0; j < sampleNum - 1; j++)
	{
		for (i = 0; i < sampleNum - j && (i+1) < sampleNum; i++)
		{
			if (adValueBuf[i] > adValueBuf[i+1])
			{
				temp = adValueBuf[i];
				adValueBuf[i] = adValueBuf[i+1];
				adValueBuf[i+1] = temp;
			}
		}
	}
	
	for (i = 1; i < sampleNum - 1; i++)
	{
		sum += adValueBuf[i];
	}
	
	return (sum / sampleNum);
}


//��λֵ�˲�������
U16 filterMedianSample (U16 *adValueBuf, int sampleNum)
{
	int i, j;
	U16 temp = 0, sum = 0;
	
	for (j = 0; j < sampleNum - 1; j++)
	{
		for (i = 0; i < sampleNum - j && (i+1) < sampleNum; i++)
		{
			if (adValueBuf[i] > adValueBuf[i+1])
			{
				temp = adValueBuf[i];
				adValueBuf[i] = adValueBuf[i+1];
				adValueBuf[i+1] = temp;
			}
		}
	}
	
	for (i = 1; i < sampleNum - 1; i++)
	{
		sum += adValueBuf[i];
	}
	
	return (adValueBuf[(sampleNum - 1) / 2]);
}

