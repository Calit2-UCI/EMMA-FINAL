
#include "LLFAPs.h"

namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LLFAPs::LLFAPs()
{
	reset();
}

LLFAPs::LLFAPs(LLFAPs *f)
{
	reset();
	set(f);
}

LLFAPs::LLFAPs(LLFAPs *f, double in)
{
	reset();
	set(f,in);
}

void LLFAPs::reset()
{
	for (int i = 0; i < NFAP; i++)
	{
		value[i] = 0;
	}
}

void LLFAPs::filter(LLFAPs *f)
{
	int i;
	for(i =0; i< NFAP;i++)
		value[i] *= (int)(f->value[i]/1000.0);
}

void LLFAPs::set(const LLFAPs *f)
{
	int i;
	for(i =0; i< NFAP;i++)
		value[i] = f->value[i];
}

void LLFAPs::merge(LLFAPs *f)
{
	int i;
	for(i =0; i< NFAP;i++)
	{
		if(value[i] == 0 && f->value[i]!=0)
		{
			value[i] += f->value[i];
		}
		else
			if(value[i] != 0 && f->value[i]!=0)
			{
				value[i] += f->value[i];
				value[i] /= 2;
			}
	}
}

void LLFAPs::set(LLFAPs *f, double in)
{
	int i;
	for(i =0; i< NFAP;i++)
	{
		value[i] = (int)(f->value[i] * in);
	}

}

LLFAPs::~LLFAPs()
{

}

}
