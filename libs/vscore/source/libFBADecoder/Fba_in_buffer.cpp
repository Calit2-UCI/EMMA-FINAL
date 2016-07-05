
#include "Fba_in_buffer.h"

#include "FBADecoder.h"

// C++
#include <stdio.h>
#include <stdlib.h>
#ifdef _DEBUG
#include <assert.h>
#endif

// Win32
#ifdef WIN32
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fba_in_buffer::Fba_in_buffer()
{
}

Fba_in_buffer::Fba_in_buffer(int inputstream)
{
	init(inputstream);
}

Fba_in_buffer::~Fba_in_buffer()
{
}
	
void Fba_in_buffer::closeStream()
{
	if(inStream)
		close(inStream);
}

/*
   This function is called right before fix length string is about to
   be output after arithmetic coding
*/
void Fba_in_buffer::adjust_bits()
{
	int incnt_save;
	p -= 3;
	if(p<0)
		p+=buffSize_FBA;
	incnt_save = incnt;
	incnt = 0;
	getbits(8 - incnt_save + 2);
	bitcnt -= (8 - incnt_save + 2);
	bitcnt -= 14;
}

int Fba_in_buffer::cntbits()
{
	return bitcnt;
}

/* This function should only be called by getbits() */
int Fba_in_buffer::getbit()
{
	int t;
	int loadLength;

	//fill the buffer from stream if needed
	int d = inp - p;
	loadLength = buffSize_FBA - inp;
	if (d < 0)
	{
		d = buffSize_FBA + d;
		loadLength /= 2;
	}
	if (d < 100)
	{
		#ifdef WIN32
			int l = _read(inStream,buf+inp,loadLength); //java: inputStream.read(buf, inp, loadLength)
		#else
			int l = read(inStream,buf+inp,loadLength); //java: inputStream.read(buf, inp, loadLength)
		#endif
		if (l > 0)
			inp += l;
		if (inp == buffSize_FBA)
		{
			inp = 0;
			/*			for (int i = 0; i < 10; i++)
			buf[i] = buf[buffSize_FBA + i - 10];*/
		}
	}

	if (incnt == 0)
	{
		inbfr = buf[p++];
		if (p == buffSize_FBA)
			p = 0;
		incnt = 8;
	}
	t = (inbfr & 0x80) >> 7;
	inbfr <<= 1;
	incnt -= 1;
	return t;
}

int Fba_in_buffer::getbits(int n)
{
	int i;
	int ibuf = 0;
	for (i = 0; i < n; i++)
	{
		/* MSB first */
		ibuf <<= 1;
		ibuf |= getbit();
	}
	bitcnt += n;
	return ibuf;
}

void Fba_in_buffer::init(int inputstream)
{
	inStream = inputstream;
	initbits();
}

void Fba_in_buffer::initbits()
{
	incnt = 0;
	p = 0;
	inp = 0;
	bitcnt = 0;
}

void Fba_in_buffer::next_start_code() 
{
	if(incnt==0) 
	{
		getbits(8);
	} 
	else 
	{
		getbits(incnt);
	}
}

int Fba_in_buffer::nextbits_bytealigned()
{
	int incnt_save;
	unsigned char inbfr_save;
	int sc;
	int flag = 0;
	incnt_save = incnt;
	inbfr_save = inbfr;
	if (incnt == 0)
	{
		getbits(8);
		flag = 1;
	}
	getbits(incnt);
	sc = getbits(32);
	incnt = incnt_save;
	inbfr = inbfr_save;
	if (flag == 1)
	{
		p -= 5;
	}
	else
	{
		p -= 4;
	}
	if(p<0)
		p+=buffSize_FBA;
	return sc;
}

void Fba_in_buffer::resetbits()
{
	bitcnt = 0;
}

}
