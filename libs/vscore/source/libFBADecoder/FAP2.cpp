
#include "FAP2.h"

namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FAP2::FAP2()
{
	reset();
}

FAP2::FAP2(FAP2 *f)
{
	reset();
	set(f);
}

FAP2::FAP2(FAP2 *f, double in)
{
	reset();
	set(f,in);
}

FAP2::~FAP2()
{
	
}

void FAP2::set(FAP2 *f, double in)
{
	expression1 = f->expression1;
	expression2 = f->expression2;
	intensity1 = (int)(f->intensity1 * in);
	intensity2 = (int)(f->intensity2 * in);
	initBit = f->initBit;
	defBit = f->defBit;
}

void FAP2::set(const FAP2 *f)
{
	expression1 = f->expression1;
	expression2 = f->expression2;
	intensity1 = f->intensity1;
	intensity2 = f->intensity2;
	initBit = f->initBit;
	defBit = f->defBit;
}

bool FAP2::isEqual(FAP2 *f)
{
	if(expression1 != f->expression1) return false;
	if(expression2 != f->expression2) return false;
	if(intensity1 != f->intensity1) return false;
	if(intensity2 != f->intensity2) return false;
	if(initBit != f->initBit) return false;
	if(defBit != f->defBit) return false;
	return true;
}

void FAP2::merge(FAP2 *f)
{
	if(isVoid() && !f->isVoid())
		set(f);
}

int FAP2::isVoid()
{
	if(expression1 == 0 && expression2 == 0)
		return(1);
	return(0);
}

void FAP2::reset()
{
	expression1 = 0;
	expression2 = 0;
	intensity1 = 0;
	intensity2 = 0;
	initBit = 0;
	defBit = 0;
}

}
