#ifndef __HEADER_WAVIO_H
#define __HEADER_WAVIO_H

#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#ifdef __WIN32__
	#ifndef WIN32
    	#define WIN32
    #endif //WIN32
#endif //__WIN32__
#ifdef WIN32
#include "mfcc.h"

mfcc_BOOL LoadWav(char *filename, mfcc_SIGNAL *s);//double **data, int *datasize);
mfcc_BOOL WriteWav(char *filename, mfcc_SIGNAL *s);// double *data, int *datasize);

mfcc_BOOL CloseSignal(mfcc_SIGNAL *s);

#endif // WIN32

#endif //__HEADER_WAVIO_H
 

