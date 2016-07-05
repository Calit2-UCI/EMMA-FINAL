/*
 * GLVideoTexture.h
 *
 *  Created on: 16/01/2012
 *      Author: narada
 */


#ifndef GLVIDEOTEXTURE_H
#define GLVIDEOTEXTURE_H

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

#elif defined (IPHONE)
    #include <OpenGLES/ES1/gl.h>
#endif

#include <pthread.h>
#include "RGBTextureArray.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <iostream>
#include <cstdlib>


/**
* @class GLVideoTexture
* @brief Class that allows to store images from a video-stream, read them and display them through an OpenGL texture
*
* @see RGBTextureArray, NaradaVideoDecoderFFMPEG
*
*/
class GLVideoTexture : public RGBTextureArray
{
public:
    GLVideoTexture(int w, int h, float cenX, float cenY, int vWidth, int vHeight);
    GLVideoTexture();
    ~ GLVideoTexture();
    bool drawVideoFrame();


private:
	void createVideoDisplay();
    void setFrameSize(int w, int h);
    void setSize(int w, int h);
    bool setNextFrame(unsigned char* buffer);
    void initTexture();
    void resizeVideoDisplay();
    void resizeWindow(int w, int h);

    void updateFrame();
    void prepareNextFrame();
    unsigned char* getNextFrame();
	bool dropNextFrame();
	int getWidth();
	int getHeight();
	int getFrameWidth();
	int getFrameHeight();
	void setLineWidth(int lineWidth);
	int getLineWidth();
	void setLastFrame();
	bool isLastFrame();
	int getBufferedFramesNum();

	std::vector<unsigned char*> frames;
	int counter;
	pthread_mutex_t framesMutex;

	int lineWidth;
	bool lastFrameLoad;
	bool update;
	enum GL_VIDEOTEX_STATUS {GL_VIDEOTEX_NOT_INIT, GL_VIDEOTEX_INIT, GL_VIDEOTEX_READY };
	GL_VIDEOTEX_STATUS init;



    GLfloat videoDisplayTextureArray[20];
    GLuint videoDisplayBuff;
    GLint videoTexture[1];

    int width;
    int height;
    float aspectRatio;
    float displayHeight;
    int frameWidth;
    int frameHeight;
    int frameXOffset;
    int frameYOffset;
    float centerX;
    float centerY;
    int viewWidth;
    int viewHeight;
    unsigned char* data;//pointer to our resized image
    unsigned char* nextFrame;//pointer to our resized to show image


};

#endif // GLVIDEOTEXTURE_H
