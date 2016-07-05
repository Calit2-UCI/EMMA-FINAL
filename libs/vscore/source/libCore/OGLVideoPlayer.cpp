/*
 * OGLVideoPlayer.cpp
 *
 *  Created on: 02/02/2012
 *      Author: narada
 */

#include "OGLVideoPlayer.h"



/**
 * @brief Constructor: initializes vars and creates a new GLVideoTexture, with the specified arguments.
 *
 * @param centerX   x offset from the origin in the main view to the center of the video display
 * @param centerY   y offset from the origin in the main view to the center of the video display
 * @param viewW   width of the video display
 * @param viewH   height of the video display
 * @see GLVideoTexture()
 */
OGLVideoPlayer::OGLVideoPlayer(int centerX, int centerY, int viewW, int viewH) {
	frameDraw = 0;
	finishedPlaying = false;
#ifndef IPHONE
	videoDecoder = NULL;
#endif
	image = new GLVideoTexture(1024,1024, centerX, centerY, viewW, viewH);

}


/**
 * @brief Destructor: destroys the OGLVideoPlayer object. In the process it also destroys the videoDecoder and the videotexture image
 *
 */
OGLVideoPlayer::~OGLVideoPlayer() {
	// TODO Auto-generated destructor stub
	this->stop();
	std::cout << "calling video player delete" << std::endl;
#ifndef IPHONE    
	if(videoDecoder)
		delete videoDecoder;
#endif
	delete image;
}


/**
 * @brief plays in a new thread the movie specified in the arguments, inside the OpenGL video display
 *
 * @param pathToMovie   path to the movie file to be played
 * @see NaradaVideoDecoderFFMPEG::NaradaVideoDecoderFFMPEG(), go()
 */
bool OGLVideoPlayer::play(std::string pathToMovie)
{
#ifndef IPHONE
	videoDecoder = new NaradaVideoDecoderFFMPEG(image, pathToMovie);
	return videoDecoder->go();
#endif
}


/**
 * @brief stop stops the process of decoding and displaying a movie.
 *

 * @see NaradaVideoDecoderFFMPEG::stopDecoding()
 */
void OGLVideoPlayer::stop()
{
#ifndef IPHONE    
	videoDecoder->stopDecoding();
#endif
	finishedPlaying = true;
}


/**
 * @brief plays in a new thread the movie specified in the arguments, inside the OpenGL video display
 *
 * @param w   new width of the main window view
 * @param h   new height of the main window view
 * @see GLVideoTexture::resizeWindow()
 */
void OGLVideoPlayer::resizeWindow(int w, int h)
{
	image->resizeWindow(w, h);
}


/**
 * @brief returns whether the video has finished playing or not
 *
 * @return true if the video has finished, false otherwise
 *
 */
bool OGLVideoPlayer::isFinishedPlaying()
{
	return finishedPlaying;
}


/**
 * @brief draws in OpenGL the next frame of the video, if any. Its a blocking function that waits until there's a frame
 *  available. The function returns whether the video has finished or not.
 *
 * @see NaradaVideoDecoderFFMPEG::wakeIfNotEnoughFrames(), GLVideoTexture::drawVideoFrame()
 *
 * @return true if the video has arrived to the end, false otherwise
 *
 */
bool OGLVideoPlayer::drawNextFrame()
{
	if(!finishedPlaying)
	{
		bool hasFrame = false;
#ifndef IPHONE        
		videoDecoder->wakeIfNotEnoughFrames();
#endif
//		pthread_mutex_lock(&imageMutex);
//		unsigned char* data = image->getNextFrame();
//		pthread_mutex_unlock(&imageMutex);
//		if(data)
//		{
//			SaveFrame(data, image->getWidth(), image->getHeight(), image->getLineWidth(), frameDraw);
//			hasFrame=true;
//			free(data);
//		}
//
////			pthread_mutex_lock(&imageMutex);
////			image->dropNextFrame();
////			pthread_mutex_unlock(&imageMutex);
//		}
//
//		if(hasFrame)
//		{
//			frameDraw++;
//		}

		((GLVideoTexture*)image)->drawVideoFrame();
		if(image->isLastFrame())
			finishedPlaying = true;

	}
	return finishedPlaying;
}

