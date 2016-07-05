/*
 * NaradaVideoDecoderFFMPEG.h
 *
 *  Created on: 01/02/2012
 *      Author: Narada
 */

//          Copyright Narada Robotics 2008 - 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef NARADAVIDEODECODERFFMPEG_H_
#define NARADAVIDEODECODERFFMPEG_H_

#include <pthread.h>
#include <cstdio>
#include <cstring>
#include "RGBTextureArray.h"

#include <stdint.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>


/**
* @class NaradaVideoDecoderFFMPEG
* @brief Class that allows decoding a video through the FFMPEG library, and extract each of the frames
*
* @see RGBTextureArray
*
*/
class NaradaVideoDecoderFFMPEG
{
public:
	NaradaVideoDecoderFFMPEG(RGBTextureArray* textureArray, std::string path);
	~NaradaVideoDecoderFFMPEG();
	void wakeIfNotEnoughFrames();
	void stopDecoding();

	bool go();


private:

    void setupAndDecode();
	bool initializeDecoder();
	bool startDecoding();
	bool decodeFrame();
	void closeCodec();
	void waitIfEnoughFrames();

	static void *start_decoder_thread(void *obj)
	{
		reinterpret_cast<NaradaVideoDecoderFFMPEG *>(obj)->setupAndDecode();
	}


    volatile bool stopRequested;
    volatile bool running;
    volatile bool processPaused;
    pthread_mutex_t conditionMutex;
    pthread_cond_t enoughFramesCond;
    pthread_t videoThread;
    std::string pathToMovie;

	RGBTextureArray* rgbArray;
    AVFormatContext *pFormatCtx;
    int             videoStream;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVFrame         *pFrame;
    AVFrame         *pFrameRGB;
    AVPacket        packet;
    int             frameFinished;
    struct SwsContext *img_convert_ctx;
    int imageBufferedMaxFrames;
    int imageBufferedMinFrames;
    bool stopBuffering;


};


#endif /* NARADAVIDEODECODERFFMPEG_H_ */
