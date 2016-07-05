
#include "FBACodingParameters.h"

namespace VisageSDK
{

const int fba_cooondef_dci_max[73]={14, 14, 63, 6, 63, 6, 63, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860};
const int fba_cooondef_dci_min[73]={0, 0, 0, 0, 0, 0, 0, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860};
const int fba_cooondef_dcp_max[73]={14, 14, 63, 6, 63, 6, 63, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860, 1860};
const int fba_cooondef_dcp_min[73]={-14, -14, -63, -6, -63, -6, -63, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860, -1860};

CodingParameters::CodingParameters()
{
	paramset_mask = 3;
	frame_rate = 25;
	seconds = 0;
	frequency_offset = 0;
	fap_quant = 1;
	bap_quant = 1;
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i=0;i<73;i++)
		i_max[i] = fba_cooondef_dci_max[i];
	for(i=0;i<73;i++)
		i_min[i] = fba_cooondef_dci_min[i];
	for(i=0;i<73;i++)
		p_max[i] = fba_cooondef_dcp_max[i];
	for(i=0;i<73;i++)
		p_min[i] = fba_cooondef_dcp_min[i];

	for(i=0;i<NUM_BAPS;i++)
		bap_i_max[i] = bap_p_max[i] = 1859;
	for(i=0;i<NUM_BAPS;i++)
		bap_i_min[i] = bap_p_min[i] = -1859;
}


/** Copy the coding parameters.
* Copy the coding parameters from another coding parameters instance.
* @param cp coding parameters to copy.
*/
void CodingParameters::copy(CodingParameters *cp)
{
	paramset_mask = cp->paramset_mask;
	frame_rate = cp->frame_rate ;
	seconds = cp->seconds;
	frequency_offset = cp->frequency_offset;
	fap_quant = cp->fap_quant;
	bap_quant = cp->bap_quant;
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i=0;i<73;i++)
		i_max[i] = cp->i_max[i];
	for(i=0;i<73;i++)
		i_min[i] = cp->i_min[i];
	for(i=0;i<73;i++)
		p_max[i] = cp->p_max[i];
	for(i=0;i<73;i++)
		p_min[i] = cp->p_min[i];
	for(i=0;i<NUM_BAPS;i++)
		bap_i_max[i] = cp->bap_i_max[i];
	for(i=0;i<NUM_BAPS;i++)
		bap_i_min[i] = cp->bap_i_min[i];
	for(i=0;i<NUM_BAPS;i++)
		bap_p_max[i] = cp->bap_p_max[i];
	for(i=0;i<NUM_BAPS;i++)
		bap_p_min[i] = cp->bap_p_min[i];
}


/** Compare the coding parameters.
* Compare the coding parameters with another coding parameters instance.
* @param cp coding parameters to compare with.
* @return if the coding parameters are the same, return 1, if there is any difference return 0
*/
int CodingParameters::isEqual(CodingParameters *cp)
{
	if(paramset_mask != cp->paramset_mask) return 0;
	if(frame_rate != cp->frame_rate) return 0;
	if(seconds != cp->seconds) return 0;
	if(frequency_offset != cp->frequency_offset) return 0;
	if(fap_quant != cp->fap_quant) return 0;
	if(bap_quant != cp->bap_quant) return 0;
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i=0;i<73;i++)
		if(i_max[i] != cp->i_max[i]) return 0;
	for(i=0;i<73;i++)
		if(i_min[i] != cp->i_min[i]) return 0;
	for(i=0;i<73;i++)
		if(p_max[i] != cp->p_max[i]) return 0;
	for(i=0;i<73;i++)
		if(p_min[i] != cp->p_min[i]) return 0;
	for(i=0;i<NUM_BAPS;i++)
		if(bap_i_max[i] != cp->bap_i_max[i]) return 0;
	for(i=0;i<NUM_BAPS;i++)
		if(bap_i_min[i] != cp->bap_i_min[i]) return 0;
	for(i=0;i<NUM_BAPS;i++)
		if(bap_p_max[i] != cp->bap_p_max[i]) return 0;
	for(i=0;i<NUM_BAPS;i++)
		if(bap_p_min[i] != cp->bap_p_min[i]) return 0;

	return 1;
}

}
