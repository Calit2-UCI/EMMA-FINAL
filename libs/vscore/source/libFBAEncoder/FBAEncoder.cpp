
#include "FBAEncoder.h"

// C++
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Win32
#ifdef WIN32
#define S_IRWXG 1
#include <windows.h> //necessary for messagebox in windows
#else
	#include <unistd.h>
#endif


#include <fcntl.h>
#include <sys/stat.h>

namespace VisageSDK
{

static int fba_cooondef_bap_pred_scale[32] = {0,1,2,3,5,7,9,11,14,17,20,23,27,31,35,39,43,47,52,57,62,67,72,77,82,88,94,100,106,113,120,127};

/*
** Right now QP and IQP are the same. We use IQP just in case we decide
** to use different QP values for intra and inter frames later
*/
static int fba_cooondef_QP[NFAP] = {1, 1, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
	        1, 1, 1, 1, 128, 128, 128, 128, 1, 1, 1, 1, 2, 2, 
		2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 512, 170,
		170, 170, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 
		1, 1, 1, 1};
static int fba_cooondef_IQP[NFAP] = {1, 1, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
						1, 1, 1, 1, 128, 128, 128, 128, 1, 1, 1, 1, 2, 2, 
						2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 512, 170,
						170, 170, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 
						1, 1, 1, 1};



#define fba_cooondef_DEFAULTMIN -1000
#define fba_cooondef_DEFAULTMAX 1000

CFBAEncoder::CFBAEncoder(int objectFunctionality)
{
	return;
}

CFBAEncoder::CFBAEncoder()
{ 
	produceOutFapFile=false;
	produceOutBapFile=false;
	low=0;
	high=fba_cooondef_top;
	opposite_bits=0;
	length=0;
	zerorun=0;
	head_cnt = 0;
	ibit_cnt = 0;
	pbit_cnt = 0;
	total_bit_cnt = 0;
	// Added by KStankovic
	for (int i = 0; i < ENCODER_BUFFER_SIZE; i++)
		outbfr[i] = 0;

	fop.cur_frame_bit_cnt = 0;
	//
	return; 
}


CFBAEncoder::CFBAEncoder(char *outFAPFileName,char *outBAPFileName)
{ 
	if (outFAPFileName) 
	{
		produceOutFapFile=true;
		strcpy(fap_out_file,outFAPFileName);
	}
	else 
	{
		produceOutFapFile=false;
	} 
	if (outBAPFileName) 
	{
		produceOutBapFile=true;
		strcpy(fap_out_file,outFAPFileName);
	}
	else 
	{
		produceOutBapFile=false;
	} 

	low=0;
	high=fba_cooondef_top;
	opposite_bits=0;
	length=0;
	zerorun=0;
	head_cnt = 0;
	ibit_cnt = 0;
	pbit_cnt = 0;
	total_bit_cnt = 0;
	// Added by KStankovic
	for (int i = 0; i < ENCODER_BUFFER_SIZE; i++)
		outbfr[i] = 0;

	fop.cur_frame_bit_cnt = 0;
	//
	return; 
}

void CFBAEncoder::encodeFBAPFiles( char *epffile, char *fapfile, char *bapfile, char *outfile, char *fap_outfile)
{
	char *arg[6];
	char empty[1] = "";

	arg[0] = empty;
	arg[1] = epffile;
	arg[2] = fapfile;
	arg[3] = bapfile;
	arg[4] = outfile;
	arg[5] = fap_outfile;

	encode(6,arg);

}
int CFBAEncoder::encode(int argc, char **argv) 
{
	int i, j;
	int maxn, minn;
	char tag[4];
	bool frame_started = false;
	unsigned char *buf;
	int size;
	int temp_index;
	int temp_fba_paramset = 0;/*edp*/
	
	initialize(argc, argv);
	initbits();
	
	/* fba_sequence() function is embedded here */
	head_cnt += putbits(32, FBA_OBJECT_START_CODE);
	
	while(get_line(epf_fp)) {
		
		fscanf(epf_fp, "%s", tag);
		
		if( strcmp(tag, "FFR")==0 ) {
			assure_fap_frame();
			fscanf(epf_fp, "%d", &fop.fo.frame_num);
		} else if( strcmp(tag, "BFR")==0 ) {
			assure_bap_frame();
			fscanf(epf_fp, "%d", &fop.bo.frame_num);
		} else if( strcmp(tag, "ISI")==0 ) {
			fscanf(epf_fp, "%d", &fop.is_intra);
			#ifdef _DEBUG
			assert(fop.is_intra==0 || fop.is_intra==1);
			#endif
		} else if( strcmp(tag, "FRR")==0 ) {
			fop.fo.is_frame_rate = true;
			fscanf(epf_fp, "%d%d%d",
				&fop.fo.frame_rate, &fop.fo.fr_seconds, &fop.fo.fr_frequency_offset);
			#ifdef _DEBUG
			assert(fop.fo.frame_rate>=0 && fop.fo.frame_rate<=255);
			assert(fop.fo.fr_seconds>=0 && fop.fo.fr_seconds<=15);
			assert(fop.fo.fr_frequency_offset==0 || fop.fo.fr_frequency_offset==1);
			#endif
		} else if( strcmp(tag, "BRR")==0 ) {
			fop.bo.is_frame_rate = true;
			fscanf(epf_fp, "%d%d%d",
				&fop.bo.frame_rate, &fop.bo.fr_seconds, &fop.bo.fr_frequency_offset);
			#ifdef _DEBUG
			assert(fop.bo.frame_rate>=0 && fop.bo.frame_rate<=255);
			assert(fop.bo.fr_seconds>=0 && fop.bo.fr_seconds<=15);
			assert(fop.bo.fr_frequency_offset==0 || fop.bo.fr_frequency_offset==1);
			#endif
		} else if( strcmp(tag, "TC")==0 ) {
			fop.fo.is_time_code = true;
			fscanf(epf_fp, "%d%d%d%d", &fop.fo.time_code_hours,
				&fop.fo.time_code_minutes, &fop.marker_bit, &fop.fo.time_code_seconds);
		} else if( strcmp(tag, "BTC")==0) {
			fop.bo.is_time_code = true;
			fscanf(epf_fp, "%d%d%d%d", &fop.bo.time_code_hours,
				&fop.bo.time_code_minutes, &fop.marker_bit, &fop.bo.time_code_seconds);
		} else if( strcmp(tag, "SKN")==0 ) {
			fop.fo.skip_frames = true;
			fscanf(epf_fp, "%d", &fop.fo.number_of_frames_to_skip);
		} else if( strcmp(tag, "BSK")==0 ) {
			fop.bo.skip_frames = true;
			fscanf(epf_fp, "%d", &fop.bo.number_of_frames_to_skip);
		} else if( strcmp(tag, "FMT")==0 ) {
			assure_fap_frame();
			for(i=0; i<N_FAP_GROUP; i++) {
				fscanf(epf_fp, "%d", &fop.fo.fap_mask_type[i]);
				#ifdef _DEBUG
				assert(fop.fo.fap_mask_type[i]>=0 && fop.fo.fap_mask_type[i]<=3);
				#endif
				if(fop.fo.fap_mask_type[i]==1 || fop.fo.fap_mask_type[i]==2) {
					fop.fo.new_mask_flag = true;
					for(j=fop.fo.mask_index[i]; j<fop.fo.mask_index[i+1]; j++) {
						fscanf(epf_fp, "%d", &fop.fo.fap_mask[j]);
						#ifdef _DEBUG
						assert(fop.fo.fap_mask[j]==0 || fop.fo.fap_mask[j]==1);
						#endif
					}
				} else if(fop.fo.fap_mask_type[i]==0) {
					for(j=fop.fo.mask_index[i]; j<fop.fo.mask_index[i+1]; j++) {
						fop.fo.fap_mask[j] = 0;
					}
				} else {
					fop.fo.new_mask_flag = true;
					for(j=fop.fo.mask_index[i]; j<fop.fo.mask_index[i+1]; j++) {
						fop.fo.fap_mask[j] = 1;
					}
				}
			}
		} else if( strcmp(tag, "BMT")==0 ) {
			assure_bap_frame();
			for(i=0; i<BAP_NUM_GROUPS; i++) {
				fscanf(epf_fp, "%d", &fop.bo.bap_mask_type[i]);
				#ifdef _DEBUG
				assert(fop.bo.bap_mask_type[i]>=0 && fop.bo.bap_mask_type[i]<=3);
				#endif
				if(fop.bo.bap_mask_type[i]==1) {
					fop.bo.new_mask_flag = true;
					for(j=0;j<fop.bo.baps_in_group[i]; j++) {
						temp_index = fop.bo.bap_mask_index[i][j];
						fscanf(epf_fp, "%d", &fop.bo.bap_mask[temp_index]);
						#ifdef _DEBUG
						assert(fop.bo.bap_mask[temp_index]==0 || fop.bo.bap_mask[temp_index]==1);
						#endif
					}
				}
				else if (fop.bo.bap_mask_type[i]==0) {
					for(j=0;j<fop.bo.baps_in_group[i]; j++) {
						temp_index = fop.bo.bap_mask_index[i][j];
						fop.bo.bap_mask[temp_index] = 0;
					}
				}
				else if (fop.bo.bap_mask_type[i]==3) {
					fop.bo.new_mask_flag = true;
					for(j=0;j<fop.bo.baps_in_group[i]; j++) {
						temp_index = fop.bo.bap_mask_index[i][j];
						fop.bo.bap_mask[temp_index] = 1;
					}
				}
				else
				{
					char tmpmsg[300];
					sprintf(tmpmsg,"EPF file contains wrong BAP mask type for group %d\n", i);
					#ifdef WIN32
					MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
					#endif
					exit(-102);
				}
			}
		} else if( strcmp(tag, "FQU")==0 ) {
			assure_fap_frame();
			fop.fo.is_fap_quant = true;
			fscanf(epf_fp, "%d", &fop.fo.quant);
			#ifdef _DEBUG
			assert(fop.fo.quant>=0 && fop.fo.quant<=31);
			#endif
			if(fop.fo.quant) 
				for(i=2;i<7;i+=2) {
					fop.fo.Irange_ve_max[i] = 63/fop.fo.quant;
					fop.fo.range_ve_max[i] = 63/fop.fo.quant;
					fop.fo.range_ve_min[i] = -63/fop.fo.quant;
				}
      } else if( strcmp(tag, "BQU")==0 ) {
	assure_bap_frame();
	fop.bo.is_bap_quant = true;
	fscanf(epf_fp, "%d", &fop.bo.quant);
	#ifdef _DEBUG
	assert(fop.bo.quant>=0 && fop.bo.quant<=31);
	#endif
		} else if( strcmp(tag, "FIX")==0 ) {
			assure_fap_frame();
			fop.fo.is_i_new_max = true;
	/*fscanf(epf_fp, "%d%d%d%d%d", &maxn, &maxn, &maxn, &maxn, &maxn); -edp*/
	/*fscanf(epf_fp, "%d %d %d", &maxn, &maxn, &maxn); remove faps12 min/max*-edp*/
			for(i=2; i<NFAP; i++) {
				fscanf(epf_fp, "%d", &maxn);
				fop.fo.i_new_max[i] = (maxn+RANGE_DELTA-1)/RANGE_DELTA;
				maxn = fop.fo.i_new_max[i]*RANGE_DELTA; 
				if(fop.fo.quant) {
					fop.fo.Irange_max[i] = maxn/fop.fo.quant;
				} else {
					fop.fo.Irange_max[i] = maxn;
				}
			}
		} else if( strcmp(tag, "FIN")==0 ) {
			assure_fap_frame();
			fop.fo.is_i_new_min = true;
	/*fscanf(epf_fp, "%d%d%d%d%d", &minn, &minn, &minn, &minn, &minn);-edp*/
	/*fscanf(epf_fp, "%d %d %d", &minn, &minn, &minn);remove faps12 min/max -edp*/
			for(i=2; i<NFAP; i++) {
				fscanf(epf_fp, "%d", &minn);
				fop.fo.i_new_min[i] = -(minn-RANGE_DELTA+1)/RANGE_DELTA;
				minn = -fop.fo.i_new_min[i]*RANGE_DELTA; 
				if(fop.fo.quant) {
					fop.fo.Irange_min[i] = minn/fop.fo.quant;
				} else {
					fop.fo.Irange_min[i] = minn;
				}
			}
		} else if( strcmp(tag, "FPX")==0 ) {
			assure_fap_frame();
			fop.fo.is_p_new_max = true;
	/*fscanf(epf_fp, "%d%d%d%d%d", &maxn, &maxn, &maxn, &maxn, &maxn);-edp*/
	/*fscanf(epf_fp, "%d %d %d", &maxn, &maxn, &maxn);remove faps12 min/max -edp*/
			for(i=2; i<NFAP; i++) {
				fscanf(epf_fp, "%d", &maxn);
				fop.fo.p_new_max[i] = (maxn+RANGE_DELTA-1)/RANGE_DELTA;
				maxn = fop.fo.p_new_max[i]*RANGE_DELTA; 
				if(fop.fo.quant) {
					fop.fo.range_max[i] = maxn/fop.fo.quant;
				} else {
					fop.fo.range_max[i] = maxn;
				}
			}
		} else if( strcmp(tag, "FPN")==0 ) {
			assure_fap_frame();
			fop.fo.is_p_new_min = true;
	/*fscanf(epf_fp, "%d%d%d%d%d", &minn, &minn, &minn, &minn, &minn); -edp*/
	/*fscanf(epf_fp, "%d %d %d", &minn, &minn, &minn);remove faps12 min/max -edp*/
			for(i=2; i<NFAP; i++) {
				fscanf(epf_fp, "%d", &minn);
				fop.fo.p_new_min[i] = -(minn-RANGE_DELTA+1)/RANGE_DELTA;
				minn = -fop.fo.p_new_min[i]*RANGE_DELTA; 
				if(fop.fo.quant) {
					fop.fo.range_min[i] = minn/fop.fo.quant;
				} else {
					fop.fo.range_min[i] = minn;
				}
			}
		} else if( strcmp(tag, "BIX")==0 ) {
			assure_bap_frame();
			fop.bo.is_i_new_max = true;
			for(i=0; i<NUM_BAPS; i++) {
				fscanf(epf_fp, "%d", &maxn);
				fop.bo.i_new_max[i] = (maxn+RANGE_DELTA-1)/RANGE_DELTA;
				maxn = fop.bo.i_new_max[i]*RANGE_DELTA; 
				if(fop.bo.quant) {
					fop.bo.Irange_max[i] = maxn/fba_cooondef_bap_pred_scale[fop.bo.quant];
				} else {
					fop.bo.Irange_max[i] = maxn;
				}
			}
		} else if( strcmp(tag, "BIN")==0 ) {
			assure_bap_frame();
			fop.bo.is_i_new_min = true;
			for(i=0; i<NUM_BAPS; i++) {
				fscanf(epf_fp, "%d", &minn);
				fop.bo.i_new_min[i] = -(minn-RANGE_DELTA+1)/RANGE_DELTA;
				minn = -fop.bo.i_new_min[i]*RANGE_DELTA; 
				if(fop.bo.quant) {
					fop.bo.Irange_min[i] = minn/fba_cooondef_bap_pred_scale[fop.bo.quant];
				} else {
					fop.bo.Irange_min[i] = minn;
				}
			}
		} else if( strcmp(tag, "BPX")==0 ) {
			assure_bap_frame();
			fop.bo.is_p_new_max = true;
			for(i=0; i<NUM_BAPS; i++) {
				fscanf(epf_fp, "%d", &maxn);
				fop.bo.p_new_max[i] = (maxn+RANGE_DELTA-1)/RANGE_DELTA;
				maxn = fop.bo.p_new_max[i]*RANGE_DELTA; 
				if(fop.bo.quant) {
					fop.bo.range_max[i] = maxn/fba_cooondef_bap_pred_scale[fop.bo.quant];
				} else {
					fop.bo.range_max[i] = maxn;
				}
			}
		} else if( strcmp(tag, "BPN")==0 ) {
			assure_bap_frame();
			fop.bo.is_p_new_min = true;
			for(i=0; i<NUM_BAPS; i++) {
				fscanf(epf_fp, "%d", &minn);
				fop.bo.p_new_min[i] = -(minn-RANGE_DELTA+1)/RANGE_DELTA;
				minn = -fop.bo.p_new_min[i]*RANGE_DELTA; 
				if(fop.bo.quant) {
					fop.bo.range_min[i] = minn/fba_cooondef_bap_pred_scale[fop.bo.quant];
				} else {
					fop.bo.range_min[i] = minn;
				}
			}
		} else if( strcmp(tag, "FBM")==0) {
			fscanf(epf_fp, "%d", &temp_fba_paramset);
			#ifdef _DEBUG
			assert(temp_fba_paramset>=0 && temp_fba_paramset<=3);      
			#endif
			if(frame_started)
				finish_frame();
			/*	else  // IP removed the "else" statement, otherwise the fba_paramset is never set!?*/
			fop.fba_paramset = temp_fba_paramset;/*edp*/
			reset_frame();
			frame_started = true;
		}
		else 
		{
			char tmpmsg[300];
			sprintf(tmpmsg,"tag %s unknown\n", tag);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-2);
		}
  } 
  
  
  if(frame_started)
	  finish_frame();
  
  head_cnt += next_start_code();
  head_cnt += putbits(32, FBA_OBJECT_END_CODE);
  
  size = bitcount();
  buf = outbfr;
  write_bits(fop.stream_fh,buf,size);
  
  
  cleanup();
  return 1;
}

void CFBAEncoder::cleanup(void) 
{
  int i;
  for(i=0; i<NFAP; i++) {
		if(fop.fo.Ifreq[i]) free(fop.fo.Ifreq[i]);
		if(fop.fo.Icumul_freq[i]) free(fop.fo.Icumul_freq[i]);
		if(fop.fo.freq[i]) free(fop.fo.freq[i]);
		if(fop.fo.cumul_freq[i]) free(fop.fo.cumul_freq[i]);
  }
  for(i=0; i<7; i++) {
    if(fop.fo.Ifreq_ve[i]) free(fop.fo.Ifreq_ve[i]);
    if(fop.fo.Icumul_ve[i]) free(fop.fo.Icumul_ve[i]);
    if(fop.fo.freq_ve[i]) free(fop.fo.freq_ve[i]);
    if(fop.fo.cumul_ve[i]) free(fop.fo.cumul_ve[i]);
  }
	if(fop.fo.fap_mask) free(fop.fo.fap_mask);
	if(fop.fo.fap_mask2) free(fop.fo.fap_mask2);
	if(fop.fo.prev_fap_mask) free(fop.fo.prev_fap_mask);

  if(bap_fp)
	  fclose(bap_fp);
  if(fap_fp)
	  fclose(fap_fp);
  fclose(epf_fp);
#ifdef WIN32
  _close(fop.stream_fh);
#else
  close(fop.stream_fh);
#endif
  if(fop.bo.bap_out_fp)
	  fclose(fop.bo.bap_out_fp);
  if(fop.bo.bap_out_fp)
	  fclose(fop.bo.bap_out_fp);

  for (i=0; i<NUM_BAPS; i++) {
    if(fop.bo.Ifreq[i]) free(fop.bo.Ifreq[i]);
    if(fop.bo.Icumul_freq[i]) free(fop.bo.Icumul_freq[i]);
    if(fop.bo.freq[i]) free(fop.bo.freq[i]);
    if(fop.bo.cumul_freq[i]) free(fop.bo.cumul_freq[i]);
  }

/*
	for(i=0; i<NFAP; i++) {
		if(fop.fo.Ifreq[i]) free(fop.fo.Ifreq[i]);
		if(fop.fo.Icumul_freq[i]) free(fop.fo.Icumul_freq[i]);
		if(fop.fo.freq[i]) free(fop.fo.freq[i]);
		if(fop.fo.cumul_freq[i]) free(fop.fo.cumul_freq[i]);
	}
	if(fop.fo.fap_mask) free(fop.fo.fap_mask);
	if(fop.fo.fap_mask2) free(fop.fo.fap_mask2);
	if(fop.fo.prev_fap_mask) free(fop.fo.prev_fap_mask);
*/	
}


void CFBAEncoder::finish_frame(void)
{
	int i,minn,maxn,hquant;/*-edp*/
	
	unsigned char *buf;
	int size;
	
	if(fop.fo.p_new_max[3] == 0 && fop.fo.p_new_min[3] == 0) 
	{/*init min/max values if not set before*/
		hquant = fop.fo.quant/2;
		if(fop.fo.quant)
		{
			for(i=2;i<7;i+=2) 
			{
				fop.fo.Irange_ve_max[i] = (63+hquant)/fop.fo.quant;
				fop.fo.range_ve_max[i] = (63+hquant)/fop.fo.quant;
				fop.fo.range_ve_min[i] = -(63+hquant)/fop.fo.quant;
			}
		}
		for(i=2;i<NFAP;i++) 
		{/*set init minmax values -edp*/
			fop.fo.i_new_max[i] = (fba_cooondef_DEFAULTMAX+RANGE_DELTA-1)/RANGE_DELTA;
			maxn = fop.fo.i_new_max[i]*RANGE_DELTA;
			if(fop.fo.quant) fop.fo.Irange_max[i] = maxn/fop.fo.quant;
			else fop.fo.Irange_max[i] = maxn;
			
			fop.fo.i_new_min[i] = -(fba_cooondef_DEFAULTMIN-RANGE_DELTA+1)/RANGE_DELTA;
			minn = -fop.fo.i_new_min[i]*RANGE_DELTA;
			if(fop.fo.quant) fop.fo.Irange_min[i] = minn/fop.fo.quant;
			else fop.fo.Irange_min[i] = minn;
			
			fop.fo.p_new_max[i] = (fba_cooondef_DEFAULTMAX+RANGE_DELTA-1)/RANGE_DELTA;
			maxn = fop.fo.p_new_max[i]*RANGE_DELTA;
			if(fop.fo.quant) fop.fo.range_max[i] = maxn/fop.fo.quant;
			else fop.fo.range_max[i] = maxn;
			
			fop.fo.p_new_min[i] = -(fba_cooondef_DEFAULTMIN-RANGE_DELTA+1)/RANGE_DELTA;
			minn = -fop.fo.p_new_min[i]*RANGE_DELTA;
			if(fop.fo.quant) fop.fo.range_min[i] = minn/fop.fo.quant;
			else fop.fo.range_min[i] = minn;
		}
		
	}
	
	fba_object_plane_header();
	code_frame();
	
	size = bitcount();
	buf = outbfr;
	write_bits(fop.stream_fh,buf,size);
	initbits_frame();
}


#define BAP_OUT_FNAME  "bap.out"

void CFBAEncoder::initialize(int argc, char **argv)
{
  int i, bap_nframes, fap_nframes;
  int fap_frame_rate, bap_frame_rate;
  char fap_basename[128], bap_basename[128];

	
	if(argc != 6)
		{
			char tmpmsg[300];
			sprintf(tmpmsg,"Usage: %s input_epf input_fap input_bap out_stream out_fap\n",argv[0]);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-6);
		}
	
	/* XXX should get rid of set_ve_ranges and use initlialized var */
	set_ve_ranges();
	initialize_fop();
	
	if((epf_fp = fopen(argv[1], "r")) == NULL)
	{
		char tmpmsg[300];
		sprintf(tmpmsg,"initialize: cannot open EPF file %s\n", argv[1]);
		#ifdef WIN32
		MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
		#endif
		exit(-7);
	}

	if(argv[2])
	{
		if((fap_fp = fopen(argv[2], "r")) == NULL)
		{
			char tmpmsg[300];
			sprintf(tmpmsg, "initialize: cannot open FAP file %s\n", argv[2]);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-8);
		}
	}
	else
		fap_fp = 0;
		
	if(argv[3])
	{
		if((bap_fp = fopen(argv[3], "r")) == NULL)
		{
			char tmpmsg[300];
			sprintf(tmpmsg, "initialize: cannot open BAP file %s\n", argv[3]);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-108);
		}
	}
	else
		bap_fp = 0;

	fop.fo.suggested_gender = 0;
	fop.fo.coding_type = PREDICTIVE_CODING_TYPE;
	fop.bo.coding_type = PREDICTIVE_CODING_TYPE;
	#ifdef WIN32
		_unlink(argv[4]); //just to make sure it does not exist
		fop.stream_fh = _open( argv[4], _O_WRONLY | _O_CREAT | _O_BINARY, _S_IREAD | _S_IWRITE  );
	#else
		unlink(argv[4]); //just to make sure it does not exist
		fop.stream_fh = open( argv[4], O_WRONLY | O_CREAT, S_IRWXG  );
	#endif
	if (fop.stream_fh == -1)
	{
		char tmpmsg[300];
		sprintf(tmpmsg, "initialize: cannot open Stream file %s\n", argv[4]);
		#ifdef WIN32
		MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
		#endif
		exit(-10);
	}


	fop.fo.fap_out_fp = fop.bo.bap_out_fp = 0;

	if(fap_fp)
	{
		if((fop.fo.fap_out_fp = fopen(argv[5], "w")) == NULL)
		{
			char tmpmsg[300];
			sprintf(tmpmsg, "initialize: cannot open FAP out file %s\n", argv[5]);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-11);
		}
		fprintf(fop.fo.fap_out_fp, "# reconstructed fap file\n");
	}
	if(bap_fp)
	{
		if((fop.bo.bap_out_fp = fopen(BAP_OUT_FNAME, "w")) == NULL)
		{
			char tmpmsg[300];
			sprintf(tmpmsg, "initialize: cannot open BAP out file %s\n", BAP_OUT_FNAME);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-11);
		}
		fprintf(fop.bo.bap_out_fp, "# reconstructed bap file\n");
	}
	
	
	/* version */
	if(fap_fp)
	{
		if(!get_line(fap_fp)) 
		{
			char tmpmsg[300];
			sprintf(tmpmsg, "main: expected line in .fap\n");
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-13);
		}
		fscanf(fap_fp, "%f", &fap_version);
		if(fabsf(fap_version - 2.2f) > 0.01f)
		{
			char tmpmsg[300];
			sprintf(tmpmsg, "WARNING: FAP file version %2.1f != 2.2\n", fap_version);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Warning",MB_ICONERROR);
			#endif
		}
		fprintf(fop.fo.fap_out_fp, "%2.1f ", fap_version);
	}
	
	if(bap_fp)
	{
		if(!get_line(bap_fp)) 
		{
			char tmpmsg[300];
			sprintf(tmpmsg, "main: expected line in .bap\n");
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-113);
		}
		fscanf(bap_fp, "%f", &bap_version);
		if(fabsf(bap_version - 3.1f) > 0.01f && fabsf(bap_version - 3.2f) > 0.01f)
		{
			char tmpmsg[300];
			sprintf(tmpmsg, "WARNING: BAP file version %2.1f is not 3.1 or 3.2\n", bap_version);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Warning",MB_ICONERROR);
			#endif
		}
		fprintf(fop.bo.bap_out_fp, "%2.1f ", bap_version);
	}

	
	/* basename */
	if(fap_fp)
	{
		fscanf(fap_fp, "%s", fap_basename);
		fprintf(fop.fo.fap_out_fp, "%s ", fap_basename);
	}
	if(bap_fp)
	{
		fscanf(bap_fp, "%s", bap_basename);
		fprintf(fop.bo.bap_out_fp, "%s ", bap_basename);
	}
	
	/* frame rate */
	if(fap_fp)
	{
		fscanf(fap_fp, "%d", &fap_frame_rate);
		fprintf(fop.fo.fap_out_fp, "%d ", fap_frame_rate);
	}
	
	if(bap_fp)
	{
		fscanf(bap_fp, "%f", &bap_frame_rate);
		fprintf(fop.bo.bap_out_fp, "%d ", bap_frame_rate);
	}

	
	/* number of frames */
	if(fap_fp)
	{
		fscanf(fap_fp, "%d", &fap_nframes);
		fprintf(fop.fo.fap_out_fp, "%d\n", fap_nframes);
	}
	
	if(bap_fp)
	{
		fscanf(bap_fp, "%d", &bap_nframes);
		fprintf(fop.bo.bap_out_fp, "%d\n", bap_nframes);
	}

  fop.fo.fap_mask = (int*) malloc(NFAP*sizeof(int));
	fop.fo.fap_mask2 = (int*) malloc(NFAP*sizeof(int));
	fop.fo.prev_fap_mask = (int*) malloc(NFAP*sizeof(int));
	for(i=0; i<NFAP; i++) {
		fop.fo.prev_fap_mask[i] = 0;
		fop.fo.freq[i] = fop.fo.cumul_freq[i] = NULL;
		fop.fo.Ifreq[i] = fop.fo.Icumul_freq[i] = NULL;
	}
	for(i=0; i<7; i++) {
		fop.fo.Ifreq_ve[i] = fop.fo.Icumul_ve[i] = NULL;
		fop.fo.freq_ve[i] = fop.fo.cumul_ve[i] = NULL;
	}
	
	for(i=2;i<NFAP;i++) 
	{/*set init minmax values -edp*/
		fop.fo.i_new_max[i] = 0;
		fop.fo.Irange_max[i] = 0;
		
		fop.fo.i_new_min[i] = 0;
		fop.fo.Irange_min[i] = 0;
		
		fop.fo.p_new_max[i] = 0;
		fop.fo.range_max[i] = 0;
		
		fop.fo.p_new_min[i] = 0;
		fop.fo.range_min[i] = 0;
	}
	
	fop.bo.bap_mask = (int*) malloc(NUM_BAPS*sizeof(int));
	fop.bo.bap_mask2 = (int*) malloc(NUM_BAPS*sizeof(int));
	fop.bo.prev_bap_mask = (int*) malloc(NUM_BAPS*sizeof(int));
	for(i=0; i<NUM_BAPS; i++) 
	{
		fop.bo.prev_bap_mask[i] = 0;
		fop.bo.freq[i] = fop.bo.cumul_freq[i] = NULL;
		fop.bo.Ifreq[i] = fop.bo.Icumul_freq[i] = NULL;
	}

	fapMaskWarningGiven = bapMaskWarningGiven = false;
}

void CFBAEncoder::write_bits(int streamHandle, unsigned char *bits, int size)
{
	static unsigned char byte = 0;
	static int bitcnt = 0;
	int i,j;
	int nbytes = size / 8;
	int nrestbits = size % 8;
	int bit;

	if(!streamHandle)
	{
		byte = 0;
		bitcnt = 0;
		return;
	}

	if(nrestbits)
	{
		nbytes++; //take last incomplete byte into account
		bits[nbytes-1] <<= (8-nrestbits); // shift bits in last byte to the edge
	}

	for(i=0;i<nbytes;i++)
	{
		for(j=0;j<8;j++)
		{
			bit = 128 & bits[i];
			bits[i]<<=1;

			byte<<=1;
			if(bit) 
				byte |= 1;
			bitcnt++;

			if(bitcnt==8)
			{
				#ifdef WIN32
					_write(streamHandle,&byte,1);
				#else
					write(streamHandle,&byte,1);
				#endif
				bitcnt=0;
			}

			size--;
			if(size == 0) 
				break;
		}
		if(size == 0) 
			break;
	}
}


void CFBAEncoder::setEncoderParameters()
{
	int i,maxn,minn;
	
	fop.fba_paramset = ep.paramset_mask; // we code both FAPs and BAPs
	
	if(ep.paramset_mask & FAP_PARAMSET_MASK)
	{
		fop.fo.is_frame_rate = true;
		fop.fo.frame_rate = ep.frame_rate;
		fop.fo.fr_seconds = ep.seconds;
		fop.fo.fr_frequency_offset = ep.frequency_offset;
		#ifdef _DEBUG
		assert(fop.fo.frame_rate>=0 && fop.fo.frame_rate<=255);
		assert(fop.fo.fr_seconds>=0 && fop.fo.fr_seconds<=15);
		assert(fop.fo.fr_frequency_offset==0 || fop.fo.fr_frequency_offset==1);
		#endif
		fop.fo.is_fap_quant = true;
		fop.fo.quant = ep.fap_quant;
		#ifdef _DEBUG
		assert(fop.fo.quant>=0 && fop.fo.quant<=31);
		#endif
		if(fop.fo.quant) 
		{
			for(i=2;i<7;i+=2) 
			{
				fop.fo.Irange_ve_max[i] = 63/fop.fo.quant;
				fop.fo.range_ve_max[i] = 63/fop.fo.quant;
				fop.fo.range_ve_min[i] = -63/fop.fo.quant;
			}
		}
		fop.fo.is_i_new_max = true;
		for(i=0; i<7; i++)
			fop.fo.Irange_ve_max[i] = ep.i_max[i];
		for(i=2; i<NFAP; i++) 
		{
			maxn=ep.i_max[i+5];
			fop.fo.i_new_max[i] = (maxn+RANGE_DELTA-1)/RANGE_DELTA;
			maxn = fop.fo.i_new_max[i]*RANGE_DELTA; 
			if(fop.fo.quant) 
			{
				fop.fo.Irange_max[i] = maxn/fop.fo.quant;
			} 
			else 
			{
				fop.fo.Irange_max[i] = maxn;
			}
		}
		fop.fo.is_i_new_min = true;
		for(i=0; i<7; i++)
			fop.fo.Irange_ve_min[i] = ep.i_min[i];
		for(i=2; i<NFAP; i++) 
		{
			minn=ep.i_min[i+5];
			fop.fo.i_new_min[i] = -(minn-RANGE_DELTA+1)/RANGE_DELTA;
			minn = -fop.fo.i_new_min[i]*RANGE_DELTA; 
			if(fop.fo.quant) 
			{
				fop.fo.Irange_min[i] = minn/fop.fo.quant;
			} 
			else 
			{
				fop.fo.Irange_min[i] = minn;
			}
		}
		fop.fo.is_p_new_max = true;
		for(i=0; i<7; i++)
			fop.fo.range_ve_max[i] = ep.p_max[i];
		for(i=2; i<NFAP; i++) 
		{
			maxn=ep.p_max[i+5];
			fop.fo.p_new_max[i] = (maxn+RANGE_DELTA-1)/RANGE_DELTA;
			maxn = fop.fo.p_new_max[i]*RANGE_DELTA; 
			if(fop.fo.quant) 
			{
				fop.fo.range_max[i] = maxn/fop.fo.quant;
			} 
			else 
			{
				fop.fo.range_max[i] = maxn;
			}
		}
		fop.fo.is_p_new_min = true;
		for(i=0; i<7; i++)
			fop.fo.range_ve_min[i] = ep.p_min[i];
		for(i=2; i<NFAP; i++) 
		{
			minn=ep.p_min[i+5];
			fop.fo.p_new_min[i] = -(minn-RANGE_DELTA+1)/RANGE_DELTA;
			minn = -fop.fo.p_new_min[i]*RANGE_DELTA; 
			if(fop.fo.quant) 
			{
				fop.fo.range_min[i] = minn/fop.fo.quant;
			} 
			else 
			{
				fop.fo.range_min[i] = minn;
			}
		}
		
		
	}
	
	
	
	
	
	
	
	
	if(ep.paramset_mask & BAP_PARAMSET_MASK)
	{
		// BAP coding parameters
		
		
		fop.bo.is_frame_rate = true;
		fop.bo.frame_rate = ep.frame_rate;
		fop.bo.fr_seconds = ep.seconds;
		fop.bo.fr_frequency_offset = ep.frequency_offset;
		fop.bo.is_bap_quant = true;
		fop.bo.quant = ep.bap_quant;
		#ifdef _DEBUG
		assert(fop.bo.quant>=0 && fop.bo.quant<=31);
		#endif
		
		fop.bo.is_i_new_max = true;
		for(i=0; i<NUM_BAPS; i++) 
		{
			maxn = ep.bap_i_max[i];
			fop.bo.i_new_max[i] = (maxn+RANGE_DELTA-1)/RANGE_DELTA;
			maxn = fop.bo.i_new_max[i]*RANGE_DELTA; 
			if(fop.bo.quant) 
			{
				fop.bo.Irange_max[i] = maxn/fba_cooondef_bap_pred_scale[fop.bo.quant];
			} 
			else 
			{
				fop.bo.Irange_max[i] = maxn;
			}
		}
		fop.bo.is_i_new_min = true;
		for(i=0; i<NUM_BAPS; i++) 
		{
			
			minn = ep.bap_i_min[i];
			fop.bo.i_new_min[i] = -(minn-RANGE_DELTA+1)/RANGE_DELTA;
			minn = -fop.bo.i_new_min[i]*RANGE_DELTA; 
			if(fop.bo.quant) 
			{
				fop.bo.Irange_min[i] = minn/fba_cooondef_bap_pred_scale[fop.bo.quant];
			} 
			else 
			{
				fop.bo.Irange_min[i] = minn;
			}
		}
		fop.bo.is_p_new_max = true;
		for(i=0; i<NUM_BAPS; i++) 
		{
			
			maxn = ep.bap_p_max[i];
			fop.bo.p_new_max[i] = (maxn+RANGE_DELTA-1)/RANGE_DELTA;
			maxn = fop.bo.p_new_max[i]*RANGE_DELTA; 
			if(fop.bo.quant) 
			{
				fop.bo.range_max[i] = maxn/fba_cooondef_bap_pred_scale[fop.bo.quant];
			} 
			else 
			{
				fop.bo.range_max[i] = maxn;
			}
		}
		fop.bo.is_p_new_min = true;
		for(i=0; i<NUM_BAPS; i++) 
		{
			minn = ep.bap_p_min[i];
			fop.bo.p_new_min[i] = -(minn-RANGE_DELTA+1)/RANGE_DELTA;
			minn = -fop.bo.p_new_min[i]*RANGE_DELTA; 
			if(fop.bo.quant) 
			{
				fop.bo.range_min[i] = minn/fba_cooondef_bap_pred_scale[fop.bo.quant];
			} 
			else 
			{
				fop.bo.range_min[i] = minn;
			}
		}
	}
}


void CFBAEncoder::initializeEncoding()
{
	int i;

	
	/* XXX should get rid of set_ve_ranges and use initlialized var */
	set_ve_ranges();
	initialize_fop();

	if (produceOutFapFile) 
	{

		if((fop.fo.fap_out_fp = fopen(fap_out_file, "w")) == NULL) // FAP Text File
		{
			char tmpmsg[300];
			sprintf(tmpmsg, "initialize: cannot open FAP out file %s\n", fap_out_file);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-16);
		}

		
	
		/* Read Header Information */
		fprintf(fop.fo.fap_out_fp, "# reconstructed fap file\n");
	
		/* version */
		fprintf(fop.fo.fap_out_fp, "%2.1f ", 2.1f);
		
		fprintf(fop.fo.fap_out_fp, "%s ", fap_out_file);
		/* frame rate */
		fprintf(fop.fo.fap_out_fp, "%d ", ep.frame_rate);
		
		/* number of frames */
		fprintf(fop.fo.fap_out_fp, "%d\n", 10000);
	}

	
	fop.fo.suggested_gender = 0;
	fop.fo.coding_type = PREDICTIVE_CODING_TYPE;
	fop.bo.coding_type = PREDICTIVE_CODING_TYPE;
	fop.fo.fap_out_fp = fop.bo.bap_out_fp = 0;
	
	fop.fo.fap_mask = (int*) malloc(NFAP*sizeof(int));
	fop.fo.fap_mask2 = (int*) malloc(NFAP*sizeof(int));
	fop.fo.prev_fap_mask = (int*) malloc(NFAP*sizeof(int));
	for(i=0; i<NFAP; i++) 
	{
		fop.fo.prev_fap_mask[i] = 0;
		fop.fo.freq[i] = fop.fo.cumul_freq[i] = NULL;
		fop.fo.Ifreq[i] = fop.fo.Icumul_freq[i] = NULL;
	}
	for(i=0; i<7; i++) 
	{
		fop.fo.Ifreq_ve[i] = fop.fo.Icumul_ve[i] = NULL;
		fop.fo.freq_ve[i] = fop.fo.cumul_ve[i] = NULL;
	}
	
	for(i=2;i<NFAP;i++) 
	{/*set init minmax values -edp*/
		fop.fo.i_new_max[i] = 0;
		fop.fo.Irange_max[i] = 0;
		
		fop.fo.i_new_min[i] = 0;
		fop.fo.Irange_min[i] = 0;
		
		fop.fo.p_new_max[i] = 0;
		fop.fo.range_max[i] = 0;
		
		fop.fo.p_new_min[i] = 0;
		fop.fo.range_min[i] = 0;
	}

	if (produceOutBapFile) 
	{

		if((fop.bo.bap_out_fp = fopen(bap_out_file, "w")) == NULL) // BAP Text File
		{
			char tmpmsg[300];
			sprintf(tmpmsg, "initialize: cannot open BAP out file %s\n", bap_out_file);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-116);
		}

		
	
		/* Header Information */
		fprintf(fop.bo.bap_out_fp, "# reconstructed bap file\n");
	
		/* version */
		fprintf(fop.bo.bap_out_fp, "%2.1f ", 3.2f);
		
		fprintf(fop.bo.bap_out_fp, "%s ", bap_out_file);
		/* frame rate */
		fprintf(fop.bo.bap_out_fp, "%d ", ep.frame_rate);
		
		/* number of frames */
		fprintf(fop.bo.bap_out_fp, "%d\n", 10000);
	}

	fop.bo.bap_mask = (int*) malloc(NUM_BAPS*sizeof(int));
	fop.bo.bap_mask2 = (int*) malloc(NUM_BAPS*sizeof(int));
	fop.bo.prev_bap_mask = (int*) malloc(NUM_BAPS*sizeof(int));
	for(i=0; i<NUM_BAPS; i++) 
	{
		fop.bo.prev_bap_mask[i] = 0;
		fop.bo.freq[i] = fop.bo.cumul_freq[i] = NULL;
		fop.bo.Ifreq[i] = fop.bo.Icumul_freq[i] = NULL;
	}

/*	

	
	fop.fo.fap_mask = new int[NFAP];
	fop.fo.fap_mask2 = new int[NFAP];
	fop.fo.prev_fap_mask = new int[NFAP];
	for(i=0; i<NFAP; i++) {
		fop.fo.prev_fap_mask[i] = 0;
		fop.fo.freq[i] = fop.fo.cumul_freq[i] = NULL;
		fop.fo.Ifreq[i] = fop.fo.Icumul_freq[i] = NULL;
	}
	for(i=0; i<7; i++) {
		fop.fo.Ifreq_ve[i] = fop.fo.Icumul_ve[i] = NULL;
		fop.fo.freq_ve[i] = fop.fo.cumul_ve[i] = NULL;
	}
*/	

	initbits();

	/* fba_sequence() function is embedded here */
	head_cnt += putbits(32, FBA_OBJECT_START_CODE);
		
	setEncoderParameters();

	// Changed by Goran Mrzljak
	// before: frame_time = 1000 / fop.fo.frame_rate;
	// if face animation isn't used (body animation only), then fop.fo.frame_rate is not initilized!
  frame_time = 1000 / ep.frame_rate;
  fop.is_intra = 1;
  fop.fo.frame_num=0;
  fop.bo.frame_num=0;
}

void CFBAEncoder::assure_fap_frame(void)
{
	if(fop.fba_paramset & FAP_PARAMSET_MASK)
		return;
	else
	{
		char tmpmsg[300];
		sprintf(tmpmsg, "got a FAP tag, unexpected\n");
		#ifdef WIN32
		MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
	#endif
		exit(-18);
	}
}

void CFBAEncoder::assure_bap_frame(void)
{
	if(fop.fba_paramset & BAP_PARAMSET_MASK)
		return;
	else
	{
		char tmpmsg[300];
		sprintf(tmpmsg, "got a BAP tag, unexpected\n");
		#ifdef WIN32
		MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
		#endif
		exit(-118);
	}
}

/* XXX this should be changed back... */
void CFBAEncoder::set_ve_ranges(void)
{
	int i;
	
	fop.fo.Irange_ve_max[0] = 14;
	fop.fo.Irange_ve_max[1] = 14;
	fop.fo.Irange_ve_max[2] = 63;
	fop.fo.Irange_ve_max[3] = 6;
	fop.fo.Irange_ve_max[4] = 63;
	fop.fo.Irange_ve_max[5] = 6;
	fop.fo.Irange_ve_max[6] = 63;
	
	i = 7;
	while(i--)
		fop.fo.Irange_ve_min[i] = 0;
	
	fop.fo.range_ve_max[0] = 14;
	fop.fo.range_ve_max[1] = 14;
	fop.fo.range_ve_max[2] = 63;
	fop.fo.range_ve_max[3] = 6;
	fop.fo.range_ve_max[4] = 63;
	fop.fo.range_ve_max[5] = 6;
	fop.fo.range_ve_max[6] = 63;

	i = 7;
	while(i--)
		fop.fo.range_ve_min[i] = -fop.fo.range_ve_max[i];
}
void CFBAEncoder::initialize_fop(void)
{
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
  int i,j;

	reset_frame();
	
    fop.is_startcode = 0;/*-edp*/
    fop.fba_paramset = 1;
	fop.fo.quant = 1;
	fop.is_intra = true;
	fop.marker_bit = true;
	fop.fo.is_time_code = false;
	fop.fo.time_code_hours = 0;
	fop.fo.time_code_minutes = 0;
	fop.fo.time_code_seconds = 0;
	
	fop.bo.quant = 1;
	fop.bo.is_time_code = false;
	fop.bo.time_code_hours = 0;
	fop.bo.time_code_minutes = 0;
	fop.bo.time_code_seconds = 0;

	/* Following replaces 
	int mask_index[] = {0, 2, 18, 30, 38, 42, 47, 50, 60, 64, 68}; */
	fop.fo.mask_index[0] = 0;
	fop.fo.mask_index[1] = 2;
	fop.fo.mask_index[2] = 18;
	fop.fo.mask_index[3] = 30;
	fop.fo.mask_index[4] = 38;
	fop.fo.mask_index[5] = 42;
	fop.fo.mask_index[6] = 47;
	fop.fo.mask_index[7] = 50;
	fop.fo.mask_index[8] = 60;
	fop.fo.mask_index[9] = 64;
	fop.fo.mask_index[10] = 68;
	
	for (i=0; i<BAP_NUM_GROUPS; i++) {
		fop.bo.baps_in_group[i] = baps_in_group[i];
		for (j=0; j<baps_in_group[i]; j++) {
			fop.bo.bap_mask_index[i][j] = bap_mask_index[i][j]-1;
			fop.bo.minmaxset[fop.bo.bap_mask_index[i][j]] = 0;/*edp*/
		}
		
	}
	for (i=2; i<NFAP;i++) {
		fop.fo.minmaxset[i] = 0;/*edp*/
	}
	fop.fo.havesetminmax = 0;
	fop.bo.havesetminmax = 0;
}

void CFBAEncoder::reset_frame(void)
{
	fop.is_startcode = 0;/*edp*/
	fop.fo.is_frame_rate = false;
	fop.fo.skip_frames = false;
	
	fop.fo.new_mask_flag = false;
	fop.fo.is_i_new_max = false;
	fop.fo.is_i_new_min = false;
	fop.fo.is_p_new_max = false;
	fop.fo.is_p_new_min = false;
	fop.fo.is_fap_quant = false;

	fop.bo.is_frame_rate = false;
	fop.bo.skip_frames = false;
	
	fop.bo.new_mask_flag = false;
	fop.bo.is_i_new_max = false;
	fop.bo.is_i_new_min = false;
	fop.bo.is_p_new_max = false;
	fop.bo.is_p_new_min = false;
	fop.bo.is_bap_quant = false;
}

/* return false on EOF */
bool CFBAEncoder::get_line(FILE *ifp) 
{
	int firstChar; /* XXX must be int, not char */
	char buf[BUFSIZ];
	
	while(feof(ifp) == 0) {
		if ((firstChar = fgetc(ifp)) == EOF) return false;
		if (firstChar == '#') { /* skip comments */
			if(!fgets(buf, BUFSIZ, ifp))
				return false;
		} else if(firstChar == '\t' || firstChar == ' ' ||
			firstChar == '\n') { /* skip tracing empty space */
			;
		} else {
			ungetc (firstChar, ifp);
			if(feof(ifp)) return false;
			return true;
		}
	}
	return false;
}
void CFBAEncoder::initbits()
{
  outcnt = 8; /*  stream level bit counter */
  outcnt_frame = 8; /* frame level bit counter, reset for each frame */
  bytecnt = 0;
}


int CFBAEncoder::putbits(int n, int val)
{
  int i;
  unsigned int mask;

  mask = 1 << (n-1); /* selects first (leftmost) bit */

  for (i=0; i<n; i++) {
    outbfr[bytecnt] <<= 1;

    if (val & mask)
      outbfr[bytecnt]|= 1;

    mask >>= 1; /* select next bit */
    outcnt--;
    outcnt_frame--;

    if (outcnt_frame==0) /* 8 bit buffer full */ {
      outcnt_frame = 8;
      bytecnt++;
	  if(bytecnt >= ENCODER_BUFFER_SIZE)
	  {
		  char tmpmsg[300];
		  sprintf(tmpmsg,"Encoder buffer size exceeded (%d > %d)\n",bytecnt,ENCODER_BUFFER_SIZE);
		  #ifdef WIN32
		  MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
		#endif
		  exit(-126);
	  }

    }

   if (outcnt==0) /* 8 bit buffer full */ {
      outcnt = 8;
     }
  }
  return n;
}

int CFBAEncoder::encodeFrame( FBAPs *fbap,
                    unsigned char *outBuffer, int buffer_size_bytes,long *pts)
{
	int i,j,bit_size,byte_size;
	int *msk;

	if(!fbap)
		return 0;
	
	if(fop.fo.frame_num)
		fop.is_intra = 0;

	FAPs *fap = fbap->faps;
	BAPs *bap = fbap->baps;

	if(ep.paramset_mask & FAP_PARAMSET_MASK)
	{
		msk = fap->groupMask;
		for(i=0; i<N_FAP_GROUP; i++)
		{
			fop.fo.fap_mask_type[i]=fap->maskType[i];
			#ifdef _DEBUG
			assert(fop.fo.fap_mask_type[i]>=0 && fop.fo.fap_mask_type[i]<=3);
			#endif
			if(fop.fo.fap_mask_type[i]==1 || fop.fo.fap_mask_type[i]==2) 
			{
				fop.fo.new_mask_flag = true;
				for(j=fop.fo.mask_index[i]; j<fop.fo.mask_index[i+1]; j++) 
				{
					//				fop.fo.fap_mask[j]= msk[0];
					//				msk++;
					fop.fo.fap_mask[j]= msk[j];
					#ifdef _DEBUG
					assert(fop.fo.fap_mask[j]==0 || fop.fo.fap_mask[j]==1);
					#endif
				}
			} 
			else 
				if(fop.fo.fap_mask_type[i]==0) 
				{
					for(j=fop.fo.mask_index[i]; j<fop.fo.mask_index[i+1]; j++) 
					{
						fop.fo.fap_mask[j] = 0;
					}
				} 
				else
				{
					fop.fo.new_mask_flag = true;
					for(j=fop.fo.mask_index[i]; j<fop.fo.mask_index[i+1]; j++) 
					{
						fop.fo.fap_mask[j] = 1;
					}
				}
		}
		
		// if mask changed, we have to code an intra frame
		for(i=0; i<NFAP; i++) 
		{
			if(fop.fo.fap_mask[i] != fop.fo.prev_fap_mask[i])
			{
				fop.is_intra = 1;
			}
		}
		
		// if coding parameters changed, we have to set them and code an intra frame
		if(!ep.isEqual(&ep_prev))
		{
			ep_prev.copy(&ep);
			fop.is_intra = 1;
		}
		
	}

	if(ep.paramset_mask & BAP_PARAMSET_MASK)
	{
		// BAPs
		msk = bap->groupMask;
		int temp_index;
		
		for(i=0; i<BAP_NUM_GROUPS; i++)
		{
			fop.bo.bap_mask_type[i]=bap->maskType[i];
			#ifdef _DEBUG
			assert(fop.bo.bap_mask_type[i]>=0 && fop.bo.bap_mask_type[i]<=3);
			#endif
			if(fop.bo.bap_mask_type[i]==1) 
			{
				fop.bo.new_mask_flag = true;
				for(j=0;j<fop.bo.baps_in_group[i]; j++) 
				{
					temp_index = fop.bo.bap_mask_index[i][j];
					fop.bo.bap_mask[temp_index]= msk[temp_index];
					#ifdef _DEBUG
					assert(fop.bo.bap_mask[temp_index]==0 || fop.bo.bap_mask[temp_index]==1);
					#endif
				}
			} 
			else 
				if(fop.bo.bap_mask_type[i]==0) 
				{
					for(j=0;j<fop.bo.baps_in_group[i]; j++) 
					{
						temp_index = fop.bo.bap_mask_index[i][j];
						fop.bo.bap_mask[temp_index] = 0;
					}
				} 
				else
					if(fop.bo.bap_mask_type[i]==3)
					{
						fop.bo.new_mask_flag = true;
						for(j=0;j<fop.bo.baps_in_group[i]; j++) 
						{
							temp_index = fop.bo.bap_mask_index[i][j];
							fop.bo.bap_mask[temp_index] = 1;
						}
					}
					else
					{
						char tmpmsg[300];
						sprintf(tmpmsg,"Encoder parameters contain wrong BAP mask type for group %d\n", i);
						#ifdef WIN32
						MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
						#endif
						exit(-102);
					}
		}
		
		// if mask changed, we have to code an intra frame
		for(i=0; i<NUM_BAPS; i++) 
		{
			if(fop.bo.bap_mask[i] != fop.bo.prev_bap_mask[i])
			{
				fop.is_intra = 1;
			}
		}
	}
	
	// if intra frame
	
	if(fop.is_intra) 
	{
		setEncoderParameters();
		init_arith();
		bap_init_arith();
	}
	
	if(ep.paramset_mask & FAP_PARAMSET_MASK)
	{
		// set FAP values	
		if(fop.fo.fap_mask[0]) 
		{
			fop.fo.ve_orig[0] = fap->fap1.viseme1;
			fop.fo.ve_orig[1] = fap->fap1.viseme2;
			fop.fo.ve_orig[2] = fap->fap1.blend;
			fop.fo.viseme_def = fap->fap1.defBit;
		}
		if(fop.fo.fap_mask[1]) {
			fop.fo.ve_orig[3] = fap->fap2.expression1;
			fop.fo.ve_orig[4] = fap->fap2.expression2;
			fop.fo.ve_orig[5]  = fap->fap2.intensity1;
			fop.fo.ve_orig[6] = fap->fap2.intensity2;
			fop.fo.init_face  = fap->fap2.initBit;
			fop.fo.expression_def = fap->fap2.defBit;
		}
		for(i=2; i<NFAP; i++) 
		{
			if(fop.fo.fap_mask[i]) 
				fop.fo.fap_orig[i] = fap->llf.value[i];
		}
		
		if (produceOutFapFile) 
		{
			for(i=0; i<NFAP; i++) 
			{
				fprintf(fop.fo.fap_out_fp, "%d ", fop.fo.fap_mask[i]);
			}
			fprintf(fop.fo.fap_out_fp, "\n");
			fprintf(fop.fo.fap_out_fp, "%d ", fop.fo.frame_num);
		}
	}
	
	if(ep.paramset_mask & BAP_PARAMSET_MASK)
	{
		// set BAP values	
		for(i=0; i<NUM_BAPS; i++) 
		{
			if(fop.bo.bap_mask[i]) 
				fop.bo.bap_orig[i] = bap->value[i];
		}
		
		if (produceOutBapFile) 
		{
			for(i=0; i<NUM_BAPS; i++) 
			{
				fprintf(fop.bo.bap_out_fp, "%d ", fop.bo.bap_mask[i]);
			}
			fprintf(fop.bo.bap_out_fp, "\n");
			fprintf(fop.bo.bap_out_fp, "%d ", fop.bo.frame_num);
		}
	}

	fba_object_plane_header();

	fop.cur_frame_bit_cnt = 0;

	fba_object_plane_data();
	
	if(fop.is_intra) 
	{
		ibit_cnt += fop.cur_frame_bit_cnt;
		if(ep.paramset_mask & FAP_PARAMSET_MASK)
		{
			for(i=0; i<NFAP; i++) 
			{
				fop.fo.prev_fap_mask[i] = fop.fo.fap_mask[i];
			}
		}
		if(ep.paramset_mask & BAP_PARAMSET_MASK)
		{
			for (i=0; i<NUM_BAPS; i++) 
			{
				fop.bo.prev_bap_mask[i] = fop.bo.bap_mask[i];
			}
		}
	} 
	else 
		pbit_cnt += fop.cur_frame_bit_cnt;
	
	fop.fo.frame_num++;
	fop.bo.frame_num++;

	(*pts) = fop.fo.frame_num * frame_time;
	reset_frame();			
    
	bit_size = bitcount();
	byte_size = bit_size/8 + 1;
	for(i=0;i<byte_size;i++)
	{
		if(i >= buffer_size_bytes)
		{
			char tmpmsg[300];
			sprintf(tmpmsg,"Encoding buffer size exceeded (%d >= %d). Increase the size of the buffer in the call to CFBAEncoder::encodeFrame()\n",i,buffer_size_bytes);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-127);
		}
		outBuffer[i] = outbfr[i];
	}
	initbits_frame();
	return (bit_size);
}

/* return total number of generated bits */
int CFBAEncoder::bitcount()
{
  return 8*bytecnt + (8-outcnt_frame);
}

void CFBAEncoder::initbits_frame()
{
  outcnt_frame = 8; /* frame level bit counter, reset for each frame */
  bytecnt = 0;
}

int CFBAEncoder::expand_mask() 
{
	int i;
	for(i=0; i<NFAP; i++) 
		if(fop.fo.fap_mask[i]==1 && fop.fo.prev_fap_mask[i]==0)
			return 1;
	return 0;
}

void  CFBAEncoder::init_arith() {

  int i, j, n;

  if(fop.fo.fap_mask[0]) {
	delete fop.fo.Ifreq_ve[0];
	delete fop.fo.Icumul_ve[0];
	delete fop.fo.Ifreq_ve[1];
	delete fop.fo.Icumul_ve[1];
	delete fop.fo.Ifreq_ve[2];
	delete fop.fo.Icumul_ve[2];

	n = fop.fo.Irange_ve_max[0] - fop.fo.Irange_ve_min[0] + 1;
	fop.fo.Ifreq_ve[0] = new int[n];
	fop.fo.Icumul_ve[0] = new int[n+1];


	for(j=0; j<n; j++) {
		fop.fo.Ifreq_ve[0][j] = 1;
		fop.fo.Icumul_ve[0][j] = n-j;
	}
	fop.fo.Icumul_ve[0][n] = 0;

	n = fop.fo.Irange_ve_max[1] - fop.fo.Irange_ve_min[1] + 1;
	fop.fo.Ifreq_ve[1] = new int[n];
	fop.fo.Icumul_ve[1] = new int[n+1];

	for(j=0; j<n; j++) {
		fop.fo.Ifreq_ve[1][j] = 1;
		fop.fo.Icumul_ve[1][j] = n-j;
	}
	fop.fo.Icumul_ve[1][n] = 0;

	n = fop.fo.Irange_ve_max[2] - fop.fo.Irange_ve_min[2] + 1;
	fop.fo.Ifreq_ve[2] = new int[n];
	fop.fo.Icumul_ve[2] = new int[n+1];

	for(j=0; j<n; j++) {
		fop.fo.Ifreq_ve[2][j] = 1;
		fop.fo.Icumul_ve[2][j] = n-j;
	}
	fop.fo.Icumul_ve[2][n] = 0;
  }

  if(fop.fo.fap_mask[1]) {
	  delete fop.fo.Ifreq_ve[3];
	  delete fop.fo.Icumul_ve[3];
	  delete fop.fo.Ifreq_ve[4];
	  delete fop.fo.Icumul_ve[4];
	  delete fop.fo.Ifreq_ve[5];
	  delete fop.fo.Icumul_ve[5];
	  delete fop.fo.Ifreq_ve[6];
	  delete fop.fo.Icumul_ve[6];

      n = fop.fo.Irange_ve_max[3] - fop.fo.Irange_ve_min[3] + 1;
	  fop.fo.Ifreq_ve[3] = new int[n];
	  fop.fo.Icumul_ve[3] = new int[n+1];
        for(j=0; j<n; j++) {
                fop.fo.Ifreq_ve[3][j] = 1;
                fop.fo.Icumul_ve[3][j] = n-j;
        }
        fop.fo.Icumul_ve[3][n] = 0;
 
	  n = fop.fo.Irange_ve_max[4] - fop.fo.Irange_ve_min[4] + 1;
	  fop.fo.Ifreq_ve[4] = new int[n];
	  fop.fo.Icumul_ve[4] = new int[n+1];
	  for(j=0; j<n; j++) {
		fop.fo.Ifreq_ve[4][j] = 1;
		fop.fo.Icumul_ve[4][j] = n-j;
	  }
	fop.fo.Icumul_ve[4][n] = 0;

        n = fop.fo.Irange_ve_max[5] - fop.fo.Irange_ve_min[5] + 1;
	  fop.fo.Ifreq_ve[5] = new int[n];
	  fop.fo.Icumul_ve[5] = new int[n+1];
        
        for(j=0; j<n; j++) {
                fop.fo.Ifreq_ve[5][j] = 1;
                fop.fo.Icumul_ve[5][j] = n-j;
        }
        fop.fo.Icumul_ve[5][n] = 0;
 
	n = fop.fo.Irange_ve_max[6] - fop.fo.Irange_ve_min[6] + 1;
	  fop.fo.Ifreq_ve[6] = new int[n];
	  fop.fo.Icumul_ve[6] = new int[n+1];
  
	for(j=0; j<n; j++) {
		fop.fo.Ifreq_ve[6][j] = 1;
		fop.fo.Icumul_ve[6][j] = n-j;
	}
	fop.fo.Icumul_ve[6][n] = 0;
  }

  for(i=2; i<NFAP; i++) {
	if(fop.fo.fap_mask[i]) {
		delete fop.fo.Ifreq[i];
		delete fop.fo.Icumul_freq[i];

		n = fop.fo.Irange_max[i]-fop.fo.Irange_min[i]+1;
		fop.fo.Ifreq[i] = new int[n];
		fop.fo.Icumul_freq[i] = new int[n+1];
		for(j=0; j<n; j++) {
			fop.fo.Ifreq[i][j] = 1;
			fop.fo.Icumul_freq[i][j] = n-j;
		}
		fop.fo.Icumul_freq[i][n] = 0;
	}
  }
  if(fop.fo.fap_mask[0]) {
		delete fop.fo.freq_ve[0];
		delete fop.fo.cumul_ve[0];
		delete fop.fo.freq_ve[1];
		delete fop.fo.cumul_ve[1];
		delete fop.fo.freq_ve[2];
		delete fop.fo.cumul_ve[2];
        n = fop.fo.range_ve_max[0] - fop.fo.range_ve_min[0] + 1;
		fop.fo.freq_ve[0] = new int[n];
        fop.fo.cumul_ve[0] = new int[n+1];
		  for(j=0; j<n; j++) {
                fop.fo.freq_ve[0][j] = 1;
                fop.fo.cumul_ve[0][j] = n-j;
        }
        fop.fo.cumul_ve[0][n] = 0;
 
        n = fop.fo.range_ve_max[1] - fop.fo.range_ve_min[1] + 1;
		fop.fo.freq_ve[1] = new int[n];
		fop.fo.cumul_ve[1] = new int[n+1];
        for(j=0; j<n; j++) {
                fop.fo.freq_ve[1][j] = 1;
                fop.fo.cumul_ve[1][j] = n-j;
        }
        fop.fo.cumul_ve[1][n] = 0;
 
	n = fop.fo.range_ve_max[2] - fop.fo.range_ve_min[2] + 1;
	fop.fo.freq_ve[2] = new int[n];
	fop.fo.cumul_ve[2] = new int[n+1];
	for(j=0; j<n; j++) {
		fop.fo.freq_ve[2][j] = 1;
		fop.fo.cumul_ve[2][j] = n-j;
	}
	fop.fo.cumul_ve[2][n] = 0;
  }

  if(fop.fo.fap_mask[1]) {
		delete fop.fo.freq_ve[3];
		delete fop.fo.cumul_ve[3];
		delete fop.fo.freq_ve[4];
		delete fop.fo.cumul_ve[4];
		delete fop.fo.freq_ve[5];
		delete fop.fo.cumul_ve[5];
		delete fop.fo.freq_ve[6];
		delete fop.fo.cumul_ve[6];
        n = fop.fo.range_ve_max[3] - fop.fo.range_ve_min[3] + 1;
		fop.fo.freq_ve[3] = new int[n];
        fop.fo.cumul_ve[3] = new int[n+1];
        for(j=0; j<n; j++) {
                fop.fo.freq_ve[3][j] = 1;
                fop.fo.cumul_ve[3][j] = n-j;
        }
        fop.fo.cumul_ve[3][n] = 0;
 
	n = fop.fo.range_ve_max[4] - fop.fo.range_ve_min[4] + 1;
	fop.fo.freq_ve[4] = new int[n];
	fop.fo.cumul_ve[4] = new int[n+1];
	for(j=0; j<n; j++) {
		fop.fo.freq_ve[4][j] = 1;
		fop.fo.cumul_ve[4][j] = n-j;
	}
	fop.fo.cumul_ve[4][n] = 0;

        n = fop.fo.range_ve_max[5] - fop.fo.range_ve_min[5] + 1;
		fop.fo.freq_ve[5] = new int[n];
        fop.fo.cumul_ve[5] = new int[n+1];
        for(j=0; j<n; j++) {
                fop.fo.freq_ve[5][j] = 1;
                fop.fo.cumul_ve[5][j] = n-j;
        }
        fop.fo.cumul_ve[5][n] = 0;

	n = fop.fo.range_ve_max[6] - fop.fo.range_ve_min[6] + 1;
	fop.fo.freq_ve[6] = new int[n];
	fop.fo.cumul_ve[6] = new int[n+1];
	for(j=0; j<n; j++) {
		fop.fo.freq_ve[6][j] = 1;
		fop.fo.cumul_ve[6][j] = n-j;
	}
	fop.fo.cumul_ve[6][n] = 0;
  }

  for(i=2; i<NFAP; i++) {
	if(fop.fo.fap_mask[i]) {
		delete fop.fo.freq[i];
		delete fop.fo.cumul_freq[i];
		n = fop.fo.range_max[i]-fop.fo.range_min[i]+1;
		fop.fo.freq[i] = new int[n];
		fop.fo.cumul_freq[i] = new int[n+1];
		for(j=0; j<n; j++) {
			fop.fo.freq[i][j] = 1;
			fop.fo.cumul_freq[i][j] = n-j;
		}
		fop.fo.cumul_freq[i][n] = 0;
	}
  }
}

int CFBAEncoder::next_start_code()
{
  int mask;
  int ret;
  
  if (outcnt==1) {
    return(putbits(1, 0));
  } else if(outcnt==8) {
    return(putbits(8, 127));
  } else {
    mask = (1<<(outcnt-1))-1;
    ret = outcnt;
    return(putbits(outcnt, mask));
  }
}

int CFBAEncoder::finalizeBitstream(unsigned char *outBuffer, int buffer_size_bytes,long *pts)
{
	int i;
	
	head_cnt += next_start_code();
	head_cnt += putbits(32, FBA_OBJECT_END_CODE);
	for(i=0; i<NFAP; i++) {
		if(fop.fo.Ifreq[i]) delete fop.fo.Ifreq[i];
		if(fop.fo.Icumul_freq[i]) delete fop.fo.Icumul_freq[i];
		if(fop.fo.freq[i]) delete fop.fo.freq[i];
		if(fop.fo.cumul_freq[i]) delete fop.fo.cumul_freq[i];
	}
	if(fop.fo.fap_mask) {
        free(fop.fo.fap_mask);
        fop.fo.fap_mask = NULL;
    }
    if(fop.fo.fap_mask2) {
        free(fop.fo.fap_mask2);
        fop.fo.fap_mask2 = NULL;
    }
    if(fop.fo.prev_fap_mask) {
        free(fop.fo.prev_fap_mask);
        fop.fo.prev_fap_mask = NULL;
    }
    if (produceOutFapFile) {
        fclose(fop.fo.fap_out_fp);
	}
    if (produceOutBapFile) {
        fclose(fop.bo.bap_out_fp);
    }
        
        
	int bit_size = bitcount();
	int byte_size = bit_size/8 + 1;
	for(i=0;i<byte_size;i++)
	{
		if(i >= buffer_size_bytes)
		{
			char tmpmsg[300];
			sprintf(tmpmsg,"finalizeBitstream: Encoding buffer size exceeded (%d >= %d). Increase the size of the buffer in the call to CFBAEncoder::encodeFrame()\n",i,buffer_size_bytes);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-128);
		}
		outBuffer[i] = outbfr[i];
	}
	initbits_frame();
	return (bit_size);
}



void CFBAEncoder::code_frame_rate()
{
  fop.cur_frame_bit_cnt += putbits(8, fop.fo.frame_rate);
  fop.cur_frame_bit_cnt += putbits(4, fop.fo.fr_seconds);
  fop.cur_frame_bit_cnt += putbits(1, fop.fo.fr_frequency_offset);
}


void CFBAEncoder::code_time_code()
{
  fop.cur_frame_bit_cnt += putbits(5, fop.fo.time_code_hours);
  fop.cur_frame_bit_cnt += putbits(6, fop.fo.time_code_minutes);
  fop.cur_frame_bit_cnt += putbits(1, fop.marker_bit);
  fop.cur_frame_bit_cnt += putbits(6, fop.fo.time_code_seconds);
}


void CFBAEncoder::code_skip_frames() 
{
  int ivalue;

  do {
    if(fop.fo.number_of_frames_to_skip<15) {
      ivalue = fop.fo.number_of_frames_to_skip;
    } else {
      ivalue = 15;
      fop.fo.number_of_frames_to_skip -= 15;
    }
    fop.cur_frame_bit_cnt += putbits(4, ivalue);
  } while (ivalue==15);
}


void CFBAEncoder::code_fap_group_mask(int n)
{
  int i;
  for(i=fop.fo.mask_index[n]; i<fop.fo.mask_index[n+1]; i++) 
    fop.cur_frame_bit_cnt += putbits(1, fop.fo.fap_mask[i]);
}


void CFBAEncoder::code_new_minmax()
{
  int i, j;

  if(fop.fo.is_i_new_max) {
    for(i=2, j=0; i<NFAP; i++, j++) {
      if(i==18 || i==30 || i==38 || i== 42 ||
	 i==47 || i==50 || i==60 || i== 64) {
	j=0;
      }
      if(!(j&3)) 
	fop.cur_frame_bit_cnt += putbits(1, MARKER_BIT);
      
      if(fop.fo.fap_mask[i]) 
	fop.cur_frame_bit_cnt += putbits(5, fop.fo.i_new_max[i]);
    }
  }
  if(fop.fo.is_i_new_min) {
    for(i=2, j=0; i<NFAP; i++, j++) {
      if(i==18 || i==30 || i==38 || i== 42 ||
	 i==47 || i==50 || i==60 || i== 64) {
	j=0;
      }
      if(!(j&3)) 
	fop.cur_frame_bit_cnt += putbits(1, MARKER_BIT);
      
      if(fop.fo.fap_mask[i]) 
	fop.cur_frame_bit_cnt += putbits(5, fop.fo.i_new_min[i]);
    }
  }
  if(fop.fo.is_p_new_max) {
    for(i=2, j=0; i<NFAP; i++, j++) {
      if(i==18 || i==30 || i==38 || i== 42 ||
	 i==47 || i==50 || i==60 || i== 64) {
	j=0;
      }
      if(!(j&3)) {
	fop.cur_frame_bit_cnt += putbits(1, MARKER_BIT);
      }
      if(fop.fo.fap_mask[i]) {
	fop.cur_frame_bit_cnt += putbits(5, fop.fo.p_new_max[i]);
      }
    }
  }
  if(fop.fo.is_p_new_min) {
    for(i=2, j=0; i<NFAP; i++, j++) {
      if(i==18 || i==30 || i==38 || i== 42 ||
	 i==47 || i==50 || i==60 || i== 64) {
	j=0;
      }
      if(!(j&3))
	fop.cur_frame_bit_cnt += putbits(1, MARKER_BIT);
      
      if(fop.fo.fap_mask[i])
	fop.cur_frame_bit_cnt += putbits(5, fop.fo.p_new_min[i]);
    }
  }
}

void  CFBAEncoder::code_ifap()
{
  int i, qstep;
  int symbol;
  int nbits;

  if(fop.fo.fap_mask[0]) 
    code_viseme(1);
  
  if(fop.fo.fap_mask[1]) 
    code_expression(1);

  for(i=2; i<NFAP; i++) {
    if(fop.fo.fap_mask[i]) { /* code the ith fap */
      qstep = fba_cooondef_IQP[i]*fop.fo.quant;
      fop.fo.fap_Q[i] = quant(fop.fo.fap_orig[i], qstep);
      symbol = fop.fo.fap_Q[i] - fop.fo.Irange_min[i];
      check_range(symbol, i, 0);
      nbits = AR_Encode(symbol, fop.fo.Icumul_freq[i]);
      fop.cur_frame_bit_cnt += nbits;
      update(symbol, i);
      fop.fo.fap_prev[i] = dequant(fop.fo.fap_Q[i], qstep);
      if (produceOutFapFile) {
		  fprintf(fop.fo.fap_out_fp, "%d ", fop.fo.fap_prev[i]);
      }
    }
  }
  nbits = encoder_flush();
  fop.cur_frame_bit_cnt += nbits;
  if (produceOutFapFile) {
	  fprintf(fop.fo.fap_out_fp, "\n");
  }
 }

void CFBAEncoder::code_pfap() {

  int i, qstep;
  int symbol;
  int nbits;

  /* XXX why skip_frames here? */
  if(fop.fo.fap_mask[0]) 
    code_viseme(0);
  
  if(fop.fo.fap_mask[1]) 
    code_expression(0);

  for(i=2; i<NFAP; i++) {
    if(fop.fo.fap_mask[i]) { /* code the ith fap */
      qstep = fba_cooondef_QP[i]*fop.fo.quant;
      fop.fo.fap_diff[i] = pred(fop.fo.fap_orig[i], fop.fo.fap_prev[i]);
      fop.fo.fap_Q[i] = quant(fop.fo.fap_diff[i], qstep);
      symbol = fop.fo.fap_Q[i] - fop.fo.range_min[i];
      check_range(symbol, i, 1);
      nbits = AR_Encode(symbol, fop.fo.cumul_freq[i]);
      fop.cur_frame_bit_cnt += nbits;
      update(symbol, i);
      fop.fo.fap_diff[i] = dequant(fop.fo.fap_Q[i], qstep);
      fop.fo.fap_prev[i] = recon(fop.fo.fap_diff[i], fop.fo.fap_prev[i]);
      if (produceOutFapFile) {
		  fprintf(fop.fo.fap_out_fp, "%d ", fop.fo.fap_prev[i]);
      }
    }
  }
  nbits = encoder_flush();
  fop.cur_frame_bit_cnt += nbits;
  if (produceOutFapFile) {
	fprintf(fop.fo.fap_out_fp, "\n");
  }

}

void CFBAEncoder::code_viseme(int is_intra) {
  int nbits;
  int symbol;

  if(is_intra) {
    symbol = fop.fo.ve_orig[0];
    nbits = AR_Encode(symbol, fop.fo.Icumul_ve[0]);
    update_viseme_expression(symbol, 0);
    fop.fo.ve_prev[0] = fop.fo.ve_orig[0];

    symbol = fop.fo.ve_orig[1];
    nbits += AR_Encode(symbol, fop.fo.Icumul_ve[1]);
    update_viseme_expression(symbol, 1);
    fop.fo.ve_prev[1] = fop.fo.ve_orig[1];

    fop.fo.viseme_blend_Q = quant(fop.fo.ve_orig[2], fop.fo.quant);
    symbol = fop.fo.viseme_blend_Q;
    nbits += AR_Encode(symbol, fop.fo.Icumul_ve[2]);
    update_viseme_expression(symbol, 2);
    fop.fo.ve_prev[2] = dequant(fop.fo.viseme_blend_Q, fop.fo.quant);

    nbits += encoder_flush();
    nbits += putbits(1, fop.fo.viseme_def);
    fop.cur_frame_bit_cnt += nbits;
    if (produceOutFapFile) {
		fprintf(fop.fo.fap_out_fp, "%d %d %d %d ", fop.fo.ve_prev[0], fop.fo.ve_prev[1],
				fop.fo.ve_prev[2], fop.fo.viseme_def);
	}
	    
  } else {
    symbol = fop.fo.ve_orig[0]-fop.fo.ve_prev[0];
    symbol -= fop.fo.range_ve_min[0];
    nbits = AR_Encode(symbol, fop.fo.cumul_ve[0]);
    update_viseme_expression(symbol, 0);
    fop.fo.ve_prev[0] = fop.fo.ve_orig[0];

    symbol = fop.fo.ve_orig[1]-fop.fo.ve_prev[1];
    symbol -= fop.fo.range_ve_min[1];
    nbits += AR_Encode(symbol, fop.fo.cumul_ve[1]);
    update_viseme_expression(symbol, 1);
    fop.fo.ve_prev[1] = fop.fo.ve_orig[1];

    fop.fo.ve_diff[2] = pred(fop.fo.ve_orig[2], fop.fo.ve_prev[2]);
    fop.fo.viseme_blend_Q = quant(fop.fo.ve_diff[2], fop.fo.quant);
    symbol = fop.fo.viseme_blend_Q - fop.fo.range_ve_min[2];
    nbits += AR_Encode(symbol, fop.fo.cumul_ve[2]);
    update_viseme_expression(symbol, 2);
    fop.fo.ve_diff[2] = dequant(fop.fo.viseme_blend_Q, fop.fo.quant);
    fop.fo.ve_prev[2] = recon(fop.fo.ve_diff[2], fop.fo.ve_prev[2]);

    nbits += encoder_flush();
    nbits += putbits(1, fop.fo.viseme_def);
    fop.cur_frame_bit_cnt += nbits;
    if (produceOutFapFile) {
		fprintf(fop.fo.fap_out_fp, "%d %d %d %d ", fop.fo.ve_prev[0], fop.fo.ve_prev[1],
				fop.fo.ve_prev[2], fop.fo.viseme_def);
	}

  }
}


void CFBAEncoder::code_expression(int is_intra) 
{
  int nbits;
  int symbol;

  if(is_intra) {
    symbol = fop.fo.ve_orig[3];
    nbits = AR_Encode(symbol, fop.fo.Icumul_ve[3]);
    update_viseme_expression(symbol, 3);
    fop.fo.ve_prev[3] = fop.fo.ve_orig[3];

    fop.fo.expr_intensity1_Q = quant(fop.fo.ve_orig[4], fop.fo.quant);
    symbol = fop.fo.expr_intensity1_Q;
    nbits += AR_Encode(symbol, fop.fo.Icumul_ve[4]);
    update_viseme_expression(symbol, 4);
    fop.fo.ve_prev[4] = dequant(fop.fo.expr_intensity1_Q, fop.fo.quant);

    symbol = fop.fo.ve_orig[5];
    nbits += AR_Encode(symbol, fop.fo.Icumul_ve[5]);
    update_viseme_expression(symbol, 5);
    fop.fo.ve_prev[5] = fop.fo.ve_orig[5];

    fop.fo.expr_intensity2_Q = quant(fop.fo.ve_orig[6], fop.fo.quant);
    symbol = fop.fo.expr_intensity2_Q;
    nbits += AR_Encode(symbol, fop.fo.Icumul_ve[6]);
    update_viseme_expression(symbol, 6);
    fop.fo.ve_prev[6] = dequant(fop.fo.expr_intensity2_Q, fop.fo.quant);

    nbits += encoder_flush();
    nbits += putbits(1, fop.fo.init_face);
    nbits += putbits(1, fop.fo.expression_def);
    fop.cur_frame_bit_cnt += nbits;
    if (produceOutFapFile) {
		fprintf(fop.fo.fap_out_fp, "%d %d %d %d %d %d ", fop.fo.ve_prev[3], fop.fo.ve_prev[4],
				fop.fo.ve_prev[5], fop.fo.ve_prev[6], fop.fo.init_face, fop.fo.expression_def);
    }
	    
  } else {
    symbol = fop.fo.ve_orig[3]-fop.fo.ve_prev[3];
    symbol -= fop.fo.range_ve_min[3];
    nbits = AR_Encode(symbol, fop.fo.cumul_ve[3]);
    update_viseme_expression(symbol, 3);
    fop.fo.ve_prev[3] = fop.fo.ve_orig[3];

    fop.fo.ve_diff[4] = pred(fop.fo.ve_orig[4], fop.fo.ve_prev[4]);
    fop.fo.expr_intensity1_Q = quant(fop.fo.ve_diff[4], fop.fo.quant);
    symbol = fop.fo.expr_intensity1_Q - fop.fo.range_ve_min[4];
    nbits += AR_Encode(symbol, fop.fo.cumul_ve[4]);
    update_viseme_expression(symbol, 4);
    fop.fo.ve_diff[4] = dequant(fop.fo.expr_intensity1_Q, fop.fo.quant);
    fop.fo.ve_prev[4] = recon(fop.fo.ve_diff[4], fop.fo.ve_prev[4]);

    symbol = fop.fo.ve_orig[5]-fop.fo.ve_prev[5];
    symbol -= fop.fo.range_ve_min[5];
    nbits += AR_Encode(symbol, fop.fo.cumul_ve[5]);
    update_viseme_expression(symbol, 5);
    fop.fo.ve_prev[5] = fop.fo.ve_orig[5];

    fop.fo.ve_diff[6] = pred(fop.fo.ve_orig[6], fop.fo.ve_prev[6]);
    fop.fo.expr_intensity2_Q = quant(fop.fo.ve_diff[6], fop.fo.quant);
    symbol = fop.fo.expr_intensity2_Q - fop.fo.range_ve_min[6];
    nbits += AR_Encode(symbol, fop.fo.cumul_ve[6]);
    update_viseme_expression(symbol, 6);
    fop.fo.ve_diff[6] = dequant(fop.fo.expr_intensity2_Q, fop.fo.quant);
    fop.fo.ve_prev[6] = recon(fop.fo.ve_diff[6], fop.fo.ve_prev[6]);

    nbits += encoder_flush();
    nbits += putbits(1, fop.fo.init_face);
    nbits += putbits(1, fop.fo.expression_def);
    fop.cur_frame_bit_cnt += nbits;
    if (produceOutFapFile) {
		fprintf(fop.fo.fap_out_fp, "%d %d %d %d %d %d ", fop.fo.ve_prev[3], 
				fop.fo.ve_prev[4], fop.fo.ve_prev[5], fop.fo.ve_prev[6], 
				fop.fo.init_face, fop.fo.expression_def);
	}
  }
}

void  CFBAEncoder::check_range(int symbol, int i, int inter) 
{
  if(inter) {
    symbol += fop.fo.range_min[i];
    if(symbol < fop.fo.range_min[i] || symbol > fop.fo.range_max[i]) {
		char tmpmsg[400];
		sprintf(tmpmsg, "Error: AR coding out of range:\nParameter #%d: %d\nRange: (%d, %d)\nNeed to reset %dth inter parameter range (p_max, p_min in API or FPX, FPN in the EPF file)!\n", i, symbol, 
			fop.fo.range_min[i], fop.fo.range_max[i], i);
		#ifdef WIN32
		MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
		#endif
		exit(-20);
    }
  } else {
    symbol += fop.fo.Irange_min[i];
    if(symbol < fop.fo.Irange_min[i] || symbol > fop.fo.Irange_max[i]) {
		char tmpmsg[400];
		sprintf(tmpmsg, "Error: AR coding out of range:\nParameter #%d: %d\nRange: (%d, %d)\nNeed to reset %dth intra parameter range (i_max, i_min in API or FIX, FIN in the EPF file)!\n", i, symbol, 
			fop.fo.Irange_min[i], fop.fo.Irange_max[i], i);
		#ifdef WIN32
		MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
		#endif
      exit(-21);
    }
  }
}
int CFBAEncoder::AR_Encode(int index, int cumul_freq[ ])
{
  int bitcount=0;

  if (index<0) 
    return -1; /* Escape Code */

  length = high - low + 1;
  high = low - 1 + (length * cumul_freq[index]) / cumul_freq[0];
  low += (length * cumul_freq[index+1]) / cumul_freq[0];

  for ( ; ; ) {
    if (high < fba_cooondef_q2) {
      bitcount+=bit_opp_bits(0);
    }
    else if (low >= fba_cooondef_q2) {
      bitcount+=bit_opp_bits(1);	
      low -= fba_cooondef_q2; 
      high -= fba_cooondef_q2;
    }
    else if (low >= fba_cooondef_q1 && high < fba_cooondef_q3) {
      opposite_bits += 1; 
      low -= fba_cooondef_q1; 
      high -= fba_cooondef_q1;
    }
    else break;
 
    low *= 2; 
    high = 2*high+1;
  }
  return bitcount;
}

int CFBAEncoder::bit_opp_bits(int bit) /* Output a bit and the following opposite bits */              
{                                   
  int bitcount=0;

  bitcount = bit_in_psc_layer(bit);

  while(opposite_bits > 0){
    bitcount += bit_in_psc_layer(!bit);
    opposite_bits--;
  }
  return bitcount;
}

/*********************************************************************
 *
 *      Name:           encoder_flush
 *
 *      Description:    Completes arithmetic coding stream before any
 *        fixed length codes are transmitted.
 *
 *      Input:          None
 *
 *      Returns:        Number of bits used.
 *
 *      Side Effects:   Resets low, high, zerorun and opposite_bits 
 *        variables.
 *
 *      Author:         pmulroy@visual.bt.co.uk
 *
 *********************************************************************/

int CFBAEncoder::encoder_flush()
{
  int bitcount = 0;

  opposite_bits++;
  if (low < fba_cooondef_q1) {
    bitcount+=bit_opp_bits(0);
  }
  else {
    bitcount+=bit_opp_bits(1);
  }
  low = 0; 
  high = fba_cooondef_top;

  zerorun=0;

  return bitcount;
}

/*********************************************************************
 *
 *      Name:           bit_in_psc_layer
 *
 *      Description:    Inserts a bit into output bitstream and avoids
 *        picture start code emulation by stuffing a one
 *        bit.
 *
 *      Input:          Bit to be output.
 *
 *      Returns:        Nothing
 *
 *      Side Effects:   Updates zerorun variable.
 *
 *      Author:         pmulroy@visual.bt.co.uk
 *
 *********************************************************************/

int CFBAEncoder::bit_in_psc_layer(int bit)
{
  int bitcount = 0;

  if (zerorun == 22) {
    bitcount += putbits (1, 1);
    zerorun = 0;
  }

  bitcount += putbits (1, bit);

  if (bit)
    zerorun = 0;
  else
    zerorun++;

  return bitcount;
}

/*
 * update module for adaptive arithmetic coding
 *
 * Author: Wei Wu (weiwu@stallion.risc.rockwell.com)
 * Date:   1/14/97
 *
 */

void CFBAEncoder::update(int symbol, int index)
{
  int n;
  int *cumul_tmp, *freq_tmp;

  if(fop.is_intra) {
    n = fop.fo.Irange_max[index]-fop.fo.Irange_min[index]+1;
    cumul_tmp = fop.fo.Icumul_freq[index];
    freq_tmp = fop.fo.Ifreq[index];
  } else {
    n = fop.fo.range_max[index]-fop.fo.range_min[index]+1;
    cumul_tmp = fop.fo.cumul_freq[index];
    freq_tmp = fop.fo.freq[index];
  }
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


void CFBAEncoder::update_viseme_expression(int symbol, int index)
{
  int n;
  int *cumul_tmp, *freq_tmp;

  if(fop.is_intra) {
    n = fop.fo.Irange_ve_max[index] - fop.fo.Irange_ve_min[index]+1;
    cumul_tmp = fop.fo.Icumul_ve[index];
    freq_tmp = fop.fo.Ifreq_ve[index];
  } else {
    n = fop.fo.range_ve_max[index] - fop.fo.range_ve_min[index]+1;
    cumul_tmp = fop.fo.cumul_ve[index];
    freq_tmp = fop.fo.freq_ve[index];
  }
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


int CFBAEncoder::quant(int fap, int qstep)
{
  int level;
  int qfap;

  if(qstep) {
  	level = (fabs(fap)<=qstep/2)?0:(fabs(fap)+qstep/2)/qstep;
  	qfap = sign(fap) * level;
  	return qfap;
  } else { /* 0 stepsize means no quantization */
	return fap;
  }

}

int CFBAEncoder::dequant(int qfap, int qstep) 
{
  if(qstep) {
  	return qfap*qstep;
  } else {
	return qfap;
  }
}


int  CFBAEncoder::pred(int fap_curr, int fap_prev) 
{
  int fap_diff;

  fap_diff = fap_curr - fap_prev;
  return fap_diff;
}

int CFBAEncoder::recon(int fap_diff, int fap_prev)
{
  int fap_rec;

  fap_rec = fap_diff + fap_prev;
  return fap_rec;
}

void CFBAEncoder::code_frame(void) 
{
  int i, n;
  int j, curframe_num;

  if(fop.is_intra) {
    init_arith();
    bap_init_arith();

  }
  
  if(fop.fba_paramset & FAP_PARAMSET_MASK) 
  {
	  /* Read FAPs */
	  do 
	  {
		  if(!get_line(fap_fp)) 
		  {
			  char tmpmsg[300];
			  sprintf(tmpmsg,"finish_frame_fap: expected line in .fap\n");
			  #ifdef WIN32
			  MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			  exit(-102);
		  }
		  
		  for(i=0; i<NFAP; i++) 
		  {
			  fscanf(fap_fp, "%d", &fop.fo.fap_mask2[i]);			  
			  
			  if(fop.fo.fap_mask2[i] != fop.fo.fap_mask[i])
			  {
				  if(!fapMaskWarningGiven)
				  {
					  fapMaskWarningGiven = true;
					  char tmpmsg[300];
					  sprintf(tmpmsg,"FAP mask for FAP %d in FAP file is different from the mask from EPF file (FAP file: %d, EPF file: %d)",i,fop.fo.fap_mask2[i],fop.fo.fap_mask[i]);
					  #ifdef WIN32
					  MessageBox(0,tmpmsg,"Warning",MB_ICONERROR);
						#endif
				  }
			  }
		  }

		  if(!get_line(fap_fp)) 
		  {
			  char tmpmsg[300];
			  sprintf(tmpmsg,"finish_frame_fap: expected line in .fap\n");
			  #ifdef WIN32
			  MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
				#endif
			  exit(-103);
		  }

		  fscanf(fap_fp, "%d", &n);
		  #ifdef _DEBUG
		  assert(n<=fop.fo.frame_num);
		  #endif
		  if(fop.fo.fap_mask2[0]) 
			  fscanf(fap_fp, "%d%d%d%d", &fop.fo.ve_orig[0],
			  &fop.fo.ve_orig[1], &fop.fo.ve_orig[2], &fop.fo.viseme_def);
		  else
			  fop.fo.ve_orig[0] = fop.fo.ve_orig[1] = fop.fo.ve_orig[2] = fop.fo.viseme_def = 0;
		  
		  if(fop.fo.fap_mask2[1]) 
			  fscanf(fap_fp, "%d%d%d%d%d%d", &fop.fo.ve_orig[3],
				  &fop.fo.ve_orig[4], &fop.fo.ve_orig[5], 
				  &fop.fo.ve_orig[6], &fop.fo.init_face, 
				  &fop.fo.expression_def);
		  else
			  fop.fo.ve_orig[3] = fop.fo.ve_orig[4] = fop.fo.ve_orig[5] = fop.fo.ve_orig[6] = fop.fo.expression_def = 0;

		  for(i=2; i<NFAP; i++) 
		  {
			  if(fop.fo.fap_mask2[i]) 
				  fscanf(fap_fp, "%d", &fop.fo.fap_orig[i]);
			  else
				  fop.fo.fap_orig[i] = 0; 
		  }
	  } while(n<fop.fo.frame_num);
	  
	  for(i=0; i<NFAP; i++) {
		  fprintf(fop.fo.fap_out_fp, "%d ", fop.fo.fap_mask[i]);
	  }
	  fprintf(fop.fo.fap_out_fp, "\n");
	  fprintf(fop.fo.fap_out_fp, "%d ", fop.fo.frame_num);
  }

  if(fop.fba_paramset & BAP_PARAMSET_MASK) {
  /* Read BAPs */
    do {
      if(!get_line(bap_fp)) 
	  {
			char tmpmsg[300];
			sprintf(tmpmsg,"finish_frame_bap: (1) expected line in .bap\n");
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-104);
	  }
      
      for(j=0;j<NUM_BAPS;j++) 
	  {
		  if(fabsf(bap_version - 3.2f) < 0.01f)
			  fscanf(bap_fp, "%d", &fop.bo.bap_mask2[j]);  // BAP files v3.2 have extension BAPs
		  else
		  {
			  if(j < NBAP) // this is adapted to read files without extension BAPs, i.e. files in versions older than 3.2
				  fscanf(bap_fp, "%d", &fop.bo.bap_mask2[j]);
			  else
				  fop.bo.bap_mask2[j] = 0;
		  }

		  if(fop.bo.bap_mask2[j] != fop.bo.bap_mask[j])
		  {
			  if(!bapMaskWarningGiven)
			  {
				  bapMaskWarningGiven = true;
				  char tmpmsg[300];
				  sprintf(tmpmsg,"BAP mask for BAP %d in BAP file is different from the mask from EPF file (BAP file: %d, EPF file: %d)",j,fop.bo.bap_mask2[j],fop.bo.bap_mask[j]);
				  #ifdef WIN32
				  MessageBox(0,tmpmsg,"Warning",MB_ICONERROR);
					#endif
			  }
		  }
      }
      
      if(!get_line(bap_fp))
	  {
			char tmpmsg[300];
			sprintf(tmpmsg,"finish_frame_bap: (1) expected line in .bap\n");
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-103);
	  }
      
      /* 
      ** Read frame number
      */
      
      fscanf(bap_fp, "%d", &curframe_num);
	  #ifdef _DEBUG
      assert(curframe_num<=fop.bo.frame_num);
	  #endif
      
      /*
      ** Read in data 
      */
      
     for(i=0; i<NUM_BAPS; i++) 
	  {
		  if(fop.bo.bap_mask2[i])
			  fscanf(bap_fp, "%d", &fop.bo.bap_orig[i]);
		  else
			  fop.bo.bap_orig[i] = 0;
      }
    } while(curframe_num < fop.bo.frame_num);
    
    for(i=0; i<NUM_BAPS; i++) {
      fprintf(fop.bo.bap_out_fp, "%d ", fop.bo.bap_mask[i]);
    }
    fprintf(fop.bo.bap_out_fp, "\n");
    fprintf(fop.bo.bap_out_fp, "%d ", fop.bo.frame_num);
  }

  /* 
  ** Create the bitstream
  */
  fop.cur_frame_bit_cnt = 0;
  fba_object_plane_data();

  if(fop.is_intra) 
  {
	  ibit_cnt += fop.cur_frame_bit_cnt;
	  if(fop.fo.new_mask_flag && expand_mask()) 
	  {
		  for(i=0; i<NFAP; i++) 
		  {
			  fop.fo.prev_fap_mask[i] = fop.fo.fap_mask[i];
		  }
	  }
	  if(fop.bo.new_mask_flag) 
	  {
		  for (i=0; i<NUM_BAPS; i++) 
		  {
			  fop.bo.prev_bap_mask[i] = fop.bo.bap_mask[i];
		  }
	  }
  } 
  else 
	  pbit_cnt += fop.cur_frame_bit_cnt;
}


void CFBAEncoder::fba_object_plane_header() 
{
  if (fop.is_startcode) { /*changed from is_intra -edp*/
    fop.cur_frame_bit_cnt = next_start_code();
    fop.cur_frame_bit_cnt += putbits(32, FBA_OBJECT_PLANE_START_CODE);
  }
  fop.cur_frame_bit_cnt += putbits( 1, fop.is_intra);
  fop.cur_frame_bit_cnt += putbits(2, fop.fba_paramset);

  /* Temporal header */
  if (fop.is_intra) {
    if (fop.fba_paramset & FAP_PARAMSET_MASK) {

      fop.cur_frame_bit_cnt += putbits(1, fop.fo.is_frame_rate);
      if(fop.fo.is_frame_rate) {
	code_frame_rate();
      }
      fop.cur_frame_bit_cnt += putbits(1, fop.fo.is_time_code);
      if(fop.fo.is_time_code) {
	code_time_code();
      }
    }
  }/*v17*/
  if (fop.fba_paramset & FAP_PARAMSET_MASK) {/*v17*/
    fop.cur_frame_bit_cnt += putbits(1, fop.fo.skip_frames);
    if(fop.fo.skip_frames) {
      code_skip_frames();
    }
  }
    
  if (fop.is_intra) {
    if ((fop.fba_paramset & BAP_PARAMSET_MASK) && !(fop.fba_paramset & FAP_PARAMSET_MASK)) {
      fop.cur_frame_bit_cnt += putbits(1, fop.bo.is_frame_rate);
      if(fop.bo.is_frame_rate) {
	bap_code_frame_rate();
      }
      
      fop.cur_frame_bit_cnt += putbits(1, fop.bo.is_time_code);
      if(fop.bo.is_time_code) {
	bap_code_time_code();
      }
    }
  }      /*v17*/
  if ((fop.fba_paramset & BAP_PARAMSET_MASK) && !(fop.fba_paramset & FAP_PARAMSET_MASK)) {/*v17*/
    fop.cur_frame_bit_cnt += putbits(1, fop.bo.skip_frames);
    if(fop.bo.skip_frames) {
      bap_code_skip_frames();
    }
  }
}


void CFBAEncoder::fba_object_plane_data() 
{
  int i,j;
  static int fr_no=0;
  int coding_type = fop.fo.coding_type; /* 6-25-98 */
  static int no_frames=0;

    fr_no++;

  if (fop.fba_paramset & FAP_PARAMSET_MASK) {

    if(fop.is_intra) {

      fop.cur_frame_bit_cnt += putbits(5, fop.fo.quant);
      printf("fop.fo.quant(5) = %d\n",fop.fo.quant);

      for(i=0; i<N_FAP_GROUP; i++) {
	fop.cur_frame_bit_cnt += putbits(1, MARKER_BIT);
	fop.cur_frame_bit_cnt += putbits(2, fop.fo.fap_mask_type[i]);
	if(fop.fo.fap_mask_type[i]==1 || fop.fo.fap_mask_type[i]==2) {
	  code_fap_group_mask(i);
	}
      }

      fop.cur_frame_bit_cnt += putbits(1, fop.fo.suggested_gender);
      fop.cur_frame_bit_cnt += putbits(1, coding_type); /* 3-3-98 *//*changed 2 to 1 -edp*/
      if (coding_type == PREDICTIVE_CODING_TYPE) {
	if(fop.fo.havesetminmax) for(i=0;i<NFAP;i++) if(fop.fo.fap_mask[i] && !fop.fo.minmaxset[i]) {/*check that minmax sent for each fap*/
	  fop.fo.is_i_new_max = 1;fop.fo.is_i_new_min = 1;fop.fo.is_p_new_max = 1;fop.fo.is_p_new_min = 1;
	  break;
	}
	fop.cur_frame_bit_cnt += putbits(1, fop.fo.is_i_new_max);
	fop.cur_frame_bit_cnt += putbits(1, fop.fo.is_i_new_min);
	fop.cur_frame_bit_cnt += putbits(1, fop.fo.is_p_new_max);
	fop.cur_frame_bit_cnt += putbits(1, fop.fo.is_p_new_min);
      	code_new_minmax();
	code_ifap();
      }
      else {
//	code_i_segment();
      }
    }
    else {
      if (coding_type == PREDICTIVE_CODING_TYPE) {
	code_pfap();
      } else {
//	code_p_segment();
      }
    }
  }

  if (fop.fba_paramset & BAP_PARAMSET_MASK) {

    if(fop.is_intra) {

      fop.cur_frame_bit_cnt += putbits(5, fop.bo.quant);


      for(i=0; i<BAP_NUM_GROUPS; i++) {
	fop.cur_frame_bit_cnt += putbits(1, MARKER_BIT);
	fop.cur_frame_bit_cnt += putbits(2, fop.bo.bap_mask_type[i]);
	if(fop.bo.bap_mask_type[i]==1) {
	  code_bap_group_mask(i);
	}
      }

      fop.cur_frame_bit_cnt += putbits(1, fop.fo.suggested_gender);
      fop.cur_frame_bit_cnt += putbits(1, coding_type); /* 3-3-98 *//*changed 2 to 1 -edp*/

      if (coding_type == PREDICTIVE_CODING_TYPE) {
	if(fop.bo.havesetminmax) for(i=0;i<BAP_NUM_GROUPS;i++) for(j=0;j<fop.bo.baps_in_group[i]; j++) if(fop.bo.bap_mask[fop.bo.bap_mask_index[i][j]] && !fop.bo.minmaxset[fop.bo.bap_mask_index[i][j]]) {/*check that minmax sent for each bap*/
	  fop.bo.is_i_new_max = 1;fop.bo.is_i_new_min = 1;fop.bo.is_p_new_max = 1;fop.bo.is_p_new_min = 1;
	  break;
	}
	fop.cur_frame_bit_cnt += putbits(1, fop.bo.is_i_new_max);
	fop.cur_frame_bit_cnt += putbits(1, fop.bo.is_i_new_min);
	fop.cur_frame_bit_cnt += putbits(1, fop.bo.is_p_new_max);
	fop.cur_frame_bit_cnt += putbits(1, fop.bo.is_p_new_min);

	/* has_extendable_baps = 0 for the moment 
	fop.cur_frame_bit_cnt += putbits(1, 0);   */ 

	bap_code_new_minmax();
	code_ibap();
      }
      else {
//	bap_code_i_segment();
      }
    } else {
      if (coding_type == PREDICTIVE_CODING_TYPE) {
	code_pbap();
      } else {
//	bap_code_p_segment();
      }
    }
  }
}

void CFBAEncoder::bap_code_frame_rate() 
{
  fop.cur_frame_bit_cnt += putbits(8, fop.bo.frame_rate);
  fop.cur_frame_bit_cnt += putbits(4, fop.bo.fr_seconds);
  fop.cur_frame_bit_cnt += putbits(1, fop.bo.fr_frequency_offset);
}


void CFBAEncoder::bap_code_time_code() 
{
  fop.cur_frame_bit_cnt += putbits(5, fop.bo.time_code_hours);
  fop.cur_frame_bit_cnt += putbits(6, fop.bo.time_code_minutes);
  fop.cur_frame_bit_cnt += putbits(1, fop.marker_bit);
  fop.cur_frame_bit_cnt += putbits(6, fop.bo.time_code_seconds);
}


void CFBAEncoder::bap_code_skip_frames() 
{
  int ivalue;

  do {
    if(fop.bo.number_of_frames_to_skip<15) {
      ivalue = fop.bo.number_of_frames_to_skip;
    } else {
      ivalue = 15;
      fop.bo.number_of_frames_to_skip -= 15;
    }
    fop.cur_frame_bit_cnt += putbits(4, ivalue);
  } while (ivalue==15);
}

void CFBAEncoder::bap_code_new_minmax()
{
  int i, j;
  int temp_index;

  if(fop.bo.is_i_new_max) {
    fop.bo.havesetminmax = 1;
    for (i=0; i<BAP_NUM_GROUPS; i++) {
        for(j=0;j<fop.bo.baps_in_group[i]; j++) {
	  temp_index = fop.bo.bap_mask_index[i][j];
	  if(!(j&3)) 
	    fop.cur_frame_bit_cnt += putbits(1, MARKER_BIT);
	  if(fop.bo.bap_mask[temp_index]) {
	    fop.cur_frame_bit_cnt += putbits(5, fop.bo.i_new_max[temp_index]);
	    fop.bo.minmaxset[temp_index] = 1;/* just set here. assume that I/P min max all set for a given bap -edp*/
	  }
	}
    }
  }

  if(fop.bo.is_i_new_min) {
    for (i=0; i<BAP_NUM_GROUPS; i++) {
        for(j=0;j<fop.bo.baps_in_group[i]; j++) {
	  temp_index = fop.bo.bap_mask_index[i][j];
	  if(!(j&3)) 
	    fop.cur_frame_bit_cnt += putbits(1, MARKER_BIT);
	  if(fop.bo.bap_mask[temp_index]) 
	    fop.cur_frame_bit_cnt += putbits(5, fop.bo.i_new_min[temp_index]);
	}
    }
  }

  if(fop.bo.is_p_new_max) {
    for (i=0; i<BAP_NUM_GROUPS; i++) {
        for(j=0;j<fop.bo.baps_in_group[i]; j++) {
	  temp_index = fop.bo.bap_mask_index[i][j];
	  if(!(j&3)) 
	    fop.cur_frame_bit_cnt += putbits(1, MARKER_BIT);
	  if(fop.bo.bap_mask[temp_index]) 
	    fop.cur_frame_bit_cnt += putbits(5, fop.bo.p_new_max[temp_index]);
	}
    }
  }

  if(fop.bo.is_p_new_min) {
    for (i=0; i<BAP_NUM_GROUPS; i++) {
        for(j=0;j<fop.bo.baps_in_group[i]; j++) {
	  temp_index = fop.bo.bap_mask_index[i][j];
	  if(!(j&3)) 
	    fop.cur_frame_bit_cnt += putbits(1, MARKER_BIT);
	  if(fop.bo.bap_mask[temp_index]) 
	    fop.cur_frame_bit_cnt += putbits(5, fop.bo.p_new_min[temp_index]);
	}
    }
  }

}

void CFBAEncoder::code_ibap() 
{
	int i, j, k, qstep;
	int symbol;
	int nbits;
	
	for (k=0; k<BAP_NUM_GROUPS; k++) {
		for(j=0;j<fop.bo.baps_in_group[k]; j++) {
			i = fop.bo.bap_mask_index[k][j];
			if(fop.bo.bap_mask[i]) { /* code the ith bap */
				qstep = fba_cooondef_BQP[i]*fba_cooondef_bap_pred_scale[fop.bo.quant];
				fop.bo.bap_Q[i] = quant(fop.bo.bap_orig[i], qstep);
				symbol = fop.bo.bap_Q[i] - fop.bo.Irange_min[i];
				bap_check_range(symbol, i, 0);
				nbits = AR_Encode(symbol, fop.bo.Icumul_freq[i]);
				fop.cur_frame_bit_cnt += nbits;
				bap_update(symbol, i);
				fop.bo.bap_prev[i] = dequant(fop.bo.bap_Q[i], qstep);
				if (produceOutBapFile)
					fprintf(fop.bo.bap_out_fp, "%d ", fop.bo.bap_prev[i]);
			}
		}
	}
	nbits = encoder_flush();
	fop.cur_frame_bit_cnt += nbits;
	if (produceOutBapFile)
		fprintf(fop.bo.bap_out_fp, "\n");
}

void CFBAEncoder::code_pbap() 
{
	
	int i, j, k, qstep;
	int symbol;
	int nbits;
	
	
	/* v17
	fop.cur_frame_bit_cnt += putbits(1, fop.bo.skip_frames);
	if(fop.bo.skip_frames) 
    bap_code_skip_frames();
    */
	
	for (k=0; k<BAP_NUM_GROUPS; k++) {
		for(j=0;j<fop.bo.baps_in_group[k]; j++) {
			i = fop.bo.bap_mask_index[k][j];
			if(fop.bo.bap_mask[i]) { /* code the ith bap */
				qstep = fba_cooondef_BQP[i]*fba_cooondef_bap_pred_scale[fop.bo.quant];
				fop.bo.bap_diff[i] = pred(fop.bo.bap_orig[i], fop.bo.bap_prev[i]);
				fop.bo.bap_Q[i] = quant(fop.bo.bap_diff[i], qstep);
				symbol = fop.bo.bap_Q[i] - fop.bo.range_min[i];
				bap_check_range(symbol, i, 1);
				nbits = AR_Encode(symbol, fop.bo.cumul_freq[i]);
				fop.cur_frame_bit_cnt += nbits;
				bap_update(symbol, i);
				fop.bo.bap_diff[i] = dequant(fop.bo.bap_Q[i], qstep);
				fop.bo.bap_prev[i] = recon(fop.bo.bap_diff[i], fop.bo.bap_prev[i]);
				if (produceOutBapFile)
					fprintf(fop.bo.bap_out_fp, "%d ", fop.bo.bap_prev[i]);
			}
		}
	}
	nbits = encoder_flush();
	fop.cur_frame_bit_cnt += nbits;
	if (produceOutBapFile)
		fprintf(fop.bo.bap_out_fp, "\n");
	
}

void CFBAEncoder::bap_check_range(int symbol, int i, int inter) 
{
	if(inter) 
	{
		symbol += fop.bo.range_min[i];
		if(symbol < fop.bo.range_min[i] || symbol > fop.bo.range_max[i]) 
		{
			char tmpmsg[300];
			sprintf(tmpmsg,"Error: AR coding out of range[3]: \tParameter #%d: %d \tRange: (%d, %d) \tNeed to reset %dth inter parameter range!\n", i, symbol, 
				fop.bo.range_min[i], fop.bo.range_max[i],i);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-103);
		}
	} 
	else 
	{
		symbol += fop.bo.Irange_min[i];
		if(symbol < fop.bo.Irange_min[i] || symbol > fop.bo.Irange_max[i]) 
		{
			char tmpmsg[300];
			sprintf(tmpmsg,"Error: AR coding out of range[4]: \tParameter #%d: %d \tRange: (%d, %d) \tNeed to reset %dth intra parameter range!\n", i, symbol, 
				fop.bo.Irange_min[i], fop.bo.Irange_max[i],i);
			#ifdef WIN32
			MessageBox(0,tmpmsg,"Error - exiting",MB_ICONERROR);
			#endif
			exit(-104);
		}
	}
}

void CFBAEncoder::bap_init_arith() 
{

  int i, j, n;

  for(i=0; i<NUM_BAPS; i++) {

	if(fop.bo.bap_mask[i]) {
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
    if(fop.bo.bap_mask[i]) {
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

void CFBAEncoder::code_bap_group_mask(int n) 
{
  int i;
  int temp_index;

  for(i=0;i<fop.bo.baps_in_group[n]; i++) {
    temp_index = fop.bo.bap_mask_index[n][i];
    fop.cur_frame_bit_cnt += putbits(1, fop.bo.bap_mask[temp_index]);
  }
}


void CFBAEncoder::bap_update(int symbol, int index) 
{
  int n;
  int *cumul_tmp, *freq_tmp;

  if(fop.is_intra) {
    n = fop.bo.Irange_max[index]-fop.bo.Irange_min[index]+1;
    cumul_tmp = fop.bo.Icumul_freq[index];
    freq_tmp = fop.bo.Ifreq[index];
  } else {
    n = fop.bo.range_max[index]-fop.bo.range_min[index]+1;
    cumul_tmp = fop.bo.cumul_freq[index];
    freq_tmp = fop.bo.freq[index];
  }
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




/*
void CFBAEncoder::fba_object_plane() 
{
  int i;
  int coding_type = PREDICTIVE_CODING_TYPE; 

  if(fop.is_intra) {

    fop.cur_frame_bit_cnt = next_start_code();
    fop.cur_frame_bit_cnt += putbits(32, FBA_OBJECT_PLANE_START_CODE);
  }

    fop.cur_frame_bit_cnt += putbits(1, fop.is_intra); 
    fop.cur_frame_bit_cnt += putbits(2, fop.fba_paramset);

  if(fop.is_intra) { 
    fop.cur_frame_bit_cnt += putbits(1, fop.fo.is_frame_rate);

    if(fop.fo.is_frame_rate) {
		code_frame_rate();
    }
    fop.cur_frame_bit_cnt += putbits(1, fop.fo.is_time_code);
    if(fop.fo.is_time_code) {
		code_time_code();
    }
  }
    fop.cur_frame_bit_cnt += putbits(1, fop.fo.skip_frames);
    if(fop.fo.skip_frames) {
		code_skip_frames();
    }

  switch(fop.fba_paramset) {
  case FAP_PARAMSET_MASK:
    
    if(fop.is_intra) {

      fop.cur_frame_bit_cnt += putbits(5, fop.fo.quant);

      for(i=0; i<N_FAP_GROUP; i++) {
	fop.cur_frame_bit_cnt += putbits(1, MARKER_BIT);
	fop.cur_frame_bit_cnt += putbits(2, fop.fo.fap_mask_type[i]);
	if(fop.fo.fap_mask_type[i]==1 || fop.fo.fap_mask_type[i]==2) {
		code_fap_group_mask(i);
	}
      }

      fop.cur_frame_bit_cnt += putbits(2, coding_type); 
      if (coding_type == PREDICTIVE_CODING_TYPE) {
          fop.cur_frame_bit_cnt += putbits(1, fop.fo.is_i_new_max);
          fop.cur_frame_bit_cnt += putbits(1, fop.fo.is_i_new_min);
          fop.cur_frame_bit_cnt += putbits(1, fop.fo.is_p_new_max);
          fop.cur_frame_bit_cnt += putbits(1, fop.fo.is_p_new_min);
		  code_new_minmax();
		  code_ifap();
      } else {
		  code_i_segment();
      }
    } else {
      if (coding_type == PREDICTIVE_CODING_TYPE) {
		  code_pfap();
      } else {
		  code_p_segment();
      }
    }
    break;
  case FDP_PARAMSET_MASK:
    break;
  case BAP_PARAMSET_MASK:
	break;
  case BDP_PARAMSET_MASK:
    break;
  }
}
*/

// Added by KStankovic
CFBAEncoder::~CFBAEncoder()
{
	int i;

	for (i = 0; i < NUM_BAPS; i++)
	{
		if (fop.bo.Icumul_freq[i] != NULL) {
            free(fop.bo.Icumul_freq[i]);
            fop.bo.Icumul_freq[i] = NULL;
        }
        if (fop.bo.cumul_freq[i] != NULL) {
            free(fop.bo.cumul_freq[i]);
            fop.bo.cumul_freq[i] = NULL;
        }
		if (fop.bo.Ifreq[i] != NULL) {
            free(fop.bo.Ifreq[i]);
            fop.bo.Ifreq[i] = NULL;
        }
		if (fop.bo.freq[i] != NULL) {
            free(fop.bo.freq[i]);
            fop.bo.freq[i] = NULL;
        }
	}

	if (fop.bo.prev_bap_mask != NULL) {
        free(fop.bo.prev_bap_mask);
        fop.bo.prev_bap_mask = NULL;
    }
	if (fop.bo.bap_mask2 != NULL) {
        free(fop.bo.bap_mask2);
        fop.bo.bap_mask2 = NULL;
    }
	if (fop.bo.bap_mask != NULL) {
        free(fop.bo.bap_mask);
        fop.bo.bap_mask = NULL;
    }

	for (i = 0; i < 7; i++)
	{
		if (fop.fo.cumul_ve[i] != NULL) {
            delete [] fop.fo.cumul_ve[i];
            fop.fo.cumul_ve[i] = NULL;
        }
		if (fop.fo.freq_ve[i] != NULL) {
            delete [] fop.fo.freq_ve[i];
            fop.fo.freq_ve[i] = NULL;
        }
		if (fop.fo.Icumul_ve[i] != NULL) {
            delete [] fop.fo.Icumul_ve[i];
            fop.fo.Icumul_ve[i] = NULL;
        }
		if (fop.fo.Ifreq_ve[i] != NULL) {
            delete [] fop.fo.Ifreq_ve[i];
            fop.fo.Ifreq_ve[i] = NULL;
        }
	}
}
//

}
