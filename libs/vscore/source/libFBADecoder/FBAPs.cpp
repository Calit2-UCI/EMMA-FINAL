
#include "FBAPs.h"

namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FBAPs::FBAPs()
{
	faps = new FAPs();
	baps = new BAPs();
}

FBAPs::~FBAPs()
{
	if(faps) delete faps;
	if(baps) delete baps;
}

FBAPs::FBAPs(const FBAPs *fbaps)
{
	faps = new FAPs();
	baps = new BAPs();
	set(fbaps);
}

FBAPs::FBAPs(FBAPs *fbaps, double in)
{
	faps = new FAPs();
	baps = new BAPs();
	set(fbaps,in);
}

void FBAPs::reset()
{
	if(!faps) faps = new FAPs();
	if(!baps) baps = new BAPs();

	faps->reset();
	baps->reset();
}

void FBAPs::set(const FBAPs *fbaps)
{
	if(!fbaps)
		return;

	setFAPs(fbaps->faps);
	setBAPs(fbaps->baps);
}

void FBAPs::setFAPs(const FAPs *_faps)
{
	if(!faps) 
		faps = new FAPs(_faps);
	else
		faps->set(_faps);
}

void FBAPs::setBAPs(const BAPs *_baps)
{
	if(!baps) 
		baps = new BAPs(_baps);
	else
		baps->set(_baps);
}


void FBAPs::merge(FBAPs *fbaps)
{
	if(!fbaps)
		return;

	if(!faps) faps = new FAPs();
	if(!baps) baps = new BAPs();

	faps->merge(fbaps->faps);
	baps->merge(fbaps->baps);
}

void FBAPs::set(FBAPs *fbaps, double in)
{
	if(!fbaps)
		return;

	if(!faps) faps = new FAPs();
	if(!baps) baps = new BAPs();

	faps->set(fbaps->faps,in);
	baps->set(fbaps->baps);
}


void FBAPs::setExpression(int expression1, int expression2,int intensity1, int intensity2)
{
	if(!faps) faps = new FAPs();

	faps->fap2.expression1 = expression1;
	faps->fap2.expression2 = expression2;
	faps->fap2.intensity1 = intensity1;
	faps->fap2.intensity2 = intensity2;
}

void FBAPs::setViseme(int viseme1, int viseme2,int blend)
{
	if(!faps) faps = new FAPs();

	faps->fap1.viseme1 = viseme1;
	faps->fap1.viseme2 = viseme2;
	faps->fap1.blend = blend;
}


void FBAPs::setFAP(int fap, int value)
{
	if(!faps) faps = new FAPs();

	faps->llf.value[fap] = value;
}

void FBAPs::setFBAP(char *fbap, int value)
{
	int i;

	for(i=2;i<NFAP;i++)
		if(!strcmp(fbap,FAPNameString[i]))
		{
			setFAP(i,value);
			return;
		}
	for(i=0;i<NBAP;i++)
		if(!strcmp(fbap,BAPNameString[i]))
		{
			setBAP(i,value);
			return;
		}
}

int FBAPs::getFAP(int fap)
{
	if(!faps)
		return 0;

	return(faps->llf.value[fap]);
}


void FBAPs::setBAP(int bap, int value)
{
	if(!baps) baps = new BAPs();

	baps->value[bap] = value;
}


int FBAPs::getBAP(int bap)
{
	if(!baps)
		return 0;

	return(baps->value[bap]);
}

void FBAPs::setMask(bool maskValue)
{
	if(faps)
		faps->setMask(maskValue);
	if(baps)
		baps->setMask(maskValue);
}

void FBAPs::setMaskIfDiff(FBAPs *fbaps)
{
	if(!fbaps)
		return;

	if(faps)
		faps->setMaskIfDiff(fbaps->faps);
	if(baps)
		baps->setMaskIfDiff(fbaps->baps);
}

void FBAPs::setMask(FBAPs *fbaps)
{
	if(!fbaps)
		return;

	if(faps)
		faps->setMask(fbaps->faps);
	if(baps)
		baps->setMask(fbaps->baps);
}

}
