
#ifndef __HEADER_MFCC_H
#define __HEADER_MFCC_H

#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int mfcc_UINT;
typedef double mfcc_double;
typedef unsigned short mfcc_BOOL;
typedef unsigned long  mfcc_DWORD;
typedef unsigned short mfcc_WORD;


typedef struct SIGNAL_tag
{
	mfcc_double *signal_data;
	mfcc_UINT signal_data_lenght;
	mfcc_UINT fs;
}mfcc_SIGNAL;

typedef struct VECTOR_tag
{
	mfcc_double *data;
	mfcc_UINT data_dimension;
}mfcc_VECTOR;

typedef struct VECTORLIST_tag
{
	mfcc_VECTOR *vector;
	mfcc_UINT vector_number;
	mfcc_UINT vector_dimension;
}mfcc_VECTORLIST;

mfcc_BOOL Mfcc(mfcc_SIGNAL s, mfcc_UINT k, mfcc_UINT n, mfcc_VECTORLIST *c);
double HammingPonder(mfcc_UINT n, mfcc_UINT i);
void Hamming(mfcc_double *ham,short n);
double Frq2Mel(mfcc_double f);
double Mel2Frq(mfcc_double Mel);

mfcc_BOOL VectorListFree(mfcc_VECTORLIST *vectorlist);
mfcc_BOOL VectorListCreate(mfcc_VECTORLIST *vectorlist, mfcc_UINT vector_number, mfcc_UINT vector_dimension);

double distdtw (mfcc_VECTORLIST x, mfcc_VECTORLIST y);

#ifdef __cplusplus
}
#endif

#endif // __HEADER_MFCC_H


 
