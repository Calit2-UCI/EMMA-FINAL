//!
//!  \file OGL_viewer.hpp
//!
//! \author Enrique Oriol <eoriol@naradarobotics.com>
//!
//! \date 03-03-2010
//!



#ifndef OGL_VIEWER_HPP
#define OGL_VIEWER_HPP

#ifdef WIN32
	#include <gl/glut.h>    // Header File For The GLUT Library
	#include <gl/freeglut_ext.h>
	#include <gl/glaux.h>
#elif defined(LINUX)
	#include <GL/glut.h>    // Header File For The GLUT Library
	#include <GL/freeglut_ext.h>
#elif defined(MAC)
	#include <GLUT/glut.h>    // Header File For The GLUT Library
	#include <OpenGL/glext.h>
#elif defined(IPHONE)
	#include <OpenGLES/EAGL.h>
	#include <OpenGLES/ES2/gl.h>
    #import <UIKit/UIImage.h>
#endif
#include <iostream>
#include <exception>

#include "OGLVirtualCharacterViewer.h"
#include "VRMLFitToScreenTraverser.h"
#include "FAPlayer.h"
#include "AFM.h"


#include <math.h>
#include <string>

#define ELAPSED_MS 30
#define bgWidthX 0.5f
#define bgWidthY 0.5f

using std::string;
using namespace VisageSDK;


class OGLViewer : public OGLVirtualCharacterViewer
{
	
	public:
		
		~OGLViewer(){};
    
    void contextSpecificConfig(){};
    
    bool startRenderLoop(){ return YES; };
	
    OGLViewer(unsigned int width, unsigned int height, string background, bool persp=true):OGLVirtualCharacterViewer(width, height, background, persp)
		{
			
			_bgOffsetX = 0;
			_bgOffsetY = 0;
			
		};
	
		
		float getRotX(){return this->yRot;};
	
		float getRotY(){return this->xRot;};

		
		
		bool clearScene();
	
		bool setPerspective(GLfloat width, GLfloat height);
	
	
		

		virtual void RenderToScreen();
	
//		void Close();
		
//		void translateBackground(float incX, float incY);
	
		GLuint loadTexture(UIImage* image);
	
//		bool changeBackgroundImage(UIImage* image);
    
        bool changeBackgroundImage(NSString* fileString);
	
		bool setDefaultBackground();
	
//		float getAbsoluteYaw() {return _mYRotAngle;}
//		float getAbsolutePitch() {return _mXRotAngle;}
	
//		float getAbsoluteX() {return _mXTranslate;}
//		float getAbsoluteY() {return _mYTranslate;}
	
//		float getTranslateFitX(){return _mXTranslateFit;}
//		float getTranslateFitY(){return _mYTranslateFit;}
						  


	private:
		bool _rotating;
		float _bgOffsetX;
		float _bgOffsetY;
//		bool _mBackfaceCull;
		

		void setRenderingMode();

//		void fitToScreen();

//		void fitToScreen(float zoom);
		
		
		
};
			




#endif


