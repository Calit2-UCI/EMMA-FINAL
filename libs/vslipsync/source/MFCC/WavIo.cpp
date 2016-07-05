#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#ifdef __WIN32__
	#ifndef WIN32
    	#define WIN32
    #endif //WIN32
#endif //__WIN32__

//#ifdef WIN32
//#include <windows.h>
//#include <mmsystem.h>
//#endif

#ifndef LINUX
#include "WavIo.h"
#include "memalloc.h"
#include "stdio.h"
#include "mfcc.h"
#endif

#ifndef MAXWORD
	#define MAXWORD			0xFFFF
#endif

#ifdef MSVC

/* LoadWav : load data stored in the wav file
 * filename:  file path (long path)
 * data : pointer which will receive the loaded datas
 *     data must point to 0x00 when calling the
 *     function. (data must NOT be allocated)
 *     Memory will be allocated with
 *     MEMALLOC functions
 */
mfcc_BOOL LoadWav(char *filename, mfcc_SIGNAL *s) //double **data, int *datasize)
{
	FILE *f = fopen(filename,"rb");
	int size, format, samp_freq, num_samp_bits, NumSamples;
	mfcc_DWORD dw;
	mfcc_WORD w;

	fread(&dw, sizeof(mfcc_DWORD), 1, f);
	if (dw != 0x46464952) /* RIFF*/
	  return false;

	fread(&dw, sizeof(mfcc_DWORD), 1, f);
	fread( &dw, sizeof(mfcc_DWORD), 1, f);
	if (dw != 0x45564157) /* WAVE */
		return false;

	fread(&dw, sizeof(mfcc_DWORD), 1, f);
	if (dw != 0x20746d66) /* fmt */
		return false;
	fread( &dw, sizeof(mfcc_DWORD), 1, f);

	/* format */
	fread(&w, sizeof(mfcc_WORD), 1, f);
	if (w != 7 && w != 1) 
		return false;
	format=w;

	/* read the number of channels */
	fread( &w, sizeof(mfcc_WORD), 1, f);
	if (w != 1) 
		return false;

	fread( &dw, sizeof(mfcc_DWORD), 1, f);
	samp_freq=dw;

    /* Skip a few bytes */
	fread( &dw, sizeof(mfcc_DWORD), 1, f);
	fread( &w, sizeof(mfcc_WORD), 1, f);

    fread( &w, sizeof(mfcc_WORD), 1, f);
	if (w!=16)
		return false;
	num_samp_bits=w;

    if ((num_samp_bits == 8 && (format != 7 && format != 6)) ||
	    (num_samp_bits == 16 && format != 1))
		return false;

	fread( &dw, sizeof(mfcc_DWORD), 1, f);
	if (dw != 0x61746164) /* data */
		return false;

	fread( &dw, sizeof(mfcc_DWORD), 1, f);
	size=dw;
	if (size == -1)
		NumSamples = -1;
	else 
	{
          if (format == 6 || format == 7) 
			  NumSamples = size;
          else if (format == 1) 
			  NumSamples = size / 2;
	}
	signed short *buf;
	int numread;
	int i=0;
	s->signal_data_lenght=0;
	s->signal_data = (mfcc_double *) MEMALLOC(NumSamples * sizeof(double) );
	buf = (signed short *) MEMALLOC(NumSamples * sizeof(signed short) );
	numread = (int)fread( buf, sizeof(signed short), NumSamples, f );	//CHANGED BY I.K. added explicit cast
	if (numread)
	{
		for (int i=0 ; i<NumSamples ; ++i)
		{
			s->signal_data[i] = ( (mfcc_double) buf[i] ) / MAXWORD;
		}
	}
	s->signal_data_lenght=NumSamples;
	s->fs=samp_freq;
	MEMFREE(buf);
	fclose(f);
	return true;
}

/* WriteWav : write data to wav file
 * filename:  file path (long path)
 * data : pointer from which data will be copied from
 * datasize : size of data to write
 */
mfcc_BOOL WriteWav(char *filename, mfcc_double *data, int *datasize)
{
	return true;
}

mfcc_BOOL CloseSignal(mfcc_SIGNAL *s)
{
	MEMFREE(s->signal_data);
	s->fs=0;
	s->signal_data=0;
	s->signal_data_lenght=0;
	return true;
}

//#else // None Win32 plateforme

//#error Not yet implemented for not-Win32 plateform!

#endif // WIN32


