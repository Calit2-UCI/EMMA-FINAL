/*
 * OGLVirtualCharacterViewer.h
 *
 *  Created on: 16/01/2012
 *      Author: narada
 */

#ifndef OGLVIRTUALCHARACTERVIEWER_H_
#define OGLVIRTUALCHARACTERVIEWER_H_


#ifdef WIN32
	#include <GL/glew.h>
	#define GL_GLEXT_PROTOTYPES
	#include <gl/glut.h>    // Header File For The GLUT Library
	#include <gl/freeglut_ext.h>
	#include <gl\glaux.h>
	#include <gl/glext.h>
	#ifdef GLEW_VERSION_1_5_IS_NOT_SUPPORTED //address the standard extensions to glew ARB extensions
		#define glGenBuffers GLEW_GET_FUN(__glewGenBuffersARB)
		#define glBindBuffer GLEW_GET_FUN(__glewBindBufferARB)
		#define glBufferData GLEW_GET_FUN(__glewBufferDataARB)
		#define glBufferSubData GLEW_GET_FUN(__glewBufferSubDataARB)
		#define glDeleteBuffers GLEW_GET_FUN(__glewDeleteBuffersARB)
		#define glGetBufferParameteriv GLEW_GET_FUN(__glewGetBufferParameterivARB)
		#define glGetBufferPointerv GLEW_GET_FUN(__glewGetBufferPointervARB)
		#define glGetBufferSubData GLEW_GET_FUN(__glewGetBufferSubDataARB)
		#define glIsBuffer GLEW_GET_FUN(__glewIsBufferARB)
		#define glMapBuffer GLEW_GET_FUN(__glewMapBufferARB)
		#define glUnmapBuffer GLEW_GET_FUN(__glewUnmapBufferARB)
	#endif

#elif defined(LINUX)
	#define GL_GLEXT_PROTOTYPES
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
#elif defined(MAC)
	#include <GLUT/glut.h>    // Header File For The GLUT Library
	#include <OpenGL/glext.h>
#elif defined(IPHONE)
    #include <OpenGLES/EAGL.h>
    #include <OpenGLES/ES2/gl.h>
#endif


#include <iostream>
#include <exception>
#include <math.h>
#include <string>


#include "BaseOGLVRMLModelRenderer.h"
#include "VRMLFitToScreenTraverser.h"
#include "Image.hpp"
#include "VirtualCharacter.hpp"



namespace VisageSDK {


/**
* @class OGLVirtualCharacterViewer
* @brief Class that simplifies basic render of a VirtualCharacter
*
* @see VirtualCharacter, BaseOGLVRMLModelRenderer
*
*/

class OGLVirtualCharacterViewer : public BaseOGLVRMLModelRenderer {
public:
	OGLVirtualCharacterViewer(int width, int height, std::string background="", bool persp=false);
	virtual ~OGLVirtualCharacterViewer();

    void setSize(int w, int h);
    virtual bool startRenderLoop(){ return true; };
    virtual void contextSpecificConfig(){};

    VirtualCharacter* loadCharacter(string pPath);
    std::vector<std::string> updateScene();
    
    void resizeGL(int width, int height);
    
    float getRealFrameRate();


protected:
    
    static const float frMinAveragingTime = 1.0;
    
	int xRot;
	int yRot;
	int zRot;
	float translateX;
	float translateY;
	float translateZ;
	float accelX;
	float accelY;
	float accelZ;

	int myWidth;
	int myHeight;
	double zNear;
	double zFar;
	double perspectTanFOVY;
	double perspectFOVY;
	GLfloat backgroundTextureArray[20];
	GLint bgtexture[1];
	bool mBackfaceCull;
	bool readyToRender;
	std::string _bg_img;
	GLuint backgroundBuff;
	bool perspective;
	bool drawBackground;
    
    unsigned long long timeCounterInitFR;
    unsigned long long timeCounterEndFR;
    int frameCounterFR;
    float realFrameRate;


	VirtualCharacter *character;


    bool setPerspective(GLfloat width, GLfloat height);
    void loadModelViewMatrix();
    void createTextureArray();
    bool drawBackgroundImage();
    GLuint loadTexture(std::string filename);

    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    void addXRotation(int angle);
	void addYRotation(int angle);
	void addZRotation(int angle);


    void initializeGL();
    void paintGL();

    void updateRealFrameRate();

};

}

#endif /* OGLVIRTUALCHARACTERVIEWER_H_ */
