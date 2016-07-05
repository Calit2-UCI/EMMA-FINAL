
#ifndef __Fba_in_buffer_h__
#define __Fba_in_buffer_h__

#ifdef VISAGE_STATIC
	#define VISAGE_DECLSPEC
#else
	#if defined(MAC) || defined(LINUX)
		#define VISAGE_DECLSPEC __attribute__((__visibility__("default")))

	#else
		#ifdef VISAGE_EXPORTS
			#define VISAGE_DECLSPEC __declspec(dllexport)
		#else
			#define VISAGE_DECLSPEC __declspec(dllimport)
		#endif
	#endif

#endif

#include <unistd.h>


namespace VisageSDK
{

#define buffSize_FBA 1000

class VISAGE_DECLSPEC Fba_in_buffer  
{
public:
	Fba_in_buffer();
	Fba_in_buffer(int inputstream);
	virtual ~Fba_in_buffer();
	void next_start_code() ;
	int nextbits_bytealigned();
	int getbits(int n);
	void init(int inputstream);
	void adjust_bits();
	void resetbits();
	int cntbits();
	
	void closeStream();


private:
	int getbit();
	void initbits();

	int inStream;
	unsigned char buf[buffSize_FBA];
	unsigned char inbfr;
	int p;
	int inp;
	int incnt;
	int bitcnt;
};

} // __Fba_in_buffer_h__

#endif // __Fba_in_buffer_h__
