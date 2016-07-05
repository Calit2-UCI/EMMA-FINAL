//============================================================================
// Name        : AudioContext.cpp
// Author      : Enrique Oriol
// Version     :
// Copyright   : Narada Robotics
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "AudioContext.hpp"

#include <iostream>
using namespace std;

namespace VisageSDK
{
    int AudioContext::numAudioResources = 0;
    ALCdevice* AudioContext::audioDevice = NULL;
    ALCcontext* AudioContext::alContext = NULL;
    

	AudioContext::AudioContext(bool stream)
	{
		initAudioContext(stream);
	}

	AudioContext::~AudioContext()
	{
		deleteContext();
	}


	void AudioContext::initAudioContext(bool stream)
	{
		ALuint error;
        
        //When opening the first audioContext, the device is initialized. It will be closed when releasing the last audioContext.
        if(!numAudioResources)
        {
            audioDevice = alcOpenDevice(NULL);
            
            if(audioDevice)
            {
                alContext = alcCreateContext(audioDevice, NULL);
                setContextCurrent();
                if ((error = alGetError()) != AL_NO_ERROR)
                {
                    std::cout<< "cannot create context " << error << std::endl;
                    return;
                }
                else
                    std::cout<< "audioContext created " << std::endl;
            }
            else
            {
                std::cout << "cannot open audio device" << std::endl;
                return;
            }
            
        }
        
        numAudioResources++;
        
		


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

	void AudioContext::setContextCurrent()
	{
		ALuint error = 0;
		alcMakeContextCurrent(alContext);
		if ((error = alGetError()) != AL_NO_ERROR)
			std::cout<< "cannot make context current: " << error << std::endl;
	}


	ALboolean AudioContext::loadWAVonContext(AudioResource *audio)
	{
		// Load wav data into a buffer.

		setContextCurrent();
		alGenBuffers(1, &alBuffer);

        if (alGetError() != AL_NO_ERROR) {
            cout << "Error while alGenBuffers in AudioContext::loadWAVonContext" << endl;
		   return AL_FALSE;
        }


	   audio->wavLoader();
        
        ALenum format = audio->getFormat();
        ALCvoid* data = audio->getData();
        ALsizei size = audio->getSize();
        ALsizei frequency = audio->getFrequency();


	   alBufferData(alBuffer, format, data, size, frequency);

	   audio->deleteData();


        ALenum error = alGetError();
	   if (error == AL_NO_ERROR)
		   return AL_TRUE;

        std::cout << "Error in loadWAVonContext: " << error << std::endl;
        std::cout << "Format: " << format << ", Size: " << size << ", Frequency: " << frequency << std::endl;

	   return AL_FALSE;
	}



	ALboolean AudioContext::loadCaptureOnContext(AudioResource *audio)
	{


	setContextCurrent();
	// Load capture data into a buffer.
	alGenBuffers(1, &alBuffer);
	if (alGetError() != AL_NO_ERROR)
	   return AL_FALSE;


	std::cout << "loading capture on context:" << std::endl;
		std::cout << "format: " << audio->getFormat() << std::endl;
		std::cout << "size: " << audio->getSize() << std::endl;
		std::cout << "freq:" << audio->getFrequency() << std::endl;

	alBufferData(alBuffer, audio->getFormat(), audio->getData(), audio->getSize(), audio->getFrequency());

	audio->deleteData();


	// Do another error check and return.
	if (alGetError() == AL_NO_ERROR)
	   return AL_TRUE;

	return AL_FALSE;
	}




	ALboolean AudioContext::bindAudioToBuffer(AudioResource *audio)
	{
		setContextCurrent();
        if (alGetError() != AL_NO_ERROR)
        {
            std::cout << "Cannot change context" << std::endl;
			return AL_FALSE;
        }
		alGenSources(1, audio->getSource());
        ALuint error = alGetError();
		if ( error != AL_NO_ERROR)
        {
            std::cout << "Cannot get sources: " << error << std::endl;            
			return AL_FALSE;
        }

		alSourcei (*audio->getSource(), AL_BUFFER,   alBuffer   );
		if (alGetError() != AL_NO_ERROR)
        {
            std::cout << "Cannot link buffer with source" << std::endl;
			return AL_FALSE;
        }
		return AL_TRUE;
	}


	void AudioContext::deleteContext()
	{
		setContextCurrent();
		if (alGetError() != AL_NO_ERROR)
			std::cout << "some error occured" << std::endl;



		if(alBuffer)
        {
			alDeleteBuffers(1, &alBuffer);
            std::cout << "deleted Buffer" << std::endl;            
        }
        
        
        numAudioResources--;
        
        if(!numAudioResources)
        {
            std::cout << "make context NULL" << std::endl;
            alcMakeContextCurrent(NULL);
            
            std::cout << "destroying context" << std::endl;
            alcDestroyContext(alContext);
            
            std::cout << "Last audio context is being deleted, so closing device" << std::endl;
            alcCloseDevice(audioDevice);
            audioDevice = NULL;
            
            ALCcontext *pContext;
            pContext = alcGetCurrentContext();
		    if(!pContext)
		    	std::cout << "context has been completely removed" << std::endl;
        }

	}

}

