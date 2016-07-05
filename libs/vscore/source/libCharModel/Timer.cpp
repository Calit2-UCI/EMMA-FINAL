
#include "Timer.h"

#include <time.h>

namespace VisageSDK
{

Timer::Timer()
{
	ticks0 = 0;
	stopped = true;
}

void Timer::start()
{
	//ticks0 = getTickCount();
	ticks0 = clock();

	stopped = false;
}

void Timer::stop()
{
	stopped = true;
}

void Timer::reset()
{
	ticks0 = 0;
	if( stopped == false )
		start();
}

bool Timer::isRunning() const
{
	return !stopped;
}

Timer::Time Timer::getTime() const
{
	if( ticks0 <= 0 )
		return Timer::Time();

	Timer::Time ct;
	//unsigned long dticks = GetTickCount() - ticks0;
	//ct.hr = dticks / 3600000;
	//ct.min = dticks / 60000 - ct.hr * 60;
	//ct.s = dticks / 1000 - ct.hr * 3600 - ct.min * 60;
	//ct.ms = dticks - ct.hr * 3600000 - ct.min * 60000 - ct.s * 1000;
	clock_t dticks = clock() - ticks0;
	double dseconds = (dticks / CLOCKS_PER_SEC);
	ct.hr = (unsigned long)( dseconds /3600 );
	ct.min =(unsigned long) ( dseconds /60 - ct.hr * 60 );
	ct.s = (unsigned long) ( dseconds - ct.hr * 3600 - ct.min * 60 );
	ct.ms = (unsigned long)( dseconds * 1000 - ct.hr * 3600000 - ct.min * 60000 - ct.s * 1000 );

	return ct;
}

string Timer::getTimeStr() const
{
	Timer::Time ct = getTime();

	return toString<unsigned long>( ct.hr ) + ":" +
		toString<unsigned long>( ct.min ) + ":" +
		toString<unsigned long>( ct.s ) + "." +
		toString<unsigned long>( ct.ms );
}

//unsigned long Timer::getTickCount() const
//{
//	unsigned long t0 = 0;
//
//	#ifdef WIN32
//	t0 = GetTickCount();
//	#endif
//
//	return t0;
//}

}
