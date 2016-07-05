//
//  NaradaAudioContext.cpp
//  NaradaAVTools
//
//  Created by Enrique Oriol on 05/11/10.
//  Copyright 2010 Narada Robotics. All rights reserved.
//

#include "NaradaAudioContext.hpp"



	NaradaAudioContext::NaradaAudioContext(bool stream)
	{
		initNaradaAudioContext(stream);
	}

	NaradaAudioContext::~NaradaAudioContext()
	{
		deleteContext();
	}


	void NaradaAudioContext::initNaradaAudioContext(bool stream)
	{
		ALuint error;

		audioDevice = alcOpenDevice(NULL);
		if(audioDevice)
		{
			alContext = alcCreateContext(audioDevice, NULL);
			alcMakeContextCurrent(alContext);
		}

		if ((error = alGetError()) != AL_NO_ERROR)
		{
			std::cout<< "cannot create context " << error << std::endl;
			return;
		}

		if(!stream)
		{	// Generate Buffers
			alGenBuffers(1, &alBuffer);


			if ((error = alGetError()) != AL_NO_ERROR)
			{
				std::cout<< "alGenBuffers error: " << error << std::endl;
				return;
			}
		}
	//

	}


	ALboolean NaradaAudioContext::loadWAVonContext(NaradaAudioResource *audio)
	{
		// Load wav data into a buffer.

		alGenBuffers(1, &alBuffer);

	   if (alGetError() != AL_NO_ERROR)
		   return AL_FALSE;


	   audio->wavLoader();


	   alBufferData(alBuffer, audio->getFormat(), audio->getData(), audio->getSize(), audio->getFrequency());

	   audio->deleteData();
        
        //rarreaza 2012-01-24 to delete the new data in high pitch (avoid leak)
        audio->deleteDataHighPitch();


	   if (alGetError() == AL_NO_ERROR)
		   return AL_TRUE;


	   return AL_FALSE;
	}



	ALboolean NaradaAudioContext::loadCaptureOnContext(NaradaAudioResource *audio)
	{


	// Load capture data into a buffer.
	alGenBuffers(1, &alBuffer);
	if (alGetError() != AL_NO_ERROR)
	   return AL_FALSE;


	std::cout << "loading capture on context:" << std::endl;
		std::cout << "format: " << audio->getFormat() << std::endl;
		std::cout << "size: " << audio->getSize() << std::endl;
        std::cout << "size High Pitch: " << audio->getSizeHighPitch() << std::endl;
		std::cout << "freq:" << audio->getFrequency() << std::endl;

	alBufferData(alBuffer, audio->getFormat(), audio->getData(), audio->getSize(), audio->getFrequency());

	audio->deleteData();
        //rarreaza 2012-01-24 to delete the new data in high pitch (avoid leak)
    audio->deleteDataHighPitch();


	// Do another error check and return.
	if (alGetError() == AL_NO_ERROR)
	   return AL_TRUE;

	return AL_FALSE;
	}




	ALboolean NaradaAudioContext::bindAudioToBuffer(NaradaAudioResource *audio)
	{
		alGenSources(1, audio->getSource());
		if (alGetError() != AL_NO_ERROR)
			return AL_FALSE;

		alSourcei (*audio->getSource(), AL_BUFFER,   alBuffer   );
		if (alGetError() != AL_NO_ERROR)
			return AL_FALSE;

		return AL_TRUE;
	}


	void NaradaAudioContext::deleteContext()
	{
		if (alGetError() != AL_NO_ERROR)
			std::cout << "some error occured" << std::endl;



		if(alBuffer)
			alDeleteBuffers(1, &alBuffer);
		std::cout << "make context NULL" << std::endl;
		alcMakeContextCurrent(NULL);

		std::cout << "destroying context" << std::endl;
		alcDestroyContext(alContext);

		std::cout << "closing device" << std::endl;
		alcCloseDevice(audioDevice);

		   ALCcontext *pContext;
		   pContext = alcGetCurrentContext();
		    if(!pContext)
		    	std::cout << "context has been completely removed" << std::endl;

	}



