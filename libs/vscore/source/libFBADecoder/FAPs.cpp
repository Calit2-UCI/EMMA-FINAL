
#include "FAPs.h"

// C++
#include <stdlib.h>


namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FAPs::FAPs()
{
	reset();
}

FAPs::~FAPs()
{

}

FAPs::FAPs(const FAPs *faps)
{
	reset();
	set(faps);
}

FAPs::FAPs(FAPs *faps, double in)
{
	reset();
	set(faps,in);
}

void FAPs::set(const FAPs *faps)
{
	if(!faps)
		return;

	llf.set(&faps->llf);
	fap1.set(&faps->fap1);
	fap2.set(&faps->fap2);
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i =0; i< N_FAP_GROUP;i++)
		maskType[i] = faps->maskType[i];
	for(i =0; i< NFAP;i++)
		groupMask[i] = faps->groupMask[i];
}

void FAPs::merge(FAPs *faps)
{
	if(!faps)
		return;

	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i =0; i< N_FAP_GROUP;i++)
	{
		switch(maskType[i])
		{
		case 0:
			maskType[i] = faps->maskType[i];
			break;
		case 1:
			break;
		case 2:
			if(faps->maskType[i] == 1)
				maskType[i] = faps->maskType[i];
			break;
		}
	}
	for(i =0; i< NFAP;i++)
		if(!groupMask[i])
			groupMask[i] = faps->groupMask[i];

	llf.merge(&faps->llf);
	fap1.merge(&faps->fap1);
	fap2.merge(&faps->fap2);
}

void FAPs::filter(FAPs *filter)
{
	if(!filter) return;

	llf.filter(&filter->llf);
	fap2.set(&fap2, filter->fap2.intensity1 / 1000.0);
	if(filter->fap1.blend == 0)
	{
		fap1.viseme1 = 0;
		fap1.viseme2 = 0;
		fap1.blend = 0;
	}
}

void FAPs::set(FAPs *faps, double in)
{
	if(!faps)
		return;

	llf.set(&faps->llf,in);
	fap1.set(&faps->fap1);
	fap2.set(&faps->fap2,in);
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i =0; i< N_FAP_GROUP;i++)
		maskType[i] = faps->maskType[i];
	for(i =0; i< NFAP;i++)
		groupMask[i] = faps->groupMask[i];
}

void FAPs::interpolateLR()
{
	interpolateLeftRightFAPs(6,7);
	interpolateLeftRightFAPs(8,9);
	interpolateLeftRightFAPs(10,11);
	interpolateLeftRightFAPs(12,13);
	interpolateLeftRightFAPs(19,20);
	interpolateLeftRightFAPs(21,22);
	interpolateLeftRightFAPs(23,24);
	interpolateLeftRightFAPs(25,26);
	interpolateLeftRightFAPs(27,28);
	interpolateLeftRightFAPs(29,30);
	interpolateLeftRightFAPs(31,32);
	interpolateLeftRightFAPs(33,34);
	interpolateLeftRightFAPs(35,36);
	interpolateLeftRightFAPs(37,38);
	interpolateLeftRightFAPs(39,40);
	interpolateLeftRightFAPs(41,42);
	interpolateLeftRightFAPs(53,54);
	interpolateLeftRightFAPs(55,56);
	interpolateLeftRightFAPs(57,58);
	interpolateLeftRightFAPs(59,60);
	interpolateLeftRightFAPs(61,62);
	interpolateLeftRightFAPs(65,66);
	interpolateLeftRightFAPs(67,68);
}

void FAPs::interpolateLeftRightFAPs(int l, int r)
{
	l -= 1;
	r -= 1;

		if(groupMask[l] == 1 && groupMask[r] == 0)
		{
			llf.value[r] = llf.value[l];
//			groupMask[r] = 1;
		}
		if(groupMask[r] == 1 && groupMask[l] == 0)
		{
			llf.value[l] = llf.value[r];
//			groupMask[l] = 1;
		}
}

void FAPs::reset()
{
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for (i = 0; i < NFAP; i++)
	{
		groupMask[i] = 1;
	}
	for (i = 0; i < N_FAP_GROUP; i++)
	{
			maskType[i] = 3;
	}
	fap1.reset();
	fap2.reset();
	llf.reset();
}

void FAPs::setMask(bool maskValue)
{
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for (i = 0; i < NFAP; i++)
	{
		if(maskValue)
			groupMask[i] = 1;
		else
			groupMask[i] = 0;
	}
	for (i = 0; i < N_FAP_GROUP; i++)
	{
		maskType[i] = 1;
	}
}

void FAPs::setMaskIfDiff(FAPs *faps)
{
	if(!faps)
		return;
	if(!fap1.isEqual(&(faps->fap1)))
		groupMask[0] = 1;
	if(!fap2.isEqual(&(faps->fap2)))
		groupMask[1] = 1;
	for (int i = 2; i < NFAP; i++)
	{
		if(llf.value[i] != faps->llf.value[i])
			groupMask[i] = 1;
	}
}

void FAPs::setMask(FAPs *faps)
{
	if(!faps)
		return;

	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for (i = 0; i < NFAP; i++)
	{
		groupMask[i] = faps->groupMask[i];
	}
	for (i = 0; i < N_FAP_GROUP; i++)
	{
			maskType[i] = faps->maskType[i];
	}
}

}
