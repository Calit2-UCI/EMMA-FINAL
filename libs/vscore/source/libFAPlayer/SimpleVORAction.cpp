
#include "SimpleVORAction.h"

// C++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>



namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SimpleVORAction::~SimpleVORAction()
{
	// Added by KStankovic
	if (fbaps != NULL)
	//
		delete fbaps;
}

SimpleVORAction::SimpleVORAction()
{
	fbaps = new FBAPs();
	hlimit = (int)((30.0 / 57.2957795) * 1e5);
	vlimit = (int)((30.0 / 57.2957795) * 1e5);
}

SimpleVORAction::SimpleVORAction(int horizontalLimit, int verticalLimit)
{
	fbaps = new FBAPs();
	hlimit = (int)((horizontalLimit / 57.2957795) * 1e5);
	vlimit = (int)((verticalLimit / 57.2957795) * 1e5);
}

FBAPs *SimpleVORAction::getFBAPs(long globalTime, FBAPs *lastFBAPs, VisageCharModel* model) 
{
	
	int v = lastFBAPs->getFAP(head_pitch);
	int h = lastFBAPs->getFAP(head_yaw);
	float vfactor,hfactor;
	
	if(h>hlimit || v>vlimit || h<(-hlimit) || v<(-vlimit)) 
	{
		fbaps->reset();
	}
	else
	{
		if(h>=0)
			hfactor = (float)(hlimit-h);
		else
			hfactor = (float)(hlimit+h);
		hfactor /= hlimit;
		hfactor*=3.0;
		if(hfactor < 1.0)
			h=(int)((float)h * hfactor);

		if(v>=0)
			vfactor = (float)(vlimit-v);
		else
			vfactor = (float)(vlimit+v);
		vfactor /= vlimit;
		vfactor*=3.0;
		if(vfactor < 1.0)
			v=(int)((float)v * vfactor);

		fbaps->setFAP(pitch_r_eyeball,-v);
		fbaps->setFAP(pitch_l_eyeball,-v);
		fbaps->setFAP(yaw_l_eyeball,-h);
		fbaps->setFAP(yaw_r_eyeball,-h);
	}
	
	return fbaps;
};


void SimpleVORAction::start(long globalTime)
{
}

void SimpleVORAction::stop()
{
	fbaps->reset(); // reset the fbaps to neutral position
}

}
