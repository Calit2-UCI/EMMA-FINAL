//============================================================================
// Name        : AudioStreamResource.hpp
// Author      : Enrique Oriol
// Version     :
// Copyright   : Narada Robotics
// Description : Hello World in C++, Ansi-style
//============================================================================


#ifndef __HPP_HEADER__AUDIOSTREAMRESOURCE_HPP
	#define __HPP_HEADER__AUDIOSTREAMRESOURCE_HPP


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

	#include <stdlib.h>
	#include <cstdio>
	#include <cstring>

	#include "AudioContext.hpp"



	#define STREAMING_BUFFER_SIZE 512
	#define N_BUFFERS 36




namespace VisageSDK
{

class AudioContext;


class AudioStreamResource{

	public:

	short* streamingBufferData[N_BUFFERS];

		AudioStreamResource();

		virtual ~AudioStreamResource();

		AudioStreamResource(AudioContext *ctx);

		AudioStreamResource(char* file);

		AudioStreamResource(AudioContext* ctx, char* file);


		ALboolean initStreamFromWAV();

		ALboolean genBuffersAndSources();

		ALboolean play();

		ALboolean update();

		void stop();

		void rewind();

		bool isPlaying();


		void printFormat();

		void setFormat(ALenum form);

		void setSize(ALsizei dim);

		void setFrequency(ALsizei f);



		ALuint* getSource();

		ALuint getBytesPerFrame();

		ALenum getFormat();

		ALsizei getSize();

		ALsizei getFrequency();

		char* getFilename();

		AudioContext *getContext();



	protected:

		ALuint alBuffersID[N_BUFFERS];

		ALsizei audioSize;

		ALsizei headerSize;

		ALsizei bufferSize;

		int bufferIndex;
	
		int muteBufferID;

		int update_time_us;
	
		bool mute;
	
		bool fullCPUusage;

		AudioContext* context;

		ALenum format;

		ALsizei freq;

		ALboolean loop;

		char* filename;

		FILE* audioFile;

		ALuint alSource;
		//TODO IDEA: bind sourcePos and orientation to 3D model mouth
		// Position of the source sound.
		ALfloat SourcePos[3];
		// Velocity of the source sound.
		ALfloat SourceVel[3];
		// Position of the listener.
		ALfloat ListenerPos[3];
		// Velocity of the listener.
		ALfloat ListenerVel[3];
		// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
		ALfloat ListenerOri[6];


		virtual ALboolean loadNextStreamInBuffer(ALuint buffID);

		ALboolean emptyBuffers();

		void init();

		void bindListener();

		ALboolean wavLoader();

		void bindSourceAtt();

		bool openFile();

		void closeFile();

		ALboolean readHeader();

		bool readAudioStream(ALCvoid* buff, int bufferSize, int startOffset);

//		bool readAudioStream(short* buff, int bufferSize, int startOffset);

		void release();

		int getBufferIdentifier(ALuint buff);


	};

}
#endif

