#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#ifndef BOOL
typedef unsigned short BOOL;
#endif
#ifndef UINT
typedef unsigned int UINT;
#endif

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#include <math.h>
#include <string.h>


#ifndef PI
#define PI 3.14159265358979323846264
#endif
#ifndef log2
#define log2(x) (log(x)/0.6931471805599453)
#endif
#ifndef log10
#define log10(x) (log(x)/2.302585092994045)
#endif
#ifndef ln
#define ln(x) log(x)
#endif


#include <stdio.h>
#include <stdlib.h>
//#include <memory.h>

#include "mfcc.h"
#include "memalloc.h"
#include "fftsg_h.c"

mfcc_double PreAccent(mfcc_double z);
#define PREACCENT_FACTOR 0.9
#define NOPREACCTENT

// "manual" idct algorthm: formula:
// f_i=2/N * sum(F_k*cos(PI*k*(i+1/2)/N), k, 0, N-1), for all i from 0 to N-1
// thanks to "Numerical Poems in C" book, page 519
__inline void idct(int size, mfcc_double * data_in, mfcc_double * data_out)
{
	memset(data_out,0,sizeof(mfcc_double)*size);
	for ( int i=0; i<size; ++i )
	{
		for ( int k=0 ; k<size ; ++k )
			 data_out[i] += data_in[k]*cos((mfcc_double)(PI*k*(i+0.5)/size));
		data_out[i] *= (mfcc_double) 2.0/size;
	}

}


/********************************************************************************************************************
	SIGNAL definition & function
********************************************************************************************************************/
//typedef struct SIGNAL_tag
//{
//	mfcc_double *signal_data;
//	mfcc_UINT signal_data_lenght;
//}SIGNAL;

/********************************************************************************************************************
	VECTOR & VECTORLIST definition & function
********************************************************************************************************************/
// VECTOR : one accoustical vector (a column in spectre)
//typedef struct VECTOR_tag
//{
//	mfcc_double *data;
//	mfcc_UINT data_dimension;
//}VECTOR;



mfcc_BOOL VectorFree(mfcc_VECTOR *vector)
{
	if ((*vector).data_dimension==0)
		return true;
	MEMFREE((*vector).data);
	(*vector).data_dimension=0;
	return true;
}

mfcc_BOOL VectorCreate(mfcc_VECTOR *vector, mfcc_UINT n)
{
	if ((*vector).data_dimension !=0 )
		MEMFREE( (*vector).data );
	(*vector).data = (mfcc_double*) MEMALLOC(sizeof(mfcc_double) * n);
	if (!(*vector).data)
		return false;
	(*vector).data_dimension=n;
	return true;
}

/*****/

// VECTORLIST : list of VECTOR (~ a spectre)
//typedef struct VECTORLIST_tag
//{
//	VECTOR *vector;
//	mfcc_UINT vector_number;
//	mfcc_UINT vector_dimension;
//}mfcc_VECTORLIST;


mfcc_BOOL VectorListCreate(mfcc_VECTORLIST *vectorlist, mfcc_UINT vector_number, mfcc_UINT vector_dimension)
{
	if ((*vectorlist).vector_number!=0)
		VectorListFree(vectorlist);
	(*vectorlist).vector = (mfcc_VECTOR*) MEMALLOC( sizeof( mfcc_VECTOR ) * vector_number );
	if ((*vectorlist).vector == 0x00)
		return false;
	for ( mfcc_UINT i = 0 ; i < vector_number ; ++i)
	{
		if (!VectorCreate(&((*vectorlist).vector[i]), vector_dimension))
			return false;
	}
	(*vectorlist).vector_number=vector_number;
	(*vectorlist).vector_dimension=vector_dimension;
	return true;
}

mfcc_BOOL VectorListFree(mfcc_VECTORLIST *vectorlist)
{
	if ((*vectorlist).vector_number==0)
		return true;
	for (mfcc_UINT i=0; i<(*vectorlist).vector_number ; ++i)
	{
		if (!VectorFree(&((*vectorlist).vector[i])))
			return false;
	}
	MEMFREE((*vectorlist).vector);
	(*vectorlist).vector_number=0;
	return true;
}

/********************************************************************************************************************
	DTW functions
********************************************************************************************************************/
/* Compute the Euclidian distance between vector x and vector y (must have same dimension).
 *  Fomula: sqrt(sum((x-y).^2));
 *      _______________
 *     |  n
 *     | __           2
 *     | \  | x  - y |
 * r = | /_ |  i    i|
 *    \| i=0
 *
 */
mfcc_double disteucl(mfcc_VECTOR x, mfcc_VECTOR y)
{
	mfcc_double r=0;
	for (mfcc_UINT i = 0 ; (i < x.data_dimension) && (i < y.data_dimension) ; ++i )
	{
		r += pow(x.data[i] - y.data[i],2);
	}
	return sqrt(r);
}

#define DTW_INFINITY 100000
#define min(a, b)  (((a) < (b)) ? (a) : (b)) 

/* Return a "distance" between 2 set of accoustical vectors
 * Lists can be different in size, BUT vectors must have same
 * dimension
 * Use DTW algorithm (Dynamic Time Wrapping)
 */
mfcc_double distdtw (mfcc_VECTORLIST x, mfcc_VECTORLIST y)
{
	mfcc_double **g;
	mfcc_double d;
	mfcc_UINT i,j;
	mfcc_UINT I = x.vector_number,
		J = y.vector_number;


	/*{
		FILE *f = fopen ("cmp1.txt", "wt");
		char buf[256];
		fwrite("cmp1=[",sizeof(char),6,f);
		for (i = 0; i < x.vector_number; ++i)
		{
			for (int j = 0; j < (int) x.vector_dimension; ++j)
			{
				fwrite(" ",sizeof(char), 1,f);
				sprintf(buf,"%.8f",x.vector[i].data[j]);
				fwrite(" ",sizeof(char), 1,f);
				fwrite(buf,sizeof(char), strlen(buf),f);
			}
			fwrite("; ",sizeof(char), 2,f);
		}
		fwrite("];",sizeof(char), 2,f);
		fclose(f);
	}*/
	/*{
		FILE *f = fopen ("cmp2.txt", "wt");
		char buf[256];
		fwrite("cmp2=[",sizeof(char),6,f);
		for (i = 0; i < y.vector_number; ++i)
		{
			for (int j = 0; j < (int) y.vector_dimension; ++j)
			{
				fwrite(" ",sizeof(char), 1,f);
				sprintf(buf,"%.8f",y.vector[i].data[j]);
				fwrite(" ",sizeof(char), 1,f);
				fwrite(buf,sizeof(char), strlen(buf),f);
			}
			fwrite("; ",sizeof(char), 2,f);
		}
		fwrite("];",sizeof(char), 2,f);
		fclose(f);
	}*/


	// First: create a double matrix:
	// line : size of x
	// column : size of y
	g = (mfcc_double **) MEMALLOC( (I + 1) * sizeof( mfcc_double ) );
	for ( i = 0 ; i < I + 1 ; ++i)
	{
		g[i] = (mfcc_double *) MEMALLOC( (J + 1 ) * sizeof( mfcc_double ) );
	}
	// first line and first colunm filled by "INFINITY" number
	for ( i = 0 ; i < I + 1 ; ++i)
	{
		g[i][0] = DTW_INFINITY;
	}
	for ( j = 0 ; j < J + 1 ; ++j)
	{
		g[0][j] = DTW_INFINITY;
	}
	// first element (0,0) must be 0
	g[0][0] = 0;

	// seek matrix line by line
	for (i = 1 ; i < I + 1; ++i)
	{
		for (j = 1 ; j < J + 1; ++j)
		{
			// compute euclidian distance between the
			// i-1'th element of x and the i'th element
			// of y (euclidian distance can be computed
			// because vectors have same dimensions)
			d = disteucl(x.vector[i-1],y.vector[j-1]);
			// put the min value in g(i,j):
			g[i][j] = min(min(	g[i-1][j] + d,
								g[i-1][j-1] + d + d ),
								g[i][j-1] + d);
		}
	}
	mfcc_double r = (g[I][J]) / (I + J);
	for ( i = 0 ; i < I + 1 ; ++i)
	{
		MEMFREE(g[i]);
	}
	MEMFREE(g);
	return r;
}

/********************************************************************************************************************
	MFCC Functions
********************************************************************************************************************/

/* Create Mel Frequecy Cepstral Coefficients
 * s : signal to create MFCC's from
 * k : number of coefficients to create
 * n : size of hamming windows (must be a power of 2)
 * c : list of VECTOR which will receive MFCC's
 *     MUST BE UNINITIALIZED!
 */
mfcc_BOOL Mfcc(mfcc_SIGNAL s, mfcc_UINT k,mfcc_UINT n, mfcc_VECTORLIST *c)
{
	const mfcc_double fl=0,
		fh = 0.5;
	/* Creating Frames */
	mfcc_VECTORLIST frames;
	mfcc_VECTORLIST fftframes;
	mfcc_VECTORLIST idctframes;
	mfcc_UINT frame_number;
	mfcc_UINT i,j;

	frames.vector_number=0;
	frames.vector=0;
	frames.vector_dimension=0;

	if (s.signal_data_lenght < n /2)
	{
		return false;
	}


	frame_number = (mfcc_UINT) (s.signal_data_lenght / (n / 2) ) - 1;

	VectorListCreate(&frames, frame_number, n);

	for ( i = 0 ; i < frame_number; ++i)
	{
		for ( j = ((int)(n/2)) * i; j < ((int)(n/2)) * i + n; ++j )
		{
			// hamming ponderation
 			frames.vector[i].data[j - ((int)(n/2)) * i] = PreAccent(s.signal_data[j]) *  HammingPonder(n,j - ((int)(n/2)) * i);
		}
		frames.vector[i].data_dimension=n;
		/*{
			FILE *f = fopen ("frame.txt", "wt");
			char buf[256];
			fwrite("f=[",sizeof(char),3,f);
			for (int j = 0; j < (int) frames.vector[i].data_dimension; ++j)
			{
				fwrite(" ",sizeof(char), 1,f);
				sprintf(buf,"%.8f",frames.vector[i].data[j]);
				fwrite(" ",sizeof(char), 1,f);
				fwrite(buf,sizeof(char), strlen(buf),f);
				fwrite("; ",sizeof(char), 2,f);
			}
			fwrite("];",sizeof(char), 2,f);
			fclose(f);
		}*/

	}
	/* frame created*/

	/* computing FFT of each frame */
	fftframes.vector_number=0;
	fftframes.vector=0;
	fftframes.vector_dimension=0;
	VectorListCreate(&fftframes, frame_number,n);
	for ( i = 0 ; i < frame_number; ++i)
	{
		memcpy(fftframes.vector[i].data,frames.vector[i].data,n*sizeof(mfcc_double));
		rdft(n,1,fftframes.vector[i].data);
/*		{
			FILE *f = fopen ("fftframe.txt", "wt");
			char buf[256];
			fwrite("ft=[",sizeof(char),3,f);
			for (int j = 0; j < (int) fftframes.vector[i].data_dimension; ++j)
			{
				fwrite(" ",sizeof(char), 1,f);
				sprintf(buf,"%.8f",fftframes.vector[i].data[j]);
				fwrite(" ",sizeof(char), 1,f);
				fwrite(buf,sizeof(char), strlen(buf),f);
				fwrite("; ",sizeof(char), 2,f);
			}
			fwrite("];",sizeof(char), 2,f);
			fclose(f);
		}*/
	}
	// warning: fftframes.vector[i].data[2*j] => Real part of fft
	//			fftframes.vector[i].data[2*j+1] => Im part of fft
	/* frame FFT computed */

/*	{
		FILE *f = fopen ("spectre.txt", "wt");
		char buf[256];
		fwrite("s=[",sizeof(char),3,f);
		for (i = 0; i < fftframes.vector_number; ++i)
		{
			for (int j = 0; j < (int) floor(fftframes.vector_dimension/2); ++j)
			{
				fwrite(" ",sizeof(char), 1,f);
				sprintf(buf,"%.8f", sqrt(pow((fftframes.vector[i].data[2*j]),2) + pow((fftframes.vector[i].data[2*j+1]),2)));
				fwrite(" ",sizeof(char), 1,f);
				fwrite(buf,sizeof(char), strlen(buf),f);
			}
			fwrite("; ",sizeof(char), 2,f);
		}
		fwrite("];",sizeof(char), 2,f);
		fclose(f);
	}*/


	/* Creating Mel Filter bank */

    // création du banc de filtre (techniquement, il s'agit 
	// de k filtres passe bandes (de forme Hamming)
	// conversion de l'axe des abscisses (fréquences)
	// de Hertz en Mel
	mfcc_double melsize = Frq2Mel(s.fs/2);
	// en effet, dans cet échelle, le découpage sera
	// linéraire (donc de taille constante == melfiltersize!)
	// taille de chacun des filtres dans l'échelle des mels
	mfcc_double melfiltersize = 2 * (melsize / (k+1 +1));
	// explication: si k+1 est le nombre de filtre qu'il faut créer 
	// (car le 0ème est inutile donc il sera virer)
	// on veut faire un découpage de l'étendu melsize pour y mettre les
	// k+1 filtres mais qui se recouvrent 2 à deux par moitier:
	/* pour k+1=12 filtres:
        |------||------||------||------||------||------|
	|------||------||------||------||------||------|
    .0                                                 .melsize
	ou pour k+1=12 filtres:
	    |------||------||------||------||------||------|
	|------||------||------||------||------||------||------|
	.0                                                     .melsize
	si on découpe chaque étendu de filtre (|-----|) en 2
	on se rencompte qu'il y aura (k+1) + 1 "mini-filtres":
	pour
        |------||------||------||------||------||------|
	|------||------||------||------||------||------|
	on a
	|--||--||--||--||--||--||--||--||--||--||--||--||--| 13 mini-filtres
	un filtre sera 2 fois plus gros.
    */
	typedef struct MELFILTERREP_tag {mfcc_double begin_point , end_point;} MELFILTERREP;
	MELFILTERREP *melfilterrep;
	melfilterrep=(MELFILTERREP *) MEMALLOC(sizeof(MELFILTERREP)*(k+1));
	for (i = 0 ; i < k+1 ; ++i )
	{
		melfilterrep[i].begin_point = (i * (melfiltersize / 2));
		melfilterrep[i].end_point =  (i *  (melfiltersize / 2)) + melfiltersize;
	}

/*	{
		FILE *f = fopen ("melfilterrep.txt", "wt");
		char buf[256];
		fwrite("m=[",sizeof(char),3,f);
		for (i = 0; i < k+1; ++i)
		{
			fwrite(" ",sizeof(char), 1,f);
			sprintf(buf,"%.8f",melfilterrep[i].begin_point);
			fwrite(" ",sizeof(char), 1,f);
			fwrite(buf,sizeof(char), strlen(buf),f);
			fwrite(", ",sizeof(char), 2,f);
			fwrite(" ",sizeof(char), 1,f);
			sprintf(buf,"%.8f",melfilterrep[i].end_point);
			fwrite(" ",sizeof(char), 1,f);
			fwrite(buf,sizeof(char), strlen(buf),f);
			fwrite("; ",sizeof(char), 2,f);
		}
		fwrite("];",sizeof(char), 2,f);
		fclose(f);

	}*/

	mfcc_VECTORLIST melfilterbank;
	melfilterbank.vector_number=0;
	VectorListCreate(&melfilterbank,k+1, n/2);

	// creating mel filter bank
	// note: it's between frequencies 0 and n/2
	// cause of mirror effect (from FFT)
	mfcc_double a;
	mfcc_double b;

	for ( i = 0 ; i < k+1 ; ++i )
	{
		a = Mel2Frq(melfilterrep[i].begin_point) / (s.fs/2) * n/2;
		b = Mel2Frq(melfilterrep[i].end_point) / (s.fs/2) * n/2;
		memset(melfilterbank.vector[i].data,0,sizeof(mfcc_VECTOR));
		for (j = (mfcc_UINT) floor(a) ; j < (mfcc_UINT) floor(b) ; ++j)
		{
			melfilterbank.vector[i].data[j] = 
				HammingPonder((mfcc_UINT) (int) floor( floor(b) - floor(a) ),
					(mfcc_UINT) (int) floor(j - floor(a)));
		}
	}

	MEMFREE(melfilterrep);
	/*{
		FILE *f = fopen ("melfilterbank.txt", "wt");
		char buf[256];
		fwrite("mc=[",sizeof(char),4,f);
		for (i = 0; i < melfilterbank.vector_number; ++i)
		{
			for (int j = 0; j < (int) melfilterbank.vector_dimension; ++j)
			{
				fwrite(" ",sizeof(char), 1,f);
				sprintf(buf,"%.8f",melfilterbank.vector[i].data[j]);
				fwrite(" ",sizeof(char), 1,f);
				fwrite(buf,sizeof(char), strlen(buf),f);
			}
			fwrite("; ",sizeof(char), 2,f);
		}
		fwrite("];",sizeof(char), 2,f);
		fclose(f);
	}*/
	/* Mel Filter bank created */
	mfcc_VECTORLIST melframes;
	mfcc_UINT l;

	melframes.vector_number=0;
	melframes.vector=0;
	melframes.vector_dimension=0;
	VectorListCreate(&melframes, frame_number, k+1);
	// doing matrix multiplication 
	// for each frame
	for (i = 0; i < frame_number ; ++i)
	{
		// computing the mel frenquency scaled coefficients
		// computing each coefficient
		for (j = 0 ; j < k+1; ++j)
		{
			//doing c_ij = sum (a_il * b_lj, l=1..n/2)
			for (l = 0; l < (mfcc_UINT) n / 2; ++l)
			{
				/*  Computing coefficient from power spectrum: 
                                    _____________________        2
						        /  |                          \
				formula: ... x (   |  Re(fft)^2 + Im(fft)^2    )  ^         =  Re(fft)^2 + Im(fft)^2
							    \ \|                          /   |
								\_____________________________/   |
								         module                   power
										                           
				*/
				melframes.vector[i].data[j] += melfilterbank.vector[j].data[l] * ( pow(fftframes.vector[i].data[2*l], 2) + pow(fftframes.vector[i].data[2*l+1], 2) );
			}
			// take logarithm
			melframes.vector[i].data[j] = log10(melframes.vector[i].data[j]);
		}
	}

	// note: the 0'th coefficient seems to be always null (should not).
	// it's not important, because we will not use it (use the k coefficents between
	// 1'th and k+1'th coeff (corresponding to MFCC theory, 0'th coeff is unsual because it only
	// supplies energy information)
	// TODO: fix problem with 0'th coeff
	// DONE
/*	{
		FILE *f = fopen ("name.txt", "wt");
		char buf[256];
		fwrite("ms=[",sizeof(char),4,f);
		for (i = 0; i < melframes.vector_number; ++i)
		{
			for (int j = 0; j < (int) melframes.vector_dimension; ++j)
			{
				fwrite(" ",sizeof(char), 1,f);
				sprintf(buf,"%.8f ",melframes.vector[i].data[j]);
				fwrite(" ",sizeof(char), 1,f);
				fwrite(buf,sizeof(char), strlen(buf),f);
			}
			fwrite("; ",sizeof(char), 2,f);
		}
		fwrite("];",sizeof(char), 2,f);
		fclose(f);

		FILE *f1 = fopen ("name1.txt","wt");

		for (i = 0; i < (int) melframes.vector_dimension; ++i)
		{
			for (int j = 0; j < melframes.vector_number; ++j)
			{
				fwrite("  ",sizeof(char), 1,f);
				if (melframes.vector[j].data[i] < 0){
					sprintf(buf,"%.8f ",melframes.vector[j].data[i]);
					fwrite("  ",sizeof(char), 1,f);
				}
				else {
					sprintf(buf,"%.8f ",melframes.vector[j].data[i]);
					fwrite("   ",sizeof(char), 1,f);
				}
				fwrite(buf,sizeof(char), strlen(buf),f);
			}
			fwrite("\n ",sizeof(char), 2,f);
		} 
		fclose(f1);
	}*/

	/* Computing idct(log|.|) of each frame */
	idctframes.vector_number=0;
	idctframes.vector=0;
	idctframes.vector_dimension=0;
	VectorListCreate(&idctframes, frame_number, k+1);

	for ( i = 0 ; i < frame_number; ++i)
	{
		//memcpy(idctframes.vector[i].data,melframes.vector[i].data,(k+1)*sizeof(double));
/*		{
			char buf[256];
			FILE *f = fopen ("idctbefore.txt", "wt");
			fwrite("cb=[",sizeof(char),4,f);
			for (int j = 0; j < (int) idctframes.vector[i].data_dimension; ++j)
			{
				fwrite(" ",sizeof(char), 1,f);
				sprintf(buf,"%.5f ;",idctframes.vector[i].data[j]);
				fwrite(" ",sizeof(char), 1,f);
				fwrite(buf,sizeof(char), strlen(buf),f);
			}
			fwrite("];",sizeof(char), 2,f);
			fclose(f);
		}
*/
//        idctframes.vector[i].data[0] *= 0.5;
//		ddct(k+1,1,idctframes.vector[i].data); // idct
		idct(k+1,melframes.vector[i].data, idctframes.vector[i].data); // idct
//        for (j = 0; j < (k+1); ++j) 
//		{
//            idctframes.vector[i].data[j] *= 2.0 / (k+1);
//		}
		//ddct(k+1,-1,idctframes.vector[i].data);

/*		{
			char buf[256];
			FILE *f = fopen ("idctafter.txt", "wt");
			fwrite("ca=[",sizeof(char),4,f);
			for (int j = 0; j < (int) idctframes.vector[i].data_dimension; ++j)
			{
				fwrite(" ",sizeof(char), 1,f);
				sprintf(buf,"%.5f ;",idctframes.vector[i].data[j]);
				fwrite(" ",sizeof(char), 1,f);
				fwrite(buf,sizeof(char), strlen(buf),f);
			}
			fwrite("];",sizeof(char), 2,f);
			fclose(f);

			FILE *f1 = fopen ("idctafter1.txt", "wt");
			int b = 0;
		
			for (j = 0; j < (int) idctframes.vector[i].data_dimension; ++j)
			{
				
				if(b!=0){
				fwrite(" ",sizeof(char), 1,f1);
				sprintf(buf,"%.5f ",idctframes.vector[i].data[j]);
				fwrite("\n ",sizeof(char), 2,f1);
				fwrite(" ",sizeof(char), 1,f1);
				fwrite(buf,sizeof(char), strlen(buf),f1);
				}
				b = 1;
			}
			
	
		fclose(f1);


		}*/

	
	}
	/* MFCC created!!! */

	c->vector_number=0;
	c->vector=0;
	c->vector_dimension=0;
	VectorListCreate(c,frame_number,k);
	for ( i = 0 ; i < frame_number; ++i)
	{
		for ( j = 1 ; j < k + 1; ++j) // do not take the first coefficient! 
		{
			c->vector[i].data[j-1]=idctframes.vector[i].data[j];
		}
	}
	VectorListFree(&frames);
	VectorListFree(&fftframes);
	VectorListFree(&idctframes);
	VectorListFree(&melfilterbank);
	VectorListFree(&melframes);
	return true;
}

/********************************************************************************************************************
	Misc Functions
********************************************************************************************************************/

/* Return the i'th point of a
 * hamming window (dimension : n samples)
 */
mfcc_double HammingPonder(mfcc_UINT n, mfcc_UINT i)
{
	mfcc_double v;
	v = 2 * PI / (n-1);
	mfcc_double t = 0.54 - 0.46 * cos( v * i );
	return 0.54 - 0.46 * cos( v * i );
}

/* Creating Hamming window
 *  ham : pointer to a allocated and empty table
 *  with n entries within.
 */
void Hamming(mfcc_double *ham,short n)
{
	short i;
	mfcc_double v;
	v = 2 * PI / (n-1);
	for ( i = 0 ; i < n ; ++i)
		*(ham + i) = 0.54 - 0.46 * cos( v * i );
	return;
}

/* Convert frequency f from Hertz to Mel
 */
mfcc_double Frq2Mel(mfcc_double f)
{
  return 2595.037 * log10(1.0 + f / 700.0);
}

/* Convert frequency f from Mel to Hertz
 */
mfcc_double Mel2Frq(mfcc_double Mel)
{
	mfcc_double f = 700.0 * (pow(10.0, Mel / 2595.037) - 1.0);
  return 700.0 * (pow(10.0, Mel / 2595.037) - 1.0);
}


/* apply the following formula to get high
 * frequencies stronger
 */
__inline mfcc_double PreAccent(mfcc_double z)
{
#ifdef NOPREACCTENT
	return z;
#else
	return (z)?(1 - PREACCENT_FACTOR * pow(z,-1)) :(0);
#endif
}

