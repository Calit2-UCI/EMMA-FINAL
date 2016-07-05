//============================================================================
// Name        : AudioStreamResource.hpp
// Author      : Enrique Oriol
// Version     :
// Copyright   : Narada Robotics
// Description : Hello World in C++, Ansi-style
//============================================================================


#ifndef __HPP_HEADER__AUDIOSTREAMSILENTRESOURCE_HPP
	#define __HPP_HEADER__AUDIOSTREAMSILENTRESOURCE_HPP


	#include <stdlib.h>
	#include <cstdio>
	#include <cstring>

	#include "AudioStreamResource.hpp"



	#define SILENT_STREAMING_BUFFER_SIZE 512



namespace VisageSDK
{


class AudioStreamSilentResource{

	public:


		virtual ~AudioStreamSilentResource();

		AudioStreamSilentResource(char* file);

		bool initStreamFromWAV();
    

		void setSize(size_t dim);

		void setFrequency(size_t f);

		size_t getSize();

		size_t getFrequency();
    
        unsigned int getBytesPerFrame();

		char* getFilename();



	protected:


		size_t audioSize;

		size_t headerSize;

		size_t bufferSize;

		int bufferIndex;
	
        short *bufferData;

		size_t freq;
    
        unsigned int bytesPerFrame;

		char* filename;

		FILE* audioFile;



		virtual bool loadNextStreamInBuffer();
    
        virtual void handleProcessBuffer(){};
    
        virtual bool handleEndLipSync(bool finalReached)
        {
            if(finalReached)
                return false;
            return true;
        };
    
        void allocBuffer();    

        void deallocBuffer();


		bool openFile();

		void closeFile();

		bool readHeader();
    
        bool readWAV();

		bool readAudioStream(void* buff, int bufferSize, int startOffset);


	};

}
#endif

