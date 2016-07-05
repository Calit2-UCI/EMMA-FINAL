
#ifndef __SimpleFbaAction_h__
#define __SimpleFbaAction_h__

#include "FbaAction.h"

// C++
#include <stdlib.h>
#include <stdio.h>

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

namespace VisageSDK
{

/**
* A basic face and body animation action.
* This is the simplest implementation of an FbaAction, the abstract class representing a face and body animation action, i.e. a face and body
* animation sequence. 
*
* The main responsability of an FbaAction implementation is to be able to provide
* the set of face and body animation parameters for a given time of animation. To make the implementation
* of FbaAction subclasses more convenient, the basic class SimpleFbaAction is provided. It implements all
* required functions of the FbaAction, so the user just needs to extend it by inserting the mechanism
* for setting the Face and Body Animation Parameters in the getFBAPs() function. For convenience, full source code of
* the SimpleFbaAction class can be found in the src folder.
* 
*
*
* Implemented in visagefaplayer.lib
*
* Demonstrated in <a href="../Samples/Example1/doc/index.html">Example1</a> and <a href="../Samples/Example6/doc/index.html">Example6</a>.
*
* @see FbaAction
*/
class VISAGE_DECLSPEC SimpleFbaAction  : public FbaAction
{
public:
	/**
	* Constructor. Constructs an empty SimpleFbaAction.
	*/
	SimpleFbaAction();

	/**
	* Destructor. 
	*/
	virtual ~SimpleFbaAction();


	/**
	* Get FBAPs. This function returns the Face and Body Animation Parameters (FBAPs)
	* for the time globalTime. This function is called by the FAPlayer when the FbaAction is played in the FAPlayer.
	* This implementation just returns the locally stored fbaps without updating them. In your implementation, 
	* you should
	* implement the update mechanism that will update the fbaps based on the provided local time. The local
	* time is calculated by subtracting the reference start time from the passed global time. Therefore, the local time is simply
	* the time (in milliseconds) since the the action was started, i.e. since the start() method was called by the FAPlayer.
	*
	* @param globalTime the time for which the FBAPs are requested, in milliseconds
	* @param lastFBAPs the final FBAPs from the previous frame of animation; this implementation ignores this parameter
	* @param model the character model currently used in the player; this implementation ignores this parameter
	* @return the Face and Body Animation Parameters (FBAPs) for the requested time
	*/
	FBAPs *getFBAPs(long globalTime, FBAPs *lastFBAPs, VisageCharModel *model);


	/**
	* Start the action.
	* This function is called by the FAPlayer when the playing starts. It may be called many times. The current global time
	* is passed, so the FbaAction stores this as the reference start time. The start time is subsequently used
	* in getFBAPs() in order to calculate the localTime.
	*
	* @param globalTime the reference time when the action is started, in milliseconds
	*/
	void start(long globalTime);

	/**
	* Stop playing tracks. Reset the face animation track.
	*/
	void stop();


	/**
	* Return the name of the action type.
	*
	* @return the name of the action type (SimpleFbaAction). Modify the name in your implementation.
	*/
	char* actionTypeName() {return (char *)("SimpleFbaAction");};

	long globalStartTime;
	FBAPs *fbaps;
};

}

#endif // __SimpleFbaAction_h__
