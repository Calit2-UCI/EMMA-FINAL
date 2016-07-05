//============================================================================
// Name        : AudioResource.hpp
// Author      : Enrique Oriol
// Version     :
// Copyright   : Narada Robotics
// Description : Hello World in C++, Ansi-style
//============================================================================


#ifndef __HPP_HEADER__AUDIORESOURCE_HPP
#define __HPP_HEADER__AUDIORESOURCE_HPP

	//OpenAL libraries
	#ifdef ECLIPSE_MAC
		#include <al.h>
		#include <alc.h>
	#endif
        #if defined(MAC) || defined(IPHONE)
//                #include <OpenAL/al.h>
//                #include <OpenAL/alc.h>
        #endif
	#ifdef LINUX
		#include <AL/al.h>
		#include <AL/alc.h>
	#endif
	#ifdef WIN32
		#include <al.h>
		#include <alc.h>
	#endif

#include <cstdio>
#include <cstring>
#include <cstdlib>


#include "AudioContext.hpp"
#include "AudioCapture.hpp"


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

class  AudioContext;



class VISAGE_DECLSPEC AudioResource
{

	public:

		AudioResource();

		~AudioResource();

		AudioResource(AudioCapture* capture);

		AudioResource(char* file);

		AudioResource(AudioContext* ctx);

		AudioResource(AudioContext* ctx, char* file);

		void init();

		void bindListener();

		void deleteSource();

		void deleteData();
		
		void wavLoader();

		ALboolean writeToWAV(char* filename);

		ALboolean loadFromWAV(char* filename);

		ALboolean loadFromCapture(AudioCapture* capture);

		ALboolean addFromCapture(ALCvoid* data, ALsizei size, ALenum format, ALsizei frequency, int currentPos);

		ALboolean endAddingFromCapture();

		void play();

		void stop();

		void rewind();

		bool isPlaying();

		void bindSourceAtt();


		void setFilename(char* filename);

		void setFormat(ALenum form);

		void setSize(ALsizei dim);

		void setFrequency(ALsizei f);

		void setData(ALCvoid* dat);

		void copyData(ALCvoid* dat);

		void addData(ALCvoid* dat, ALsizei increaseSize, bool firstTime);


		ALuint* getSource();

		ALenum getFormat();

		ALsizei getSize();

		ALsizei getFrequency();

		char* getFilename();

		ALCvoid* getData();



	private:


		AudioContext* context;

		ALenum format;

		ALsizei size;

		ALCvoid* data;

		ALsizei freq;

		ALboolean loop;

		char* filename;

		ALuint alSource;

		//TODO IDEA: bind sourcePos and orientation to 3D model mouth

		ALfloat SourcePos[3];

		ALfloat SourceVel[3];

		ALfloat ListenerPos[3];

		ALfloat ListenerVel[3];

		// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
		ALfloat ListenerOri[6];

};

}

#endif

