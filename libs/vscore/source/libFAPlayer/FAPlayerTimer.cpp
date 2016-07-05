

#include "FAPlayerTimer.h"

// C++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include <sys/timeb.h>

namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FAPlayerTimer::~FAPlayerTimer()
{
}

FAPlayerTimer::FAPlayerTimer()
{
	currentTime = startTime = clockStartTime = 0L;
	currentFrame = 0;
	frameRate = 25;
	running = false;
	paused = false;
}


long FAPlayerTimer::getCurrentTime()
{
	return(currentTime);
}

int FAPlayerTimer::getCurrentFrame()
{
	return(currentFrame);
}

void FAPlayerTimer::start(long _startTime)
{
#ifdef WIN32
	struct _timeb timebuffer;
	_ftime( &timebuffer );
	clockStartTime = 1000 * (long) timebuffer.time + timebuffer.millitm;	//CHANGED BY I.K. added explicit cast
#else
	struct timeb timebuffer;
	ftime( &timebuffer );
	clockStartTime = 1000 * (long) timebuffer.time + timebuffer.millitm;	//CHANGED BY I.K. added explicit cast
#endif

	startTime = _startTime;
	currentTime = startTime;
	currentFrame = getFrame(currentTime);
	running = true;
	paused = false;
}

void FAPlayerTimer::start()
{
	start(0L);
}

void FAPlayerTimer::stop()
{
	running = false;
	paused = false;
	currentFrame = 0;
	currentTime = 0L;
}


void FAPlayerTimer::updateRT()
{
	if(paused)
		return;
	if(!running)
		return;

	#ifdef WIN32
		struct _timeb timebuffer;
		_ftime( &timebuffer );
	#else
		struct timeb timebuffer;
		ftime(&timebuffer);
	#endif
	long clockTime = 1000 * (long)timebuffer.time + timebuffer.millitm;	//CHANGED BY I.K. added explicit cast
	
	currentTime = (clockTime - clockStartTime) + startTime;
	currentFrame = (int)((currentTime * frameRate) / 1000);
}

void FAPlayerTimer::pause(bool pause)
{
	if(!running)
		return;
	if(paused == pause)
		return;

	paused = pause;

	if(!pause)
		start(currentTime);
}

void FAPlayerTimer::nextFrame()
{
	if(!running) return;
	if(paused) return;

	currentFrame++;
	currentTime = getTime(currentFrame);

}

void FAPlayerTimer::setFrame(int nfr)
{
	currentFrame=nfr;
	currentTime = getTime(nfr);
}

int FAPlayerTimer::getFrame(long t)
{
	return((t * (long)frameRate) / 1000L);

}

int FAPlayerTimer::getTime(int fr)
{
	long t;
	
	t = (long)(((long)fr * 1000L) / frameRate);

	return t;
}

void FAPlayerTimer::setFrameRate(float _frameRate)
{
	frameRate = _frameRate;
}

float FAPlayerTimer::getFrameRate()
{
	return(frameRate);

}

}
