//============================================================================
// Name        : AudioCapture.cpp
// Author      : Enrique Oriol
// Version     :
// Copyright   : Narada Robotics
// Description : captures audio from a recorder device using OpenAL
//============================================================================

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <stdlib.h>

#include <unistd.h>

#include "AudioCapture.hpp"

namespace VisageSDK
{


	AudioCapture::AudioCapture()
	{
		format = AL_FORMAT_MONO16;
		dataBufferSize = MAX_DATA_SAMPLES;
		freq = 16000;



		init();
	}


	AudioCapture::AudioCapture(ALenum form, ALsizei dataSize, ALsizei fm)
	{
		format = form;
		dataBufferSize = dataSize;
		freq = fm;


		init();
	}


	AudioCapture::~AudioCapture()
	{
		alcCaptureCloseDevice(captureDevice);
		if(data)
			delete data;
	}



	void AudioCapture::init()
	{
		switch(format)
		{
			case AL_FORMAT_MONO8:
				bytesPerSample = 1;
				break;
			case AL_FORMAT_MONO16:
				bytesPerSample = 2;
				break;
			default:
				bytesPerSample = 1;
				std::cout << "not properly format" << std::endl;
				break;
		}

		captureBufferSize = freq*SECONDS_TO_READ*bytesPerSample;


		realSize = 0;

		data = (ALCvoid*)malloc(dataBufferSize);

		captureDevice = alcCaptureOpenDevice(NULL, freq, format, captureBufferSize);//tenemos buffer para un segundo (8000samples/s(Hz)*2channels(stereo)*16bits/sample)
	}



	void AudioCapture::captureData()
	{
		std::cout << "capturing data" << std::endl;

		alcCaptureStart(captureDevice);

		int numOfSeconds = 0;
		while(numOfSeconds < 5)
		{
			ALint samples;
			alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES, bytesPerSample, &samples);
			if(samples >0)
			{
				alcCaptureSamples(captureDevice, (ALCvoid*) ((ALCchar*)data + realSize), samples);
				std::cout << "num of samples captured:" << samples << std::endl;

				removeNoise(samples);

				realSize = realSize + samples*bytesPerSample;

			}
#ifdef WIN32
			Sleep(1000*SECONDS_TO_READ);
#else
			sleep(SECONDS_TO_READ);
#endif
			numOfSeconds = numOfSeconds + SECONDS_TO_READ;
		}

		std::cout << "exit bucle" << std::endl;

		alcCaptureStop(captureDevice);
	}


	void AudioCapture::removeNoise(ALint numOfSamples)
	{
		//POR AHORA NADA
	}



	//getters


	ALenum AudioCapture::getFormat()
	{
		return format;
	}

	ALsizei AudioCapture::getSize()
	{
		return realSize;
	}

	ALsizei AudioCapture::getFrequency()
	{
		return freq;
	}

	ALCvoid* AudioCapture::getData()
	{
		return data;
	}



	void AudioCapture::deleteData()
	{
		if(data)
			free(data);
	}



}



