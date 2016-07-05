
#include "SimpleFbaAction.h"

// C++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>



namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SimpleFbaAction::~SimpleFbaAction()
{
	// Added by KStankovic
	if (fbaps != NULL)
	//
		delete fbaps;
}

SimpleFbaAction::SimpleFbaAction()
{
	fbaps = new FBAPs();
}


FBAPs *SimpleFbaAction::getFBAPs(long globalTime, FBAPs *lastFBAPs, VisageCharModel *model) 
{
	long localTime = globalTime - globalStartTime;

	// here should come the code to update the fbaps as a function of localTime

	return fbaps;
};


void SimpleFbaAction::start(long globalTime)
{
	globalStartTime = globalTime;

	// here should come any necessary initializations to be performed when the action starts
}

void SimpleFbaAction::stop()
{
	fbaps->reset(); // reset the fbaps to neutral position

	// here should come any other necessary actions to be performed when the action stops
}

}
