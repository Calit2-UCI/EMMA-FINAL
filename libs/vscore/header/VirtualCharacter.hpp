/*
 * VirtualCharacter.h
 *
 *  Created on: 09/01/2012
 *      Author: narada
 */

#ifndef VIRTUALCHARACTER_H_
#define VIRTUALCHARACTER_H_

#ifdef WIN32
	#include <GL/glew.h>
	#define GL_GLEXT_PROTOTYPES
	#include <gl/glut.h>    // Header File For The GLUT Library
	#include <gl/freeglut_ext.h>
	#include <gl\glaux.h>
	#include <gl/glext.h>
#elif defined(LINUX)
	#include <GL/gl.h>

#elif defined(MAC)
	#include <GLUT/glut.h>    // Header File For The GLUT Library
	#include <OpenGL/glext.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <exception>
#include "BaseOGLVRMLModelRenderer.h"
#include "VRMLFitToScreenTraverser.h"
#include "FAPlayer.h"
#include "FbaFileAction.h"
#include "AFM.h"
#include "Point3d.h"
#include "NaradaException.h"
#include "OGLVideoPlayer.h"
#include "NaradaNotificationCenter.hpp"


#include "VirtualActionManager.h"




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



#define V_CHARACTER_BASIC_MOVEMENT_FACTOR 20

namespace VisageSDK
{



class VirtualActionManager;


/**
* @class VirtualCharacter
* @brief Class that simplifies basic usage of a character like loading it, play actions and talk
* it also permits show videos through OpenGL texture
*
* @see OGLVirtualCharacterViewer
*
*/
    class VISAGE_DECLSPEC VirtualCharacter
{

	public:
    
        static const std::string naradaNotificationJustFinishedSpeaking;
        static const std::string naradaNotificationJustFinishedTemporaryAction;
        static const std::string naradaNotificationShutUp;
        static const std::string naradaNotificationLoadingProgressPercentage;

		VirtualCharacter(std::string, bool perspective=true, float perspTanFOVY=0.557851739);
		virtual ~VirtualCharacter();


		void setSpeakRealTime(bool realTime);
		bool say(std::string);
		bool repeat(std::string wavFile, bool temporary=false);
		bool shutUp();
		bool isPlaying();
		bool isReadyToPlayLipsync();

		//bool justFinishedPlaying();
		bool justFinishedSpeaking();

		bool increaseVoiceVolume();

		bool decreaseVoiceVolume();

		std::vector<std::string> updateCharacter();
		void resizeWindow(int w, int y);

		bool playVideo(std::string pathToVideo, int centerX, int centerY);
		bool drawVideo();


//		bool loadActions();
		bool playFaceBackground();
		bool playBodyBackground();
		bool stopFaceBackground();
		bool stopBodyBackground();
		bool playAction(std::string actionName, long msDelay=0);
        bool playTemporaryAction(std::string path, std::string actionFileName, long msDelay=0);
        int getNFramesFromAction(std::string actName);
		bool stopAction(std::string actName);
		bool stopActions();
		bool stopAllActions();

		void setTranslationX(float transX);
		void setTranslationY(float transY);
		void setTranslationZ(float transZ);
    
        void setNormalizedTranslationX(float transX);
        void setNormalizedTranslationY(float transY);
        void setNormalizedTranslationZ(float transZ);   

		void addTranslationX(float transX);
		void addTranslationY(float transY);
		void addTranslationZ(float transZ);
    
        void addNormalizedTranslationX(float transX);
        void addNormalizedTranslationY(float transY);
        void addNormalizedTranslationZ(float transZ);    
    
        void setXRotation(int angle);
        void setYRotation(int angle);
        void setZRotation(int angle);
        
        void addXRotation(int angle);
        void addYRotation(int angle);
        void addZRotation(int angle);
        
        int getXRotation();
        int getYRotation();
        int getZRotation();
    
        float getXTranslation();
        float getYTranslation();
        float getZTranslation(); 
    
        float getXNormalizedTranslation();
        float getYNormalizedTranslation();
        float getZNormalizedTranslation();
    

		GLfloat getModelWidth();
		GLfloat getModelHeight();
    
        float basicMovementUnit;// 1/20 of the model width, to ensure that moves over any model behaves similar.

		FAPlayer* getFAPlayer(){return m_FAPlayer;}
		VRMLModel* getVRMLModel(){return  dynamic_cast<AFM*>(m_FAPlayer->getFaceModel())->getVCModel();}
		Point3d getCenter(){return Point3d(translateX, translateY, translateZ);}
        Point3d getRotation(){return Point3d(xRot, yRot, zRot);};
		GLfloat getDistance(){return distance;}

		float getPerspectiveLimitInX(){return perspVisibleLimitX;}
		float getPerspectiveLimitInY(){return perspVisibleLimitY;}
    
        void zoomToFitWidthPercentage(float percentage);
    
    
        void addModelLoadProgressCheckpoints(int nCheckpoints);
        void handlerForLoadProgress();

	protected:

        void txDbX();
        void txDbY();
        void txDbZ();

		bool centerCharacter(float perspTanFOVY);
		bool getName(std::string name);
		bool setName(std::string name);

		bool adjustPerspectiveLimits();




		int xRot;
		int yRot;
		int zRot;
		int viewWidth;
		int viewHeight;
        bool perspective;
		float perspTanFOVY;
		float perspVisibleLimitX;
		float perspVisibleLimitY;
		GLfloat translateX;
		GLfloat translateY;
		GLfloat translateZ;
        GLfloat centerX;
        GLfloat centerY;
        GLfloat centerZ;
		GLfloat distance;
		GLfloat modelWidth;
		GLfloat modelHeight;
		float accelX;
		float accelY;
		float accelZ;
		std::string characterName;
		std::string pathToCharacter;
		VirtualActionManager *actions;
		AFM *afm;
		FAPlayer* m_FAPlayer;
		OGLVideoPlayer* videoPlayer;
    
    
        int modelLoadProgressCheckpoints;
        int modelLoadProgressAchieved;



};

}

#endif /* VIRTUALCHARACTER_H_ */
