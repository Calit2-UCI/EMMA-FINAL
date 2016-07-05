/*
 * GLVideoTexture.cpp
 *
 *  Created on: 16/01/2012
 *      Author: narada
 */


#include "GLVideoTexture.h"



/**
 *
 * @brief stores the given data, understood as a RGB raw image, in a file, numbered with iFrame index,
 *  to allow the storage of several images from a video, for example, without replacing each image with the next one.
 *
 * @param data uchar array of RGB raw data image
 * @param width width of the image in pixels
 * @param height height of the image in pixels
 * @param lineWidth width of each line of the image in bytes, for a correct alignment
 * @param iFrame index of the image
 *
 */
static void SaveFrame(unsigned char* data, int width, int height, int lineWidth, int iFrame)
{
    FILE *pFile;
    char szFilename[32];
    int  y;

    // Open file
    sprintf(szFilename, "frame%d.ppm", iFrame);
    pFile=fopen(szFilename, "wb");
    if(pFile==NULL)
        return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for(y=0; y<height; y++)
        fwrite((data + y*lineWidth), 1, width*3, pFile);

    // Close file
    fclose(pFile);
}



/**
 *
 * @brief Constructor: Creates the GLVideoTexture object. Initialize data members and allocate memory
 *  for nextFrame and data images.
 *
 * @param w width of the video
 * @param h height of the video
 * @param cenX displacement in X axis from the viewport's origin to the center of the texture
 * @param cenY displacement in Y axis from the viewport's origin to the center of the texture
 * @param vWidth width of the viewport
 * @param vHeight height of the viewport
 */
GLVideoTexture::GLVideoTexture(int w, int h, float cenX, float cenY, int vWidth, int vHeight)
{
	update = false;
	init = GL_VIDEOTEX_NOT_INIT;
	counter =0;
	lastFrameLoad = false;
    data = 0;
    nextFrame = 0;
    width = w;
    height = h;
    frameWidth = w;
    frameHeight = h;
    frameXOffset = 0;
    frameYOffset = 0;
    lineWidth = 3*width;
    displayHeight = 0.23;
    aspectRatio = w/h;
    centerX = cenX;
    centerY = cenY;
    viewWidth = vWidth;
    viewHeight = vHeight;
    nextFrame = (unsigned char*) calloc(3*width*height, sizeof(unsigned char));
    data = (unsigned char*) calloc(3*frameWidth*frameHeight, sizeof(unsigned char));
    pthread_mutex_init(&framesMutex, NULL);


}



/**
 *
 * @brief Constructor: Creates the GLVideoTexture object. Initialize data members with default values
 * and allocate memory for nextFrame and data images.
 *
 */
GLVideoTexture::GLVideoTexture()
{
	update = false;
	init = GL_VIDEOTEX_NOT_INIT;
	lastFrameLoad = false;
    data = 0;
    nextFrame = 0;
    width = 1024;
    height = 512;
    frameWidth = width;
    frameHeight = height;
    frameXOffset = 0;
    frameYOffset = 0;
    displayHeight = 0.5;
    aspectRatio = width/height;
    nextFrame = (unsigned char*) calloc(3*width*height, sizeof(unsigned char));
    data = (unsigned char*) calloc(frameWidth*frameHeight*3, sizeof(unsigned char));

//    createVideoDisplay();
//    initTexture();
}



/**
 *
 * @brief Destuctor: Detroy the object. Deletes openGL buffer and texture, the framesMutex,
 *  and in case that data or nextFrame are asigned, it also frees them.
 *
 */
GLVideoTexture::~GLVideoTexture()
{

	glDeleteBuffers(1, &videoDisplayBuff);

	glDeleteTextures(1, (GLuint*)videoTexture);
    
    if(data)
        free(data);
    data = NULL;
    if(nextFrame)
    	free(nextFrame);
    nextFrame = NULL;
    pthread_mutex_destroy(&framesMutex);
}



/**
 * @brief Return the number of frames already decoded and waiting to be displayed in the buffer.
 *
 * @return the number of buffered frames
 *
 */
int GLVideoTexture::getBufferedFramesNum()
{
	pthread_mutex_lock(&framesMutex);
	int ret = frames.size();
	pthread_mutex_unlock(&framesMutex);

	return ret;
}



/**
 *
 * @brief Adjust the display frame size with the values passed as arguments. In case that width or height
 *  are smaller that the dimensions of the video data, it adds the need offset (frameXOffset or frameYOffset)
 *  so the image can be cropped when stored in the frame. In case those dimensions are bigger than the ones
 *  from the video, a WARNING message is write in the stdout.
 *  It also adjusts the aspect ratio data member
 *
 * @param w new width of the frame
 * @param h new height of the frame
 *
 */
void GLVideoTexture::setFrameSize(int w, int h)
{
    frameWidth = w;
    frameHeight = h;


    aspectRatio = (float)frameWidth/frameHeight;

    if(frameWidth < width)
        frameXOffset = (width-frameWidth)/2;

    else if (frameWidth > width)
        std::cout << "WARNING!! frame width is bigger than data width";

    if(frameHeight < height)
        frameYOffset = (height-frameHeight)/2;

    else if (frameHeight > height)
        std::cout << "WARNING!! frame height is bigger than data width";

    std::cout << "frame x offset " << frameXOffset << std::endl;
    std::cout << "frame y offset " << frameYOffset << std::endl;
}



/**
 *
 * @brief calls the setFrameSize function, and sets the update flag to true and the init data member
 *  to GL_VIDEOTEX_INIT
 *
 * @param w new width of the frame
 * @param h new height of the frame
 *
 * @see setFrameSize()
 *
 */
void GLVideoTexture::setSize(int w, int h)
{
	setFrameSize(w, h);

	update = true;
	init = GL_VIDEOTEX_INIT;
	std::cout << "resize set to true" << std::endl;
}



/**
 * @brief Width getter. Returns the width of the video image
 *
 * @return width of the video image.
 *
 */
int GLVideoTexture::getWidth()
{
	return width;
}



/**
 * @brief height getter. Returns the height of the video image
 *
 * @return height of the video image.
 *
 */
int GLVideoTexture::getHeight()
{
	return height;
}



/**
 *
 * @brief replaces the line width of the image, in bytes, by the argument.
 *
 * @param lw new line width of the image, in bytes
 *
 */
void GLVideoTexture::setLineWidth(int lw)
{
	lineWidth = lw;
	std::cout << "line Width is " << lw << std::endl;
}



/**
 *
 * @brief Line width getter. Returns the line width of the image, in bytes.
 *
 */
int GLVideoTexture::getLineWidth()
{
	return lineWidth;
}



/**
 *
 * @brief Creates a new uchar RGB raw data image, copies the data from the argument into this image, and
 *  then stores it in the frames vector using mutual exclusion.
 *
 * @param buffer uchar pointer to the RGB raw data of the video image
 *
 * @return true
 *
 */
bool GLVideoTexture::setNextFrame(unsigned char* buffer)
{
	unsigned char* image = (unsigned char *)malloc(3*frameWidth*frameHeight);
	memcpy(image, buffer, frameWidth*frameHeight*3);

	pthread_mutex_lock(&framesMutex);
	frames.push_back(image);
	pthread_mutex_unlock(&framesMutex);

	return true;
}



/**
 *
 * @brief gets the first element from the frames vector using mutual exclusion, and returns it.
 *
 * @return a uchar pointer to an RGB raw data array containing the next frame if it's available, NULL otherwise
 *
 */
unsigned char* GLVideoTexture::getNextFrame()
{
	if(frames.size())
	{
		pthread_mutex_lock(&framesMutex);
		unsigned char* img = frames.front();
		frames.erase(frames.begin());
		pthread_mutex_unlock(&framesMutex);

		return img;
	}

	return NULL;
}



/**
 *
 * @brief delete the first element in the frames vector using mutual exclusion.
 *
 * @return true if there's some frame available to delete, false otherwise.
 *
 */
bool GLVideoTexture::dropNextFrame()
{
	if(frames.size())
	{
		std::cout << "dropping next frame" << std::endl;

		pthread_mutex_lock(&framesMutex);
		unsigned char* img = frames.front();
		free(img);
		frames.erase(frames.begin());
		pthread_mutex_unlock(&framesMutex);

		return true;
	}
	return false;
}



/**
 *
 * @brief set lastFrameLoad data member to true, so the user of the GLVideoTexture can check that the
 * last frame of the video has been loaded.
 *
 * @see OGLVideoPlayer::drawNextFrame()
 *
 */
void GLVideoTexture::setLastFrame()
{
	lastFrameLoad = true;
}



/**
 *
 * @brief return whether the last frame of the video has been processed or not.
 *
 * @return true in case that lastFrameLoad is true and the frames vector is empty. False otherwise.
 */
bool GLVideoTexture::isLastFrame()
{
	if(lastFrameLoad)
	{
		if(frames.size()==0)
			return true;
	}
	return false;
}



/**
 *
 * @brief Update the frame to be rendered as a texture by OpenGL.
 *  Updates the nextFrame uchar array with the new video image to display, and updates
 *  the videoTexture with this data, using the openGL glTexSubImage2D() function.
 *
 *  @see prepareNextFrame()
 *
 */
void GLVideoTexture::updateFrame()
{

    prepareNextFrame();

    glBindTexture(GL_TEXTURE_2D, videoTexture[0]);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, nextFrame);

}



/**
 *
 * @brief recovers the next available video image frame to be displayed, and copies it
 *  in the nextFrame data member
 *
 * @todo TODO TAKE CARE with usage of frameXOffset and frameYOffset, its not clear. It should be tested
 *   because looks like when there's frameXOffset, it swaps the image, so it looks like upside down
 */
void GLVideoTexture::prepareNextFrame()
{

	data = getNextFrame();
	if(!data)
	{
		memset(nextFrame, 0, width*height*3);
	}
	else
	{


		if((frameXOffset == 0 ) && (frameYOffset == 0))
		{
			memcpy(nextFrame, data, frameWidth*frameHeight*3);
		}
		else if(frameXOffset==0 && frameYOffset !=0)
		{
			memcpy(nextFrame, data, frameWidth*frameHeight*3);
		}
		else
		{
			int reverseOffset = 3*frameWidth*(frameHeight-1);
			for(int i=0; i<frameHeight; i++)
			{
				memcpy(nextFrame + 3*width*i, data + reverseOffset - 3*frameWidth*i, frameWidth*3);
			}
		}

//		if(counter%30==0)
//		{
//			SaveFrame(data, frameWidth, frameHeight, 3*frameWidth, counter + 100000000);
//			SaveFrame(nextFrame, width, height, 3*width, counter);
//		}

		if(data)
		{
			free(data);
			data = NULL;
		}
	}

}




/**
 *
 * @brief initializes the OpenGL texture.
 * OpenGL calls to prepare the texture, texture binding, linear filtering, light, etc
 *
 */
void GLVideoTexture::initTexture()
{

    GLuint tex_ID;

    glGenTextures(1, &tex_ID);					// Create The Texture

    // Typical Texture Generation Using Data From The Bitmap
    //by KaiK
    glActiveTexture( GL_TEXTURE0 );
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_ID);


#ifndef IPHONE
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nextFrame);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nextFrame);
#endif

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glBindTexture(GL_TEXTURE_2D, 0);


    videoTexture[0] = tex_ID;

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT5);
	float color5f[4] = {1.0, 1.0, 1.0, 1.0};
	//glLightfv(GL_LIGHT5, GL_AMBIENT, color5f );
	glLightfv(GL_LIGHT5, GL_DIFFUSE, color5f );
    glDisable(GL_LIGHT5);

}



/**
 *
 * @brief Draws the next frame as a texture.
 *  OpenGL commands to draw the next video frame as a texture. If the video texture has not been initialized,
 *  it returns false, and if the video display has not been created, creates it and initializes the texture.
 *  Before drawing, calls the updateFrame() function, so the frame is updated.
 *
 * @return false if not video texture not initialized, true otherwise.
 * @see createVideoDisplay(), initTexture(), updateFrame()
 *
 */
bool GLVideoTexture::drawVideoFrame()
{
	switch(init)
	{
		case GL_VIDEOTEX_NOT_INIT:
			std::cout << "GL video texture not initialized yet" << std::endl;
			return false;

		case GL_VIDEOTEX_INIT:
			createVideoDisplay();
			initTexture();
			init = GL_VIDEOTEX_READY;

		case GL_VIDEOTEX_READY:
			updateFrame();
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();

#ifdef IPHONE
            glOrthof(0, viewWidth, 0, viewHeight, -40.0f, 40.0f);
#else
			glOrtho(0, viewWidth, 0, viewHeight, -40.0f, 40.0f);
#endif
			glEnable(GL_LIGHT5);


			glEnable(GL_TEXTURE_2D);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, videoTexture[0]);

			glBindBuffer(GL_ARRAY_BUFFER, videoDisplayBuff);

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 20, 0);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 20, (void *)12);

			glEnable(GL_LIGHT5);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glDisable(GL_LIGHT5);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE);
			glClear(GL_DEPTH_BUFFER_BIT);

			glDisable(GL_LIGHT5);
			return true;

	}

    return true;

}



/**
 *
 * @brief creates the vertex of the video display adjusting them to the object width and height,
 * generates a vertex buffer and associates them the video display vertex.
 *
 */
void GLVideoTexture::createVideoDisplay()
{
	resizeVideoDisplay();

    videoDisplayBuff = 0;

    glGenBuffers(1, &videoDisplayBuff);
    glBindBuffer(GL_ARRAY_BUFFER, videoDisplayBuff);

    unsigned int size = 20 * sizeof(GLfloat);
    glBufferData(GL_ARRAY_BUFFER, size, videoDisplayTextureArray, GL_DYNAMIC_DRAW);
}



/**
 *
 * @brief updates the information when the view window is resized, so the center of the
 *  video display is updated.
 *
 * @param w new width of the view
 * @param h new height of the view
 *
 */
void GLVideoTexture::resizeWindow(int w, int h)
{
	centerX = centerX + (w-viewWidth)/2;
	centerY = centerY + (h-viewHeight)/2;
	viewWidth = w;
	viewHeight = h;
}



/**
 *
 * @brief resize the video display moving their vertices, to fit the requirements
 *  imposed by frame, image and view.
 *
 */
void GLVideoTexture::resizeVideoDisplay()
{
	float x;
	float y;

	if(viewWidth>viewHeight)
		y = displayHeight/2*viewHeight;
	else
		y = displayHeight/2*viewWidth;

	x = aspectRatio*y;

	float texY = (float)frameHeight/(float)height;
	float texX = (float)frameWidth/(float)width;

    videoDisplayTextureArray[0] = centerX-x;	//coords v0
    videoDisplayTextureArray[1] = centerY+y;	//coords v0
    videoDisplayTextureArray[2] = 35.0f;	//coords v0
    videoDisplayTextureArray[3] = 0.0f;	//texture coord v0
    videoDisplayTextureArray[4] = texY;	//texture coord v0


    videoDisplayTextureArray[5] = centerX-x;	//coords v1
    videoDisplayTextureArray[6] = centerY-y;	//coords v1
    videoDisplayTextureArray[7] = 35.0f;	//coords v1
    videoDisplayTextureArray[8] = 0.0f;	//texture coord v1
    videoDisplayTextureArray[9] = 0.0f;	//texture coord v1


    videoDisplayTextureArray[10] = centerX+x;	//coords v2
    videoDisplayTextureArray[11] = centerY+y;	//coords v2
    videoDisplayTextureArray[12] = 35.0f;	//coords v2
    videoDisplayTextureArray[13] = texX;	//texture coord v2
    videoDisplayTextureArray[14] = texY;	//texture coord v2


    videoDisplayTextureArray[15] = centerX+x;	//coords v3
    videoDisplayTextureArray[16] = centerY-y;	//coords v3
    videoDisplayTextureArray[17] = 35.0f;	//coords v3
    videoDisplayTextureArray[18] = texX;	//texture coord v3
    videoDisplayTextureArray[19] = 0.0f;	//texture coord v3


    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(videoDisplayTextureArray), videoDisplayTextureArray);

}

