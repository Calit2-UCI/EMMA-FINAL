
#include "FBADecoder.h"

// C++
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cerrno>

#ifdef _DEBUG
#include <assert.h>
#endif

// Win32
#ifdef WIN32
#include <windows.h> //necessary for messagebox in windows
#endif

#ifdef WIN32
	#include <io.h> //No't standard, think about stdio.h as an alternative
#endif

#include <fcntl.h>

// this is a trick to make it compile in VS.net
namespace std { void _Xran(void) { }
void _Xlen(void) { } }

namespace VisageSDK
{

#define null 0

static int fba_cooondef_bap_pred_scale[32] = {0,1,2,3,5,7,9,11,14,17,20,23,27,31,35,39,43,47,52,57,62,67,72,77,82,88,94,100,106,113,120,127};

// This is the constructor of a class that has been exported.
// see FBADecoder.h for the class definition
CFBADecoder::CFBADecoder(char *fn)
{ 

	zerorun = 0;
	suggested_gender = 0;
	coding_type = 0;
	real_frame_rate = DEF_FRAME_RATE;
	NEXT_PTS = 0.0;
	first_intra_segment = 1;
	first_action=1;
	fr_no=0;
	no_frames=0;

	//int fh = fopen(fn, "rb"); //Why not???? ->Then it would be mandatory to analize necesary changes in fop.init(fh)
	#ifdef WIN32
		int fh = _open(fn, _O_BINARY | _O_RDONLY);
	#else
		int fh = open(fn, O_RDONLY);
	#endif

	if(fh < 0)
	{
		std::cout << "FBADecoder can not open fba bitstream file >"<< fn << "< " << strerror(errno) << std::endl;

		#ifdef WIN32
		char msg[300];
		sprintf(msg,"FBADecoder Can not open fba bitstream file >%s< (%s)",fn/*,strerror(errno)*/);
		MessageBox(0,msg,"Error",MB_ICONERROR);
		#endif
		exit(0);
	}
	fop.init(fh);
	int sc = fop.stream.getbits(32);
	#ifdef _DEBUG
	assert(sc==FBA_OBJECT_START_CODE || sc==FBA_OBJECT_START_CODE_OLD);
	#endif

	if (!(sc==FBA_OBJECT_START_CODE || sc==FBA_OBJECT_START_CODE_OLD))
	{
		#ifdef WIN32
		MessageBox(0,"FBA bitstream start code corrupt.","Error",MB_ICONERROR);
		#endif
		exit(0);
	}
	/*
	assert(sc==FBA_OBJECT_START_CODE);

	if (!(sc==FBA_OBJECT_START_CODE))
	{
		#ifdef WIN32
		MessageBox(0,"FBA bitstream start code corrupt.","Error",MB_ICONERROR);
		#endif
		exit(0);
	}
	*/
	// Added by KStankovic
	fop.fo.Irange_max[3] = 0;
	fop.fo.Irange_min[3] = 0;
	//
	init();
	return; 
}
/*
CFBADecoder::~CFBADecoder()
{ 
}*/
void CFBADecoder::bit_out_psc_layer()
{
	bit = fop.stream.getbits(1);
	if (zerorun == 22)
	{
		if (bit == 0)
		{
			#ifdef WIN32
			MessageBox(0,"Marker bit stuffing error","Error",MB_ICONERROR);
			#endif
			exit(0);
		}
		bit = fop.stream.getbits(1);
		zerorun = 0;
		if (bit == 0)
			zerorun = 1;
		return;
	}
	if (bit == 0)
	{
		zerorun++;
	}
	else
	{
		zerorun = 0;
	}
}
int CFBADecoder::decode_a_symbol(int cumul_freq[])
{
	length = high - low + 1;
	cum = (-1 + (code_value - low + 1) * cumul_freq[0]) / length;
	for (sacindex = 1; cumul_freq[sacindex] > cum; sacindex++);
	high = low - 1 + (length * cumul_freq[sacindex - 1]) / cumul_freq[0];
	low += (length * cumul_freq[sacindex]) / cumul_freq[0];
	for (;;)
	{
		if (high < fba_cooondef_q2);
		else
			if (low >= fba_cooondef_q2)
			{
				code_value -= fba_cooondef_q2;
				low -= fba_cooondef_q2;
				high -= fba_cooondef_q2;
			}
			else
				if (low >= fba_cooondef_q1 && high < fba_cooondef_q3)
				{
					code_value -= fba_cooondef_q1;
					low -= fba_cooondef_q1;
					high -= fba_cooondef_q1;
				}
				else
				{
					break;
				}
		low *= 2;
		high = 2 * high + 1;
		bit_out_psc_layer();
		code_value = 2 * code_value + bit;
	}
	return (sacindex - 1);
}

void CFBADecoder::decode_expression(int is_intra)
{
	int symbol;
	if (is_intra != 0)
	{
		symbol = decode_a_symbol(fop.fo.Icumul_ve[3]);
		symbol += fop.fo.Irange_ve_min[3];
		fop.fo.ve_prev[3] = symbol;
		fbaps.faps->fap2.expression1 = fop.fo.ve_prev[3];
		symbol = decode_a_symbol(fop.fo.Icumul_ve[4]);
		fop.fo.expression_intensity1_Q = symbol + fop.fo.Irange_ve_min[4];
		fop.fo.ve_prev[4] = dequant(fop.fo.expression_intensity1_Q, fop.fo.fap_quant);
		fbaps.faps->fap2.intensity1 = fop.fo.ve_prev[4];
		symbol = decode_a_symbol(fop.fo.Icumul_ve[5]);
		symbol += fop.fo.Irange_ve_min[5];
		fop.fo.ve_prev[5] = symbol;
		fbaps.faps->fap2.expression2 = fop.fo.ve_prev[5];
		symbol = decode_a_symbol(fop.fo.Icumul_ve[6]);
		fop.fo.expression_intensity2_Q = symbol + fop.fo.Irange_ve_min[6];
		fop.fo.ve_prev[6] = dequant(fop.fo.expression_intensity2_Q, fop.fo.fap_quant);
		fbaps.faps->fap2.intensity2 = fop.fo.ve_prev[6];
		fop.stream.adjust_bits();
		fbaps.faps->fap2.initBit = fop.stream.getbits(1);
		fbaps.faps->fap2.defBit = fop.stream.getbits(1);
	}
	else
	{
		symbol = decode_a_symbol(fop.fo.cumul_ve[3]);
		update_viseme_expression(symbol, 3);
		fop.fo.ve_diff[3] = symbol + fop.fo.range_ve_min[3];
		fop.fo.ve_prev[3] = recon(fop.fo.ve_diff[3], fop.fo.ve_prev[3]);
		fbaps.faps->fap2.expression1 = fop.fo.ve_prev[3];
		symbol = decode_a_symbol(fop.fo.cumul_ve[4]);
		update_viseme_expression(symbol, 4);
		fop.fo.expression_intensity1_Q = symbol + fop.fo.range_ve_min[4];
		fop.fo.ve_diff[4] = dequant(fop.fo.expression_intensity1_Q, fop.fo.fap_quant);
		fop.fo.ve_prev[4] = recon(fop.fo.ve_diff[4], fop.fo.ve_prev[4]);
		fbaps.faps->fap2.intensity1 = fop.fo.ve_prev[4];
		symbol = decode_a_symbol(fop.fo.cumul_ve[5]);
		update_viseme_expression(symbol, 5);
		fop.fo.ve_diff[5] = symbol + fop.fo.range_ve_min[5];
		fop.fo.ve_prev[5] = recon(fop.fo.ve_diff[5], fop.fo.ve_prev[5]);
		fbaps.faps->fap2.expression2 = fop.fo.ve_prev[5];
		symbol = decode_a_symbol(fop.fo.cumul_ve[6]);
		update_viseme_expression(symbol, 6);
		fop.fo.expression_intensity2_Q = symbol + fop.fo.range_ve_min[6];
		fop.fo.ve_diff[6] = dequant(fop.fo.expression_intensity2_Q, fop.fo.fap_quant);
		fop.fo.ve_prev[6] = recon(fop.fo.ve_diff[6], fop.fo.ve_prev[6]);
		fbaps.faps->fap2.intensity2 = fop.fo.ve_prev[6];
		fop.stream.adjust_bits();
		fbaps.faps->fap2.initBit = fop.stream.getbits(1);
		fbaps.faps->fap2.defBit = fop.stream.getbits(1);
	}
}
void CFBADecoder::decode_fap_group_mask(int n)
{
	int i;
	for (i = fop.fo.mask_index[n]; i < fop.fo.mask_index[n + 1]; i++)
	{
		fop.fo.fap_mask[i] = fop.stream.getbits(1);
		fbaps.faps->groupMask[i] = fop.fo.fap_mask[i];
	}
}
void CFBADecoder::decode_frame_rate()
{
	fop.fo.frame_rate = fop.stream.getbits(8);
	fop.fo.seconds = fop.stream.getbits(4);
	fop.fo.frequency_offset = fop.stream.getbits(1);
}
void CFBADecoder::decode_ifap()
{
	int i;
	int symbol, qstep;


	if (fop.fo.fap_mask[0] != 0)
	{
		decoder_reset();
		decode_viseme(1);
	}
	if (fop.fo.fap_mask[1] != 0)
	{
		decoder_reset();
		decode_expression(1);
	}
	decoder_reset();
	for (i = 2; i < NFAP; i++)
	{
		if (fop.fo.fap_mask[i] != 0)
		{
			symbol = decode_a_symbol(fop.fo.Icumul_freq[i]);
			fop.fo.fap_Q[i] = symbol + fop.fo.Irange_min[i];
			qstep = fop.fo.IQP[i] * fop.fo.fap_quant;
			fop.fo.fap_prev[i] = dequant(fop.fo.fap_Q[i], qstep);
			fbaps.faps->llf.value[i] = fop.fo.fap_prev[i];
		}
	}
}
void CFBADecoder::decode_new_minmax()
{
	int i, j;
	int minmax;
	int marker;
	int scaler;
	if (fop.fo.fap_quant != 0)
		scaler = fop.fo.fap_quant;
	else
		scaler = 1;
	if (fop.fo.is_i_new_max != 0)
	{
		for (i = 2, j = 0; i < NFAP; i++, j++)
		{
			if (i == 18 || i == 30 || i == 38 || i == 42 || i == 47 || i == 50 || i == 60 || i == 64)
			{
				j = 0;
			}
			if ((j & 3) == 0)
			{
				marker = fop.stream.getbits(1);
			}
			if (fop.fo.fap_mask[i] != 0)
			{
				minmax = fop.stream.getbits(5);
				cp.i_max[i+5] = minmax*RANGE_DELTA;
				fop.fo.Irange_max[i] = minmax * RANGE_DELTA / scaler;
			}
		}
	}
	if (fop.fo.is_i_new_min != 0)
	{
		for (i = 2, j = 0; i < NFAP; i++, j++)
		{
			if (i == 18 || i == 30 || i == 38 || i == 42 || i == 47 || i == 50 || i == 60 || i == 64)
			{
				j = 0;
			}
			if ((j & 3) == 0)
			{
				marker = fop.stream.getbits(1);
			}
			if (fop.fo.fap_mask[i] != 0)
			{
				minmax = fop.stream.getbits(5);
				cp.i_min[i+5] = -minmax*RANGE_DELTA;
				fop.fo.Irange_min[i] = -minmax * RANGE_DELTA / scaler;
			}
		}
	}
	if (fop.fo.is_p_new_max != 0)
	{
		for (i = 2, j = 0; i < NFAP; i++, j++)
		{
			if (i == 18 || i == 30 || i == 38 || i == 42 || i == 47 || i == 50 || i == 60 || i == 64)
			{
				j = 0;
			}
			if ((j & 3) == 0)
			{
				marker = fop.stream.getbits(1);
			}
			if (fop.fo.fap_mask[i] != 0)
			{
				minmax = fop.stream.getbits(5);
				cp.p_max[i+5] = minmax*RANGE_DELTA;
				fop.fo.range_max[i] = minmax * RANGE_DELTA / scaler;
			}
		}
	}
	if (fop.fo.is_p_new_min != 0)
	{
		for (i = 2, j = 0; i < NFAP; i++, j++)
		{
			if (i == 18 || i == 30 || i == 38 || i == 42 || i == 47 || i == 50 || i == 60 || i == 64)
			{
				j = 0;
			}
			if ((j & 3) == 0)
			{
				marker = fop.stream.getbits(1);
			}
			if (fop.fo.fap_mask[i] != 0)
			{
				minmax = fop.stream.getbits(5);
				cp.p_min[i+5] = -minmax*RANGE_DELTA;
				fop.fo.range_min[i] = -minmax * RANGE_DELTA / scaler;
			}
		}
	}
}
void CFBADecoder::decode_pfap()
{
	int i;
	int symbol, qstep;
	if (fop.fo.fap_mask[0] != 0)
	{
		decoder_reset();
		decode_viseme(0);
	}
	if (fop.fo.fap_mask[1] != 0)
	{
		decoder_reset();
		decode_expression(0);
	}
	decoder_reset();
	for (i = 2; i < NFAP; i++)
	{
		if (fop.fo.fap_mask[i] != 0)
		{
			symbol = decode_a_symbol(fop.fo.cumul_freq[i]);
			update(symbol, i);
			fop.fo.fap_Q[i] = symbol + fop.fo.range_min[i];
			qstep = fop.fo.QP[i] * fop.fo.fap_quant;
			fop.fo.fap_diff[i] = dequant(fop.fo.fap_Q[i], qstep);
			fop.fo.fap_prev[i] = recon(fop.fo.fap_diff[i], fop.fo.fap_prev[i]);
			fbaps.faps->llf.value[i] = fop.fo.fap_prev[i];
		}
	}
}
void CFBADecoder::decode_skip_frames()
{
	int ivalue;
	fop.fo.number_of_frames_to_skip = 0;
	do
	{
		ivalue = fop.stream.getbits(4);
		fop.fo.number_of_frames_to_skip += ivalue;
	}
	while (ivalue == 15);
}
void CFBADecoder::decode_time_code()
{
	fop.fo.time_code_hours = fop.stream.getbits(5);
	fop.fo.time_code_minutes = fop.stream.getbits(6);
	fop.fo.marker_bit = fop.stream.getbits(1);
	fop.fo.time_code_seconds = fop.stream.getbits(6);
}
void CFBADecoder::decode_viseme(int is_intra)
{
	int symbol;
	if (is_intra != 0)
	{
		symbol = decode_a_symbol(fop.fo.Icumul_ve[0]);
		symbol += fop.fo.Irange_ve_min[0];
		fop.fo.ve_prev[0] = symbol;
		fbaps.faps->fap1.viseme1 = fop.fo.ve_prev[0];
		symbol = decode_a_symbol(fop.fo.Icumul_ve[1]);
		symbol += fop.fo.Irange_ve_min[1];
		fop.fo.ve_prev[1] = symbol;
		fbaps.faps->fap1.viseme2 = fop.fo.ve_prev[1];
		symbol = decode_a_symbol(fop.fo.Icumul_ve[2]);
		fop.fo.viseme_blend_Q = symbol + fop.fo.Irange_ve_min[2];
		fop.fo.ve_prev[2] = dequant(fop.fo.viseme_blend_Q, fop.fo.fap_quant);
		fbaps.faps->fap1.blend = fop.fo.ve_prev[2];
		fop.stream.adjust_bits();
		fbaps.faps->fap1.defBit = fop.stream.getbits(1);
	}
	else
	{
		symbol = decode_a_symbol(fop.fo.cumul_ve[0]);
		update_viseme_expression(symbol, 0);
		fop.fo.ve_diff[0] = symbol + fop.fo.range_ve_min[0];
		fop.fo.ve_prev[0] = recon(fop.fo.ve_diff[0], fop.fo.ve_prev[0]);
		fbaps.faps->fap1.viseme1 = fop.fo.ve_prev[0];
		symbol = decode_a_symbol(fop.fo.cumul_ve[1]);
		update_viseme_expression(symbol, 1);
		fop.fo.ve_diff[1] = symbol + fop.fo.range_ve_min[1];
		fop.fo.ve_prev[1] = recon(fop.fo.ve_diff[1], fop.fo.ve_prev[1]);
		fbaps.faps->fap1.viseme2 = fop.fo.ve_prev[1];
		symbol = decode_a_symbol(fop.fo.cumul_ve[2]);
		update_viseme_expression(symbol, 2);
		fop.fo.viseme_blend_Q = symbol + fop.fo.range_ve_min[2];
		fop.fo.ve_diff[2] = dequant(fop.fo.viseme_blend_Q, fop.fo.fap_quant);
		fop.fo.ve_prev[2] = recon(fop.fo.ve_diff[2], fop.fo.ve_prev[2]);
		fbaps.faps->fap1.blend = fop.fo.ve_prev[2];
		fop.stream.adjust_bits();
		fbaps.faps->fap1.defBit = fop.stream.getbits(1);
	}
}
// returns 0 finished
int CFBADecoder::decodeFrame()
{
	fba_object_plane_header();
	fba_object_plane_data();
	fbaps.faps->interpolateLR();

	cp.paramset_mask = face_paramset_mask;

	if(face_paramset_mask & FAP_PARAMSET_MASK)
	{
		cp.frame_rate = fop.fo.frame_rate ;
		cp.seconds = fop.fo.seconds;
		cp.frequency_offset = fop.fo.frequency_offset;
	}
	else
	{
		cp.frame_rate = fop.bo.frame_rate ;
		cp.seconds = fop.bo.seconds;
		cp.frequency_offset = fop.bo.frequency_offset;
	}

	cp.fap_quant = fop.fo.fap_quant;
	cp.bap_quant = fop.bo.bap_quant;

/* this is now done directly in decode_new_minmax
	for(i=0; i<7; i++)
		cp.i_max[i] = fop.fo.Irange_ve_max[i];
	for(i=2; i<NFAP; i++) 
		cp.i_max[i+5] = fop.fo.Irange_max[i];

	for(i=0; i<7; i++)
		cp.i_min[i] = fop.fo.Irange_ve_min[i];
	for(i=2; i<NFAP; i++) 
		cp.i_min[i+5] = fop.fo.Irange_min[i];

	for(i=0; i<7; i++)
		cp.p_max[i] = fop.fo.range_ve_max[i];
	for(i=2; i<NFAP; i++) 
		cp.p_max[i+5] = fop.fo.range_max[i];

	for(i=0; i<7; i++)
		cp.p_min[i] = fop.fo.range_ve_min[i];
	for(i=2; i<NFAP; i++) 
		cp.p_min[i+5] = fop.fo.range_min[i];

	for(i=0; i<NUM_BAPS; i++) 
		cp.bap_p_min[i] = fop.bo.range_min[i];
	for(i=0; i<NUM_BAPS; i++) 
		cp.bap_p_max[i] = fop.bo.range_max[i];
	for(i=0; i<NUM_BAPS; i++) 
		cp.bap_i_min[i] = fop.bo.Irange_min[i];
	for(i=0; i<NUM_BAPS; i++) 
		cp.bap_i_max[i] = fop.bo.Irange_max[i];
*/
	
	if (fop.stream.nextbits_bytealigned() == FBA_OBJECT_END_CODE || fop.stream.nextbits_bytealigned() == FBA_OBJECT_END_CODE_OLD)
		return 0;
	return 1;
}

void CFBADecoder::decoder_reset()
{
	int i;
	zerorun = 0;
	code_value = 0;
	low = 0;
	high = fba_cooondef_top;
	for (i = 1; i <= 16; i++)
	{
		bit_out_psc_layer();
		code_value = 2 * code_value + bit;
	}
}

int CFBADecoder::dequant(int qfap, int qstep)
{
	if (qstep != 0)
	{
		return qfap * qstep;
	}
	else
	{
		return qfap;
	}
}



void CFBADecoder::fba_object_plane_header()
{
	int sc;
	int is_frame_rate = 0;
	int skip_frames = 0;

	fop.stream.resetbits();
	sc = fop.stream.nextbits_bytealigned();
	if (sc == FBA_OBJECT_PLANE_START_CODE  || sc == FBA_OBJECT_PLANE_START_CODE_OLD)
	{
		fop.stream.next_start_code();
		sc = fop.stream.getbits(32);
	}
	/*
	else
	{
		#ifdef WIN32
		MessageBox(0,"FBA object plane start code corrupt.","Error",MB_ICONERROR);
		#endif
		exit(0);
	}
*/

	is_intra = fop.stream.getbits(1);
	face_paramset_mask = fop.stream.getbits(2);
	if (is_intra != 0)
	{
		if (face_paramset_mask & FAP_PARAMSET_MASK) 
		{
			is_frame_rate = fop.stream.getbits(1);
			if (is_frame_rate != 0)
			{
				decode_frame_rate();
				if (fop.fo.frequency_offset != 0)
					real_frame_rate = ((double) fop.fo.frame_rate + ((double) fop.fo.seconds / 16)) * (1000 / 1001);
				else
					real_frame_rate = (double) fop.fo.frame_rate + ((double) fop.fo.seconds / 16);
			}
			fop.fo.is_time_code = fop.stream.getbits(1);
			if (fop.fo.is_time_code != 0)
			{
				decode_time_code();
			}
		}
	}
	
	if (face_paramset_mask & FAP_PARAMSET_MASK) 
	{
		skip_frames = fop.stream.getbits(1);
		if (skip_frames != 0)
		{
			decode_skip_frames();
		}
		else
		{
			fop.fo.number_of_frames_to_skip = 0;
		}
	}
    
	if (is_intra != 0) 
	{
		if ((face_paramset_mask & BAP_PARAMSET_MASK) && !(face_paramset_mask & FAP_PARAMSET_MASK)) 
		{
			fop.bo.is_frame_rate = fop.stream.getbits(1);
			if(fop.bo.is_frame_rate) 
			{
				bap_decode_frame_rate();
				real_frame_rate = fop.bo.frame_rate;
			}
			fop.bo.is_time_code = fop.stream.getbits(1);
			if(fop.bo.is_time_code) 
			{
				bap_decode_time_code();
			}
		}
	}
	if ((face_paramset_mask & BAP_PARAMSET_MASK) && !(face_paramset_mask & FAP_PARAMSET_MASK)) 
	{
		fop.bo.skip_frames = fop.stream.getbits(1);
		if(fop.bo.skip_frames) 
		{
			bap_decode_skip_frames();
		} 
		else 
		{
			fop.bo.number_of_frames_to_skip = 0;
		}
	}
}

int CFBADecoder::fba_object_plane_data()
{
	int i, j;
	int marker;
	int hquant;
	int bitcnt;
	/* 6-26-98 */
	int minn,maxn;/*-edp*/
	
	if (face_paramset_mask & FAP_PARAMSET_MASK) 
	{
		if(is_intra != 0) 
		{
			fop.fo.fap_quant = fop.stream.getbits(5);
			hquant = fop.fo.fap_quant/2;
			
			if(fop.fo.Irange_max[3] == 0 && fop.fo.Irange_min[3] == 0) 
			{
				/*init min/max values if not set before -edp*/
				if(fop.fo.fap_quant)
				{
					for(i=2;i<7;i+=2) 
					{
						cp.i_max[i] = fop.fo.Irange_ve_max[i] = (63+hquant)/fop.fo.fap_quant;
						cp.p_max[i] = fop.fo.range_ve_max[i] = (63+hquant)/fop.fo.fap_quant;
						cp.p_min[i] = fop.fo.range_ve_min[i] = -(63+hquant)/fop.fo.fap_quant;
					}
				}
				for(i=2;i<NFAP;i++) 
				{/*set init minmax values -edp*/
					cp.i_max[i+5] = fop.fo.Idefault_max[i];
					maxn = (fop.fo.Idefault_max[i]+RANGE_DELTA-1)/RANGE_DELTA;
					maxn *= RANGE_DELTA;
					if(fop.fo.fap_quant)
						fop.fo.Irange_max[i] = maxn/fop.fo.fap_quant;
					else fop.fo.Irange_max[i] = maxn;
					
					cp.i_min[i+5] = fop.fo.Idefault_min[i];
					minn = (fop.fo.Idefault_min[i]-RANGE_DELTA+1)/RANGE_DELTA;
					minn *= RANGE_DELTA;
					if(fop.fo.fap_quant)
						fop.fo.Irange_min[i] = minn/fop.fo.fap_quant;
					else 
						fop.fo.Irange_min[i] = minn;
					
					cp.p_max[i+5] = fop.fo.default_max[i];
					maxn = (fop.fo.default_max[i]+RANGE_DELTA-1)/RANGE_DELTA;
					maxn *= RANGE_DELTA;
					if(fop.fo.fap_quant)
						fop.fo.range_max[i] = maxn/fop.fo.fap_quant;
					else 
						fop.fo.range_max[i] = maxn;
					
					cp.p_min[i+5] = fop.fo.default_min[i];
					minn = (fop.fo.default_min[i]-RANGE_DELTA+1)/RANGE_DELTA;
					minn *= RANGE_DELTA;
					if(fop.fo.fap_quant)
						fop.fo.range_min[i] = minn/fop.fo.fap_quant;
					else 
						fop.fo.range_min[i] = minn;
				}
			}
			
			for(i=0; i<N_FAP_GROUP; i++) 
			{
				marker = fop.stream.getbits(1);
				#ifdef _DEBUG
				assert(marker);
				#endif
				fop.fo.fap_mask_type[i] = fop.stream.getbits(2);
				fbaps.faps->maskType[i] = fop.fo.fap_mask_type[i];
				if(fop.fo.fap_mask_type[i]==1 || fop.fo.fap_mask_type[i]==2) 
				{
					decode_fap_group_mask(i);
				} 
				else 
					if(fop.fo.fap_mask_type[i]==0) 
					{
						for(j=fop.fo.mask_index[i]; j<fop.fo.mask_index[i+1]; j++) 
						{
							fop.fo.fap_mask[j] = 0;
							fbaps.faps->groupMask[j] = fop.fo.fap_mask[j];
						}
					} 
					else 
					{
						for(j=fop.fo.mask_index[i]; j<fop.fo.mask_index[i+1]; j++) 
						{
							fop.fo.fap_mask[j] = 1;
							fbaps.faps->groupMask[j] = fop.fo.fap_mask[j];
						}
					}
			}
			
			suggested_gender = fop.stream.getbits(1);
			coding_type = fop.stream.getbits(1);/*changed from 2 to 1 -edp*/
			fop.fo.coding_type = coding_type;
			#ifdef _DEBUG
			assert(coding_type==PREDICTIVE_CODING_TYPE || coding_type==DCT_CODING_TYPE);
			#endif
			
			if (coding_type == PREDICTIVE_CODING_TYPE) 
			{
				
				fop.fo.is_i_new_max = fop.stream.getbits(1);
				fop.fo.is_i_new_min = fop.stream.getbits(1);
				fop.fo.is_p_new_max = fop.stream.getbits(1);
				fop.fo.is_p_new_min = fop.stream.getbits(1);
				
				decode_new_minmax();
				
				if(fop.fo.fap_quant) 
				{
					fop.fo.Irange_ve_max[2] = (63+hquant)/fop.fo.fap_quant;
					fop.fo.Irange_ve_max[4] = (63+hquant)/fop.fo.fap_quant;
					fop.fo.Irange_ve_max[6] = (63+hquant)/fop.fo.fap_quant;
					fop.fo.range_ve_max[2] = (63+hquant)/fop.fo.fap_quant;
					fop.fo.range_ve_max[4] = (63+hquant)/fop.fo.fap_quant;
					fop.fo.range_ve_max[6] = (63+hquant)/fop.fo.fap_quant;
					fop.fo.range_ve_min[2] = -(63+hquant)/fop.fo.fap_quant;
					fop.fo.range_ve_min[4] = -(63+hquant)/fop.fo.fap_quant;
					fop.fo.range_ve_min[6] = -(63+hquant)/fop.fo.fap_quant;
					cp.i_max[2] = 63;
					cp.i_max[4] = 63;
					cp.i_max[6] = 63;
					cp.p_max[2] = 63;
					cp.p_max[4] = 63;
					cp.p_max[6] = 63;
					cp.p_min[2] = -63;
					cp.p_min[4] = -63;
					cp.p_min[6] = -63;
				}
				
				init_arith(fop.fo.fap_mask);
				decode_ifap();
			} 
			else 
			{/*
			 if (first_intra_segment) 
			 {
			 DCT_initialize();
			 first_intra_segment = 0;
			 }
			 decode_i_segment();
				*/
			}
		}
		else 
		{
			if (coding_type == PREDICTIVE_CODING_TYPE) 
			{
//				for(i=0; i<NFAP; i++) {
//					fop_data->fap_mask[i] = fop.fo.fap_mask[i];
//				}
				decode_pfap();
			}  
			else 
			{/*
				decode_p_segment();*/
			}
		}
		
		if (coding_type == PREDICTIVE_CODING_TYPE) 
		{ /* 6-26-98 */
			fop.stream.adjust_bits(); /* get ready to code next frame */
			
			if(fop.fo.is_time_code) 
			{
				PTS = (3600*fop.fo.time_code_hours+60*fop.fo.time_code_minutes+fop.fo.time_code_seconds)*1000;
			} 
			else 
			{
				PTS = (int) NEXT_PTS;
			}
			NEXT_PTS += (fop.fo.number_of_frames_to_skip+1)*1000.0/(double) real_frame_rate;
		}
		bitcnt = fop.stream.cntbits();
	}
	
    if (face_paramset_mask & BAP_PARAMSET_MASK) 
	{
		if(is_intra != 0) 
		{
			fop.bo.bap_quant = fop.stream.getbits(5);
			for(i=0; i<BAP_NUM_GROUPS; i++) 
			{
				marker = fop.stream.getbits(1);
				#ifdef _DEBUG
				assert(marker);
				#endif
				fop.bo.bap_mask_type[i] = fop.stream.getbits(2);
				fbaps.baps->maskType[i] = fop.bo.bap_mask_type[i];
				if(fop.bo.bap_mask_type[i]==1) 
				{
					bap_decode_group_mask(i);
				} 
				else 
					if(fop.bo.bap_mask_type[i]==3 || fop.bo.bap_mask_type[i]==0) 
					{
						int j;
						int temp_index;
						
						for(j=0;j<fop.bo.baps_in_group[i]; j++) 
						{
							temp_index = fop.bo.bap_mask_index[i][j];
							if(fop.bo.bap_mask_type[i]==3)
								fop.bo.bap_mask[temp_index] = 1;
							else
								fop.bo.bap_mask[temp_index] = 0;
							fbaps.baps->groupMask[temp_index] = fop.bo.bap_mask[temp_index];
						}
					}
					else 
					{
						char msg[300];
						sprintf(msg,"Error: stream contains wrong BAP mask type for group %d",i);
						#ifdef WIN32
						MessageBox(0,msg,"Error",MB_ICONERROR);
						#endif
						exit(0);
					}
					
			}
//			for(i=0; i<NUM_BAPS; i++) 
//			{
//				bop_data->bap_mask[i] = fop.bo.bap_mask[i];
//			}
			fop.bo.suggested_gender = fop.stream.getbits(1);
			coding_type = fop.stream.getbits(1);
			fop.bo.coding_type = coding_type;
			#ifdef _DEBUG
			assert(fop.bo.coding_type==PREDICTIVE_CODING_TYPE || fop.bo.coding_type==DCT_CODING_TYPE);
			#endif
			if (fop.bo.coding_type == PREDICTIVE_CODING_TYPE) 
			{
				fop.bo.is_i_new_max = fop.stream.getbits(1);
				fop.bo.is_i_new_min = fop.stream.getbits(1);
				fop.bo.is_p_new_max = fop.stream.getbits(1);
				fop.bo.is_p_new_min = fop.stream.getbits(1);
				if(fop.bo.is_i_new_max||fop.bo.is_i_new_min||
					fop.bo.is_p_new_max||fop.bo.is_p_new_min) 
				{
					bap_decode_new_minmax();
				}
				bap_init_arith(fop.bo.bap_mask);
				decode_ibap();
			} 
			else 
			{
				/*
				if (first_intra_segment) 
				{
					DCT_initialize();
					first_intra_segment = 0;
				}
				bap_decode_i_segment(bop_data);
				*/
			}
		} 
		else 
		{
			if (fop.bo.coding_type == PREDICTIVE_CODING_TYPE) 
			{
				decode_pbap();
			}
			else 
			{
				/*
				bap_decode_p_segment(bop_data);
				*/
			}
		}
		if (fop.bo.coding_type == PREDICTIVE_CODING_TYPE) 
			fop.stream.adjust_bits(); /* get ready to code next frame - only for AR -edp*/
		
		if ((face_paramset_mask & BAP_PARAMSET_MASK) && !(face_paramset_mask & FAP_PARAMSET_MASK)) 
		{
			if(fop.bo.is_time_code) 
			{
				PTS = (3600*fop.bo.time_code_hours+60*fop.bo.time_code_minutes+fop.bo.time_code_seconds)*1000;
			} 
			else 
			{
				PTS = (int) NEXT_PTS;
			}
			NEXT_PTS += (fop.bo.number_of_frames_to_skip+1)*1000.0/(double) real_frame_rate;
		}
	}
	bitcnt = fop.stream.cntbits();
	return bitcnt;
}

FBAPs *CFBADecoder::getFBAPs()
{
	return &fbaps;
}
long CFBADecoder::getTimeStamp()
{
	return PTS;
}

void CFBADecoder::init()
{
	PTS = 0;
	int ai[] = {
		0, 2, 18, 30, 38, 42, 47, 50, 60, 64, 
			68
	};
	
	int ai1[] = {
		1, 1, 4, 2, 2, 2, 2, 2, 2, 2, 
			2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 
			1, 1, 128, 128, 128, 128, 1, 1, 1, 1, 
			2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 
			2, 2, 1, 1, 1, 1, 512, 170, 170, 170, 
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
			1, 1, 1, 1, 1, 1, 1, 1
	};
	int ai2[] = {
		1, 1, 4, 2, 2, 2, 2, 2, 2, 2, 
			2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 
			1, 1, 128, 128, 128, 128, 1, 1, 1, 1, 
			2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 
			2, 2, 1, 1, 1, 1, 512, 170, 170, 170, 
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
			1, 1, 1, 1, 1, 1, 1, 1
	};
	int ai3[] = {
		14, 14, 63, 6, 63, 6, 63
	};
	int ai4[] = {
		-14, -14, -63, -6, -63, -6, -63
	};
	int  bap_mask_index[BAP_NUM_GROUPS][MAX_BAPS]={
		{1,2,3},
		{4,6,10,14},
		{5,7,11,15},
		{8,12,16,18,20,22},
		{9,13,17,19,21,23},
		{32,34,36,38,42},
		{33,35,37,39,43},
		{24,26,28,30,40,44,46},
		{25,27,29,31,41,45,47},
		{48,49,50,60,61,62,87,88,89,114,115,116},
		{54,55,56,72,73,74,99,100,101,108,109,110,120,121,122},
		{57,58,59,66,67,68,81,82,83,93,94,95,105,106,107,117,118,119},
		{63,64,65,69,70,71,75,76,77,90,91,92,102,103,104,111,112,113},
		{51,52,53,78,79,80,84,85,86,96,97,98},
		{125,131,133,137,143,145,149,155,157,161,167,169,171,173,177,179},
		{126,132,134,138,144,146,150,156,158,162,168,170,172,174,178,180},
		{123,127,129,135,139,141,147,151,153,159,163,165,175},
		{124,128,130,136,140,142,148,152,154,160,164,166,176},
		{181,182,183,184,185,186},
		{187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208},
		{209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230},
		{231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252},
		{253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274},
		{275,276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296}
	};
	int baps_in_group[BAP_NUM_GROUPS] = {
		3, 4, 4, 6, 6, 5, 5, 7, 7, 12, 15, 18, 18, 12, 16, 16, 13, 13, 6,22,22,22,22,22
	};

/*
#if 0 //standard
	static int temp_BQP[NUM_BAPS] =
	{64,64,64,128,128,128,128,256,256,128,128,256,256,
	128,128,256,256,256,256,256,256,256,256,128,128,128,128,128,128,128,128,64,64,64,64,256,256,
	64,64,256,256,128,128,128,128,256,256,128,128,128,256,256,256,128,128,128,256,256,256,128,128,128,256,256,256,256,256,256,256,256,256,
	128,128,128,256,256,256,256,256,256,256,256,256,256,256,256,128,128,128,256,256,256,256,256,256,256,256,256,
	128,128,128,256,256,256,256,256,256,128,128,128,256,256,256,128,128,128,256,256,256,128,128,128,512,512,128,128,128,128,256,256,
	128,128,128,128,256,256,128,128,128,128,256,256,128,128,128,128,256,256,128,128,128,128,256,256,128,128,128,128,256,256,128,128,128,128,256,256,
	128,128,128,128,128,128,128,128,256,256,128,128,128,128,64,64,64,64,64,64,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
#else //experiment -edp
static int temp_BQP[NUM_BAPS] = {170,170,170,170,170,170,170,256,256,170,170,256,256,
170,170,256,256,256,256,256,256,256,256,170,170,170,170,170,170,170,170,170,170,170,170,256,256,
170,170,256,256,170,170,170,170,256,256,170,170,170,256,256,256,170,170,170,256,256,256,170,170,170,256,256,256,256,256,256,256,256,256,
170,170,170,256,256,256,256,256,256,256,256,256,256,256,256,170,170,170,256,256,256,256,256,256,256,256,256,
170,170,170,256,256,256,256,256,256,170,170,170,256,256,256,170,170,170,256,256,256,170,170,170,512,512,170,170,170,170,256,256,
170,170,170,170,256,256,170,170,170,170,256,256,170,170,170,170,256,256,170,170,170,170,256,256,170,170,170,170,256,256,170,170,170,170,256,256,
170,170,170,170,170,170,170,170,256,256,170,170,170,170,170,170,170,170,170,170,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
#endif
*/

	int i,j,k;	//CHANGED BY I.K. declared iterator outside of loop
	for(i = 0; i < 11; i++)
		fop.fo.mask_index[i] = ai[i];
	
	for(j = 0; j < 68; j++)
		fop.fo.QP[j] = ai1[j];
	
	for(k = 0; k < 68; k++)
		fop.fo.IQP[k] = ai2[k];
	
	// body
	for (i=0; i<NUM_BAPS; i++)
		fop.bo.BQP[i] = fba_cooondef_BQP[i];
	for (i=0; i<NUM_BAPS; i++)
		fop.bo.IBQP[i] = fba_cooondef_BQP[i];                                  
	
	
	for(int l = 0; l < 7; l++)
	{
		fop.fo.Ifreq_ve[l] = null;
		fop.fo.Icumul_ve[l] = null;
		fop.fo.freq_ve[l] = null;
		fop.fo.cumul_ve[l] = null;
	}
	
	for(int i1 = 0; i1 < 68; i1++)
	{
		fop.fo.Ifreq[i1] = null;
		fop.fo.Icumul_freq[i1] = null;
		fop.fo.freq[i1] = null;
		fop.fo.cumul_freq[i1] = null;
	}
	
	
	//body
	for(i=0; i<NUM_BAPS; i++) 
	{
		fop.bo.Ifreq[i] = NULL;
		fop.bo.Icumul_freq[i] = NULL;
		fop.bo.freq[i] = NULL;
		fop.bo.cumul_freq[i] = NULL;
	}
	
	for(int j1 = 0; j1 < 7; j1++)
	{
		cp.p_max[j1] = cp.i_max[j1] = fop.fo.range_ve_max[j1] = fop.fo.Irange_ve_max[j1] = ai3[j1];
		cp.i_min[j1] = fop.fo.Irange_ve_min[j1] = 0;
		cp.p_min[j1] = fop.fo.range_ve_min[j1] = ai4[j1];
	}
	
	for(int k1 = 0; k1 < 11; k1++)
		fop.fo.mask_index[k1] = ai[k1];
	
	
	//body
	for (i=0; i<BAP_NUM_GROUPS; i++) 
	{
		fop.bo.baps_in_group[i] = baps_in_group[i];
		for (j=0; j<baps_in_group[i]; j++) 
		{
			fop.bo.bap_mask_index[i][j] = bap_mask_index[i][j]-1;
		}
	}
	
	fop.fo.Idefault_min[2] = -1080;
	fop.fo.Idefault_max[2] = 1080;
	fop.fo.default_min[2] = -360;
	fop.fo.default_max[2] = 360;
	fop.fo.Idefault_min[3] = -600;
	fop.fo.Idefault_max[3] = 600;
	fop.fo.default_min[3] = -180;
	fop.fo.default_max[3] = 180;
	fop.fo.Idefault_min[4] = -1860;
	fop.fo.Idefault_max[4] = 1860;
	fop.fo.default_min[4] = -600;
	fop.fo.default_max[4] = 600;
	
	for(int l1 = 5; l1 < 9; l1++)
	{
		fop.fo.Idefault_min[l1] = -600;
		fop.fo.Idefault_max[l1] = 600;
		fop.fo.default_min[l1] = -180;
		fop.fo.default_max[l1] = 180;
	}
	
	for(int i2 = 9; i2 < 11; i2++)
	{
		fop.fo.Idefault_min[i2] = -1860;
		fop.fo.Idefault_max[i2] = 1860;
		fop.fo.default_min[i2] = -600;
		fop.fo.default_max[i2] = 600;
	}
	
	for(int j2 = 11; j2 < 14; j2++)
	{
		fop.fo.Idefault_min[j2] = -600;
		fop.fo.Idefault_max[j2] = 600;
		fop.fo.default_min[j2] = -180;
		fop.fo.default_max[j2] = 180;
	}
	
	for(int k2 = 14; k2 < 17; k2++)
	{
		fop.fo.Idefault_min[k2] = -1080;
		fop.fo.Idefault_max[k2] = 1080;
		fop.fo.default_min[k2] = -360;
		fop.fo.default_max[k2] = 360;
	}
	
	fop.fo.Idefault_min[17] = -420;
	fop.fo.Idefault_max[17] = 420;
	fop.fo.default_min[17] = -180;
	fop.fo.default_max[17] = 180;
	for(int l2 = 18; l2 < 20; l2++)
	{
		fop.fo.Idefault_min[l2] = -1080;
		fop.fo.Idefault_max[l2] = 1080;
		fop.fo.default_min[l2] = -600;
		fop.fo.default_max[l2] = 600;
	}
	
	for(int i3 = 20; i3 < 22; i3++)
	{
		fop.fo.Idefault_min[i3] = -600;
		fop.fo.Idefault_max[i3] = 600;
		fop.fo.default_min[i3] = -240;
		fop.fo.default_max[i3] = 240;
	}
	
	for(int j3 = 22; j3 < 24; j3++)
	{
		fop.fo.Idefault_min[j3] = -1200;
		fop.fo.Idefault_max[j3] = 1200;
		fop.fo.default_min[j3] = -420;
		fop.fo.default_max[j3] = 420;
	}
	
	for(int k3 = 24; k3 < 26; k3++)
	{
		fop.fo.Idefault_min[k3] = -900;
		fop.fo.Idefault_max[k3] = 900;
		fop.fo.default_min[k3] = -300;
		fop.fo.default_max[k3] = 300;
	}
	
	for(int l3 = 26; l3 < 28; l3++)
	{
		fop.fo.Idefault_min[l3] = -600;
		fop.fo.Idefault_max[l3] = 600;
		fop.fo.default_min[l3] = -180;
		fop.fo.default_max[l3] = 180;
	}
	
	for(int i4 = 28; i4 < 30; i4++)
	{
		fop.fo.Idefault_min[i4] = -420;
		fop.fo.Idefault_max[i4] = 420;
		fop.fo.default_min[i4] = -120;
		fop.fo.default_max[i4] = 120;
	}
	
	for(int j4 = 30; j4 < 36; j4++)
	{
		fop.fo.Idefault_min[j4] = -900;
		fop.fo.Idefault_max[j4] = 900;
		fop.fo.default_min[j4] = -360;
		fop.fo.default_max[j4] = 360;
	}
	
	for(int k4 = 36; k4 < 40; k4++)
	{
		fop.fo.Idefault_min[k4] = -900;
		fop.fo.Idefault_max[k4] = 900;
		fop.fo.default_min[k4] = -300;
		fop.fo.default_max[k4] = 300;
	}
	
	for(int l4 = 40; l4 < 42; l4++)
	{
		fop.fo.Idefault_min[l4] = -600;
		fop.fo.Idefault_max[l4] = 600;
		fop.fo.default_min[l4] = -180;
		fop.fo.default_max[l4] = 180;
	}
	
	for(int i5 = 42; i5 < 46; i5++)
	{
		fop.fo.Idefault_min[i5] = -1080;
		fop.fo.Idefault_max[i5] = 1080;
		fop.fo.default_min[i5] = -420;
		fop.fo.default_max[i5] = 420;
	}
	
	fop.fo.Idefault_min[46] = -1080;
	fop.fo.Idefault_max[46] = 1080;
	fop.fo.default_min[46] = -420;
	fop.fo.default_max[46] = 420;
	for(int j5 = 47; j5 < 50; j5++)
	{
		fop.fo.Idefault_min[j5] = -1860;
		fop.fo.Idefault_max[j5] = 1860;
		fop.fo.default_min[j5] = -600;
		fop.fo.default_max[j5] = 600;
	}
	
	fop.fo.Idefault_min[50] = -600;
	fop.fo.Idefault_max[50] = 600;
	fop.fo.default_min[50] = -180;
	fop.fo.default_max[50] = 180;
	fop.fo.Idefault_min[51] = -1860;
	fop.fo.Idefault_max[51] = 1860;
	fop.fo.default_min[51] = -600;
	fop.fo.default_max[51] = 600;
	for(int k5 = 52; k5 < 56; k5++)
	{
		fop.fo.Idefault_min[k5] = -600;
		fop.fo.Idefault_max[k5] = 600;
		fop.fo.default_min[k5] = -180;
		fop.fo.default_max[k5] = 180;
	}
	
	for(int l5 = 56; l5 < 58; l5++)
	{
		fop.fo.Idefault_min[l5] = -1860;
		fop.fo.Idefault_max[l5] = 1860;
		fop.fo.default_min[l5] = -600;
		fop.fo.default_max[l5] = 600;
	}
	
	for(int i6 = 58; i6 < 60; i6++)
	{
		fop.fo.Idefault_min[i6] = -600;
		fop.fo.Idefault_max[i6] = 600;
		fop.fo.default_min[i6] = -180;
		fop.fo.default_max[i6] = 180;
	}
	
	for(int j6 = 60; j6 < 62; j6++)
	{
		fop.fo.Idefault_min[j6] = -540;
		fop.fo.Idefault_max[j6] = 540;
		fop.fo.default_min[j6] = -120;
		fop.fo.default_max[j6] = 120;
	}
	
	fop.fo.Idefault_min[62] = -680;
	fop.fo.Idefault_max[62] = 680;
	fop.fo.default_min[62] = -180;
	fop.fo.default_max[62] = 180;
	fop.fo.Idefault_min[63] = -900;
	fop.fo.Idefault_max[63] = 900;
	fop.fo.default_min[63] = -180;
	fop.fo.default_max[63] = 180;
	
	for(int k6 = 64; k6 < 66; k6++)
	{
		fop.fo.Idefault_min[k6] = -900;
		fop.fo.Idefault_max[k6] = 900;
		fop.fo.default_min[k6] = -240;
		fop.fo.default_max[k6] = 240;
	}
	
	for(int l6 = 66; l6 < 68; l6++)
	{
		fop.fo.Idefault_min[l6] = -900;
		fop.fo.Idefault_max[l6] = 900;
		fop.fo.default_min[l6] = -300;
		fop.fo.default_max[l6] = 300;
	}
	
}
	
void CFBADecoder::init_arith(int fap_mask[])
{
	int i, j, n;
	if (fap_mask[0] != 0)
	{
		n = fop.fo.Irange_ve_max[0] - fop.fo.Irange_ve_min[0] + 1;
		fop.fo.Ifreq_ve[0] = new int[n];
		fop.fo.Icumul_ve[0] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.Ifreq_ve[0][j] = 1;
			fop.fo.Icumul_ve[0][j] = n - j;
		}
		fop.fo.Icumul_ve[0][n] = 0;
		n = fop.fo.Irange_ve_max[1] - fop.fo.Irange_ve_min[1] + 1;
		fop.fo.Ifreq_ve[1] = new int[n];
		fop.fo.Icumul_ve[1] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.Ifreq_ve[1][j] = 1;
			fop.fo.Icumul_ve[1][j] = n - j;
		}
		fop.fo.Icumul_ve[1][n] = 0;
		n = fop.fo.Irange_ve_max[2] - fop.fo.Irange_ve_min[2] + 1;
		fop.fo.Ifreq_ve[2] = new int[n];
		fop.fo.Icumul_ve[2] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.Ifreq_ve[2][j] = 1;
			fop.fo.Icumul_ve[2][j] = n - j;
		}
		fop.fo.Icumul_ve[2][n] = 0;
	}
	if (fap_mask[1] != 0)
	{
		n = fop.fo.Irange_ve_max[3] - fop.fo.Irange_ve_min[3] + 1;
		fop.fo.Ifreq_ve[3] = new int[n];
		fop.fo.Icumul_ve[3] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.Ifreq_ve[3][j] = 1;
			fop.fo.Icumul_ve[3][j] = n - j;
		}
		fop.fo.Icumul_ve[3][n] = 0;
		n = fop.fo.Irange_ve_max[4] - fop.fo.Irange_ve_min[4] + 1;
		fop.fo.Ifreq_ve[4] = new int[n];
		fop.fo.Icumul_ve[4] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.Ifreq_ve[4][j] = 1;
			fop.fo.Icumul_ve[4][j] = n - j;
		}
		fop.fo.Icumul_ve[4][n] = 0;
		n = fop.fo.Irange_ve_max[5] - fop.fo.Irange_ve_min[5] + 1;
		fop.fo.Ifreq_ve[5] = new int[n];
		fop.fo.Icumul_ve[5] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.Ifreq_ve[5][j] = 1;
			fop.fo.Icumul_ve[5][j] = n - j;
		}
		fop.fo.Icumul_ve[5][n] = 0;
		n = fop.fo.Irange_ve_max[6] - fop.fo.Irange_ve_min[6] + 1;
		fop.fo.Ifreq_ve[6] = new int[n];
		fop.fo.Icumul_ve[6] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.Ifreq_ve[6][j] = 1;
			fop.fo.Icumul_ve[6][j] = n - j;
		}
		fop.fo.Icumul_ve[6][n] = 0;
	}
	for (i = 2; i < NFAP; i++)
	{
		if (fap_mask[i] != 0)
		{
			n = fop.fo.Irange_max[i] - fop.fo.Irange_min[i] + 1;
			fop.fo.Ifreq[i] = new int[n];
			fop.fo.Icumul_freq[i] = new int[n+1];
			for (j = 0; j < n; j++)
			{
				fop.fo.Ifreq[i][j] = 1;
				fop.fo.Icumul_freq[i][j] = n - j;
			}
			fop.fo.Icumul_freq[i][n] = 0;
		}
	}
	if (fap_mask[0] != 0)
	{
		n = fop.fo.range_ve_max[0] - fop.fo.range_ve_min[0] + 1;
		fop.fo.freq_ve[0] = new int[n];
		fop.fo.cumul_ve[0] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.freq_ve[0][j] = 1;
			fop.fo.cumul_ve[0][j] = n - j;
		}
		fop.fo.cumul_ve[0][n] = 0;
		n = fop.fo.range_ve_max[1] - fop.fo.range_ve_min[1] + 1;
		fop.fo.freq_ve[1] = new int[n];
		fop.fo.cumul_ve[1] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.freq_ve[1][j] = 1;
			fop.fo.cumul_ve[1][j] = n - j;
		}
		fop.fo.cumul_ve[1][n] = 0;
		n = fop.fo.range_ve_max[2] - fop.fo.range_ve_min[2] + 1;
		fop.fo.freq_ve[2] = new int[n];
		fop.fo.cumul_ve[2] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.freq_ve[2][j] = 1;
			fop.fo.cumul_ve[2][j] = n - j;
		}
		fop.fo.cumul_ve[2][n] = 0;
	}
	if (fap_mask[1] != 0)
	{
		n = fop.fo.range_ve_max[3] - fop.fo.range_ve_min[3] + 1;
		fop.fo.freq_ve[3] = new int[n];
		fop.fo.cumul_ve[3] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.freq_ve[3][j] = 1;
			fop.fo.cumul_ve[3][j] = n - j;
		}
		fop.fo.cumul_ve[3][n] = 0;
		n = fop.fo.range_ve_max[4] - fop.fo.range_ve_min[4] + 1;
		fop.fo.freq_ve[4] = new int[n];
		fop.fo.cumul_ve[4] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.freq_ve[4][j] = 1;
			fop.fo.cumul_ve[4][j] = n - j;
		}
		fop.fo.cumul_ve[4][n] = 0;
		n = fop.fo.range_ve_max[5] - fop.fo.range_ve_min[5] + 1;
		fop.fo.freq_ve[5] = new int[n];
		fop.fo.cumul_ve[5] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.freq_ve[5][j] = 1;
			fop.fo.cumul_ve[5][j] = n - j;
		}
		fop.fo.cumul_ve[5][n] = 0;
		n = fop.fo.range_ve_max[6] - fop.fo.range_ve_min[6] + 1;
		fop.fo.freq_ve[6] = new int[n];
		fop.fo.cumul_ve[6] = new int[n+1];
		for (j = 0; j < n; j++)
		{
			fop.fo.freq_ve[6][j] = 1;
			fop.fo.cumul_ve[6][j] = n - j;
		}
		fop.fo.cumul_ve[6][n] = 0;
	}
	for (i = 2; i < NFAP; i++)
	{
		if (fap_mask[i] != 0)
		{
			n = fop.fo.range_max[i] - fop.fo.range_min[i] + 1;
			fop.fo.freq[i] = new int[n];
			fop.fo.cumul_freq[i] = new int[n+1];
			for (j = 0; j < n; j++)
			{
				fop.fo.freq[i][j] = 1;
				fop.fo.cumul_freq[i][j] = n - j;
			}
			fop.fo.cumul_freq[i][n] = 0;
		}
	}
}

int CFBADecoder::recon(int fap_diff, int fap_prev)
{
	int fap_rec;
	fap_rec = fap_diff + fap_prev;
	return fap_rec;
}

void CFBADecoder::update(int symbol, int index)
{
	int n;
	int *cumul_tmp, *freq_tmp;
	n = fop.fo.range_max[index] - fop.fo.range_min[index] + 1;
	cumul_tmp = fop.fo.cumul_freq[index];
	freq_tmp = fop.fo.freq[index];
	if (cumul_tmp[0] == fba_cooondef_Max_frequency)
	{
		int i, cum;
		cum = 0;
		for (i = n - 1; i >= 0; i--)
		{
			freq_tmp[i] = (freq_tmp[i] + 1) / 2;
			cum += freq_tmp[i];
			cumul_tmp[i] = cum;
		}
		cumul_tmp[n] = 0;
	}
	freq_tmp[symbol] += 1;
	cumul_tmp[symbol] += 1;
	while (symbol > 0)
	{
		symbol -= 1;
		cumul_tmp[symbol] += 1;
	}
}

void CFBADecoder::update_viseme_expression(int symbol, int index)
{
	int n;
	int *cumul_tmp, *freq_tmp;
	n = fop.fo.range_ve_max[index] - fop.fo.range_ve_min[index] + 1;
	cumul_tmp = fop.fo.cumul_ve[index];
	freq_tmp = fop.fo.freq_ve[index];
	if (cumul_tmp[0] == fba_cooondef_Max_frequency)
	{
		int i, cum;
		cum = 0;
		for (i = n - 1; i >= 0; i--)
		{
			freq_tmp[i] = (freq_tmp[i] + 1) / 2;
			cum += freq_tmp[i];
			cumul_tmp[i] = cum;
		}
		cumul_tmp[n] = 0;
	}
	freq_tmp[symbol] += 1;
	cumul_tmp[symbol] += 1;
	while (symbol > 0)
	{
		symbol -= 1;
		cumul_tmp[symbol] += 1;
	}
}


void CFBADecoder::bap_decode_frame_rate() 
{
	
	fop.bo.frame_rate = fop.stream.getbits(8);
	fop.bo.seconds = fop.stream.getbits(4);
	fop.bo.frequency_offset = fop.stream.getbits(1);
}

void CFBADecoder::bap_decode_time_code() 
{
	
	fop.bo.time_code_hours = fop.stream.getbits(5);
	fop.bo.time_code_minutes = fop.stream.getbits(6);
	fop.bo.marker_bit = fop.stream.getbits(1);
	fop.bo.time_code_seconds = fop.stream.getbits(6);
}

void CFBADecoder::bap_decode_skip_frames(void) 
{
	
	int ivalue;
	
	fop.bo.number_of_frames_to_skip = 0;
	do {
        ivalue = fop.stream.getbits(4);
        fop.bo.number_of_frames_to_skip += ivalue;
	} while (ivalue==15);
}

void CFBADecoder::bap_decode_group_mask(int n) 
{
  int i;
  int temp_index;

  for(i=0;i<fop.bo.baps_in_group[n]; i++) {
    temp_index = fop.bo.bap_mask_index[n][i];
    fop.bo.bap_mask[temp_index] = fop.stream.getbits(1);
	fbaps.baps->groupMask[temp_index] = fop.bo.bap_mask[temp_index];
  }

}

void CFBADecoder::bap_decode_new_minmax() 
{
  int i, j;
  int minmax;
  int marker;
  int scaler;
  int temp_index;


  if (fop.bo.bap_quant) scaler = fba_cooondef_bap_pred_scale[fop.bo.bap_quant];
  else scaler = 1;
  
  if(fop.bo.is_i_new_max) {
    for (i=0; i<BAP_NUM_GROUPS; i++) {
        for(j=0;j<fop.bo.baps_in_group[i]; j++) {
          temp_index = fop.bo.bap_mask_index[i][j];
		if(!(j&3)) {
			marker = fop.stream.getbits(1);
			#ifdef _DEBUG
			assert(marker);
			#endif
		}
                if(fop.bo.bap_mask[temp_index]) {
                        minmax = fop.stream.getbits(5);
						cp.bap_i_max[temp_index] = minmax*RANGE_DELTA;
                        fop.bo.Irange_max[temp_index] = minmax*RANGE_DELTA/scaler;
                }
        }
    }
  }
  if(fop.bo.is_i_new_min) {
    for (i=0; i<BAP_NUM_GROUPS; i++) {
        for(j=0;j<fop.bo.baps_in_group[i]; j++) {
          temp_index = fop.bo.bap_mask_index[i][j];

		if(!(j&3)) {
			marker = fop.stream.getbits(1);
			#ifdef _DEBUG
			assert(marker);
			#endif
		}
                if(fop.bo.bap_mask[temp_index]) {
                        minmax = fop.stream.getbits(5);
						cp.bap_i_min[temp_index] = -minmax*RANGE_DELTA;
                        fop.bo.Irange_min[temp_index] = -minmax*RANGE_DELTA/scaler;
                }
        }
    }
  }
  if(fop.bo.is_p_new_max) {
    for (i=0; i<BAP_NUM_GROUPS; i++) {
        for(j=0;j<fop.bo.baps_in_group[i]; j++) {
          temp_index = fop.bo.bap_mask_index[i][j];

		if(!(j&3)) {
			marker = fop.stream.getbits(1);
			#ifdef _DEBUG
			assert(marker);
			#endif
		}
                if(fop.bo.bap_mask[temp_index]) {
                        minmax = fop.stream.getbits(5);
						cp.bap_p_max[temp_index] = minmax*RANGE_DELTA;
                        fop.bo.range_max[temp_index] = minmax*RANGE_DELTA/scaler;
                }
        }
    }
  }
  if(fop.bo.is_p_new_min) {
    for (i=0; i<BAP_NUM_GROUPS; i++) {
        for(j=0;j<fop.bo.baps_in_group[i]; j++) {
          temp_index = fop.bo.bap_mask_index[i][j];

		if(!(j&3)) {
			marker = fop.stream.getbits(1);
			#ifdef _DEBUG
			assert(marker);
			#endif
		}
                if(fop.bo.bap_mask[temp_index]) {
                        minmax = fop.stream.getbits(5);
						cp.bap_p_min[temp_index] = -minmax*RANGE_DELTA;
                        fop.bo.range_min[temp_index] = -minmax*RANGE_DELTA/scaler;
                }
        }
    }
  }

}

void CFBADecoder::decode_ibap() {
  int i, j, k;
  int symbol, qstep;

  decoder_reset();

  for (k=0; k<BAP_NUM_GROUPS; k++) {
    for(j=0;j<fop.bo.baps_in_group[k]; j++) {
	  i = fop.bo.bap_mask_index[k][j];
	if(fop.bo.bap_mask[i]) {
		symbol = decode_a_symbol(fop.bo.Icumul_freq[i]);
		fop.bo.bap_Q[i] = symbol + fop.bo.Irange_min[i];
		qstep = fop.bo.IBQP[i]*fba_cooondef_bap_pred_scale[fop.bo.bap_quant];
		fop.bo.bap_prev[i] = dequant(fop.bo.bap_Q[i], qstep);
		fbaps.baps->value[i] = fop.bo.bap_prev[i];
	}
    }
  }

}


void CFBADecoder::decode_pbap() 
{
  int i, j, k;
  int symbol, qstep;

  
  /*v17
  skip_frames = getbits(1);
  if(skip_frames) {
	bap_decode_skip_frames();
  } else {
	fop.bo.number_of_frames_to_skip = 0;
  }
  */  

  decoder_reset();

  for (k=0; k<BAP_NUM_GROUPS; k++) {
    for(j=0;j<fop.bo.baps_in_group[k]; j++) {
	  i = fop.bo.bap_mask_index[k][j];
	  if(fop.bo.bap_mask[i]) {
	    symbol = decode_a_symbol(fop.bo.cumul_freq[i]);
	    bap_update(symbol, i);
	    fop.bo.bap_Q[i] = symbol + fop.bo.range_min[i];
	    qstep = fop.bo.BQP[i]*fba_cooondef_bap_pred_scale[fop.bo.bap_quant];
	    fop.bo.bap_diff[i] = dequant(fop.bo.bap_Q[i], qstep);
	    fop.bo.bap_prev[i] = recon(fop.bo.bap_diff[i], fop.bo.bap_prev[i]);
	    fbaps.baps->value[i] = fop.bo.bap_prev[i];
	  }
    }
  }	
}


void CFBADecoder::bap_update(int symbol, int index) 
{
 
  int n;
  int *cumul_tmp, *freq_tmp;
 
  n = fop.bo.range_max[index]-fop.bo.range_min[index]+1;
  cumul_tmp = fop.bo.cumul_freq[index];
  freq_tmp = fop.bo.freq[index];

  if (cumul_tmp[0] == fba_cooondef_Max_frequency) {
    int i, cum;
    cum = 0;
    for(i=n-1; i>=0; i--) {
        freq_tmp[i] = (freq_tmp[i]+1)/2;
        cum += freq_tmp[i];
        cumul_tmp[i] = cum;
    }
    cumul_tmp[n] = 0;
  }
  freq_tmp[symbol] += 1;
  cumul_tmp[symbol] += 1;
  while(symbol>0) {
        symbol -= 1;
        cumul_tmp[symbol] += 1;
  }
 
}

void CFBADecoder::bap_init_arith(int *bap_mask) 
{
 
  int i, j, n;
 
  for(i=0; i<NUM_BAPS; i++) {
	if(bap_mask[i]) {
		free(fop.bo.Ifreq[i]);
		free(fop.bo.Icumul_freq[i]);
                n = fop.bo.Irange_max[i]-fop.bo.Irange_min[i]+1;
                fop.bo.Ifreq[i] = (int *) malloc(n*sizeof(int));
                fop.bo.Icumul_freq[i] = (int *) malloc((n+1)*sizeof(int));
                for(j=0; j<n; j++) {
                        fop.bo.Ifreq[i][j] = 1;
                        fop.bo.Icumul_freq[i][j] = n-j;
                }
                fop.bo.Icumul_freq[i][n] = 0;
	}
  }

  for(i=0; i<NUM_BAPS; i++) {
	if(bap_mask[i]) {
		free(fop.bo.freq[i]);
		free(fop.bo.cumul_freq[i]);
                n = fop.bo.range_max[i]-fop.bo.range_min[i]+1;
                fop.bo.freq[i] = (int *) malloc(n*sizeof(int));
                fop.bo.cumul_freq[i] = (int *) malloc((n+1)*sizeof(int));
                for(j=0; j<n; j++) {
                        fop.bo.freq[i][j] = 1;
                        fop.bo.cumul_freq[i][j] = n-j;
                }
                fop.bo.cumul_freq[i][n] = 0;
	}
  }
}


// Added by KStankovic
CFBADecoder::~CFBADecoder()
{
	int i;

	for (i = 0; i < NUM_BAPS; i++)
	{
		if (fop.bo.Icumul_freq[i] != NULL) free(fop.bo.Icumul_freq[i]);
		if (fop.bo.cumul_freq[i] != NULL) free(fop.bo.cumul_freq[i]);
		if (fop.bo.Ifreq[i] != NULL) free(fop.bo.Ifreq[i]);
		if (fop.bo.freq[i] != NULL) free(fop.bo.freq[i]);
	}

	for (i = 0; i < NFAP; i++)
	{
		if (fop.fo.freq[i] != NULL) delete [] fop.fo.freq[i];
		if (fop.fo.cumul_freq[i] != NULL) delete [] fop.fo.cumul_freq[i];
		if (fop.fo.Icumul_freq[i] != NULL) delete [] fop.fo.Icumul_freq[i];
		if (fop.fo.Ifreq[i] != NULL) delete [] fop.fo.Ifreq[i];
	}

	for (i = 0; i < 7; i++)
	{
		if (fop.fo.cumul_ve[i] != NULL) delete [] fop.fo.cumul_ve[i];
		if (fop.fo.freq_ve[i] != NULL) delete [] fop.fo.freq_ve[i];
		if (fop.fo.Icumul_ve[i] != NULL) delete [] fop.fo.Icumul_ve[i];
		if (fop.fo.Ifreq_ve[i] != NULL) delete [] fop.fo.Ifreq_ve[i];
	}
	
	fop.close();
}
//

}
