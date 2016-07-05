
#ifndef __SimpleVORAction_h__
#define __SimpleVORAction_h__

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
* An approximation of the vestibulo-ocular reflex, or gaze steadying.
* Vestibulo-ocular reflex is the action of the eyes that counters the rotation of the head, so that the gaze is
* fixed on the point of focus. This is a simple implementation of such behavior. If this FbaAction is played
* in the player, it will have the effect of the character always looking straight ahead, regardles of the head turning.
* This actually gives the impression that the character looks in the eyes of the user in front of the screen and improves
* realism. This is a crude approximation of the vestibulo-ocular reflex.
* For convenience, full source code of
* the SimpleVORAction class can be found in the src folder.
* 
*
*
* Implemented in visagefaplayer.lib
*
* Demonstrated in <a href="../Samples/Example6/doc/index.html">Example6</a>.
*
* @see FbaAction
*/
class VISAGE_DECLSPEC SimpleVORAction  : public FbaAction
{
public:
	/**
	* Constructor. Constructs a SimpleVORAction with default horizontal and vertical limits.
	*/
	SimpleVORAction();

	/**
	* Constructor. Constructs a SimpleVORAction.
	* @param horizontalLimit the horizontal limit for application of VOR, in degrees - when head is rotated more than this limit, VOR is not applied, i.e. eyes look straight in the direction of the head
	* @param verticalLimit the vertical limit for application of VOR, in degrees - when head is rotated more than this limit, VOR is not applied, i.e. eyes look straight in the direction of the head
	*/
	SimpleVORAction(int horizontalLimit, int verticalLimit);

	/**
	* Destructor. 
	*/
	virtual ~SimpleVORAction();


	/**
	* Get FBAPs. This function returns the Face and Body Animation Parameters (FBAPs)
	* for the time globalTime. This function is called by the FAPlayer when the FbaAction is played in the FAPlayer.
	* 
	* To implement the simple vestibulo-ocular reflex (VOR) behavior, we just set the eyes rotations exactly 
	* oposite of the head rotations from the previous frame (obtained through lastFBAPs), thus countering the head rotation and 
	* achieving stable gaze. However, if the head rotation exceeds the horizontal or vertical rotation limit, 
	* the VOR is abandoned and the gaze goes in the
	* direction of the head. Additionally, to avoid the jump at the limits, the VOR compensation is interpolated close to the limit. The default limits are 30 degrees, and they can be set when construction the SimpleVORAction. This is a simple approximation of the vestibulo-ocular reflex.
	*
	* @param globalTime the time for which the FBAPs are requested, in milliseconds; in this implementation we ignore this parameter
	* @param lastFBAPs the final FBAPs from the previous frame of animation
	* @param model the character model currently used in the player; in this implementation we ignore this parameter
	* @return the Face and Body Animation Parameters (FBAPs) for the requested time
	*/
	FBAPs *getFBAPs(long globalTime, FBAPs *lastFBAPs, VisageCharModel *model);


	/**
	* Start the action.
	* This function is called by the FAPlayer when the playing starts. This implementation does not care about time so it just ignores it.
	*
	* @param globalTime the reference time when the action is started, in milliseconds
	*/
	void start(long globalTime);

	/**
	* Stop playing tracks. Reset the face animation track.
	* If an audio track is playing, stops it.
	*/
	void stop();


	/**
	* Return the name of the action type.
	*
	* @return the name of the action type (SimpleVORAction).
	*/
	char* actionTypeName() {return (char *)("SimpleVORAction");};

	FBAPs *fbaps;

private:
	int hlimit,vlimit;
};

}

#endif // __SimpleVORAction_h__
