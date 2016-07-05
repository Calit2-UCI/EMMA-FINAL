/*
 * VirtualActionManager.h
 *
 *  Created on: 15/02/2012
 *      Author: narada
 */

#ifndef VIRTUALACTIONMANAGER_H_
#define VIRTUALACTIONMANAGER_H_


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <exception>
#include <vector>
#include "NaradaException.h"
#include "VirtualAction.h"

#ifdef USE_TTS
	#include "NaradaVerbioTTS.h"
#endif
#ifdef USE_LIPSYNC
	#include "LipSync.h"
#endif
#define TIME_COEF_MS_PER_LETTER 62
#define TIME_COEF_MS_PER_PUNCTUATION 165

#include "NaradaNotificationCenter.hpp"


namespace VisageSDK {

class VirtualAction;
class VirtualCharacter;


/**
* @class VirtualActionManager
* @brief Class that simplifies basic management of a actions by a VirtualCharacter
*
* @see VirtualCharacter, VirtualAction
*
*/
class VirtualActionManager {

public:
    

	static const int notLipsync = 0;
	static const int lipsyncSequentialCreatingFBA = 1;
	static const int lipsyncSequentialReadyToPlay = 2;
	static const int lipsyncSequentialPlaying = 3;
	static const int lipsyncRealTimePlaying = 4;
	static const int lipsyncDone = 5;
    static const int lipsyncShutUp = 6;


	VirtualActionManager(VirtualCharacter* character);
	virtual ~VirtualActionManager();


    void countBasicActions(std::string path, std::string characterName);
	bool playAction(std::string actionName, long msDelay=0);
    bool playTemporaryAction(std::string path, std::string actionFileName, long msDelay=0);
	bool playFaceBackground();
	bool playBodyBackground();
	bool stopFaceBackground();
	bool stopBodyBackground();
    bool stopTemporaryActions();
	bool stopAction(std::string actName);
	bool stopActions();
	bool stopAllActions();

	bool say(std::string);
	bool shutUp();
	bool isPlaying();
	bool repeat(std::string wavFile, bool temporary=false);
	//bool justFinishedPlaying();
	bool justFinishedSpeaking();
	bool isSpeaking();
	void setSpeakRealTime(bool realtime);
	int getLipsyncStatus();
	bool checkLipsyncSpeech();

#ifdef USE_TTS
		NaradaVerbioTTS * getVoice(){return voice;}
#endif
#ifdef USE_LIPSYNC
		VisageLipSync* getLipSync(){return lipsync;}
#endif

	bool checkVoiceSpeech();
	std::vector<std::string> unfinishedScheduledActions();
	bool loadActions(std::string path, std::string charName);

protected:
	std::vector<animationData*> parseActionsFile(std::string actionsFile);
	int countPunctuationMarks(std::string str);

	std::vector<VirtualAction*> actions;
    std::vector<VirtualAction*> temporaryActions;
	std::vector<std::string*> lipsyncFBAs;
	VirtualAction* backgroundBodyAction;
	VirtualAction* backgroundFaceAction;
	bool faceBackgroundActivated;
	bool bodyBackgroundActivated;

	VirtualCharacter* character;
	bool finishedSpeaking;

	bool speaking;

	bool speakRealTime;

	int lipsyncStatus;
	std::string lipsyncReadyToPlay;

#ifdef USE_TTS
		NaradaVerbioTTS * voice;
#endif
#ifdef USE_LIPSYNC
		VisageLipSync* lipsync;
#endif

};

} /* namespace VisageSDK */
#endif /* VIRTUALACTIONMANAGER_H_ */
