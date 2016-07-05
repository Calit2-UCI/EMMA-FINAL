// AFMDeformer.cpp: implementation of the AFMDeformer class.
//
//////////////////////////////////////////////////////////////////////

#include "AFMDeformer.h"
#include "FBAPs.h"


namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AFMDeformer::AFMDeformer()
{
	filter = new FBAPs();

	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i = 0;i < NFAP;i++)
		filter->faps->groupMask[i] = 1;
	for(i = 0;i < NUM_BAPS;i++)
		filter->baps->groupMask[i] = 1;
}

AFMDeformer::~AFMDeformer()
{
	// Added by KStankovic
	if (filter != NULL) delete filter;
	//
}

FBAPs *AFMDeformer::filterFBAPs(const FBAPs *fbaps)
{
	FBAPs *filteredFbaps = new FBAPs(fbaps);

	if(filter->faps->groupMask[0] == 0)
	{
		filteredFbaps->faps->fap1.viseme1 = 0;
		filteredFbaps->faps->fap1.viseme2 = 0;
		filteredFbaps->faps->fap1.blend = 0;
	}

	if(filter->faps->groupMask[1] == 0)
	{
		filteredFbaps->faps->fap2.expression1 = 0;
		filteredFbaps->faps->fap2.expression2 = 0;
		filteredFbaps->faps->fap2.intensity1 = 0;
		filteredFbaps->faps->fap2.intensity2 = 0;
	}

	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i = 2;i < NFAP;i++)
		if(filter->faps->groupMask[i] == 0)
			filteredFbaps->faps->llf.value[i] = 0;
			
	for(i = 0;i < NUM_BAPS;i++)
		if(filter->baps->groupMask[i] == 0)
			filteredFbaps->baps->value[i] = 0;

	return(filteredFbaps);
}

}
