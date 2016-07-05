
#ifndef __FbaFileAction_h__
#define __FbaFileAction_h__

#include "FbaAction.h"
//OpenAL audio management
#include "AudioContext.hpp"
#include "AudioResource.hpp"
#include "NaradaNotificationCenter.hpp"
//#include "VirtualCharacter.hpp"

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
* A face and body animation action with file read/write capabilities and optional audio track.
* The FbaFileAction class holds a face and body animation track and (optionally) an associated audio track.
* It is typically loaded from a an MPEG-4 FBA file (animation) and an audio file (audio track), and played
* in the FAPlayer. Actually, FAPlayer has convenience functions for loading the animation and
* audio files and other basic operations, thus hiding the FbaFileAction from the user. Therefore, for most operations it may
* not be necessary to use this class directly.
*
* The face and body animation track consists of a number of animation frames that are played at a
* fixed frame rate. The audio track is a sound, read from an audio file, that is played
* together with the face animation track. It is optional. If it is not present, there is
* no sound with the animation.
*
* The FbaFileAction class offers the following main functionalities:
* - reading face and body animation and audio tracks from files (MPEG-4 FBA format for animation, common formats for audio)
* - playing the animation in real time, with sound
* - playing animation frame-by-frame, or accessing a particular frame
* - building a new animation track
* - merging animation tracks
* - saving an animation track to file (MPEG-4 FBA format)
*
* This is an implementation of the FbaAction
*
* Implemented in visagefaplayer.lib
*
* Demonstrated in <a href="../Samples/Example1/doc/index.html">Example1</a>.
*
* @see FbaAction
* @see FAPlayer
*/
class VISAGE_DECLSPEC FbaFileAction  : public FbaAction
{
public:
	/**
	* Constructor. Constructs an empty FbaFileAction.
	*/
	FbaFileAction();

	/**
	* Constructor. Constructs an FbaFileAction by reading an MPEG-4 face and body animation file and (optionally) an audio file.
	* @param fbafile MPEG-4 FBA file to read (binary face and body animation file in the MPEG-4 FBA format). If an audio file
	* with the same name, but the extension ".wav" is found in the same folder, it will be loaded as audio track.
	* 
	*/
	FbaFileAction(char *fbafile);

	/**
	* Constructor. Constructs an FbaFileAction by reading an MPEG-4 face and body animation file and (optionally) an audio file.
	* @param fbafile MPEG-4 FBA file to read (binary face and body animation file in the MPEG-4 FBA format).
	* @param audiofile an audio file containing sound to be played with the face and body animation track. 
	* The file can be in any format recognised by Windows. If this parameter is NULL, no audio track is loaded.
	* 
	*/
	FbaFileAction(char *fbafile, char *audiofile);

	/**
	* Constructor. Constructs an FbaFileAction by reading an MPEG-4 face and body animation file and (optionally) an audio file.
	* @param fbafile MPEG-4 FBA file to read (binary face and body animation file in the MPEG-4 FBA format).
	* @param audiofile an audio file containing sound to be played with the face and body animation track. The file can be in any format recognised by Windows. If this parameter is NULL, no audio track is loaded.
	* @param download if download is TRUE, the constructor automatically loads all frames of animation. If FALSE, 
	* only the first few frames are loaded. In this case you have to load the rest as needed, or you can call the
	* download() function to load all frames of animation at once.
	*
	* @see download()
	*/
	FbaFileAction(char *fbafile, char *audiofile, bool download);

	/**
	* Constructor. Constructs an FbaFileAction with a set number of frames at a given frame rate.
	* @param nFrames the number of frames to be created in the new track.
	* @param frameRate the frame rate for the new track. 
	*/
	FbaFileAction(int nFrames, int frameRate);

	/**
	* Destructor. 
	*/
	virtual ~FbaFileAction();

	
	/**
	* Detects if animation has finished (Enrique Oriol)
	* @return true if animation has finished
	*/
	bool isFinished();

	/**
	* Detects if animation is being played right now (Enrique Oriol)
	* Interesting when some action have been started with a delay
	* @return true if animation is being played
	*/
	bool isBeingPlayed();
	 
	 
	 
	/**
	* Decode a single frame of animation.  
	*/
	void decodeFrame();

	/**
	* Force the loading of all animation frames.  
	* The constructors do this by default so normally this is not needed.
	*/
	void download();

	/**
	* Get FBAPs. This function returns the Face and Body Animation Parameters (FBAPs)
	* for the time t. It is called by the FAPlayer and typically should not be called directly.
	* @param globalTime the time for which the FBAPs are requested, in milliseconds
	* @param lastFBAPs the final FBAPs from the previous frame of animation; in this implementation we ignore this parameter
	* @param model the character model currently used in the player; in this implementation we ignore this parameter
	* @return the Face and Body Animation Parameters (FBAPs)
	*/
	FBAPs *getFBAPs(long globalTime, FBAPs *lastFBAPs, VisageCharModel* model);

	/**
	* Get FBAPs. This function returns the Face and Body Animation Parameters (FAPs)
	* for the frame nfr.
	* @param nfr the animation frame for which the FBAPs are requested
	* @return the Face and Body Animation Parameters (FBAPs)
	*/
	FBAPs *getFBAPs(int nfr);


	/**
	* Set playing velocity. Sets the velocity for real time playing, 1.0 being
	* the normal speed, 0.5 half-speed, 2.0 double speed etc.
	*
	* @param ve the velocity
	*/
	void setVelocity(double ve);

	/**
	* Start playing tracks. Initializes the animation track to the first frame and starts the timer for real time playing.
	* If an audio track is present, and FbaFileAction::mute flag is set to 0, starts playing
	* the audio track.
	*
	* @param globalTime the reference time when the action is started, in milliseconds
	*/
	void start(long globalTime);

	/**
	* Stop playing tracks. Reset the animation track.
	* If an audio track is playing, stops it.
	*/
	void stop();
    

    /**
     * Stops track if has audio.
     * If an audio track is playing, stops and resets it.
     */
    void shutUp();

	/**
	* Return the name of the action type.
	*
	* @return the name of the action type (FbaFileAction).
	*/
	char* actionTypeName() {return (char *)"FbaFileAction";};

	/**
	* Get number of frames. 
	* Get the total number of frames in the animation track. 
	* @return The number of frames in the animation track.
	*/
	int getNFrames();


	/**
	* Write the animation track to a file. 
	* Writes the animation track into a file in the MPEG-4 FBA format. The writing is optimized to reduce file size.
	* @param fn the name of the file to write.
	*/
	void write(char *fn);

	/**
	* Write the animation track to a file. 
	* Writes the animation track into a file in the MPEG-4 FBA format. If optimize parameter is set to true,
	* the writing will be optimized to reduce the file size. The optimization is done by verifying which animation
	* parameters are actually used (i.e. which parameters change value during the animation sequence), and only
	* the used parameters are coded. The other parameters are masked out using the masking mechanism. This is
	* a fairly basic optimization. It may be possible to achieve more efficient coding.
	* @param fn the name of the file to write.
	* @param optimize if true, writing is optimized to reduce file size; otherwise no optimization is performed.
	*/
	void write(char *fn, bool optimize);

	/**
	* Merge animation tracks. 
	* Merge one or more animation tracks with this track. The result is kept in this track.
	* If a merged animation track is longer than this track, the remaining length is cut of, 
	* i.e. only the length of this track is kept. If a merged track is shorter and its LoopMode 
	* is set, it is repeated in a loop to fill the length of this track.
	*
	* The animations are merged as follows:
	* - For the high level FAPs (visemes and expressions), if this track has a viseme/expression
	* already set to non-zero value, its value is kept and merged tracks have no influence on it. If this track has no
	* viseme/expression, the values from the first merged track that has a viseme/expression is taken.
	* - For the low-level FAPs, the value of each FAP is averaged between all non-zero values of
	* the same FAP, for all merged tracks. This means simply that the tracks that have value of 0
	* for a particular FAP have no influence on the merged value of that FAP.
	* - For the BAPs, the value of each BAP is averaged between all non-zero values of
	* the same BAP, for all merged tracks. This means simply that the tracks that have value of 0
	* for a particular BAP have no influence on the merged value of that BAP.
	*
	* In practice, merging gives best results for animation tracks that are naturally independent, like
	* a mouth movement track for speech, a track containing eyeblinks and a third track containing
	* some hands motion. Such tracks will merge naturally. Merging of tracks that act on the same FBAPs
	* may give unpredictable results.
	* @param fa the animation tracks to merge with this one.
	* @param nfa the number of animation tracks to merge.
	*/
	void merge(FbaAction **fa, int nfa);

	void encode_frame(FBAPs *f,int fbaFileHandle);

	/**
	* Get frame rate. 
	* Get the frame rate of the animation track. 
	* @return The frame rate of the animation track in frames per second (fps).
	*/
	float getFrameRate();


	/**
	* Set the loop mode.
	* Set the loop mode to 1 (on) or 0 (off). The loop mode affects the playing of the FbaFileAction
	* in the FAPlayer, as well as the behavior when the track
	* is being merged to another track. If the track is shorter than the main track to which
	* it is being merged, it is repeated (looped) to fill the time until the end of the main
	* track.
	* @param mode the loop mode 1 = on; 0 = off.
	*/
	void setLoopMode(int mode) {loopMode = mode;};

	/**
	* Set the time offset.
	* The track will start playing with this time offset
	* @param t offset in ms. Positive number means that the track is played from time t; negative number causes the track to be played after a delay.
	*/
	void setOffset(long t) {timeOffset = t;};

	/**
	* Set a new audio track.
	* Loads a new audio track from a file.
	* @param audioFile the name of the audio file to use as audio track.
	*/
	void setAudioTrack(char *audioFile);

	/**
	* Replace the zero values of animation parameters by minimal positive values to enable smooth merging.
	* The purpose of this function is to avoid the problem that sometimes appears when merging animation
	* actions. The problem is the following.
	*
	* When two sets of animation parameters (FBAPs) are merged, the rule is that if one parameter is 0 and the other
	* non-zero, the non-zero value is taken; however, if both parameters are non-zero, the average is taken (see
	* @ref FBAPs::merge() for details. This leads to a sudden jerk between two subsequent frames of animation
	* when in one frame there is a small non-zero value, and in the next frame a zero value, which often happens
	* when animation parameter values cross zero level. When this track is merged with another, in the first
	* frame the two values are averaged, but in the next frame only the value from the merged track is taken. Therefore
	* there is a jerk.
	*
	* To solve this problem, this function simply sets all animated parameter values to a minumal non-zero value.
	* Visibly, this does not make a difference, but the merging operation now works smoothly.
	*
	* @see FBAPs::merge() 
	* @see LLFAPs::merge() 
	* @see BAPs::merge() 
	*/
	void smoothZeroValues();

	/**
	* Set the audio delay.
	* The synchronisation between the audio track and animation depends on both audio and 
	* animation starting at exactly the same time. However, it is difficult to determine 
	* the different startup time for audio and animation on various platforms. If the 
	* animation seems consistently out of sync, you can modify the audio delay. It is the 
	* the delay of the audio track with respect to the animation track, expressed in 
	* milliseconds.
	* Therefore, if the audio seems to "run before the lips" increase the value of 
	* audio delay, and vice versa.
	*
	* @param t audio delay in ms. 
	*/
	void setAudioDelay(long t) {audioDelay = t;};
	void loadFilter();

	/**
	* Set the FBAPs for a given frame.
	* Set the FBAPS for the frame nfr to values provided in fbaps.
	*
	* @param nfr number of frame for which to set the FBAPs.
	* @param fbaps the FBAPs to be set at the given frame. 
	*/
	void setFBAPs(FBAPs *fbaps, int nfr);

	void discardFilter();

	/*
	// new stuff
	void FbaFileAction::setStarted(int t) {started = t;};
	int FbaFileAction::getStarted() { return started;};
	long FbaFileAction::getStartT() { return clockStartTime;};
	void FbaFileAction::setStartT(long t) {clockStartTime = t;};
*/
	/**
	* Mute playing.
	* If mute is set to 1, the audio track will not be played; if it is set to 0, the audio track
	* will be played if it exists.
	*/
	int mute;

private:
	void init();
	void loadFiles(char *_fbaFile, char *_audioFile, bool _download);
	void optimizeForWriting();

	/**
	* Get the frame number corresponding to a time. 
	* Get the frame number corresponding to the time t. 
	* @param t the time for which the frame number is requested, in milliseconds
	* @return The frame number for time t.
	*/
	int getFrame(long t);



	//bool actionFinished;
	int isAudio;
	unsigned char *audioBuffer;
	long audioDelay;
	char audioFile[200];
	char fbaFile[200];
	char filterFile[200];
	int loaded;
	FBAPs **fbapsList;
	long *timeList;
	int nFrames;
	int maxFrames;
	int loopMode;
	long timeOffset;
	float speed;
	long globalStartTime; // global time (time provided by the FAPlayer) when action started; if this action is used as time reference, this will be 0
	long localTime;       // action local time, measures since the start of the action
	int currentFaps;
	int started;
	CFBADecoder *fbadec;
	int streamLoaded;
	double velocity;
	FAPs *filter;
	FBAPs filteredFBaps;
	long lastStartTshift;
	AudioResource *audioResource;

};

}

#endif // __FbaFileAction_h__
