// NaradaVideoDecoderFFMPEG.cpp



#include "NaradaVideoDecoderFFMPEG.h"



/**
 * @brief Constructor: init data members
 *
 * @param textureArray  RGBTextureArray where each new frame should be stored
 * @param path			path to the movie that has to be decoded
 *
 */
NaradaVideoDecoderFFMPEG::NaradaVideoDecoderFFMPEG(RGBTextureArray* textureArray, std::string path)
{
	stopRequested = false;
	processPaused = false;
	pFormatCtx = NULL;
	running = false;
	pathToMovie = path;
	rgbArray = textureArray;
	img_convert_ctx = NULL;
	imageBufferedMaxFrames = 20;
	imageBufferedMinFrames = 4;
	stopBuffering = false;
	pthread_mutex_init(&conditionMutex, NULL);
	pthread_cond_init(&enoughFramesCond, NULL);
	videoThread = NULL;

}



/**
 * @brief Destructor: destroy the mutex and signal data members
 *
 */
NaradaVideoDecoderFFMPEG::~NaradaVideoDecoderFFMPEG()
{
	pthread_mutex_destroy(&conditionMutex);
	pthread_cond_destroy(&enoughFramesCond);
}



/**
 * @brief if the thread hasn't been launched yet, it launches the decoding function in a dedicated thread
 *
 * @see start_decoder_thread()
 *
 * @return false if the thread was running before this call, true otherwise
 *
 */
bool NaradaVideoDecoderFFMPEG::go()
{
	if(running != false)
	{
		std::cerr << "running was true at NaradaVideoDecoderFFMPEG::go, thread MUST exit!" << std::endl;
		return false;
	}
	running = true;
	pthread_create(&videoThread, 0, &NaradaVideoDecoderFFMPEG::start_decoder_thread, this);
	return true;
}



/**
 * @brief stops decoding a video, and tells the rgbArray that last frame has been sended.
 *
 * @see GLVideoTexture::setLastFrame()
 *
 */
void NaradaVideoDecoderFFMPEG::stopDecoding()
{
	stopRequested = true;
	rgbArray->setLastFrame();

	pthread_mutex_lock(&conditionMutex);
	int err;
	if((err=pthread_cond_broadcast(&enoughFramesCond)))
	{
		errno = err;
		perror("thread cond broadcast at NaradaVideoDecoderFFMPEG::stopDecoding throw: ");
	}

	pthread_mutex_unlock(&conditionMutex);

	if((err=pthread_join(videoThread,0)))
	{
		errno = err;
		perror("thread join at NaradaVideoDecoderFFMPEG::stopDecoding throw: ");
	}
}



/**
 * @brief initialize the FFMPEG decoder, extracting the needed data from the video source.
 * Allocates memory to read the frames from the video, and initialize the RGBArray data member
 *
 * @return true if all the process has been done right, false otherwise
 *
 */
bool NaradaVideoDecoderFFMPEG::initializeDecoder()
{
	av_register_all();
    if(avformat_open_input(&pFormatCtx, pathToMovie.c_str(), NULL, NULL)!=0)
        return false;

    if(avformat_find_stream_info(pFormatCtx,NULL)<0)
        return false;


    // Find the first video stream
    videoStream=-1;
    for(int i=0; i<pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoStream=i;
            break;
        }
    }
    if(videoStream==-1)
        return false;


    pCodecCtx=pFormatCtx->streams[videoStream]->codec;//probar avcodec_alloc_context3(codec) en su lugar -KaiK-

    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL)
        return false;

    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
        return false;

    // Hack to correct wrong frame rates that seem to be generated by some codecs
    if(pCodecCtx->time_base.num>1000 && pCodecCtx->time_base.den==1)
		pCodecCtx->time_base.den=1000;

    pFrame=avcodec_alloc_frame();
    if(pFrame==NULL)
            return false;

    pFrameRGB=avcodec_alloc_frame();
    if(pFrameRGB==NULL)
        return false;

    avpicture_alloc((AVPicture *)pFrameRGB, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

    if(img_convert_ctx == NULL) {
		int w = pCodecCtx->width;
		int h = pCodecCtx->height;

		img_convert_ctx = sws_getContext(w, h, pCodecCtx->pix_fmt, w, h,PIX_FMT_RGB24, /*SWS_BICUBIC*/SWS_FAST_BILINEAR,NULL, NULL, NULL);
		if(img_convert_ctx == NULL) {
			fprintf(stderr, "Cannot initialize the conversion context!\n");
			return false;
		}
	}

    rgbArray->setSize(pCodecCtx->width,pCodecCtx->height);
    rgbArray->setLineWidth(pFrameRGB->linesize[0]);


    std::cout << "decoder initialized" << std::endl;
    return true;

}



/**
 * @brief blocking function that sleeps the decoding process if there are too much decoded frames that hasn't been read
 *
 *
 */
void NaradaVideoDecoderFFMPEG::waitIfEnoughFrames()
{
	pthread_mutex_lock(&conditionMutex);
	if(!processPaused)
	{
		while(rgbArray->getBufferedFramesNum()>imageBufferedMaxFrames)
		{
			processPaused = true;
			int err;
			if((err=pthread_cond_wait(&enoughFramesCond, &conditionMutex)))
				std::cerr << "cond wait at NaradaVideoDecoderFFMPEG::waitIfEnoughFrames throw error: " << err << std::endl;
		}
		processPaused = false;
	}
	pthread_mutex_unlock(&conditionMutex);
}



/**
 * @brief wakes up the decoding process if there are only few decoded frames in the buffer to be read.
 *
 *
 */
void NaradaVideoDecoderFFMPEG::wakeIfNotEnoughFrames()
{
	pthread_mutex_lock(&conditionMutex);
	if(processPaused)
	{
		if(rgbArray->getBufferedFramesNum()<imageBufferedMinFrames)
		{
			processPaused = false;
			int err;
			if((err=pthread_cond_signal(&enoughFramesCond)))
				std::cerr << "cond signal at NaradaVideoDecoderFFMPEG::wakeIfNotEnoughFrames throw error: " << err << std::endl;
		}
	}
	pthread_mutex_unlock(&conditionMutex);


}



/**
 * @brief starts and does all the decoding process. It decode all the frames until the last one.
 *
 * @see waitIfEnoughFrames(), decodeFrame(), GLVideoTexture::setLastFrame()
 *
 * @return true
 *
 */
bool NaradaVideoDecoderFFMPEG::startDecoding()
{
	std::cout << "start decoding" << std::endl;


	while(!stopRequested)
	{
		waitIfEnoughFrames();

		if(av_read_frame(pFormatCtx, &packet)>=0)
		{
			if(packet.stream_index==videoStream)
			{
				decodeFrame();
			}

			av_free_packet(&packet);
		}
		else
			break;

	}

	rgbArray->setLastFrame();

	avpicture_free((AVPicture *)pFrameRGB);
	av_free(pFrame);

	return true;

}



/**
 * @brief decodes 1 frame using the ffmpeg library and stores it in the rgbArray
 *
 * @see GLVideoTexture::setNextFrame()
 *
 * @return true if the frame has been completely decoded, false otherwise
 *
 */
bool NaradaVideoDecoderFFMPEG::decodeFrame()
{
	std::cout << "decoding frame" << std::endl;
	avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
	if(frameFinished)
	{
		int ret = sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0,pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

		rgbArray->setNextFrame(pFrameRGB->data[0]);

		return true;

	}

	return false;
}



/**
 * @brief closes the ffmpeg codec
 *
 *
 */
void NaradaVideoDecoderFFMPEG::closeCodec()
{

	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	std::cout << "closing codec" <<std::endl;
}



/**
 * @brief Initializes the codec, performs the decoding process, and closes the codec.
 * This function is suposed to be called in a specific thread. When the function arrives to the end, it destroys the thread.
 *
 * @see NaradaVideoDecoderFFMPEG::go, initializeDecoder(), startDecoding(), closeCodec()
 *
 */
void NaradaVideoDecoderFFMPEG::setupAndDecode()
{
	if(initializeDecoder())
		startDecoding();
	else
		std::cerr << "error initializing decoder" << std::endl;
	closeCodec();

	pthread_exit(0);


}






