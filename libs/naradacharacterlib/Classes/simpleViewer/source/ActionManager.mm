///*
// *  ActionManager.cpp
// *  AviCuleNarada
// *
// *  Created by Enrique Oriol on 03/02/11.
// *  Copyright 2011 Narada Robotics. All rights reserved.
// *
// */
//
//#include "ActionManager.h"
//
//
//
//ActionManager::ActionManager(ANIMATION kind, Manager* mng)
//{
//
//	_manager = mng;
//	_kind = kind;
//	_isPlaying = false;
//	_repeat = false;
//	
//	loadAction();
//}
//
//
//
//ActionManager::~ActionManager()
//{
//	delete (FbaFileAction*)_action;
//	
//	_action = NULL;
//	
//}
//
//
//
//
//bool ActionManager::isPlaying()
//{
//	return _isPlaying;
//}
//
//
//
//
//
//void ActionManager::duplicateRecordedAnimFile()
//{
//	
//	const char * origPath = _manager->getWritablePathToReadFrom(string([_DEFAULT_NORMALPITCH_ACTIONS_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]));
//	char * destPath = _manager->getWritablePathTo( string([_MUTE_NORMALPITCH_ACTIONS_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]) );
//	
//	FILE *fOrig = fopen(origPath, "r");
//	FILE *fDest = fopen(destPath, "w");
//	
//	fseek(fOrig, 0, SEEK_END);
//	long fileLength = ftell(fOrig);
//	fseek(fOrig, 0, SEEK_SET);
//	
//	unsigned char *buffer = (unsigned char*)malloc(fileLength);
//	
//	fread(buffer, 1, fileLength, fOrig);
//	fwrite(buffer, 1, fileLength, fDest);
//	
//	free(buffer);
//	
//	fclose(fOrig);
//	fclose(fDest);
//	
//}
//
//
//void ActionManager::duplicateFile(char* origPath, char*destPath)
//{
//	
//	
//	FILE *fOrig = fopen(origPath, "r");
//	FILE *fDest = fopen(destPath, "w");
//	
//	fseek(fOrig, 0, SEEK_END);
//	long fileLength = ftell(fOrig);
//	fseek(fOrig, 0, SEEK_SET);
//	
//	unsigned char *buffer = (unsigned char*)malloc(fileLength);
//	
//	fread(buffer, 1, fileLength, fOrig);
//	fwrite(buffer, 1, fileLength, fDest);
//	
//	free(buffer);
//	
//	fclose(fOrig);
//	fclose(fDest);
//	
//}
//
//
//
//void ActionManager::loadAction()
//{
//	
//	char * dataPath = loadPathAndPrepare();
//	
//	std::cout << "load path and prepare on action manager: " << dataPath << std::endl;
//	if(_repeat)
//		_action = _manager->getViewer()->getPlayer()->addTrack(dataPath, 1, 0);
//	else
//		_action = _manager->getViewer()->getPlayer()->addTrack(dataPath, 0, 0);
//	
//	_action->stop();
//	
//	
//	_manager->getViewer()->getPlayer()->removeTrack(_action);
//	
//	
//	if(_kind == PLAYRECORDEDMOVIE)
//	{
//		_manager->getViewer()->getPlayer()->setPlayMode(PLAYMODE_OFFLINE);
//		_manager->getViewer()->getPlayer()->getTimer()->setFrameRate(recordingFrameRate);
//	}
//	else
//	{
//		_manager->getViewer()->getPlayer()->getTimer()->setFrameRate(defaultFrameRate);
//		_manager->getViewer()->getPlayer()->setPlayMode(PLAYMODE_REALTIME);
//	}
//
//	
//}
//
//
//
//char* ActionManager::loadPathAndPrepare()
//{
//	char *dataPath;
//	switch (_kind) {
//			
//		case LISTENING:
//			dataPath = _manager->getPathTo("listening.fba");
//			std::cout << "load path and prepare: " << dataPath << std::endl;
//			_repeat = true;
//			break;
//			
//		case PLAYRECORDEDAUDIO:
//			dataPath = _manager->getWritablePathToReadFrom( string([_DEFAULT_NORMALPITCH_ACTIONS_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]) );
//			break;
//            
//        case PLAYRECORDEDAUDIO_HIGHPITCH:
//			dataPath = _manager->getWritablePathToReadFrom( string([_DEFAULT_HIGHPITCH_ACTIONS_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]) );
//			break;
//			
//		case PLAYRECORDEDMOVIE:
//            
//            if (_manager->getDoPitchShift()) {
//                const char * origPath = _manager->getWritablePathToReadFrom( string([_DEFAULT_HIGHPITCH_ACTIONS_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]) );
//                char * destPath = _manager->getWritablePathTo( string([_MUTE_HIGHPITCH_ACTIONS_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]) );
//                duplicateFile(origPath, destPath );
//                dataPath = _manager->getWritablePathToReadFrom( string([_MUTE_HIGHPITCH_ACTIONS_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]) );
//            }else{
//                const char * origPath = _manager->getWritablePathToReadFrom( string([_DEFAULT_NORMALPITCH_ACTIONS_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]) );
//                char * destPath = _manager->getWritablePathTo( string([_MUTE_NORMALPITCH_ACTIONS_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]) );
//                duplicateRecordedAnimFile();
//                dataPath = _manager->getWritablePathToReadFrom( string([_MUTE_NORMALPITCH_ACTIONS_FILENAME cStringUsingEncoding:NSUTF8StringEncoding]) );
//            }
//			
//			break;
//			
//		case ANNOYED:
//			dataPath = _manager->getPathTo("eee.fba");
//			break;
//			
//		case LAUGH:
//			dataPath = _manager->getPathTo("risa1.fba");
//			break;
//			
//		case YAWN:
//			dataPath = _manager->getPathTo("estornudo3.fba");
//			break;
//						
//		default:
//			break;
//	}
//	
//	//int l = strlen(dataPath);
//	//char* retData = (char*)malloc(l*sizeof(char)+1);
//	//strcpy(retData, dataPath);
//	//return retData;
//	
//	return dataPath;
//}
//
//
//void ActionManager::play()
//{
//	if(!_isPlaying)
//	{
//		if(!_action)
//			loadAction();
//		else
//			_manager->getViewer()->getPlayer()->addTrack(_action);
//		start();
//	}
//}
//
//
//void ActionManager::start()
//{
//    //addTrack ya llama a action->start, no hace falta duplicarlo
//	//_action->start(_manager->getViewer()->getPlayer()->getTimer()->getCurrentTime());
//	_isPlaying = true;
//
//}
//
//void ActionManager::stop()
//{
//	_action->stop();
//}
//
//bool ActionManager::checkEndPlaying()
//{
//	if(_isPlaying && _kind != LISTENING)
//	{
//		if(((FbaFileAction*)_action)->isFinished())
//        //if(_action->isFinished())
//		{
//			_isPlaying = false;
//			remove();
//			return true;
//		}
//	}
//	return false;
//}
//
//void ActionManager::remove()
//{
//	_action->stop();
//	_manager->getViewer()->getPlayer()->removeTrack(_action);
//}
//
//FbaAction* ActionManager::getAction()
//{
//	return _action;
//}
