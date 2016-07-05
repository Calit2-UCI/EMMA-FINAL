
#ifndef __FAPlayer_h__
#define __FAPlayer_h__

#include "VisageCharModel.h"
#include "FbaFileAction.h"
#include "FAPlayerTimer.h"


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

#define PLAYMODE_REALTIME 0
#define PLAYMODE_OFFLINE 1

/**
* A multi-purpose renderer interface for the FAPlayer. If an instance of FARenderer is
* attached to the FAPlayer using FAPlayer::setRenderer() or FAPlayer::FAPlayer(AFM *afm, FARenderer *_renderer), when the FAPlayer 
* plays the animation it calls the preRender() and render() functions of the FARenderer
* in every frame of animation.
*
* This mechanism can be used for actual on-screen rendering of the animation, exporting, binding to
* other systems in plug-in implementations, or other purposes. For example,
* the visage|interactive application uses an implementation of the FARenderer::render() function to update
* the position of the time slider in the window. Importantly, the preRender() function
* also allows to alter the animation parameters just before the rendering.
*
* Note that the visage|SDK example projects actually do not use this mechanism. They implement 
* an asynchronous rendering loop in a separate thread. This is recommended for most real time applications.
*
* This is an abstract class. A subclass must be implemented and used. It this way
* various types of renderers can be implemented for different purposes.
*
* Implemented in visagefaplayer.lib
*
* @see FAPlayer
* @see FAPlayer::setRenderer()
* @see render()
* @see preRender()
*/
class VISAGE_DECLSPEC FARenderer {
public:
	FARenderer();
	~FARenderer();

	/**
	* The rendering function.
	* This is a callback function, called by the play() function of the FAPlayer when rendering
	* of the animated virtual character model is needed, i.e. once per animation cycle. It is a virtual function and has
	* to be implemented by each renderer type.
	*
	* The render() function gets the face model and the time stamp in milliseconds since beginning of animation. 
	* This data is provided by FAPlayer when it calls the render() function. The implementation of the renderer
	* can actually render the model on screen, store it for rendering in its own rendering loop, export it to file 
	* or perform some other operation on the model.
	*
	* Note that vcModel is the pointer to the actual internal model used within the player, i.e. it will keep changing
	* during the animation. It is not recommended to make any changes in the model in this function.
	* 
	* @param vcModel the character model to be rendered
	* @param timeMs current time within the animation track, in milliseconds
	* 
	* @see FAPlayer
	* @see FAPlayer::setRenderer()
	*/
	virtual void render(VisageCharModel *vcModel, long timeMs)=0;

	/**
	* The pre-rendering function.
	* This is a callback function, called by the play() function of the FAPlayer in each frame of animation just before
	* the rendering of the virtual character model. It is a virtual function and has
	* to be implemented by each renderer type. The intended usage is to allow the modification of the FBAPs
	* just before the rendering.
	*
	* The preRender() function gets the current FBAPs and the time stamp in milliseconds since beginning of animation. 
	* This data is provided by FAPlayer when it calls the preRender() function. The FBAPs can be used
	* and/or modified by the implementation of the preRender() function. If your implementation of
	* FARenderer is not intended to modify FBAPs, simply leave this function empty.
	*
	* @param fbaps the pointer to the current FBAPs. The %FBAPs can be modified.
	* @param timeMs current time within the animation track, in milliseconds
	* 
	* @see FAPlayer
	* @see FAPlayer::setRenderer()
	* @see FBAPs
	*/
	virtual void preRender(FBAPs *fbaps, long timeMs)=0;

};

/* IDLE TRACKS
* Additionaly, it can manage so-called idle tracks, the animation tracks that are used to provide
* some natural-looking motion to the face while no track is played. For even more natural-looking animation,
* the gaze following capability is provided. If the gaze-following is turned on, the face model
* looks towards the user (i.e. forward) even as it turns the head.
*/

/**
* The Face and Body Animation Player.
* This is one of the most important classes in the visage|SDK. It implements a powerful
* animation mechanism for full face and body animation. It provides both real-time and off-line
* (frame by frame) animation. It allows mixing of multiple animation tracks and dynamic adding of
* animation tracks (actions) while playing.
*
* The FAPlayer provides a fairly easy, high-level mechanism for animating the virtual character. At the
* same time, it provides a multitude of methods allowing full access and control of the animation at lower levels.
* It is recommended to look at the examples provided with the visage|SDK in order to see how the FAPlayer is used.
*
* The player is initialized by loading an Animatable Face and Body Model (AFM) from a file. Then one or
* more animation tracks are added, and animation is started by calling the play() method. 
*
* Each animation track is an FbaAction, an abstract animation class that is the basis for all animation types. This provides a very powerful mechanism
* for implementing various kinds of animation mechanisms, some of which are implemented within the visage|SDK (e.g. FbaFileAction, Visagesapi5tts, VisageLipSync), others to be
* implemented by the user, e.g. procedural animations, motion tracking etc.
*
* The tracks can also be merged together and
* saved to a file in form of an MPEG-4 FBA bitstream.
*
* The play mode can be real time or off-line. In real time mode the frames are skipped if necessary in order to
* achieve real time synchronisation with the animation time stamps. The animation therefore
* stays synchronised and corresponds to the audio track that plays together with it. In the off-line mode
* the animation plays frame-by-frame and it is guaranteed that no frames are skipped. This is convenient
* for exporters, off-line renderers, plugins for 3D animation packages such as 3ds max or Maya, and similar applications that need to get all animation frames.
*
* The timing is handled by the FAPlayerTimer class. Although the standard timing will be appropriate for most
* applications, it is possible to access and replace the timer by the user's own implementation of an
* FAPlayerTimer derived class, and thus achieve any special timing effects. To facilitate this, all members
* of FAPlayerTimer are public, allowing full access. For convenience, the source code of FAPlayerTimer is
* provided in the src folder.
*
* The generic rendering mechanism is implemented through the FARenderer class. An instance of an FARenderer can
* be provided to FAPlayer; as it plays it calls the FARenderer's render() function.
* In this way various kinds of renderers can be implemented. One example of
* a FARenderer is FAExporter, which collects the data for the whole animation and converts it into 
* keyframes convenient for exporting into keyframe-based files or systems. 
*
* Other rendering mechanisms may be implemented. For more information see the section on visage|SDK extension capabilities in visage|SDK documentation.
*
* Implemented in visagefaplayer.lib
*
* Demonstrated in <a href="../Samples/MS_Visual_C++_6.0/Example1/doc/index.html">Example1</a>, <a href="../Samples/MS_Visual_C++_6.0/Example6/doc/index.html">Example6</a>, and <a href="../Samples/MS_Visual_C++_6.0/Example7/doc/index.html">Example7</a>.
*
*/
class VISAGE_DECLSPEC FAPlayer {
public:

	/**
	* Constructor.
	* Constructs an empty FAPlayer.
	*/
	FAPlayer();

	/**
	* Constructor.
	* Constructs an FAPlayer and initializes it with a face&body model and a renderer. After this the player is ready for playing.
	* This is equivalent to calling FAPlayer(), then init(AFM *afm) and setRenderer(FARenderer *_renderer).
	*
	* @param _renderer the FARenderer that will do the rendering.
	* @param afm the Animatable Face and Body Model (AFM) file to load. The AFM file has the extension .afm. The AFM file format is an open format based on VRML97. It is fully documented in the <a href="AFM File Format Reference.pdf">File Format Reference</a>.
	* @see init(AFM *afm)
	* @see setRenderer(FARenderer *_renderer)
	* @see FARenderer
	* @see AFM
	*/
	FAPlayer(VisageCharModel *model, FARenderer *_renderer);

	/**
	* Constructor.
	* Constructs an FAPlayer and initializes it with a face&body model. After this the player is ready for playing.
	* This is equivalent to calling FAPlayer(), then init(VisageCharModel *model).
	*
	* @param model the face&body model that should be animated by the FAPlayer.
	* @see init(VisageCharModel* model)
	* @see VisageCharModel
	*/
	FAPlayer(VisageCharModel* model);

	/**
	* Destructor.
	* Destructs the FAPlayer.
	*/
	virtual ~FAPlayer();

	/**
	* Initialise the FAPlayer with a face&body model.
	* Initialises the FAPlayer with a previously loaded face&body model (VisageCharModel). It is now ready to play animation track(s).
	*
	* @param model the face&body model that should be animated by the FAPlayer.
	*
	* @see VisageCharModel
	*/
	void init(VisageCharModel* model);

	/**
	* Set the renderer.
	*
	* If an instance of an FARenderer is
	* provided to FAPlayer using this function, while playing the FAPlayer calls the FARenderer's preRender() and render() functions for each frame that 
	* needs to be rendered. In this way various kinds of renderers can be implemented. 
	* 
	* One example of
	* a FARenderer is FAExporter, which collects the data for the whole animation and converts it into 
	* keyframes convenient for exporting into keyframe-based files or systems. 
	* 
	* For real time playing, the FARenderer can implement on-screen rendering of the model. (Note that real time
	* rendering can also be achieved without setting the FARenderer, as shown in  
	* the <a href="../../../Samples/OpenGL/build/msvc8/SimplePlayer/doc/index.html">Example1</a> and other provided samples). 
	*
	* For more advanced applications, the user
	* can implement their own renderers using the FARenderer interface. It provides very complete access to the face and body
	* model and its animation.
	* 
	* Additionaly, the rendering mechanism can be used for simpler tasks, for example for updating a time
	* line slider or implement other effects during animation during the animation.
	*
	* @param _renderer the FARenderer that will do the rendering. If NULL, no renderer is used.
	* @see FARenderer
	*/
	void setRenderer(FARenderer *_renderer) {renderer = _renderer;};

	/**
	* Get the renderer.
	*
	* If an instance of an FARenderer is
	* provided to FAPlayer using this function, while playing the FAPlayer calls the FARenderer's preRender() and render() functions for each frame that 
	* needs to be rendered. In this way various kinds of renderers can be implemented. See @ref setRenderer() for details.
	* 
	* @param _renderer the FARenderer that will do the rendering. If NULL, no renderer is used.
	* @see FARenderer
	* @see setRenderer()
	*/
	FARenderer * getRenderer(FARenderer *_renderer) {return renderer;};

	// animation tracks management

	/**
	* Add an animation track from file.
	* Loads an animation track from a file and adds it to the player. Up to 100 tracks can be loaded. 
	* The track is an FbaFileAction. The track's loop mode is set to 0 (no looping) and the time offset is 0
	* @param tname the MPEG-4 FBA file (.fba) from which the animation track is to be loaded. If an audio file
	* with the same file name, but the extension ".wav", is found in the same folder, it will be loaded as audio track.
	* @return the pointer to the loaded animation track
	*
	* @see FbaFileAction
	*/
	FbaFileAction *addTrack(char *tname);

	/**
	* Add an animation track from a file.
	* Loads an animation track from a file and adds it to the player. Up to 100 tracks can be loaded. 
	* The track is an FbaFileAction. The track's loop mode and time offset are set according to given arguments.
	*
	* @param tname the MPEG-4 FBA file (.fba) from which the animation track is to be loaded. If an audio file
	* with the same file name, but the extension ".wav", is found in the same folder, it will be loaded as audio track.
	* @param loop the loop mode. If 0, the track will play once, if 1 the track will play repeatedly in a loop.
	* @param timeOffset the time offset, in milliseconds, for the track. The animation will be played from this time.
	* @return the pointer to the loaded animation track
	*
	*
	* @see FbaFileAction
	*/
	FbaFileAction *addTrack(char *tname, int loop, long timeOffset);

	/**
	* Add an animation track.
	* Adds an animation track to the player. The animation track can be any FbaAction. Up to 100 tracks can be added.
	* @param trk the FbaAction to be added as additional animation track.
	*
	* @see FbaAction
	*/
	void addTrack(FbaAction *trk);

	/**
	* Play an animation track from a file.
	* Loads an animation track from a file and plays it in the player (i.e. if the player is not already playing, play() function is called too). When the playing is finished, the track is removed from the player.
	* The track is an FbaFileAction. The track's loop mode is set to 0 (no looping) and the time offset is 0
	* @param tname the MPEG-4 FBA file (.fba) from which the additional animation track is to be loaded.
	*
	*
	* If an audio file
	* with the same file name, but the extension ".wav", is found in the same folder, it will be loaded as audio track.
	*
	* @see FbaFileAction
	* @return the pointer to the loaded animation track
	*/
	FbaFileAction *playTrack(char *tname);
	
	/**
	* Play an animation track from a file.
	* Loads an animation track from a file and plays it in the player (i.e. if the player is not already playing, play() function is called too). When the playing is finished, the track is removed from the player.
	* The track is an FbaFileAction. The track's loop mode is set to 0 (no looping) and the time offset is set to timeOffset
	* @param tname the MPEG-4 FBA file (.fba) from which the additional animation track is to be loaded.
	* @param timeOffset the time offset, in milliseconds, for the track. The animation will be played from this time.
	*
	* If an audio file
	* with the same file name, but the extension ".wav", is found in the same folder, it will be loaded as audio track.
	*
	*
	* @see FbaFileAction
	* @return the pointer to the loaded animation track
	*/
	FbaFileAction *playTrack(char *tname, long timeOffset);
	
	/**
	* Play an animation track.
	* Plays an animation track in the player (i.e. if the player is not already playing, play() function is called too). The animation track can be any FbaAction. 
	* When the playing is finished, the track is removed from the player. 
	* Note that it is only removed from the player, but *not* deleted. 
	* This means (1) that the same FbaAction can be used again and (2) that it is 
	* the responsability of the user to delete it when it is not needed anymore. 
	* @param trk the FbaAction to be played.
	*
	* @see FbaAction
	*/
	void playTrack(FbaAction *trk);

	/*
	* Play an animation track.
	* Plays an animation track in the player (i.e. if the player is not already playing, play() function is called too). The animation track can be any FbaAction. 
	* When the playing is finished, the track is removed from the player. At that time
	* the track is also deleted if the deleteAfterPlaying parameter is set to 1. If
	* this parameter is set to 0, the track is removed but not deleted.
	*
	* Set deleteAfterPlaying to 1 if the FbaAction is not going to be used again and you
	* want it to be deleted after its playing is finished.
	*
	* Seting deleteAfterPlaying to 0 is the same as simply calling playTrack(FbaAction *trk). 
	* In this case the FbaAction is only removed from the player, but *not* deleted. 
	* This means (1) that the same FbaAction can be used again and (2) that it is 
	* the responsability of the user to delete it when it is not needed anymore.
	* @param trk the FbaAction to be played.
	* @param removeAfterPlaying if this parameter is set to 1, the FbaAction is deleted after playing; if it is 0, the FbaAction is removed from the player but not actually deleted, so it can be reused.
	*
	* @see FbaAction
	* @see playTrack(FbaAction *trk)
	void FAPlayer::playTrack(FbaAction *trk, int deleteAfterPlaying);
	*/

	/**
	* Remove an animation track.
	* Removes an animation track from the player. Note that it is only removed from the player, but *not* deleted. This means (1) that the same FbaAction can be used again and (2) that it is the responsability of the user to delete it when it is not needed anymore.
	* @param trk the FbaAction to be removed from the player.
	*/
	void removeTrack(FbaAction *trk);

	/**
	* Remove an animation track name. 
	* Removes an animation track from the player. Note that it is only removed from the player, but *not* deleted. 
	*
	* @param n the number (index) of the track to delete. If n is less than 0 or greater than [number of tracks in the player - 1], no action is taken.
	*
	* @return name of the requested track, or NULL if n is less than 0 or greater than [number of tracks in the player - 1].
	*/
	void removeTrack(int n);

	/**
	* Get an animation track name. 
	* Returns the name an animation track. For the tracks of FbaFileAction type, this is the name of the loaded file. For most
	* other track types, this is the name of the action type, as returned by @ref FbaAction::actionTypeName().
	*
	* @param n the number (index) of the track.
	*
	* @return name of the requested track, or NULL if n is less than 0 or greater than [number of tracks in the player - 1].
	*/

	/**
	* Get an animation track. 
	* Returns the pointer to an animation track in the player.
	*
	* @param n the number (index) of the track to get.
	*
	* @return pointer to the requested track, or NULL if n is less than 0 or greater than [number of tracks in the player - 1].
	*/
	FbaAction* getTrack(int n);

	/**
	* Test if the player is currently playing. 
	* Returns the status of the player - playing or not.
	* @return true if the player is currently playing, false if not.
	*/
	bool isPlaying();


	/**
	* Get number of tracks. 
	* Returns the number of animation tracks in the player.
	* @return number of tracks.
	*/
	int getNTracks();

	/**
	* Check if an animation track is in the player.
	* If the animation track is currently in the player, return true, otherwise false. This is useful for timing one track after another finishes.
	* When the track is played using playTrack() it will be automatically removed from the player when it finishes playing.
	* @param trk the FbaAction to be checked.
	* @return true if track is found, false if not.
	*/
	bool checkTrack(FbaAction *trk);

	/* IDLE TRACKS
	/**
	* Add an idle track.
	* Loads an idle track. Up to 100 idle tracks can be loaded. The idle tracks the animation tracks that are used to provide
	* some natural-looking motion to the face while no track is played. 
	*
	* Each idle track has an associated attenuation factor. The attenuation factor is used to 
	* attenuate the idle track when no other action is occuring. So, when the main track is being played,
	* the idle track is played in its full amplitude. When the main track stops playing, the idle track
	* is played with the amplitude of FAPs attenuated by the attenuation factor. Setting an attenuation
	* factor is useful for example for implementing head motion. An animation track with repeated head motion
	* is loaded as idle track with an attenuation factor of, say, 0.4. Now, while the main track is not played
	* and the face is really idle, the head motion is less strong, only 40% of the full motion.
	* However, when the main track is played the head moves more, thus simulating the behavior where
	* a person moves more while he/she speaks. Obviously, it does not make sense to set an attenuation
	* factor for movements that should always have the same amplitude, e.g. eyeblinks. For such movements attenuation
	* factor is usually set to 100. There is a short transition period when the main track starts and stops
	* playing, in order to provide a smooth transition between attenuated and non-attenuated mode.
	* During the transition period the attenuation factor is interpolated between 100 and its normal value.
	*
	* @param tname the MPEG-4 FBA file (.fba) from which the additional animation track is to be loaded.
	* @param atten attenuation factor for this idle track, as percentage of full amplitude (100 for no attenuation).
	void addIdleTrack(char *tname,float atten);
*/
	/**
	* Clear all tracks.
	* This method clears all tracks, i.e. removes them from the player's play list and deletes them.
	*/
	void clearAllTracks();

	/**
	* Merge all tracks.
	* All animation tracks are merged into a single track, and this track becomes the only track in the player.
	*
	* This function is available only if at least one of the tracks is of type FbaFileAction, e.g. if it has been loaded using
	* loadMainTrack(). Otherwise the function returns without doing anything. This is because only FbaFileAction is capable of the
	* merge operation.
	*
	* For details on merging, see FbaFileAction::merge().
	*/
	void mergeAllTracks();

	/**
	* Save the animation from the player to a file. 
	* Saves the merged animation from all tracks into an MPEG-4 FBA file. 
	*
	* The length of the animation to be saved is the length of the longest animation track that has been loaded from a
	* file, i.e. the longest track of FbaFileAction type. If none of the tracks are of FbaFileAction type, by default
	* 10 seconds of animation are saved. In this case it is recomended to set the length of saved animation
	* explicitely using save(char *fn, long t).
	*
	* The saving is done frame-by-frame and not in real time. Therefore, the animation from any FbaAction that performe 
	* real time animation based on its own real time clock (e.g. speech synthesis, lip sync) may not be saved correctly.
	* For this reason both lip sync and TTS classes in visage|SDK have their own file saving methods.
	*
	* @param fn the name of the file to write.
	*
	* @see save(char *fn, long t)
	*/
	void save(char *fn);


	/**
	* Save the animation from the player to a file. 
	* Saves the merged animation from all tracks into an MPEG-4 FBA file. The length of the saved animation 
	* is t milisseconds.
	*
	* The saving is done frame-by-frame and not in real time. Therefore, the animation from any FbaAction that performe 
	* real time animation based on its own real time clock (e.g. speech synthesis, lip sync) may not be saved correctly.
	* For this reason both lip sync and TTS classes in visage|SDK have their own file saving methods.
	*
	* @param fn the name of the file to write.
	* @param t the length of animation to be saved, in milliseconds.
	*
	* @see save(char *fn, length t)
	*/
	void save(char *fn, long t);


	/**
	* Get the player's timer.
	* Get the player's timer. 
	* The FAPlayer timing is handled by the FAPlayerTimer class. Although the standard timing will be appropriate for most
	* applications, it is possible to access and change the timer. One method from the FAPlayerTimer that may be useful is FAPlayerTimer::setFrameRate(),
	* in case it would be needed to change the frame rate. The default frame rate is 25 frames per second.
	*
	* For special purposes it is even possible to replace the timer by the user's own implementation of an
	* FAPlayerTimer subclass, and thus achieve any special timing effects. To facilitate this, all members
	* of FAPlayerTimer are public, allowing full access. For convenience, the source code of FAPlayerTimer is
	* provided in the src folder.
	*
	* @return the the pointer to the player's timer.
	* @see FAPlayerTimer
	* @see setTimer()
	*/
	FAPlayerTimer *getTimer() {return timer;}

	/**
	* Set the player's timer.
	* Set the player's timer. 
	* The FAPlayer timing is handled by the FAPlayerTimer class. Although the standard timing will be appropriate for most
	* applications, this method allows to replace the timer by the user's own implementation of an
	* FAPlayerTimer derived class, and thus achieve any special timing effects. To facilitate this, all members
	* of FAPlayerTimer are public, allowing full access. For convenience, the source code of FAPlayerTimer is
	* provided in the src folder.
	*
	* @param _timer the timer to set as the player's timer.
	* @see FAPlayerTimer
	* @see getTimer()
	*/
	void setTimer(FAPlayerTimer *_timer) {timer = _timer;}

	// time/frames information

	/**
	* Get frame rate. 
	* Get the frame rate of the player. 
	*
	* The frame rate is actually determined by the FAPlayerTimer. To set the frame rate, use getTimer() and FAPlayerTimer::setFrameRate(). The default frame rate is 25 fps.
	* The frame rate only affects the offline play mode. The real time mode is not affected by this parameter.
	* @return The frame rate of the player in frames per second (fps).
	*/
	float getFrameRate();

	/**
	* Get number of frames. 
	* Get the total number of frames in the longest animation track. Only animation tracks that are loaded
	* from file have a fixed number of frames. The generic FbaAction tracks do not have a defined number of frames.
	* Therefore this method will return 0 if no animation track is an FbaFileAction, i.e. loaded from a file.
	* @return The number of frames in the longest animation track.
	*/
	int getNFrames();

	/**
	* Get the frame number corresponding to a time. 
	* Get the frame number corresponding to the time t, using the player's frame rate for the calculation. This is relevant only to offline operation. The calculation is actually performed by the FAPlayerTimer.
	* @param t the time for which the frame number is requested, in milliseconds
	* @return The frame number for time t.
	*/
	int getFrame(long t);

	// playing

	/**
	* Test if the player is ready to play. 
	* Test if the player is ready to play, i.e. if the model and at least one animation track are present. 
	* @return The returns 1 if the player is ready, 0 if not.
	*/
	int ready();

	/**
	* Set the play mode. 
	* The play mode can be real time or off-line. In real time mode the frames are skipped if necessary in order to
	* achieve real tyme synchronisation with the animation time stamps. The animation therefore
	* stays synchronised and corresponds to the audio track that plays together with it. In the off-line mode
	* the animation plays frame-by-frame and it is guaranteed that no frames are skipped. This is convenient
	* for exporters, off-line renderers and similar applications that need to get all animation frames.
	*
	* It is important to note that the frame rate in off-line mode is set by the FAPlayerTimer, and by default it
	* is 25 fps. If the animation tracks use a different frame rate, it may be desirable to change the frame rate
	* used by the player. To do su, use getTimer() and FAPlayerTimer::setFrameRate().
	*
	* The default play mode is PLAYMODE_REALTIME.
	*
	* @param mode the play mode; the value can be PLAYMODE_REALTIME or PLAYMODE_OFFLINE.
	* @see FAPlayerTimer
	* @see getTimer()
	*/
	void setPlayMode(int mode);

	/**
	* Get the play mode. 
	* The play mode can be real time or off-line. In real time mode the frames are skipped if necessary in order to
	* achieve real tyme synchronisation with the animation time stamps. The animation therefore
	* stays synchronised and corresponds to the audio track that plays together with it. In the off-line mode
	* the animation plays frame-by-frame and it is guaranteed that no frames are skipped. This is convenient
	* for exporters, off-line renderers and similar applications that need to get all animation frames.
	*
	* It is important to note that the frame rate in off-line mode is set by the FAPlayerTimer, and by default it
	* is 25 fps. If the animation tracks use a different frame rate, it may be desirable to change the frame rate
	* used by the player. To do su, use getTimer() and FAPlayerTimer::setFrameRate().
	*
	* The default play mode is PLAYMODE_REALTIME.
	*
	* @return the play mode; the value can be PLAYMODE_REALTIME or PLAYMODE_OFFLINE.
	* @see setPlayMode()
	*/
	int getPlayMode() {return playMode;};

	/**
	* Play the animation.
	* Start playing animation track(s) that are in the player.
	* By default, the player plays in real time mode and stops playing when
	* all tracks are finished or when the stop() function is called. To obtain different playing behavior, use the functions
	* listed below to set/get various playing modes and parameters.
	*
	* While the player is playing, subsequent calls to play() are ignored. To verify if the player is playing, use @see isPlaying().
	*
	* For more playing options, see play(FARenderer *_renderer, int _playForever, int _newThread).
	*
	* @see stop()
	* @see rewind()
	* @see update()
	* @see isPlaying()
	* @see setPlayMode()
	* @see setRenderer()
	* @see setAutoStopMode()
	* @see setPlayTimeRange()
	* @see setPlayFramesRange()
	* @see setRenderer()
	* @see getPlayMode()
	* @see getMultiThreadingMode()
	* @see getRenderer()
	* @see getAutoStopMode()
	* @see getPlayTimeRange()
	* @see getPlayFramesRange()
	*/
	void play();


	/**
	* Stop the animation.
	* Stop playing the animation. It does not stop any audio track that may be playing.
	*/
	void stop();

	/**
	* Update the animation.
	* Updates the animation. In real-time mode, this advances the animation by the elapsed time,
	* while in offline mode it advances the animation to the next frame. For real-time playback,
	* a call to this method should be placed somewhere in the game loop (see the SimplePlayer
	* sample for an example of the update mechanism).
	* @see play()
	*/
	int update();

	/**
	* Get the face&body model for a particular frame of animation.
	* Returns the face&body model "frozen" into a particular frame of animation.
	* This can be used to "manually" play the animation,
	* i.e. without using the play() function. Using the play() function is recommended.
	* @param frame the frame of animation for which the model is returned
	* @return the face&body model from the chosen frame of animation, returned as VRMLModel
	* @see play()
	*/
	VisageCharModel *getFaceModel(int frame);

	/**
	* Get the face&body model for a particular time in the animation.
	* Returns the face&body model "frozen" into a particular time in the animation.
	* This can be used to "manually" play the animation,
	* i.e. without using the play() function. Using the play() function is recommended.
	* @param time the time in the animation for which the model is returned, in milliseconds
	* @return the face&body model from the chosen time in the animation, returned as VRMLModel
	* @see play()
	*/
	VisageCharModel *getFaceModel(long time);

	/**
	* Rewind the animation.
	* Reset the animation to the beginning. This is automatically called by the play function, so it always plays from the beginning.
	*/
	void rewind();

	/**
	* Set the audio delay for file tracks.
	* The animation tracks loaded from file are of type FbaFileAction, and they are capable of playing an audio track
	* together with the animation. The synchronisation between the audio track and animation depends on both audio and 
	* animation starting at exactly the same time. However, it is difficult to determine 
	* the different startup time for audio and animation on various platforms. If the 
	* animation seems consistently out of sync, you can modify the audio delay. It is the 
	* the delay of the audio track with respect to the animation track, expressed in 
	* milliseconds.
	* Therefore, if the audio seems to "run before the lips" increase the value of 
	* audio delay, and vice versa. This will be applied to all audio tracks as they are loaded.
	*
	* @param d audio delay in ms. 
	*/
	void setAudioDelay(int d) {audioDelay=d;};


	/*
	* New implementation of play function.
	* Play an fba file with corresponding audio track.
	* A convenience function that loads an MPEG-4 FBA file as main animation track, attempts
	* to load an audio file with the same name as the FBA file, but with the extension ".wav",
	* and plays the animation and audio.
	* This is equivalent to calling loadMainTrack(), setAudioTrack() and then play(FARenderer *renderer).
	*
	* @param d audio delay in ms.
	* @see loadMainTrack(), setAudioTrack(), play(FARenderer *renderer)
	*/
	void new_play(FARenderer *renderer, int iStartFrm);

	/*
	* Pause animation that is currently played.
	*
	* @see new_play(FARenderer *renderer, int iStartFrm);
	*/
	int new_pause();

	/**
	* Get the face model.
	* Returns the face&body model in its current state of animation.
	* @return the face&body model returned
	*/
	VisageCharModel *getFaceModel();

	/**
	* Get the playing range. 
	* Get the time range that is played when play() function is called. The animation between start time and stop time is played.
	*
	* If both start and stop times are set to 0, the full animation is played. This is the default.
	*
	* @param start returns the start time, in milliseconds.
	* @param end returns the end time, in milliseconds.
	*
	* If AutoStop mode is off, play range is ignored.
	*
	* @see setPlayTimeRange()
	* @see getPlayFrameRange()
	* @see setPlayFrameRange()
	* @see setAutoStopMode()
	* @see getAutoStopMode()
	*/
	void getPlayTimeRange(long *start, long *end);

	/**
	* Set the playing range. 
	* Set the time range that is played when play() function is called. The animation between start time and stop time is played.
	* If the start and end values are out of range, they are clipped to the available range of animation to be played.
	*
	* If both start and stop times are set to 0, the full animation is played. This is the default.
	*
	* If AutoStop mode is off, play range is ignored.
	*
	* @param start start time, in milliseconds.
	* @param end end time, in milliseconds.
	*
	* @see setPlayFramesRange()
	* @see getPlayFramesRange()
	* @see getPlayTimeRange()
	* @see setAutoStopMode()
	* @see getAutoStopMode()
	*/
	void setPlayTimeRange(long start, long end);

	/**
	* Get the playing range. 
	* Get the time range that is played when play() function is called. The animation between start time and stop time is played.
	*
	* If both start and stop times are set to 0, the full animation is played. This is the default.
	*
	* If AutoStop mode is off, play range is ignored.
	*
	* @param start returns the start time, in animation frames.
	* @param end returns the end time, in animation frames.
	*
	* @see setPlayFramesRange()
	* @see getPlayTimeRange()
	* @see setPlayTimeRange()
	* @see setAutoStopMode()
	* @see getAutoStopMode()
	*/
	void getPlayFramesRange(int *start, int *end);

	/**
	* Set the playing range. 
	* Set the time range that is played when play() function is called. The animation between start time and stop time is played.
	* If the start and end values are out of range, they are clipped to the available range of animation to be played.
	*
	* If both start and stop times are set to 0, the full animation is played. This is the default.
	*
	* If AutoStop mode is off, play range is ignored.
	*
	* @param start start time, in animation frames.
	* @param end end time, in animation frames.
	*
	* @see getPlayFramesRange()
	* @see getPlayTimeRange()
	* @see setPlayTimeRange()
	* @see setAutoStopMode()
	* @see getAutoStopMode()
	*/
	void setPlayFramesRange(int start, int end);

	/**
	* Set AutoStop mode. 
	* When the AutoStop mode is on, the player automatically stops playing when all animation tracks are finished,
	* or if there are no animation tracks. When AutoStop is off, the player continues playing, even if empty,
	* until the stop() function is called.
	*
	* The AutoStop mode is ON by default.
	*
	* @param autoStopOn if true, AutoStop is turned on, if false it is turned off.
	*/
	void setAutoStopMode(bool autoStopOn) {autoStopMode = autoStopOn;};

	/**
	* Get AutoStop mode. 
	* When the AutoStop mode is on, the player automatically stops playing when all animation tracks are finished,
	* or if there are no animation tracks. When AutoStop is off, the player continues playing, even if empty,
	* until the stop() function is called.
	*
	* The AutoStop mode is ON by default.
	*
	* @return true if AutoStop is on, false if it is off.
	*/
	bool getAutoStopMode() { return(autoStopMode);};

	/**
	* The play mode. 
	* The play mode can be real time or off-line. In real time mode the frames are skipped if necessary in order to
	* achieve real tyme synchronisation with the animation time stamps. The animation therefore
	* stays synchronised and corresponds to the audio track that plays together with it. In the off-line mode
	* the animation plays frame-by-frame and it is guaranteed that no frames are skipped. This is convenient
	* for exporters, off-line renderers and similar applications that need to get all animation frames.
	*
	* The value can be PLAYMODE_REALTIME or PLAYMODE_OFFLINE. Default is PLAYMODE_REALTIME.
	*/
	int playMode;


	int readFiltersOnPlay;

private:
	bool autoStopMode;
	int playRangeStart;
	int playRangeEnd;
	int playing;
	void init();
	void updateAtTime(long t);
	void update(int nfr);
	void updateFaps(int nfr);
	int updateFaps();
	int new_updateFaps();
	/* IDLE TRACKS
	void updateGlobalTime();
	FAPs *updateIdleTracks(float transitionFactor);
	*/
	long lastStartT;

	FBAPs currentFBaps;
	FBAPs lastFBaps;
	VisageCharModel *face;
	FAPlayerTimer *timer;
	FARenderer *renderer;
	FbaAction *track[100];
	int track_dispensable[100];
	int nTracks;
	int stopPlaying;
	/* IDLE TRACKS
	int nIdleTracks;
	FbaAction *idleTrack[100];
	float idleTrackAtten[100];
	long initialTime;
	long globalTime;
	long lastPlayTime;
	*/
	int audioDelay;
	long currentTime;
};

}

#endif // __FAPlayer_h__
