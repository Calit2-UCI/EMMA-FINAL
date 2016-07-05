
#include "FAP1.h"

namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FAP1::FAP1()
{
	reset();
}

FAP1::FAP1(FAP1 *f)
{
	reset();	
	set(f);
}

FAP1::~FAP1()
{
	
}

void FAP1::set(const FAP1 *f)
{
	viseme1 = f->viseme1;
	viseme2 = f->viseme2;
	blend = f->blend;
	defBit = f->defBit;
}

bool FAP1::isEqual(FAP1 *f)
{
	if(viseme1 != f->viseme1) return false;
	if(viseme2 != f->viseme2) return false;
	if(blend != f->blend) return false;
	if(defBit != f->defBit) return false;
	return true;
}

void FAP1::merge(FAP1 *f)
{
	if(isVoid() && !f->isVoid())
		set(f);
}

int FAP1::isVoid()
{
	if(viseme1 == 0 && viseme2 == 0)
		return(1);
	return(0);
}

void FAP1::reset()
{
	viseme1 = 0;
	viseme2 = 0;
	blend = 0;
	defBit = 0;
}

}
