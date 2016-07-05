
#include "BAPs.h"

// C++
#include <stdlib.h>


namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BAPs::BAPs()
{
	reset();
}

BAPs::~BAPs()
{

}

BAPs::BAPs(const BAPs *baps)
{
	reset();
	set(baps);
}

void BAPs::set(const BAPs *baps)
{
	if(!baps)
		return;
	
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for (i = 0; i < NUM_BAPS; i++)
	{
		value[i] = baps->value[i];
		groupMask[i] = baps->groupMask[i];
	}
	for(i =0; i< BAP_NUM_GROUPS;i++)
	{
		maskType[i] = baps->maskType[i];
	}
}

void BAPs::merge(BAPs *baps)
{
	if(!baps)
		return;

	int i;

	for(i =0; i< BAP_NUM_GROUPS;i++)
	{
		switch(maskType[i])
		{
		case 0:
			maskType[i] = baps->maskType[i];
			break;
		case 1:
			break;
		case 2:
			if(baps->maskType[i] == 1)
				maskType[i] = baps->maskType[i];
			break;
		}
	}
	for(i =0; i< NUM_BAPS;i++)
		if(!groupMask[i])
			groupMask[i] = baps->groupMask[i];

	for(i =0; i< NUM_BAPS;i++)
	{
		if(value[i] == 0 && baps->value[i]!=0)
		{
			value[i] += baps->value[i];
		}
		else
			if(value[i] != 0 && baps->value[i]!=0)
			{
				value[i] += baps->value[i];
				value[i] /= 2;
			}
	}
}


void BAPs::reset()
{
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for (i = 0; i < NUM_BAPS; i++)
	{
		value[i] = 0;
		groupMask[i] = 1;
	}
	for (i = 0; i < BAP_NUM_GROUPS; i++)
	{
		maskType[i] = 3;
	}
}

void BAPs::setMask(bool maskValue)
{
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for (i = 0; i < NUM_BAPS; i++)
	{
		if(maskValue)
			groupMask[i] = 1;
		else
			groupMask[i] = 0;
	}
	for (i = 0; i < BAP_NUM_GROUPS; i++)
	{
		maskType[i] = 1;
	}
}

void BAPs::setMaskIfDiff(BAPs *baps)
{
	if(!baps)
		return;
	for (int i = 0; i < NUM_BAPS; i++)
	{
		if(value[i] != baps->value[i])
			groupMask[i] = 1;
	}
}

void BAPs::setMask(BAPs *baps)
{
	if(!baps)
		return;

	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for (i = 0; i < NUM_BAPS; i++)
	{
		groupMask[i] = baps->groupMask[i];
	}
	for (i = 0; i < BAP_NUM_GROUPS; i++)
	{
		maskType[i] = baps->maskType[i];
	}
}

}

