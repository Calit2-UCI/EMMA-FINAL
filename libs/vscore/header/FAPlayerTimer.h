
#ifndef __FAPlayerTimer_h__
#define __FAPlayerTimer_h__

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
* Timer for the FAPlayer.
* The FAPlayerTimer class provides the timing service for the FAPlayer. It is used automatically by
* the FAPlayer, and the user should typically not need to access it directly or worry about this class in any way. One exception where such access may be
* needed is for setting the frame rate.
*
* Although the standard timing will be appropriate for most
* applications, and the user should not need to worry about it, it is possible to access and replace the timer by the user's own implementation of an
* FAPlayerTimer derived class, and thus achieve any special timing effects, e.g. to speed up or slow down the time. To facilitate this, all members
* of FAPlayerTimer are public, allowing full access. For convenience, the source code of FAPlayerTimer is
* provided in the src folder.
*
* The timing mechanism works as follows. The FAPlayer calls the FAPlayerTimer start() method to start the timer. It then repeatedly
* calls either updateRT() or nextFrame(), depending on the playing mode. If it is real time playing, updateRT() is called; if off-line playing, 
* nextFrame() is called. In either case, the FAPlayerTimer updates the current time accordingly, either to the real time clock or
* to the time corresponding to the next frame of animation, determined by the current frame rate. The player gets the time by
* calling getCurrentTime(), and uses that time as the reference for animation.
*
* Implemented in visagefaplayer.lib
*
* @see FAPlayer
* @see FbaAction
*/
class VISAGE_DECLSPEC FAPlayerTimer
{
public:
	/**
	* Constructor. Constructs an FAPlayerTimer.
	*/
	FAPlayerTimer();

	/**
	* Destructor. 
	*/
	virtual ~FAPlayerTimer();


	/**
	* Start the timer. 
	* Start the timer and set the initial time to _startTime.
	*
	* @param _startTime the time to which the timer is set, in milliseconds.
	*/
	void start(long _startTime);

	/**
	* Start the timer. 
	* Start the timer.
	*/
	void start();

	/**
	* Stop the timer. Stop the running of the timer and reset the time to 0.
	*/
	void stop();

	/**
	* Update the timer according to real time clock. 
	* Sets the current time and frame according to the time measured by the real time clock of the timer. This is called by the FAPlayer during the real time playing.
	*
	*/
	void updateRT();

	/**
	* Pause or resume the timer action. 
	* Pause or resume the timer action. 
	* @param pause if true, timer is paused; if false, timer running is continued
	*
	*/
	void pause(bool pause);

	/**
	* Check if the timer is paused. 
	* Check if the timer is paused. 
	* @return true if timer is paused; false if timer is running or stopped
	*
	*/
	bool isPaused() {return paused;};

	/**
	* Check if the timer is running. 
	* Check if the timer is running. 
	* @return true if timer is running; false if timer is paused or stopped
	*
	*/
	bool isRunning() {return running;};

	/**
	* Update timer to the next frame. 
	* Increases the timer current frame by 1, and sets the current time accordingly. 
	* This is called by the FAPlayer  for off-line playing. For real time playing, updateRT() is used.
	*
	* @see updateRT
	*/
	void nextFrame();


	/**
	* Set current frame.
	* Sets the current timer frame. Used to jump directly to a particular time of animation.
	* @param nfr number of frame to which to set the timer.
	*/
	void setFrame(int nfr);

	/**
	* Get the frame number corresponding to a time. 
	* Get the frame number corresponding to the time t. 
	* @param t the time for which the frame number is requested, in milliseconds
	* @return The frame number for time t.
	*/
	int getFrame(long t);

	/**
	* Get the time corresponding to a frame number . 
	* Get the time in milliseconds corresponding to the frame number fr. 
	* @param fr the frame number for which the time is requested
	* @return The time in milliseconds corresponding to trame number fr.
	*/
	int getTime(int fr);

	/**
	* Get frame rate. 
	* Get the frame rate of the timer. 
	* @return The frame rate of the timer in frames per second (fps).
	*/
	float getFrameRate();

	/**
	* Set frame rate. 
	* Set the frame rate of the timer. This function should not be called while the player is playing.
	*
	* @param  _frameRate the frame rate to be set for the timer in frames per second (fps).
	*/
	void setFrameRate(float _frameRate);

	/**
	* Get the current time. 
	* Get the current time of the timer. 
	* @return The current time in milliseconds.
	*/
	long getCurrentTime();

	/**
	* Get the current frame.
	* Get the number of the current frame of the timer, corresponding to the current time. 
	* @return The number of the current frame.
	*/
	int getCurrentFrame();


	long currentTime;        // the current time produced by the timer, in milliseconds
	int currentFrame;        // the current animation frame corresponding to the currentTime
	long clockStartTime;  // the time of the real time clock at the moment the timer was started
	long startTime;       // the time offset with which the timer is started, in milliseconds; this is the value of mainTime at the moment the timer starts; usually it is 0.
	float frameRate;      // frame rate in frames per second, used for per-frame timing
	bool running;
	bool paused;
};

}

#endif // __FAPlayerTimer_h__
