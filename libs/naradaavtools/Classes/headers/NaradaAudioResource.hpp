//
//  NaradaAudioResource.hpp
//  NaradaAVTools
//
//  Created by Enrique Oriol on 05/11/10.
//  Copyright 2010 Narada Robotics. All rights reserved.
//




#ifndef __HPP_HEADER__NARADAAUDIORESOURCE_HPP
#define __HPP_HEADER__NARADAAUDIORESOURCE_HPP


#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>


#include "NaradaAudioContext.hpp"





class NaradaAudioContext;



class NaradaAudioResource
{

	public:

		NaradaAudioResource();

		~NaradaAudioResource();

		NaradaAudioResource(char* file);

		NaradaAudioResource(NaradaAudioContext* ctx);

		NaradaAudioResource(NaradaAudioContext* ctx, char* file);

		void init();

		void bindListener();

		void deleteSource();

		void deleteData();
    
        void deleteDataHighPitch();
		
		void wavLoader();

		ALboolean writeToWAV(char* filename);
    
        ALboolean writeToWAVHighPitch(char* filename);

		ALboolean loadFromWAV(char* filename);


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
    

        void setSizeHighPitch(ALsizei dim);
        void setDataHighPitch(ALCvoid* dat);
        ALsizei getSizeHighPitch();
        ALCvoid* getDataHighPitch();
        void setFilenameHighPitch(char* file);
        void lowerStoredNumSamplesBy( int factor );
        
        int getBitsPerSample();
        void setBitsPerSample(int bitsPS);




	private:


		NaradaAudioContext* context;

		ALenum format;

		ALsizei size;

		ALCvoid* data;
    
        ALsizei sizeHighPitch;
    
        ALCvoid* dataHighPitch;
        
        char* filenameHighPitch;

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
        
        //to facilitate pitch changing
        int bitsPerSample;

};


#endif

