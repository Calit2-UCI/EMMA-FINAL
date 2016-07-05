
#ifndef __CVisemes_h__
#define __CVisemes_h__

#include "FbaAction.h"
//#include "FBAEncoder.h"
#include "mfcc.h"
#include "NeuralNetwork.h"


#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

// C++
#include <vector>

#define OUT_BUFFER_SIZE 1000


namespace VisageSDK
{

using namespace std;

typedef vector<FBAPs*> ALLFBAPS;
typedef vector<int> ALLVISEMES;

class VisageLipSync;

class CVisemes  
{
public:
	CVisemes(VisageLipSync *lipSync);
	virtual ~CVisemes();

	int* PreprocessWavFile(char* strFileName);
	int* CalculateVisemes(char* strFileName);
	void constructStreamingSound();
//	void HandleFrames(DWORD wBufSize, short* wBuf, bool* finished);
		void HandleFrames(unsigned long wBufSize, short* wBuf, bool* finished,  int fs);
		void HandleFrames(unsigned long wBufSize, unsigned char* wBuf, bool* finished,  int fs);
	void StoreAnimation(); //izbaciti ? G.
	void StoreAnimation3(); //added by G. - vise ne treba
	void FinalizeStoreAnimation(); //added by G.
	void InitStoreAnimation(); //added by G.
	void LastFrameRender();
	void ClearAll();
	void FreeAllGeneratedNNetworks();

	void writeBits(int streamHandle, unsigned char *bits, int size);
	long encode_frame(int v1,int v2,int blend);
	void encode_interval(int v2, long ts1, long ts2);
	void encode_viseme(int v, long ts, int isLastFrame);

	void set_audioBuf(short* wBuf){aBuf = wBuf;};
	short* get_audioBuf(){return aBuf;};


private:
	VisageLipSync *lipSync;
	FbaAction *fa;
	char	*af;
	int		viseme;
	int		last_viseme; //A.C. added, to check which is last viseme being processed to encode_frame function
	int		frameBeggining; //time when viseme occured
	int		numVisemes;
	long	last_ts;
	long	ts;
	int		ev1;
	int		ev2;
	float	eb;
	int		byte;
	int		bitcnt;
	CFBAEncoder *encoder;
	int		fbaFileHandle;
	//unsigned char outBuffer[300];
	unsigned char outBuffer[OUT_BUFFER_SIZE];
	int		size;

	int*	realVisemeVector;
	FBAPs*  realFBAPsVector;

	ALLVISEMES  allVisemes;
	ALLVISEMES::iterator allVisemesIterator;

	ALLFBAPS  allFBAPs;
	ALLFBAPS::iterator allFBAPsIterator;

	long last_tms;
	FBAPs *tempFBAPs;
	int frameCount;

	short* aBuf;

public:

	NET n0;
	NET n1;
	NET n2;
	NET n3;
	NET n4;
	NET n5;
	NET n6;
	NET n7;
	NET n8;
	NET n9;
	NET n10;
	NET n11;
	NET n12;
	NET n13;
	NET n14;

	double 	Classes[15];
	double largest;

	long framecount;
	long framecount_mod;

};

}


#endif // __CVisemes_h__
