//!
//!  \file Manager.hpp
//!
//! \author Enrique Oriol <eoriol@naradarobotics.com>
//!
//! \date 03-03-2010
//!



#ifndef MANAGER_HPP
#define MANAGER_HPP



#include <iostream>
#include <exception>

#include <stdio.h>
#include <sys/time.h>

#include "OGLViewer.h"

#include "NaradaAudioRecorder.h"
#include "ActionManager.h"


#include "LipSync.h"

#include <sys/types.h>
#include <sys/sysctl.h>


#include <cmath>
#include <string>
#include <vector>



#define defaultFrameRate	25
#define recordingFrameRate	25 //rarreaza 2012-02-20 modified to correct buggy rotation while recording

//for moveModel Used also in ES1renderer.m
#define kMaxX 6
#define kMinX -6
#define kMaxY -6
#define kMinY -6

#define kMaxYaw 20.0f
#define kMinYaw -20.0f
#define kMaxPitch 12.0f
#define kMinPitch -12.0f

#define kInitYaw 0.0f
#define kInitPitch -4.0f

//rarreaza 2012-01-25 defines for differentiating wav and fba files for high pitch implementation
#define _DEFAULT_NORMALPITCH_AUDIO_FILENAME @"test.wav"
#define _DEFAULT_NORMALPITCH_ACTIONS_FILENAME @"test.fba"
#define _DEFAULT_HIGHPITCH_AUDIO_FILENAME @"test_high_pitch.wav"
#define _DEFAULT_HIGHPITCH_ACTIONS_FILENAME @"test_high_pitch.fba"
#define _MUTE_NORMALPITCH_ACTIONS_FILENAME @"test_Mute.fba"
#define _MUTE_HIGHPITCH_ACTIONS_FILENAME @"test_high_pitch_Mute.fba"



using std::string;
using namespace VisageSDK;


class ActionManager;

class Manager
{
	
	public:

		//Manager(){};
		
		Manager(/*FAPlayer* player*/)
		{
			//_my_faplayer = player;
			
			_doExit = false;
			_isRecordingAudio = false;
			
			
			_lipSync = false;
			
			_playRecMovie = false;
			
			_playRecMovieON = false;
			
			_playRecAudio = false;
            
            _playRecAudioInHighPitch = false;
						
			_listening = false;
			
			_annoyed = false;
			
			_laugh = false;
			
			_yawn = false;
			
			_isModelLoaded = false;
            
            _shouldDoHigherPitch=false;
            
			
			readDevice();
			

		};
	
	
		~Manager()
		{
			printf("manager destructor");
				if(_my_viewer)
					delete _my_viewer;
			if(_my_lipsync)
				delete _my_lipsync;
		}
	
		OGLViewer* getViewer(){return _my_viewer;};
	
	
		bool isModelLoaded();
	
		bool clearScene();
	
		bool setPerspective(float width, float height);
	
		bool renderScene();
	
		bool initApplication(int width, int height);
	
		bool quitApplication();
	
	
	
        void playAction(string act);
    
        void playLipSync();
	
		void playAnnoy();
	
		void playLaugh();
	
		void playYawn();
	
		void playRecordedAnim();
	
		void playRecordedAudio();
    
        void playRecordedAudioInHighPitch();
	
		bool hasFinishedPlaying();
	
		void removeActionsWithAudio();
	
		void restoreDefaultFrameRate();
	
	
		bool recordAudio();
	
		void stopRecording();
	
		void trackRotation();

		float getTrackRotX(int position);
	
		float getTrackRotY(int position);
	
		float pullTrackRotX();
		float pullTrackRotY();
	
//		float getTranslateFitX(){_my_viewer->getTranslateFitX();}
//		float getTranslateFitY(){_my_viewer->getTranslateFitY();}

	
		void rotateToInit();
		void rotateToNextPosition();
		void rotateToPosition();
	
	
		bool isRecordingAudio() {return _isRecordingAudio;}
		
		void moveModel(float incX, float incY);
		void moveModelAbs(float incX, float incY);
		
		void rotateModel(float incX, float incY);
        void rotateModelAbs(float yaw, float pitch, bool moveBackground);
		
		void moveCloser(float incZ);
		void moveCloserAbs(float incZ);
	
	
	//------------------------------------------
	


		void updateScene();
	

		BOOL isRecording() {return _playRecMovieON;}
	
	
	
		bool endRecordingAudioBecauseExceedsSize()
		{
			if(_isRecordingAudio)
			{
				if(my_recorder->hasExceededMaxSize())
					return true;
			}
			
			return false;
		}
	
	
		void readDevice();
	
		int getFrameRate();
		int getNFrames();
	
	
		char* getPathTo(string file);
	
		char* getWritablePathTo(string file);
	
		char* getWritablePathToReadFrom(string file);
	
		void changeBackgroundImage(UIImage* image);
	
		void setDefaultBackground();
    
        void setDoPitchShift(bool shouldDo);
        bool getDoPitchShift();
	
		
	private:

	
		bool _lipSync;
	
		bool _playRecMovie;
	
		bool _playRecMovieON;
	
		bool _playRecAudio;
    
        bool _playRecAudioInHighPitch;

		bool _laugh;

		bool _annoyed;
	
		bool _yawn;
	
		bool _listening;
	
		bool _isRecordingAudio;
	
	
		bool _isModelLoaded;

		bool _doExit;
	
		bool playerHasAutostop;
    
        bool _shouldDoHigherPitch;
	
		string modelName;
	
		string background;
	
		OGLViewer *_my_viewer;
    
        VirtualCharacter *character;
	
		NaradaAudioRecorder *my_recorder;
	
		VisageLipSync* _my_lipsync;
	
	
	
		vector<ActionManager*> _actions;
	
		vector<float> rotTrackX;
		vector<float> rotTrackY;
		vector<int> animationTrack;
	
		int rotTrackFrameRateAdjust;
		
	
		void loadWelcome();
	
		void loadListening();
	
		void loadRecordedAnim();
	
	
	
		void applyStatus();

		void checkOldStatus();
			

	
	
//	void setAnimation(ANIMATION anim);
//	
//	void playAnimation(ANIMATION anim);
//	
//	
//
//	void deleteActionManager(int index);
//		
//	void deleteActionManager(ANIMATION anim);
	
	
	void checkActionsPlaying();

	

		
//	FbaAction *getActionFromManager(ANIMATION anim);
	
	
		void removeAllActions();
	
	
//	ActionManager* getActionManager(ANIMATION anim);
//		
//
//	int getActionManagerIndex(ANIMATION anim);
//    
//    
	
		
};
			




#endif


