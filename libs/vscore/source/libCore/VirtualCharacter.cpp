/*
 * VirtualCharacter.cpp
 *
 *  Created on: 09/01/2012
 *  Author: narada
 */

#include "VirtualCharacter.hpp"



namespace VisageSDK
{

    const std::string VirtualCharacter::naradaNotificationJustFinishedSpeaking = "naradaNotificationJustFinishedSpeaking";
    const std::string VirtualCharacter::naradaNotificationJustFinishedTemporaryAction  = "naradaNotificationJustFinishedTemporaryAction";
    const std::string VirtualCharacter::naradaNotificationShutUp = "naradaNotificationShutUp";
    const std::string VirtualCharacter::naradaNotificationLoadingProgressPercentage = "naradaNotificationLoadingProgressPercentage";

	/**
	 * @brief Constructor: Loads an AFM model to represent the virtual character. Then initializes
	 * the player with the loaded model.
     * Reports the loading progress with narada notifications like this: postNotification(naradaNotificationLoadingProgressPercentage, loadingPercentage)
	 *
	 * @param afmPath   string to the file with the AFM model to load, as a representation of virtual character
	 * @param perspTanFOVY   float with tan(FOVY) of the perspective. It has a parameter tan(45/2) as default value.
	 */
	VirtualCharacter::VirtualCharacter(std::string afmPath, bool persp, float perspTgFOVY)
	{
        modelLoadProgressCheckpoints = 0;
        modelLoadProgressAchieved = 0;
        
        NaradaNotificationCenter::subscribeNotification(naradaNotificationLoadInitReport, this, &VirtualCharacter::addModelLoadProgressCheckpoints);
        NaradaNotificationCenter::subscribeNotification(naradaNotificationLoadProgressReport, this, &VirtualCharacter::handlerForLoadProgress);
        
		setName(afmPath);
        xRot = 0;
        yRot = 0;
        zRot = 0;
        
		modelWidth = 0;
		modelHeight = 0;
		perspVisibleLimitX = 0;
		perspVisibleLimitY = 0;
		actions = new VirtualActionManager(this);

		m_FAPlayer = new FAPlayer();
		videoPlayer = NULL;

        //count basic actions to check progress
        actions->countBasicActions(pathToCharacter, characterName);
        AFM::countRelevantNodesToMonitorizeLoadProgress(afmPath);
        
		 // load the AFM model
		afm = new AFM(const_cast<char*>(afmPath.c_str()));
		if(!afm->getVCModel()->initialized)
			throw NaradaException("cannot initialize AFM model");

		// initialize player with the loaded model
		m_FAPlayer->init(afm);
        this->perspective = persp;
		this->perspTanFOVY = perspTgFOVY;
		if(!centerCharacter(perspTanFOVY))
			throw NaradaException("cannot center AFM model");
        
		actions->loadActions(pathToCharacter, characterName);
        

	}



	/**
	 * @brief Destructor: destroys the VirtualCharacter object
	 *
	 */
	VirtualCharacter::~VirtualCharacter() {
        NaradaNotificationCenter::unsubscribeNotification(naradaNotificationLoadInitReport, this, &VirtualCharacter::addModelLoadProgressCheckpoints);
        NaradaNotificationCenter::unsubscribeNotification(naradaNotificationLoadProgressReport, this, &VirtualCharacter::handlerForLoadProgress);
        
		if(afm!=NULL)
        {
			delete afm;
            afm = NULL;
        }
		if(m_FAPlayer!=NULL)
        {
			delete m_FAPlayer;
            m_FAPlayer = NULL;
        }
		if(actions)
        {
			delete actions;
            actions = NULL;
        }
	}



	/**
	 * @brief Centers the character to appear in the middle of the viewport
	 *
	 * @param afmPath   string to the file with the AFM model to load, as a representation of virtual character
	 * @param perspTanFOVY   float with tan(FOVY) of the perspective. It has a parameter tan(45/2) as default value.
	 * @see VirtualCharacter()
	 * @return true
	 */
	bool VirtualCharacter::centerCharacter(float perspTanFOVY)
	{
		VRMLModel *theModel= dynamic_cast<AFM*>(m_FAPlayer->getFaceModel())->getVCModel();

	    // Retrieve max and min in all axes and also the worl-transformed coordinates in a vector.
	    VRMLFitToScreenTraverser *tempFitter = new VRMLFitToScreenTraverser;

	    tempFitter->Reset();
	    theModel->traverse(theModel->rootNode, tempFitter);

	    vector<Point3d> mStoredVerts;
	    GLfloat sceneMinX, sceneMaxX, sceneMinY, sceneMaxY, sceneMinZ, sceneMaxZ, mMaxRadius;

	    tempFitter->GetCoords(sceneMinX, sceneMaxX, sceneMinY, sceneMaxY, sceneMinZ, sceneMaxZ, mMaxRadius, mStoredVerts);

	    modelWidth = sceneMaxX - sceneMinX;
	    modelHeight = sceneMaxY - sceneMinY;

        distance = (GLfloat)((mMaxRadius / 2.0) / perspTanFOVY);

        // Make this constant.
        std::cout << "distance: " << distance << std::endl;

        //We translate the model to move it to the center of the scene
        translateX = - (GLfloat)((sceneMinX + (abs(sceneMaxX - sceneMinX) / 2.0)));
        translateY = - (GLfloat)((sceneMinY + (abs(sceneMaxY - sceneMinY) / 2.0)));
        translateZ = - (GLfloat)((distance + (abs(sceneMinZ - sceneMaxZ) / 2.0)));
        
        //Store the values that centers the model
        centerX = translateX;
        centerY = translateY;
        centerZ = translateZ;

        // Translate and zoom acceleration depends on size of model.
        accelX = (GLfloat)(fabs(sceneMinX - sceneMaxX) * 0.4);
        accelY = (GLfloat)(fabs(sceneMinX - sceneMaxX) * 0.4);
        accelZ = (GLfloat)(fabs(sceneMinZ - sceneMaxZ) * 0.4);

        
        std::cout << "Character centered at (x:y:z): ( " << centerX
        << " : "<< centerY << " : " << centerZ << " ) " << std::endl;

		mStoredVerts.clear();
	    delete tempFitter;

	    return true;
	}


	/**
	 * @brief Get the with of the model
	 *
	 * @see centerCharacter()
	 * @return the width of the model
	 */
	GLfloat VirtualCharacter::getModelWidth()
	{
		return modelWidth;
	}


	/**
	 * @brief Get the height of the model
	 *
	 * @see centerCharacter()
	 * @return the height of the model
	 */
	GLfloat VirtualCharacter::getModelHeight()
	{
		return modelHeight;
	}




	/**
	 * @brief Update the character actions
	 *
	 * If the character is playing (an action, has an action scheduled or it is speaking), it updates
	 * the geometry of the model. Otherwise, if the character is not playing anything, it calls the play function.
	 * It also manages, internally, the situations for recently finished actions or speech.
	 *
	 * @see VirtualActionManager::unfinishedScheduledActions()
	 *  @see checkVoiceSpeech()
	 * @return std::vector<std::string> with the names of the inmediately finished actions.
	 */
	std::vector<std::string> VirtualCharacter::updateCharacter()
	{
		std::vector<std::string>finishedActions;

		if(getFAPlayer()->isPlaying())
		{
			finishedActions = actions->unfinishedScheduledActions();
			getFAPlayer()->update();
		}
		else
			getFAPlayer()->play();

//		if(actions->checkVoiceSpeech())
//            NaradaNotificationCenter::postNotification(naradaNotificationJustFinishedSpeaking);
        
		if(actions->checkLipsyncSpeech())
            NaradaNotificationCenter::postNotification(naradaNotificationJustFinishedSpeaking);
        
		//return true;
		return finishedActions;

	}



	/**
	 * @brief Set the name of the character
	 *
	 * Takes as argument the path to the character file, extracts its name and stores it in characterName data member.
	 * It also saves the path, without file name, in the pathToCharacter data member.
	 *
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::setName(std::string name)
	{
		size_t pos1, pos2;
#ifdef WIN32
		pos1 = name.find_last_of("/\\");
#else
		pos1 = name.find_last_of("/");
#endif

		pos2 = name.find_last_of(".");

		if(pos2<=pos1)
		{
			std:cerr << "incorrect path at setName function" << std::endl;
			return false;
		}

		pathToCharacter = name.substr(0,pos1);
		characterName = name.substr(pos1+1, pos2 -pos1 -1);

		return true;

	}


	/**
	 * @brief Makes the Virtual Character stop talking
	 *
	 * TODO: implement VirtualActionManager::shutUp() function
	 *
	 * @see VirtualActionManager::shutUp()
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::shutUp()
	{
        NaradaNotificationCenter::postNotification(naradaNotificationShutUp);
		return actions->shutUp();
	}



	/**
	 * @brief Returns whether a sequential lipsync has finished creating FBA action and is ready to play it or not
	 *
	 * @see VirtualActionManager::getLipsyncStatus()
	 * @return true if the FBA has been succesfully created
	 */
	bool VirtualCharacter::isReadyToPlayLipsync()
	{
		if(actions->getLipsyncStatus() == VirtualActionManager::lipsyncSequentialReadyToPlay)
			return true;
		else
			return false;
	}


	/**
	 * @brief Starts playing the face background actions
	 *
	 * @see VirtualActionManager::playFaceBackground()
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::playFaceBackground()
	{
		return actions->playFaceBackground();
	}


	/**
	 * @brief Starts playing the body background actions
	 *
	 * @see VirtualActionManager::playBodyBackground()
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::playBodyBackground()
	{
		return actions->playBodyBackground();
	}


	/**
	 * @brief Stops playing the face background actions
	 *
	 * @see VirtualActionManager::stopFaceBackground()
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::stopFaceBackground()
	{
		return actions->stopFaceBackground();
	}


	/**
	 * @brief Stops playing the body background actions
	 *
	 * @see VirtualActionManager::stopBodyBackground()
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::stopBodyBackground()
	{
		return actions->stopBodyBackground();
	}



	/**
	 * @brief Start playing an action
	 *
	 * It plays the action labeled as the first argument, with an optional delay
	 *
	 * @param actName string name of the action, as it is write in the actions file (modelName.act)
	 * @param msDelay delay in ms to start the action. Default value is zero.
	 *
	 * @see VirtualActionManager::playAction()
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::playAction(std::string actName, long msDelay)
	{
		return actions->playAction(actName, msDelay);
	}
    
    /**
	 * @brief Start playing an unloaded action
	 *
	 * It loads the action, plays it with an optional delay, and when finished, it is removed
	 *
     * @param path path to the actions FBA file
	 * @param actionFileName string name of the action FBA file
	 * @param msDelay delay in ms to start the action. Default value is zero.
	 *
	 * @see VirtualActionManager::playTemporaryAction()
	 * @return true if the operation done successfully
	 */
    bool VirtualCharacter::playTemporaryAction(std::string path, std::string actionFileName, long msDelay)
    {
        return actions->playTemporaryAction(path, actionFileName, msDelay);
    }
    
    
    
    int VirtualCharacter::getNFramesFromAction(std::string actName)
    {
        //TODO: this should return the number of frames for the action actName. Take care of != between Action and BasicAction!!!!
        return 0;
    }



	/**
	 * @brief Stops an action
	 *
	 * It stops the action labeled as the argument
	 *
	 * @param actName string name of the action, as it is write in the actions file (modelName.act)
	 *
	 * @see VirtualActionManager::stopAction()
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::stopAction(std::string actName)
	{
		return actions->stopAction(actName);
	}


	/**
	 * @brief Stops foreground actions
	 *
	 * It stops all the foreground actions being played or queued
	 *
	 * @see VirtualActionManager::stopActions()
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::stopActions()
	{
        actions->stopTemporaryActions();
		return actions->stopActions();
	}


	/**
	 * @brief Stop all actions
	 *
	 * It stops all the actions, no matter if they are foreground or background
	 *
	 * @see VirtualActionManager::stopAllActions()
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::stopAllActions()
	{
        actions->stopTemporaryActions();
		return actions->stopAllActions();
	}


	/**
	 *
	 * @brief setter for speakRealTime property
	 *  if speakRealTime is set to TRUE, then LipSync and TTS are played while computed.
	 *  Otherwise, firts, animation is computed, and then animation is reproduced (recommended for slow devices).
	 *
	 * @param realTime boolean with the value we want to assign to speakRealtime
	 *
	 * @see VirtualActionManager::setSpeakRealTime()
	 *
	 */
	void VirtualCharacter::setSpeakRealTime(bool realTime)
	{
		actions->setSpeakRealTime(realTime);
	}


	/**
	 * @brief Says through TTS the text passed as an argument
	 *
	 * It starts the TTS action, with LipSync, that makes the VirtualCharacter say some text.
	 *
	 * @param text string with the text to be spoken
	 *
	 * @see VirtualActionManager::say()
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::say(std::string text)
	{
		return actions->say(text);
	}


	/**
	 * @brief Says through Lipsync the wav passed as an argument
	 *
	 * It starts the  LipSync, that makes the VirtualCharacter repeats some audio.
	 *
	 * @param wavFile path to the audiofile
	 *
	 * @see VirtualActionManager::repeat()
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::repeat(std::string wavFile, bool temporary)
	{
		return actions->repeat(wavFile, temporary);
	}


	/**
	 * @brief Recovers whether the character have just finished speaking or not
	 *
	 * @see VirtualActionManager::justFinishedSpeaking()
	 * @return true if the virtual character has just finished speaking, false otherwise
	 */
	bool VirtualCharacter::justFinishedSpeaking()
	{
		return actions->justFinishedSpeaking();
	}


	/**
	 * @brief Increases the volume of the TTS voice
	 *
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::increaseVoiceVolume()
	{
#ifdef USE_TTS
		return actions->getVoice()->increaseVolume();
#endif
	}


	/**
	 * @brief Decreases the volume of the TTS voice
	 *
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::decreaseVoiceVolume()
	{
#ifdef USE_TTS
		return actions->getVoice()->decreaseVolume();
#endif
	}


	/**
	 * @brief Start playing a video
	 *
	 * It creates and displays an OGLVideoPlayer, a openGL square in which surface will be reproduced
	 * the video as an OpenGL Texture
	 *
	 * @param pathToVideo path to the video file to play
	 * @param centerX center in X axis where the OGLVideoPlayer should be displayed
	 * @param centerY center in Y axis where the OGLVideoPlayer should be displayed
	 *
	 * @see OGLVideoPlayer::play()
	 * @return true if the operation done successfully
	 */
	bool VirtualCharacter::playVideo(std::string pathToVideo, int centerX, int centerY)
	{
		videoPlayer = new OGLVideoPlayer(centerX, centerY, viewWidth, viewHeight);
		if(videoPlayer->play(pathToVideo))
			return true;
		else
		{
			delete videoPlayer;
			videoPlayer = NULL;
			return false;
		}
	}


	/**
	 * @brief If there's a video being played, draws the next frame of the video
	 *
	 * It loads and draws the next frame of the video, and in case it arrives to the end,
	 * the function takes care of the resource, stopping the video and deleting it, freeing the memory.
	 *
	 * @see OGLVideoPlayer::drawNextFrame()
	 * @return true if the next frame is drawn, false otherwise
	 */
	bool VirtualCharacter::drawVideo()
	{
		if(videoPlayer)
		{
			if(!videoPlayer->isFinishedPlaying())
			{
				videoPlayer->drawNextFrame();
				return true;
			}
			else
			{
				videoPlayer->stop();
				delete videoPlayer;
				videoPlayer = NULL;
				return false;
			}
		}
		return false;
	}



	/**
	 * @brief Resize the window
	 *
	 * Replaces the values of viewWidth and viewHeight, by the ones passed as arguments, and calls
	 * the resizeWindow of videoPlayer with those new arguments.
	 *
	 * @param w new width
	 * @param h new height
	 *
	 * @see OGLVideoPlayer::resizeWindow()
	 */
	void VirtualCharacter::resizeWindow(int w, int h)
	{
		viewWidth = w;
		viewHeight = h;
        
		adjustPerspectiveLimits();

		if(videoPlayer)
		{
			videoPlayer->resizeWindow(w, h);
		}
	}



	/**
	 * @brief Adjusts the perspective limits for X and Y axis
	 *
	 * Modify the perspVisibleLimitX and perspVisibleLimitY values acordingly with the Z position of the
	 * center of the model, the FOVY, and the aspect ratio of the clipping plane.
	 *
	 *
	 * @see VirtualCharacter::setTranslationZ()
	 * @see VirtualCharacter::addTranslationZ()
	 */
	bool VirtualCharacter::adjustPerspectiveLimits()
	{
		if(perspective)
		{
	    	perspVisibleLimitX =  -translateZ * perspTanFOVY;
	    	perspVisibleLimitY = perspVisibleLimitX * ( (float)viewHeight / (float)viewWidth );
        
            //this way have a normalize move unit in the model equivalent with 1 pixel in the screen
        this->basicMovementUnit = (2*perspVisibleLimitX)/viewWidth;
        std::cout << "Adjusting perspective bounds in X to: " << -perspVisibleLimitX << " : " << perspVisibleLimitX << std::endl;
        std::cout << "Adjusting perspective bounds in Y to: " << -perspVisibleLimitY << " : " << perspVisibleLimitY << std::endl;        
        
		}
		else
		{
			float aspectRatio = (float)viewWidth/(float)viewHeight;
	       	float height = ((float)this->getModelHeight())/2;
	    	perspVisibleLimitX = height/2*aspectRatio;
	    	perspVisibleLimitY = height/2;//en realidad deberian ser dos valores Ymin e Ymax, que iria de 0 a height
            this->basicMovementUnit = (2*perspVisibleLimitX)/viewWidth;
            std::cout << "Adjusting perspective bounds in X to: " << -perspVisibleLimitX << " : " << perspVisibleLimitX << std::endl;
            std::cout << "Adjusting perspective bounds in Y to: " << -perspVisibleLimitY << " : " << perspVisibleLimitY << std::endl;        

		}

		return true;
	}
    
    
    
	/**
	 * @brief Adjusts the translateZ value so the model fits a specific percentage of the screen width. 
	 *
     * Adjusts the translateZ value so the model fits a specific percentage of the screen width. Only if using perspective
	 *
	 *
     * @param percentage is a float value bigger than 0. the value 1.0 represents a width of 100% of the screen.
	 *
	 * @see VirtualCharacter::setTranslationZ()
	 */
    void VirtualCharacter::zoomToFitWidthPercentage(float percentage)
    {
        if(perspective && percentage>0)
        {
            float needZtranslate = ((float)modelWidth/2) / (percentage * perspTanFOVY);
            setTranslationZ(-needZtranslate);
        }
    }

    

	/**
	 *
	 * @brief normalize an angle specified in degrees, to belong to the range 0-360 degrees.
	 *
	 * @param angle angle in degrees to be normalized
	 *
	 */
	static void normalizeAngle(int &angle)
	{
	    while (angle < 0)
	        angle += 360;
	    while (angle > 360)
	        angle -= 360;
	}
    
    
    /**
     * @brief Debug translate in X axis
     *
     */
    void VirtualCharacter::txDbX()
    {
        std::cout << "OGLWorld: Translate in X = " << translateX << std::endl;
    }

    /**
     * @brief Debug translate in Y axis
     *
     */
    void VirtualCharacter::txDbY()
    {
        std::cout << "OGLWorld: Translate in Y = " << translateY << std::endl;
    }

    /**
     * @brief Debug translate in Y axis
     *
     */
    void VirtualCharacter::txDbZ()
    {
        std::cout << "OGLWorld: Translate in Z = " << translateZ << std::endl;
    }

	/**
	 * @brief Sets translation value for X axis
	 *
	 * @param transX new X value
	 */
	void VirtualCharacter::setTranslationX(float transX){translateX = transX;}


	/**
	 * @brief Sets translation value for Y axis
	 *
	 * @param transY new Y value
	 */
	void VirtualCharacter::setTranslationY(float transY){translateY = transY;}



	/**
	 * @brief Sets translation value for Z axis, and adjust the perspective limits
	 *
	 * @param transZ new Z value
	 */
	void VirtualCharacter::setTranslationZ(float transZ){translateZ = transZ; adjustPerspectiveLimits(); }

    
    /**
	 * @brief Sets translation value for X axis normalized so each unit is equivalent to 1 pixel in the screen
     * the translation is supossed to be based in the screen system coordinates, taking as origin the upper-left corner
	 *
	 * @param transX value to add to translateX
	 */
	void VirtualCharacter::setNormalizedTranslationX(float transX)
    {
        translateX = centerX + (transX*this->basicMovementUnit) - perspVisibleLimitX;        
//        txDbX();
    }
    
    
	/**
	 * @brief Sets translation value for Y axis normalized so each unit is equivalent to 1 pixel in the screen
     * the translation is supossed to be based in the screen system coordinates, taking as origin the upper-left corner
	 *
	 * @param transY value to add to translateY
	 */
	void VirtualCharacter::setNormalizedTranslationY(float transY)
    {
        translateY = centerY + perspVisibleLimitY - (transY*this->basicMovementUnit) ; 
//        txDbY();
    }
    
    
	/**
	 * @brief Sets translation value for Z axis normalized so each unit is equivalent to 1 pixel in the screen, 
     * and adjust the perspective limits
     * the translation is supossed to be based in the screen system coordinates, taking as origin the upper-left corner
	 *
	 * @param transZ value to add to translateZ
	 */
	void VirtualCharacter::setNormalizedTranslationZ(float transZ){
        translateZ = transZ*this->basicMovementUnit; 
        adjustPerspectiveLimits();
//        txDbZ();
    }


	/**
	 * @brief Add translation value for X axis
	 *
	 * @param transX value to add to translateX
	 */
	void VirtualCharacter::addTranslationX(float transX){translateX += transX;}


	/**
	 * @brief Add translation value for Y axis
	 *
	 * @param transY value to add to translateY
	 */
	void VirtualCharacter::addTranslationY(float transY){translateY += transY;}


	/**
	 * @brief Add translation value for Z axis, and adjust the perspective limits
	 *
	 * @param transZ value to add to translateZ
	 */
	void VirtualCharacter::addTranslationZ(float transZ){translateZ += transZ; adjustPerspectiveLimits();}

    
    
    
    /**
	 * @brief Add translation value for X axis normalized so each unit is equivalent to 1 pixel in the screen
	 *
	 * @param transX value to add to translateX
	 */
	void VirtualCharacter::addNormalizedTranslationX(float transX){translateX += transX*this->basicMovementUnit;}
    
    
	/**
	 * @brief Add translation value for Y axis normalized so each unit is equivalent to 1 pixel in the screen
	 *
	 * @param transY value to add to translateY
	 */
	void VirtualCharacter::addNormalizedTranslationY(float transY){translateY -= transY*this->basicMovementUnit;}

    
	/**
	 * @brief Add translation value for Z axis normalized so each unit is equivalent to 1 pixel in the screen, 
     * and adjust the perspective limits
	 *
	 * @param transZ value to add to translateZ
	 */
	void VirtualCharacter::addNormalizedTranslationZ(float transZ){translateZ += transZ*this->basicMovementUnit; adjustPerspectiveLimits();}
    
    


	/**
	 *
	 * @brief sets the degrees of rotation on the X axis. Internally, it calls the normalizeAngle function.
	 *
	 * @param angle degrees to be rotated
	 * @see normalizeAngle()
	 *
	 */
	void VirtualCharacter::setXRotation(int angle)
	{
	    normalizeAngle(angle);
	    if (angle != xRot) {
	        xRot = angle;
	    }
	}



	/**
	 *
	 * @brief sets the degrees of rotation on the Y axis. Internally, it calls the normalizeAngle function.
	 *
	 * @param angle degrees to be rotated
	 * @see normalizeAngle()
	 *
	 */
	void VirtualCharacter::setYRotation(int angle)
	{
	    normalizeAngle(angle);
	    if (angle != yRot) {
	        yRot = angle;
	    }
	}



	/**
	 *
	 * @brief sets the degrees of rotation on the Z axis. Internally, it calls the normalizeAngle function.
	 *
	 * @param angle degrees to be rotated
	 * @see normalizeAngle()
	 *
	 */
	void VirtualCharacter::setZRotation(int angle)
	{
	    normalizeAngle(angle);
	    if (angle != zRot) {
	        zRot = angle;
	    }
	}



	/**
	 *
	 * @brief add some degrees of rotation on the X axis. Internally, it calls the setXRotation function.
	 *
	 * @param angle degrees to be rotated
	 * @see setXRotation()
	 *
	 */
	void VirtualCharacter::addXRotation(int angle)
	{
		int aux = angle + xRot;
	    setXRotation(aux);
	}


	/**
	 *
	 * @brief add some degrees of rotation on the Y axis. Internally, it calls the setYRotation function.
	 *
	 * @param angle degrees to be rotated
	 * @see setYRotation()
	 *
	 */
	void VirtualCharacter::addYRotation(int angle)
	{
		int aux = angle + yRot;
	    setYRotation(aux);
	}


	/**
	 *
	 * @brief add some degrees of rotation on the Z axis. Internally, it calls the setZRotation function.
	 *
	 * @param angle degrees to be rotated
	 * @see setZRotation()
	 *
	 */
	void VirtualCharacter::addZRotation(int angle)
	{
		int aux = angle + zRot;
	    setZRotation(aux);
	}




	/**
	 *
	 * @brief X Rotation getter.
	 *
	 * @return x rotation in degrees
	 */
	int VirtualCharacter::getXRotation(){return xRot;};


	/**
	 *
	 * @brief Y Rotation getter.
	 *
	 * @return y rotation in degrees
	 */
	int VirtualCharacter::getYRotation(){return yRot;};


	/**
	 *
	 * @brief Z Rotation getter.
	 *
	 * @return z rotation in degrees
	 */
	int VirtualCharacter::getZRotation(){return zRot;};
    
    /**
	 *
	 * @brief X Translation getter.
	 *
	 * @return x translation in OGLWorld units
	 */
	float VirtualCharacter::getXTranslation(){return translateX;};
    
    /**
	 *
	 * @brief Y Translation getter.
	 *
	 * @return y translation in OGLWorld units
	 */
	float VirtualCharacter::getYTranslation(){return translateY;};
    
    /**
	 *
	 * @brief Z Translation getter.
	 *
	 * @return z translation in OGLWorld units
	 */
	float VirtualCharacter::getZTranslation(){return translateZ;};
    
    /**
	 *
	 * @brief X Translation getter normalized.
	 *
	 * @return x translation normalized to Screen pixel units
	 */
	float VirtualCharacter::getXNormalizedTranslation()
    {
        float nTranslate = (translateX + perspVisibleLimitX - centerX) / this->basicMovementUnit;
        return nTranslate;
    };
    
    /**
	 *
	 * @brief Y Translation getter normalized.
	 *
	 * @return y translation normalized to Screen pixel units
	 */
	float VirtualCharacter::getYNormalizedTranslation()
    {
        float nTranslate = (-translateY + perspVisibleLimitY + centerY) / this->basicMovementUnit;
        return nTranslate;
    };
    
    /**
	 *
	 * @brief Z Translation getter normalized.
	 *
	 * @return z translation normalized to Screen pixel units
	 */
	float VirtualCharacter::getZNormalizedTranslation()
    {
        float nTranslate = translateZ / this->basicMovementUnit;
        return nTranslate;
    };
    
    
    /**
     *
     * @brief increase the number of checkpoints that will be used to monitorize the model loading progress
     *
     * @param nCheckpoints the number of checkpoints to increase
     *
     */
    void VirtualCharacter::addModelLoadProgressCheckpoints(int nCheckpoints)
    {
        this->modelLoadProgressCheckpoints = this->modelLoadProgressCheckpoints + nCheckpoints;
    }
    
    
    /**
     *
     * @brief increase the loading progress in 1 unit, and throws a notification with the current loading precentage
     * posts a narada notification like this: postNotification(naradaNotificationLoadingProgressPercentage, loadingPercentage)
     *
     *
     */
    void VirtualCharacter::handlerForLoadProgress()
    {
        this->modelLoadProgressAchieved++;

        int loadingPercentage= (this->modelLoadProgressAchieved*100)/this->modelLoadProgressCheckpoints;
        
        NaradaNotificationCenter::postNotification(naradaNotificationLoadingProgressPercentage, loadingPercentage);
        
    }
    

}
