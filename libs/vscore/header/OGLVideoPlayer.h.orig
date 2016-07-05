/*
 * OGLVideoPlayer.h
 *
 *  Created on: 02/02/2012
 *      Author: narada
 */

#ifndef OGLVIDEOPLAYER_H_
#define OGLVIDEOPLAYER_H_

#include "RGBTextureArray.h"
#include "GLVideoTexture.h"
#ifndef IPHONE
    #include "NaradaVideoDecoderFFMPEG.h"
#endif
#include <cstdio>
#include <cstdlib>
#include <iostream>


/**
* @class OGLVideoPlayer
* @brief Class that allows to display images from a Video through an OpenGL texture
*
* @see GLVideoTexture, RGBTextureArray, NaradaVideoDecoderFFMPEG
*
*/
class OGLVideoPlayer {

public:
	OGLVideoPlayer(int centerX, int centerY, int viewW, int viewH);
	virtual ~OGLVideoPlayer();

	bool play(std::string pathToMovie);
	void stop();
	bool drawNextFrame();
	bool isFinishedPlaying();
	void resizeWindow(int w, int y);


private:
#ifndef IPHONE
	NaradaVideoDecoderFFMPEG* videoDecoder;
#endif
	RGBTextureArray* image;
	int frameDraw;
	bool finishedPlaying;
};


#endif /* OGLVIDEOPLAYER_H_ */
