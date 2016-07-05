/*
 * VirtualActionManager.cpp
 *
 *  Created on: 15/02/2012
 *      Author: narada
 */

#include "VirtualActionManager.h"
#include "NaradaNotificationCenter.hpp"


namespace VisageSDK {

/**
 *
 * @brief Constructor: Initialize data members of VirtualActionManager, and associates it with a Virtual Character
 *
 * @param ch VirtualCharacter that will be assigned to the character data member
 */
VirtualActionManager::VirtualActionManager(VirtualCharacter* ch) {

	character = ch;
	finishedSpeaking = false;
	speaking = false;
	speakRealTime = true;
	faceBackgroundActivated = false;
	bodyBackgroundActivated = false;
	backgroundBodyAction = NULL;
	backgroundFaceAction = NULL;
	lipsyncStatus = VirtualActionManager::notLipsync;
	lipsyncReadyToPlay = "";
    
}

/**
 *
 * @brief Destructor: clears the actions vector, delete the backgroundBodyAction and backgroundFaceAction in case they are defined
 *
 */
VirtualActionManager::~VirtualActionManager() {
#ifdef USE_LIPSYNC

	if(lipsync)
	{
		//TODO: delete files under lipsyncFBAs. After that, clear lipsyncFBAs
		for(int i=0; i< lipsyncFBAs.size(); i++)
		{
			for(int j=0; j< actions.size(); j++)
			{
				if(actions.at(j)->isActionName(*lipsyncFBAs.at(i)))
					actions.at(j)->removeActionFiles();
			}

		}
		lipsyncFBAs.clear();
		delete lipsync;
		lipsync = NULL;
	}
#endif
    stopTemporaryActions();
    temporaryActions.clear();
	actions.clear();
	if (backgroundBodyAction)
		delete backgroundBodyAction;
	if (backgroundFaceAction)
		delete backgroundFaceAction;



}

/**
 * Check if there's some action being played that have just finished, and in that
 * case, removes it from the playlist
 *
 * @see VirtualAction::isPlaying()
 * @see VirtualAction::checkFinished()
 * @see VirtualAction::stopAction()
 * @return std::vector<std::string> with the names of the just finished actions. Handles alternation between FG and BG actions
 */
//* @return false if all sched. foreground actions have finished, true in other case. Handles alternation between FG and BG actions
std::vector<std::string> VirtualActionManager::unfinishedScheduledActions() {
	std::vector<std::string> finishedActions;
	int counter = 0;
	int faceFG = 0;
	int bodyFG = 0;

	for (int i = 0; i < actions.size(); i++) {
		if (actions.at(i)->isPlaying()) {
			counter++;
			if (actions.at(i)->checkFinished()) {
				actions.at(i)->stopAction();
				finishedActions.push_back(actions.at(i)->getActionName());
				counter--;
			} else if (actions.at(i)->isBeingPlayed()) {
				if (actions.at(i)->getKind() == FACE)
					faceFG++;
				else if (actions.at(i)->getKind() == BODY)
					bodyFG++;
				else if (actions.at(i)->getKind() == BOTH)
				{
					faceFG++;
					bodyFG++;
				}
			}
		}
	}

	if (faceBackgroundActivated) {
		if (faceFG) {
			//std::cerr << "stopping face background" << std::endl;
			stopFaceBackground();
		} else if (!backgroundFaceAction->isPlaying()) {
			std::cerr << "playing background" << std::endl;
			playFaceBackground();
		} else
			backgroundFaceAction->checkFinished();
	}
	if (bodyBackgroundActivated) {
		if (bodyFG) {
			//std::cerr << "stopping body background" << std::endl;
			stopBodyBackground();
		} else if (!backgroundBodyAction->isPlaying()) {
			std::cerr << "playing body background" << std::endl;
			playBodyBackground();
		} else
			backgroundBodyAction->checkFinished();
	}
    
    
    //remove temporary actions when finished
    for(int i=0; i< temporaryActions.size(); i++)
    {
        if (temporaryActions.at(i)->isPlaying()) {
			if (temporaryActions.at(i)->checkFinished()) {
				temporaryActions.at(i)->stopAction();
//				finishedActions.push_back(temporaryActions.at(i)->getActionName());
                VirtualAction* actionToDelete = temporaryActions.at(i);
                delete actionToDelete;
                temporaryActions.erase(temporaryActions.begin() + i);
                i--;
                NaradaNotificationCenter::postNotification(VirtualCharacter::naradaNotificationJustFinishedTemporaryAction);
			}
		}
    }

//	if(counter > 0)
//		return true;
//	else if (bodyBackgroundActivated || faceBackgroundActivated)
//		return true;
//
//	return false;
	return finishedActions;
}

/**
 *
 * @brief play an action with a specific name, and with a specific delay (default 0)
 *
 * @param actName name of the action to be played
 * @param msDelay time in ms that the init of the action must be delayed
 *
 * @return true if the action has been correctly set for playing, false otherwise.
 */
bool VirtualActionManager::playAction(std::string actName, long msDelay) {
	for (int i = 0; i < actions.size(); i++) {
		std::cout << "Action at " << i << " is: "
				<< actions.at(i)->getActionName() << std::endl;
		if (actions.at(i)->isActionName(actName)) {
			actions.at(i)->playAction(msDelay);
			return true;
		}
	}
	std::cout << "cannot load action for " << actName
			<< " there's no implementation loaded" << std::endl;

	return false;
}
    

    
/**
 *
 * @brief play an action with a specific name, and with a specific delay (default 0)
 *
 * @param actName name of the action to be played
 * @param msDelay time in ms that the init of the action must be delayed
 *
 * @return true if the action has been correctly set for playing, false otherwise.
 */
    bool VirtualActionManager::playTemporaryAction(std::string path, std::string actionFileName, long msDelay)
    {
        try
        {
            VirtualAction *act = new VirtualAction(path, actionFileName, character, actionFileName);  
            temporaryActions.push_back(act);
            temporaryActions.back()->playAction(msDelay);
            return true;
        }
        catch (NaradaException& e) {
            e.debug();
            return false;
        }
    }
    
    

/**
 *
 * @brief play the background action for face movement.
 *  If there's some backgroundFaceAction defined, plays it
 *
 * @return false if there's no backgroundFaceAction, or the result of the playAction() function otherwise.
 * @see VirtualAction::PlayAction()
 *
 */bool VirtualActionManager::playFaceBackground() {
	if (!backgroundFaceAction)
		return false;

	faceBackgroundActivated = true;
	return backgroundFaceAction->playAction();
}

/**
 *
 * @brief play the background action for body movement.
 *  If there's some backgroundBodyAction defined, plays it
 *
 * @return false if there's no backgroundBodyAction, or the result of the playAction() function otherwise.
 * @see VirtualAction::PlayAction()
 *
 */bool VirtualActionManager::playBodyBackground() {
	if (!backgroundBodyAction)
		return false;

	bodyBackgroundActivated = true;
	return backgroundBodyAction->playAction();
}

/**
 *
 * @brief stops the background action for face movement.
 *  If there's some backgroundFaceAction being played, stops it.
 *
 * @return false if there's no backgroundFaceAction, or the result of the stopAction() function otherwise.
 * @see VirtualAction::StopAction()
 *
 */bool VirtualActionManager::stopFaceBackground() {
	if (!backgroundFaceAction)
		return false;

	if (faceBackgroundActivated) {
		//faceBackgroundActivated = false;
		return backgroundFaceAction->stopAction();
	}

	return false;
}

/**
 *
 * @brief stops the background action for body movement.
 *  If there's some backgroundBodyAction being played, stops it.
 *
 * @return false if there's no backgroundBodyAction, or the result of the stopAction() function otherwise.
 * @see VirtualAction::StopAction()
 *
 */bool VirtualActionManager::stopBodyBackground() {
	if (!backgroundBodyAction)
		return false;

	if (bodyBackgroundActivated) {
		//bodyBackgroundActivated = false;
		return backgroundBodyAction->stopAction();
	}

	return false;
}
    
    
/**
 *
 * @brief stops (and deletes) the temporary actions
 *  If there's some temporary action, stops it and then deletes it
 *
 * @return false if there's no temporaryAction, true otherwise.
 * @see VirtualAction::StopAction()
 *
 */
bool VirtualActionManager::stopTemporaryActions()
{    
    bool ret = false;
    for(int i=0; i< temporaryActions.size(); i++)
    {
        temporaryActions.at(i)->stopAction();
        VirtualAction* actionToDelete = temporaryActions.at(i);
        delete actionToDelete;
        temporaryActions.erase(temporaryActions.begin() + i);
        i--;
        ret = true;
    }
    
    return ret;
 }
    
    

/**
 *
 * @brief setter for speakRealTime property
 *  if speakRealTime is set to TRUE, then LipSync and TTS are played while computed.
 *  Otherwise, firts, animation is computed, and then animation is reproduced (recommended for slow devices).
 *
 * @param realTime boolean with the value we want to assign to speakRealtime
 *
 * @see loadActions()
 *
 */
void VirtualActionManager::setSpeakRealTime(bool realTime) {
	speakRealTime = realTime;
#ifdef USE_LIPSYNC
	if(lipsync)
	delete lipsync;
	lipsync = new VisageLipSync(!speakRealTime/*not sequential*/);
	lipsync->setHandleEventMode(LS_HE_MODE_VISEME_BASED);

	character->getFAPlayer()->addTrack(lipsync);
	lipsync->stopSync();
	character->getFAPlayer()->removeTrack(lipsync);
#endif
}
    
    /**
     * @brief get the number of basicActions in the .act file and post a notification
     *
     */
    void VirtualActionManager::countBasicActions(std::string path, std::string characterName)
    {
        FILE *fp = fopen((path.append("/").append(characterName).append(".act")).c_str(),
                   "r");
        if (!fp) {
            std::cout << "cannot count actions, there's no file at " << path.c_str()
            << std::endl;
            return;
        }
                
        //////////////////
        //get the number of nodes tagged with: Shape, Coordinate (and TextureCoordinate) and Normal, to have an idea of the progress
        char fileReadLine[100];
        int relevantActionsForProgress = 0;
        while (!feof(fp))
        {
            if ( fgets (fileReadLine , 100 , fp) != NULL )
            {
                if(strstr(fileReadLine, "<basicAction>"))
                {
                    relevantActionsForProgress++;
                }
            }
        }
        //We add one extra action for progress to ensure that after start loading the last
        //action, the progress bar is not at 100%
        relevantActionsForProgress++;
        //    std::cout << "AFM file analyzed to monitorize the progress, number of relevant nodes for progress: " << relevantNodesForProgress << std::endl;
        NaradaNotificationCenter::postNotification(naradaNotificationLoadInitReport, relevantActionsForProgress);
        rewind(fp);
        
        /////////////////
        fclose(fp);
    }

/**
 *
 * @brief load all the actions associated to the character in use.
 *  If USE_TTS is enabled, it initiates the voice data member as NaradaVerbioTTS.
 *  It reads the XML .act name with the name of the character, so it can parse it and read separately all the actions
 *  defined in that file. Then, it load those actions, differentiating the background face and body animations, from the
 *  foreground ones, that are stored simply in the actions vector.
 *
 * @todo TODO: Extend the code to use lipsync easily, like if it was TTS, and limit its usage with USE_LIPSYNC symbol
 *
 * @param path path to the folder where the character and it's actions are stored
 * @param characterName name of the character
 *
 * @return false if it cannot load the actions, true otherwise
 *
 * @see parseActionsFile()
 * @see VirtualAction::VirtualAction()
 *
 */bool VirtualActionManager::loadActions(std::string path,
		std::string characterName) {
#ifdef USE_TTS
	voice = new NaradaVerbioTTS(!speakRealTime/*not sequential*/);
	voice->setHandleEventMode(LS_HE_MODE_VISEME_BASED);

	character->getFAPlayer()->addTrack(voice);
	voice->stopSync();
	character->getFAPlayer()->removeTrack(voice);
#endif

#ifdef USE_LIPSYNC
	lipsync = new VisageLipSync(!speakRealTime/*not sequential*/);
	lipsync->setHandleEventMode(LS_HE_MODE_VISEME_BASED);

	character->getFAPlayer()->addTrack(lipsync);
	lipsync->stopSync();
	character->getFAPlayer()->removeTrack(lipsync);
#endif

	FILE *fp;
	char* buffer;
	long fileSize;
	long readSize;
	VirtualAction *va;
	std::string pathToCharacter = path;

	fp = fopen((path.append("/").append(characterName).append(".act")).c_str(),
			"r");
	if (!fp) {
		std::cout << "cannot load actions, there's no file at " << path.c_str()
				<< std::endl;
		return false;
	}
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	rewind(fp);

	buffer = (char*) malloc(fileSize * sizeof(char));
	readSize = fread(buffer, 1, fileSize, fp);
	if (readSize != fileSize) {
		std::cout << "cannot read actions file, fread / fileSize " << readSize
				<< "/" << fileSize << std::endl;
		return false;
	}

	std::string actionsContent(buffer);

	fclose(fp);
	free(buffer);

	std::vector<animationData*> actString = parseActionsFile(actionsContent);

	for (int i = 0; i < actString.size(); i++) {
		if (actString.at(i)->background) {
			if (actString.at(i)->kind == FACE) {
				try {
					backgroundFaceAction = new VirtualAction(actString.at(i),
							pathToCharacter, character);
				} catch (NaradaException& e) {
					e.debug();
					continue;
				}
			} else if (actString.at(i)->kind == BODY) {
				try {
					backgroundBodyAction = new VirtualAction(actString.at(i),
							pathToCharacter, character);
				} catch (NaradaException& e) {
					e.debug();
					continue;
				}
			} else
				std::cerr << "unconsidered case of background action: "
						<< actString.at(i)->kind << std::endl;
		} else {
			try {
				va = new VirtualAction(actString.at(i), pathToCharacter,
						character);
			} catch (NaradaException& e) {
				e.debug();
				continue;
			}
			actions.push_back(va);
		}
	}

	actString.clear();

	return true;

}

/**
 *
 * @brief parses the XML file with the description of the actions.
 *  Reads the XML file, and stores each action in a vector, as an animationData. This animationData structure
 *  also contains the info about if the action will be played in loop or not, if its background or not, and if it's
 *  face, body, or both kind of animation.
 *
 * @param actionsFile string that contains the XML file with the description of the actions.
 *
 * @return a vector with each action stored as an animationData
 *
 */
std::vector<animationData*> VirtualActionManager::parseActionsFile(
		std::string actionsFile) {
	size_t start = 0;
	size_t end = 0;
	std::string actionInfo;
	std::vector<animationData*> result;
	bool bg = false;
	bool loop = false;
	ActionKind kind;

	while (start != std::string::npos) {
		start = actionsFile.find("<action ", start);
		if (start == std::string::npos)
			break;

		//background property
		start = actionsFile.find("bg=", start);
		actionInfo = actionsFile.substr(start + 3, 5);
		if (!actionInfo.compare("true "))
			bg = true;
		else if (!actionInfo.compare("false"))
			bg = false;
		else
			std::cout << "corrupt actions file, bg property undefined"
					<< std::endl;

		//loop property
		start = actionsFile.find("loop=", start);
		actionInfo = actionsFile.substr(start + 5, 5);
		if (!actionInfo.compare("true "))
			loop = true;
		else if (!actionInfo.compare("false"))
			loop = false;
		else
			std::cout << "corrupt actions file, loop property not found"
					<< std::endl;

		//kind property
		start = actionsFile.find("kind=", start);
		actionInfo = actionsFile.substr(start + 5, 4);
		if (!actionInfo.compare("face"))
			kind = FACE;
		else if (!actionInfo.compare("body"))
			kind = BODY;
		else if (!actionInfo.compare("both"))
			kind = BOTH;
		else
			std::cout << "corrupt actions file, kind property not found"
					<< std::endl;

		start = actionsFile.find("name=", start);
		end = actionsFile.find("</action>", start) + 9;
		if (end > actionsFile.size()) {
			std::cout << "corrupt actions file, </action> not found"
					<< std::endl;
			break;
		}

		animationData* res = new animationData();
		res->background = bg;
		res->loop = loop;
		res->kind = kind;
		res->xmlDescription = actionsFile.substr(start, end - start);

		result.push_back(res);
		start = end;
	}
	return result;
}

/**
 *
 * @brief checks if the voice has finished the speech or not.
 *  If speaking is true and the voice has just finished, the finishedSpeaking flag is set to true,
 *   while the speaking data member is set to false.
 *
 * @return returns true if the TTS action have finished
 *
 * @see NaradaVerbioTTS::isFinished()
 *
 * @todo TODO: Asociate it also with lipsync.
 *
 */
bool VirtualActionManager::checkVoiceSpeech() {
#ifdef USE_TTS
	if(speaking)
	{
		if(voice->isFinished())
		{
			finishedSpeaking = true;
			speaking = false;
			std::cout << "end speaking" << std::endl;
            return true;
		}
	}

#endif
    return false;

}

/**
 *
 * @brief checks if the voice has finished the speech or not.
 *  If speaking is true and the voice has just finished, the finishedSpeaking flag is set to true,
 *   while the speaking data member is set to false.
 *
 * @see NaradaVerbioTTS::isFinished()
 *
 * @todo TODO: Asociate it also with lipsync.
 *
 */
bool VirtualActionManager::checkLipsyncSpeech() {
#ifdef USE_LIPSYNC
	switch(lipsyncStatus)
	{
		case VirtualActionManager::notLipsync:
            return false;
            

		case VirtualActionManager::lipsyncSequentialReadyToPlay:
            playAction(lipsyncReadyToPlay);
            lipsyncStatus = VirtualActionManager::lipsyncSequentialPlaying;
            return false;
            
        case lipsyncShutUp:
            lipsyncStatus = VirtualActionManager::lipsyncDone;
            return false;
            
        case lipsyncRealTimePlaying:
            if(lipsync->isFinished())
            {
                lipsyncStatus = VirtualActionManager::lipsyncDone;
            }
            return false;
            
        case lipsyncDone:
            lipsyncStatus = VirtualActionManager::notLipsync;
            return true;

		default:
        return false;
		break;

	}

#endif

}

/**
 *
 * @brief lipsyncStatus getter
 *
 * @return returns the vaule of lipsyncStatus paramenter
 *
 */
int VirtualActionManager::getLipsyncStatus() {
	return lipsyncStatus;
}
    
    bool VirtualActionManager::shutUp() {
        std::cerr << "Shutting up" << std::endl;
        lipsync->stopSync();
        stopActions();
        stopAllActions();
        return stopTemporaryActions();
    }
    
/**
 *
 * @brief stops the action specified with some name
 *
 * @param actName name of the action that must be stopped
 *
 * @return false if there's no implementation with this action name, true otherwise
 *
 * @see VirtualAction::isActionName()
 * @see VirtualAction::stopAction()
 *
 */bool VirtualActionManager::stopAction(std::string actName) {
	for (int i = 0; i < actions.size(); i++) {
		if (actions.at(i)->isActionName(actName)) {
			actions.at(i)->stopAction();
			return true;
		}
	}
	std::cout << "cannot stop action for " << actName
			<< " there's no implementation loaded" << std::endl;

	return false;
}

/**
 *
 * @brief Stop all the foreground actions.
 *  It stops all the actions inside the actions vector
 *
 * @return true
 *
 * @see VirtualAction::stopAction()
 *
 */bool VirtualActionManager::stopActions() {
     lipsyncStatus= VirtualActionManager::lipsyncShutUp;

	for (int i = 0; i < actions.size(); i++)
		actions.at(i)->stopAction();

	return true;
}

/**
 *
 * @brief Stops all the actions of the character.
 * It stops all the actions inside the actions vector, but also the backgroundBodyAction and the backgroundFaceAction
 *
 * @return true
 *
 * @see VirtualAction::stopAction()
 * @see VirtualAction::stopBodyBackground()
 * @see VirtualAction::stopFaceBackground()
 *
 */bool VirtualActionManager::stopAllActions() {
	for (int i = 0; i < actions.size(); i++)
		actions.at(i)->stopAction();

	stopBodyBackground();
	stopFaceBackground();

	return true;
}

/**
 *
 * @brief Says the text passed as argument using the TTS. Previously, it analyze the text to find actions between [],
 *  and schedules them to play, with a delay depending on the position in the text and the TIME_COEF_MS_PER_LETTER and
 *  TIME_COEF_MS_PER_PUNCTUATION constants, that mainly depend on the TTS engine and language used.
 *
 * @param text text to be played with TTS
 * @return true
 *
 * @see NaradaVerbioTTS::sayText()
 * @see VirtualAction::playAction()
 */bool VirtualActionManager::say(std::string text) {
	speaking = true;
	size_t start = 0;
	size_t end = 0;
	std::vector<std::string> scheduledAction;
	std::vector<long> delay;
	while (start != std::string::npos) {
		start = text.find("[", start);
		if (start == std::string::npos)
			break;

		end = text.find("]", start) + 1;
		if (end > text.size()) {
			std::cout << "corrupt text file, closing ] not found" << std::endl;
			break;
		}
		scheduledAction.push_back(
				std::string(text.substr(start + 1, end - start - 2)));
		delay.push_back(
				(start * TIME_COEF_MS_PER_LETTER
						+ TIME_COEF_MS_PER_PUNCTUATION
								* countPunctuationMarks(text.substr(0, start))));
		text.erase(start, scheduledAction.back().size() + 2);
	}

#ifdef USE_TTS
	character->getFAPlayer()->addTrack(voice);
	voice->sayText(text);
	std::cout << "let's say: " << text << std::endl;
	character->getFAPlayer()->play();
#else
	std::cout << "USE_TTS is disabled, so cannot say: " << text << std::endl;
#endif
	for (int i = 0; i < scheduledAction.size(); i++) {
		playAction(scheduledAction.at(i), delay.at(i));
		std::cout << "programming action: " << scheduledAction.at(i)
				<< " with a delay(ms) of " << delay.at(i) << std::endl;
	}
	scheduledAction.clear();
	delay.clear();

	return true;

}

/**
 *
 * @brief returns whether the character was talking and now has finished speaking, or not
 *
 * @return true if it has finished speaking at this moment, false otherwise.
 */bool VirtualActionManager::justFinishedSpeaking() {
	if (finishedSpeaking) {
		finishedSpeaking = false;
		return true;
	}
	return false;
}

/**
 *
 * @brief returns if the character is speaking or not
 *
 * @return true if it's speaking, false otherwise
 */bool VirtualActionManager::isSpeaking() {
	return speaking;
}

/**
 *
 * @brief Repeats the WAV passed as argument using the vslipsync library.
 *
 * @param wavFile path to the WAV file, 16Khz, to be played
 * @return false if anything goes wrong, true otherwise
 *
 * @see NaradaVerbioTTS::sayText()
 * @see VirtualAction::playAction()
 */bool VirtualActionManager::repeat(std::string wavFile, bool temporary) {
#ifndef USE_LIPSYNC
	std::cout << "USE_LIPSYNC is disabled, so cannot say: " << wavFile << std::endl;
	return false;

#else

	if (speakRealTime) {
		lipsyncStatus = VirtualActionManager::lipsyncRealTimePlaying;
		character->getFAPlayer()->removeTrack(lipsync); //remove the track because I can't do it when lipsync ends
		character->getFAPlayer()->addTrack(lipsync);
		lipsync->sync((char *)wavFile.c_str(), NULL, NULL, false/*not mute*/);
		std::cout << "let's repeat: " << wavFile << std::endl;

	} else {

		std::string extension = wavFile.substr(wavFile.size() - 4, 4);
		if ((extension.compare(".wav") && extension.compare(".WAV"))) {
			std::cout << "wavFile argument has not .wav or .WAV extension, so it's not valid!! " << wavFile << std::endl;
			return false;
		}
		bool hasBeenCreatedBefore = false;
        
		
        size_t lastSlash = wavFile.find_last_of("/\\");
		std::string path = wavFile.substr(0, lastSlash);
		std::string actName = wavFile.substr(lastSlash + 1,
				wavFile.size() - 4 - (lastSlash + 1));
        
        
		std::cout << "Real act name: " << actName << std::endl;

		std::vector<std::string*>::iterator it;
		for (it = lipsyncFBAs.begin(); it < lipsyncFBAs.end(); it++) 
        {
			std::cout << "stored action names: " << (*it)->c_str() << std::endl;
			if (!(*it)->compare(actName))
				hasBeenCreatedBefore = true;
		}

		if (hasBeenCreatedBefore) 
        {
            if(lipsyncStatus!= VirtualActionManager::lipsyncShutUp)
            {
                std::cout << "the action has been created before, so it already exists" << std::endl;
                if(temporary)
                {
    #ifdef IPHONE
                    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
//                    NSArray *NSpaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//                    path = std::string([(NSString*)[NSpaths objectAtIndex:0] UTF8String]);
                    path = std::string([NSTemporaryDirectory() UTF8String]);
                    [pool release];
    #endif
                    lipsyncStatus = VirtualActionManager::lipsyncSequentialPlaying;
                    lipsyncReadyToPlay = actName;
                    playTemporaryAction(path, std::string(actName).append(".FBA"));
                }
                else
                {
                    lipsyncStatus = VirtualActionManager::lipsyncSequentialReadyToPlay;
                    lipsyncReadyToPlay = actName;
                }
            }
		}
        
        else 
        {
			std::cout << "let's create lipsync in sequential mode for: " << wavFile << std::endl;
			lipsyncStatus = VirtualActionManager::lipsyncSequentialCreatingFBA;
			lipsyncFBAs.push_back(new std::string(actName));
			character->getFAPlayer()->addTrack(lipsync);

            std::string fullPath;
            
#ifdef IPHONE
            
            

//            std::string fbaFileName = std::string(actName);
//            fbaFileName.append(".FBA");
//            
//            NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
//            
//            NSArray *NSpaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//            
//            NSString *NSfileString =	[NSString stringWithUTF8String:fbaFileName.c_str()];
//            NSString *NSpath = [ [NSpaths objectAtIndex:0] stringByAppendingPathComponent:NSfileString ];
//            
//            NSFileManager *fileManager = [NSFileManager defaultManager];
//            [fileManager removeItemAtPath:NSpath error:NULL];
//            
//            fullPath = std::string([NSpath UTF8String]);
//            path = std::string([(NSString*)[NSpaths objectAtIndex:0] UTF8String]);
//            
//            
//            
//            //Also copy WAV file to same path, so when loading the action it also loads the audio
//            NSError *error;
//            
//            NSFileManager *NSfmngr = [[NSFileManager alloc] init];
//            
//            NSfileString = [NSString stringWithUTF8String:wavFile.c_str()];
//            NSMutableString *NSfileName = [NSMutableString stringWithUTF8String:actName.c_str()];
//            //[NSfileName appendString:[NSString stringWithUTF8String:".wav"]];
//            [NSfileName appendString:@".wav"];
//            
//            NSpath = [[NSpaths objectAtIndex:0] stringByAppendingPathComponent:NSfileName];
//
//            if(![NSfmngr copyItemAtPath:NSfileString toPath:NSpath error:&error]) {
//                // handle the error
//                NSLog(@"Error during the WAV file copy: %@", [error description]);
//                
//            }
            
            
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            
            NSString *actNameStr = [[NSString stringWithUTF8String:actName.c_str()] stringByAppendingPathExtension:@"FBA"];
            NSString *wavNameStr = [[NSString stringWithUTF8String:actName.c_str()] stringByAppendingPathExtension:@"wav"];
            
            NSString *actDstPath = [NSTemporaryDirectory() stringByAppendingPathComponent:actNameStr];
            NSString *wavDstPath = [NSTemporaryDirectory() stringByAppendingPathComponent:wavNameStr];
            
            fullPath = [actDstPath UTF8String];
            
            NSError *er;
            if ([[NSFileManager defaultManager] fileExistsAtPath:actDstPath]) {
                [[NSFileManager defaultManager] removeItemAtPath:actDstPath error:&er];
            }
            
            NSString *wavSrcPath = [NSString stringWithUTF8String:wavFile.c_str()];
            
            NSData *wavData = [NSData dataWithContentsOfFile:wavSrcPath];
            [wavData writeToFile:wavDstPath atomically:YES];
            
            if (![[NSFileManager defaultManager] copyItemAtPath:wavSrcPath toPath:wavDstPath error:&er]) {
                NSLog(@"Error during the WAV file copy: %@", [er description]);
            }
            
            
            [pool release];
  
#else
            
			fullPath = std::string(path);
			fullPath.append("/");
			fullPath.append(actName);
			fullPath.append(".FBA");
            
#endif
            

			std::cout << "fullpath: " << fullPath << std::endl;
			lipsync->sync((char *)wavFile.c_str(), (char *)fullPath.c_str(), NULL, true/*mute*/);

			lipsync->stopSync();
			character->getFAPlayer()->removeTrack(lipsync);
            
            
            if(temporary)
            {
                if(lipsyncStatus!= VirtualActionManager::lipsyncShutUp)
                {
                    lipsyncStatus = VirtualActionManager::lipsyncSequentialPlaying;
                    lipsyncReadyToPlay = actName;
                    playTemporaryAction(path, std::string(actName).append(".FBA"));
                }
            }
            else
            {
                if(lipsyncStatus!= VirtualActionManager::lipsyncShutUp)
                    lipsyncStatus = VirtualActionManager::lipsyncSequentialReadyToPlay;
                lipsyncReadyToPlay = actName;

                try
                {
                    VirtualAction *va = new VirtualAction(path, std::string(actName).append(".FBA"), character, actName);
                    actions.push_back(va);
                }
                catch (NaradaException& e){ e.debug(); }
            }

		}

	}

	return true;

#endif

}

/**
 *
 * @brief Counts the punctuation marks inside a text.
 *
 * @param str string with the text to analyze
 *
 * @return the number of punctuation marks inside the text.
 * @see say()
 *
 */
int VirtualActionManager::countPunctuationMarks(std::string str) {
	int counter = 0;

	char tokens[] = ",.:;?!";
	char * pointerToPunctuation;

	pointerToPunctuation = strpbrk(str.c_str(), tokens);
	while (pointerToPunctuation != NULL) {
		counter++;
		pointerToPunctuation = strpbrk(pointerToPunctuation + 1, tokens);
	}

	std::cout << "now, found " << counter << " punctuation marks of interest"
			<< std::endl;

	return counter;

}

} /* namespace VisageSDK */
