//============================================================================
// Name        : AudioCapture.hpp
// Author      : Enrique Oriol
// Version     :
// Copyright   : Narada Robotics
// Description : captures audio from a recorder device using OpenAL
//============================================================================


#ifndef __HPP_HEADER__AUDIOCAPTURE_HPP
	#define __HPP_HEADER__AUDIOCAPTURE_HPP

	//OpenAL libraries
	#ifdef ECLIPSE_MAC
		#include <al.h>
		#include <alc.h>
	#endif
    #if defined(MAC) || defined(IPHONE)
        #include <OpenAL/al.h>
        #include <OpenAL/alc.h>
    #endif
	#ifdef LINUX
		#include <AL/al.h>
		#include <AL/alc.h>
	#endif
	#ifdef WIN32
		#include <al.h>
		#include <alc.h>
	#endif



	#define MAX_DATA_SAMPLES 256000 //16bits/sample*1sample(mono)*8segundos de grabacion a fm=16000Hz
	#define SECONDS_TO_READ	1 //


#ifdef VISAGE_STATIC
	#define VISAGE_DECLSPEC
#else
	#if defined(MAC) || defined(LINUX)
		#define VISAGE_DECLSPEC __attribute__((__visibility__("default")))

	#else
		#ifdef VISAGE_EXPORTS
			#define VISAGE_DECLSPEC __declspec(dllexport)
		#else
			#define VISAGE_DECLSPEC __declspec(dllimport)
		#endif
	#endif

#endif



namespace VisageSDK
{

	class VISAGE_DECLSPEC AudioCapture{

	public:

		AudioCapture();

		AudioCapture(ALenum form, ALsizei dataSize, ALsizei fm);

		~AudioCapture();


		void init();


		void captureData();



		//getters

		ALenum getFormat();

		ALsizei getSize();

		ALsizei getFrequency();


		ALCvoid* getData();

		void deleteData();

		void removeNoise(ALint numOfSamples);


	private:

		ALCdevice *captureDevice;

		ALenum format;

		ALCvoid* data;

		ALsizei freq;

		ALsizei realSize;

		ALsizei captureBufferSize;

		ALsizei dataBufferSize;

		unsigned char bytesPerSample;

	};

}

#endif

