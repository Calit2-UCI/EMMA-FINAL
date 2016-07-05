///*
// *  ActionManager.h
// *  AviCuleNarada
// *
// *  Created by Enrique Oriol on 03/02/11.
// *  Copyright 2011 Narada Robotics. All rights reserved.
// *
// */
//
//
//#ifndef ACTION_MANAGER_HPP
//#define ACTION_MANAGER_HPP
//
//
//
//enum ANIMATION {
//	PLAYRECORDEDMOVIE,
//	PLAYRECORDEDAUDIO,	
//    PLAYRECORDEDAUDIO_HIGHPITCH,			
//	LISTENING,
//	ANNOYED,
//	LAUGH,
//	YAWN,
//	NUM_ANIMATIONS
//};
//
//
//#include "Manager.h"
//
//
//
//
//class Manager;
//
//using namespace VisageSDK;
//
//
//
//class ActionManager
//{
//	
//public:
//	
//	
//	ActionManager(ANIMATION kind, Manager* mng);
//	
//	~ActionManager();
//	
//	
//	FbaAction* getAction();
//	
//	ANIMATION getKind(){return _kind;}
//	
//	
//	void play();
//	
//	void stop();
//	
//	void remove();
//	
//	bool isPlaying();
//	
//	bool checkEndPlaying();
//
//	
//	
//	
//private:
//	
//	void duplicateRecordedAnimFile();
//    
//    //rarreaza 2012-01-24 added to make more flexible the file copying to use in high pitch implementation
//    void duplicateFile(char* origPath, char*destPath);
//
//	
//	void loadAction();
//	
//	char* loadPathAndPrepare();
//	
//	void start();
//	
//	Manager* _manager;
//	FbaAction* _action;
//	ANIMATION _kind;
//	bool _isLoaded;
//	bool _isPlaying;
//	bool _repeat;
//	
//};
//
//
//
//#endif