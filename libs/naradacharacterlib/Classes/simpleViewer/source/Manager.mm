//!
//!  \file Manager.cpp
//!
//! \author Enrique Oriol <eoriol@naradarobotics.com>
//!
//! \date 03-03-2010
//!



#include "Manager.h"
#define MODEL_AFM_FILENAME "reanaMWC.AFM"
#define MODEL_SMALL_AFM_FILENAME "reanaMWC.AFM"
#define MODEL_FONDO_BIG "Classroom2.jpg"
#define MODEL_FONDO_SMALL "Classroom2.jpg"




//-------------------------//
//     PUBLIC FUNCTIONS    //
//-------------------------//



bool Manager::clearScene()
{
    //-KaiK- new API changes
//	if(_my_viewer->clearScene())
//		return true;
//	else
//		return false;
	
}




void Manager::playAnnoy()
{
	if(!_isRecordingAudio && !_playRecMovieON)
	{

        character->playAction("annoy");
		
	}
}

void Manager::playLaugh()
{
	if(!_isRecordingAudio && !_playRecMovieON)
	{
        character->playAction("laugh");
	}
}

void Manager::playYawn()
{
	if(!_isRecordingAudio && !_playRecMovieON)
	{
        character->playAction("yawn");
	}
}



void Manager::playRecordedAudio()
{
	_playRecAudio = true;
}

void Manager::playRecordedAudioInHighPitch()
{
	_playRecAudioInHighPitch = true;
}



void Manager::playRecordedAnim()
{	

	if(_my_lipsync)
	{
		delete _my_lipsync;
		_my_lipsync = NULL;
	}

//	setAnimation(PLAYRECORDEDMOVIE);
	_playRecMovie = true;
	
}




void Manager::playLipSync()
{
		_lipSync = true;
}






//-------------------------//
//    PRIVATE FUNCTIONS    //
//-------------------------//

//bool Manager::hasFinishedPlaying()
//{	
//	for(unsigned int i=0; i< _actions.size(); i++)
//	{
//		if(_actions[i]->getKind() != LISTENING)
//			if(_actions[i]->isPlaying())
//				return false;
//	}
//	
//	return true;
    
    //OJO esta funcion no ha sido aun implementada!!!!!
    //return !character->isPlaying();
//}







//--------------------------



//void Manager::applyStatus()
//{
//	
//		
//	if(_playRecAudio)
//	{
//		playAnimation(PLAYRECORDEDAUDIO);
//		_playRecAudio = false;
//	}
//    
//    else if(_playRecAudioInHighPitch)
//	{
//		playAnimation(PLAYRECORDEDAUDIO_HIGHPITCH);
//		_playRecAudioInHighPitch = false;
//	}
//	
//	else if(_lipSync)
//	{
//		if(_my_lipsync)
//			delete _my_lipsync;
//		_my_lipsync = new VisageLipSync(true/*sequential*/);
//
//		character->getFAPlayer()->addTrack(_my_lipsync);
//		_my_lipsync->stopSync();
//		character->getFAPlayer()->getTimer()->setFrameRate(defaultFrameRate);
//		_my_lipsync->setHandleEventMode(LS_HE_MODE_VISEME_BASED);
//		cout << "going to sync" << endl;
//		_my_lipsync->sync(getWritablePathToReadFrom(string([_DEFAULT_NORMALPITCH_AUDIO_FILENAME cStringUsingEncoding:NSUTF8StringEncoding])), getWritablePathTo( string([_DEFAULT_NORMALPITCH_ACTIONS_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]) ), NULL, true/*mute*/);
//        
//        //maybe this is all that's needed to replicate the file...
//        _my_lipsync->stopSync();
//        _my_lipsync->sync(getWritablePathToReadFrom(string([_DEFAULT_HIGHPITCH_AUDIO_FILENAME cStringUsingEncoding:NSUTF8StringEncoding])), getWritablePathTo( string([_DEFAULT_HIGHPITCH_ACTIONS_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]) ), NULL, true/*mute*/);
//        //
//		character->getFAPlayer()->removeTrack(_my_lipsync);
//		
//		delete _my_lipsync;
//		_my_lipsync = NULL;
//		_lipSync = false;
//	
//		character->getFAPlayer()->setAutoStopMode(true);
//        
////        //rarreaza 2012-01-23 duplication of the code to create 2 lipSyncs instead of just one (normal voice and hight pitch voice
////        if(_my_lipsync)
////			delete _my_lipsync;
////		_my_lipsync = new VisageLipSync(true/*sequential*/);
////        
////		_my_viewer->getPlayer()->addTrack(_my_lipsync);
////		_my_lipsync->stopSync();
////		_my_viewer->getPlayer()->getTimer()->setFrameRate(defaultFrameRate);
////		_my_lipsync->setHandleEventMode(LS_HE_MODE_VISEME_BASED);
////		cout << "going to sync" << endl;
////		_my_lipsync->sync(getWritablePathToReadFrom( string([_DEFAULT_HIGHPITCH_AUDIO_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]) ), getWritablePathTo( string([_DEFAULT_HIGHPITCH_ACTIONS_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]) ), NULL, true/*mute*/);
////        _my_viewer->getPlayer()->removeTrack(_my_lipsync);
////		
////		delete _my_lipsync;
////		_my_lipsync = NULL;
////		_lipSync = false;
////        
////		_my_viewer->getPlayer()->setAutoStopMode(true);
//        
//        //e.o.n.c
//	
//		if (getDoPitchShift()) {
//            _playRecAudioInHighPitch = true;
//        }else{
//            _playRecAudio = true;
//
//        }
//        
//		
//	}
//	
//	else if(_playRecMovie)
//	{
//		playAnimation(PLAYRECORDEDMOVIE);
//		_playRecMovieON = true;
//		_playRecMovie = false;
//		
//	}
//	else if(_annoyed)
//	{
//		playAnimation(ANNOYED);
//		_annoyed = false;
//	}
//	else if(_laugh)
//	{
//		playAnimation(LAUGH);
//		_laugh = false;
//	}
//	else if(_yawn)
//	{
//		playAnimation(YAWN);
//		_yawn = false;
//	}
//
//	
//	else if(_listening)
//	{
//		playAnimation(LISTENING);
//		_listening = false;
//	}
//	
//	if(!character->getFAPlayer()->isPlaying())
//		character->getFAPlayer()->play();
//}





//void Manager::checkActionsPlaying()
//{
//	for(unsigned int i=0; i< _actions.size(); i++)
//	{
//		if(_actions[i]->checkEndPlaying())
//		{
//			if(_actions[i]->getKind() == PLAYRECORDEDMOVIE)
//			{
//				deleteActionManager(i);
//				_playRecMovieON = false;
//				position = 0;
//				removeActionsWithAudio();
//			}
//			if (_actions[i]->getKind() == PLAYRECORDEDAUDIO) {
//				//rotateToInit();
//				position = 0;
//				cout << "Reset position" << endl;
//			}
//            if (_actions[i]->getKind() == PLAYRECORDEDAUDIO_HIGHPITCH) {
//				//rotateToInit();
//				position = 0;
//				cout << "Reset position" << endl;
//			}
//		}
//	}	
//}


//void Manager::removeActionsWithAudio()
//{
//	for(unsigned int i=0; i< _actions.size(); i++)
//	{
//		if(_actions[i]->getKind() != LISTENING)
//		{
//			deleteActionManager(i);
//			i--;
//		}
//	}	
//}
//
//
//
//void Manager::setAnimation(ANIMATION anim)
//{
//	if(!getActionManager(anim))
//	   {
//		   ActionManager* action = new ActionManager(anim, this);
//		   _actions.push_back(action);
//	   }
//}
	   

//void Manager::playAnimation(ANIMATION anim)
//{
// 	if(!getActionManager(anim))
//	   setAnimation(anim);
//	   
//	getActionManager(anim)->play();
//}
//
//
//
//void Manager::deleteActionManager(int index)
//{
//	delete _actions[index];
//	_actions.erase(_actions.begin() + index);
//}
//
//










//void Manager::moveModel(float incX, float incY)
//{
//	float absX = _my_viewer->getAbsoluteX();
//	float absY = _my_viewer->getAbsoluteY();
//	
//	float absPitch = _my_viewer->getAbsolutePitch();
//	
//	if (incX > 0 && absX > kMaxX) {
//		absX = kMaxX;
//		incX = 0;
//		cout << "Reached MAX X" << endl;
//	}
//	else if (incX < 0 && absX < kMinX) {
//		absX = kMinX;
//		incX = 0;
//		cout << "Reached min X" << endl;
//	}
//	
//	if (incY > 0 && absY > kMaxY) {
//		absY = kMaxY;
//		incY = 0;
//	}
//	else if (incY < 0 && absY < kMinY) {
//		absY = kMinY;
//		incY = 0;
//	}
////	_my_viewer->rotateModel(-incX / kMaxX * kMaxYaw, 0);
//	_my_viewer->moveModel(incX, incY);
//	
//	cout << "Translating in GLView: " << incX << ", " << incY << endl;
//}

//void Manager::moveModelAbs(float absX, float absY)
//{
//	/*
//	if (absX > kMaxX) {
//		absX = kMaxX;
//		cout << "Reached MAX X" << endl;
//	}
//	else if (absX < kMinX) {
//		absX = kMinX;
//		cout << "Reached min X" << endl;
//	}
//	
//	if (absY > kMaxY) {
//		absY = kMaxY;
//	}
//	else if (absY < kMinY) {
//		absY = kMinY;
//	}
//	*/
//	float absPitch = _my_viewer->getAbsolutePitch();
//	
//	cout << "Absolute position: (" << absX << ", " << absY << ")" << endl;
//	rotateModelAbs(-absX / kMaxX * kMaxYaw, absPitch, false);
//	_my_viewer->moveModelAbs(absX, absY);
//}
//
//
//void Manager::rotateModel(float incYaw, float incPitch)
//{
//	float yaw = _my_viewer->getAbsoluteYaw();
//	float pitch = _my_viewer->getAbsolutePitch();
//	//Check if the angles are beyond boundaries
//	if (incYaw > 0 && yaw > kMaxYaw) {
//		yaw = kMaxYaw;
//		incYaw = 0;
//	}
//	else if (incYaw < 0 && yaw < kMinYaw) {
//		yaw = kMinYaw;
//		incYaw = 0;
//	}
//	
//	if (incPitch > 0 && pitch > kMaxPitch) {
//		pitch = kMaxPitch;
//		incPitch = 0;
//	}
//	else if (incPitch < 0 && pitch < kMinPitch) {
//		pitch = kMinPitch;
//		incPitch = 0;
//	}
//	
//	//	cout << "Angle yaw: " << yaw << " deg" << endl;
//	//	cout << "Angle pitch: " << pitch << " deg" << endl;
//	
//	_my_viewer->rotateModel(incYaw, incPitch);
//}

//void Manager::rotateModelAbs(float yaw, float pitch, bool moveBackground)
//{
//	cout << "Rotating to: " << yaw << ", " << pitch << endl;
//	//Check if the angles are beyond boundaries
//	if (yaw > 0 && yaw > kMaxYaw) {
//		yaw = kMaxYaw;
//	}
//	else if (yaw < 0 && yaw < kMinYaw) {
//		yaw = kMinYaw;
//	}
//	
//	if (pitch > 0 && pitch > kMaxPitch) {
//		pitch = kMaxPitch;
//	}
//	else if (pitch < 0 && pitch < kMinPitch) {
//		pitch = kMinPitch;
//	}
//	
////	cout << "Angle yaw: " << yaw << " deg" << endl;
////	cout << "Angle pitch: " << pitch << " deg" << endl;
//	
//	_my_viewer->rotateModelAbs(yaw, pitch);
//	
//	if (!moveBackground) return;
//	
//	float transX = -yaw/100;
//	float transY = pitch/100;
//
//	_my_viewer->translateBackground(transX, transY);
//	
//}

