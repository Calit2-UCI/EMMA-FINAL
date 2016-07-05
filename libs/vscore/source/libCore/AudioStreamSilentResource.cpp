//============================================================================
// Name        : AudioStreamSilentResource.cpp
// Author      : Enrique Oriol
// Version     :
// Copyright   : Narada Robotics
// Description : Hello World in C++, Ansi-style
//============================================================================
#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <stdlib.h>
#include "AudioStreamSilentResource.hpp"

using namespace std;

namespace VisageSDK
{




	AudioStreamSilentResource::~AudioStreamSilentResource()
	{

		std::cout << "deleting stream resource" << std::endl;
		deallocBuffer();

	}





	AudioStreamSilentResource::AudioStreamSilentResource(char* file)
	{
		filename = file;
		bufferSize = STREAMING_BUFFER_SIZE;
        bufferData = NULL;
        bytesPerFrame = 1;
        
		initStreamFromWAV();

	}




    void AudioStreamSilentResource::allocBuffer()
    {
        if(!bufferData)
            bufferData = (short*)malloc(bufferSize/2*sizeof(short*));
//            bufferData = (unsigned char*)malloc(bufferSize*sizeof(unsigned char*));            
    }


    void AudioStreamSilentResource::deallocBuffer()
    {
        if(!bufferData)
        {
            free(bufferData);
            bufferData = NULL;
        }
    }
    

	//setters

	void AudioStreamSilentResource::setSize(size_t dim)
	{
		audioSize = dim;
	}

	void AudioStreamSilentResource::setFrequency(size_t f)
	{
		freq = f;
	}





	//getters


	size_t AudioStreamSilentResource::getSize()
	{
		return audioSize;
	}

	size_t AudioStreamSilentResource::getFrequency()
	{
		return freq;
	}

	char* AudioStreamSilentResource::getFilename()
	{
		return filename;
	}
    
    unsigned int AudioStreamSilentResource::getBytesPerFrame()
	{
        return bytesPerFrame;
    }










	bool AudioStreamSilentResource::readHeader()
	{
		//TAKE CARE: asume we are working on little-endian sistem
		long fileSize = 0;
		long byteCounter = 0;
		unsigned char buffer[12];
		int channels, bits;
		unsigned int frequency;


		fseek(audioFile, 0, SEEK_END);
		fileSize = ftell(audioFile);
		std::cout << "total file size: " << fileSize << std::endl;
		std::rewind(audioFile);

		//Check header

		//pass through first sub-chunk
		fread(buffer, 1, 12, audioFile);


		//check if we are reading fmt sub-chunk or not
		fread(buffer, 1, 8, audioFile);
		if(buffer[0] != 'f' || buffer[1] != 'm' || buffer[2] != 't' || buffer[3] != ' ')
		{
			fprintf(stderr, "Not 'fmt ' \n");
			closeFile();
			return false;
		}


		//check if WAV has PCM format (default)
		fread(buffer, 1, 2, audioFile);
		if(buffer[1] != 0 || buffer[0] != 1)
		{
			fprintf(stderr, "Not PCM \n");
			closeFile();
			return false;
		}


		//get channel count
		fread(buffer, 1, 2, audioFile);
		channels  = buffer[1]<<8;
		channels |= buffer[0];

		//get sample frequency
		fread(buffer, 1, 4, audioFile);
		frequency  = buffer[3]<<24;
		frequency |= buffer[2]<<16;
		frequency |= buffer[1]<<8;
		frequency |= buffer[0];

		//passing throught bytes per second and block size
		fread(buffer, 1, 6, audioFile);

		//get bit depth. 8 or 16
		fread(buffer, 1, 2, audioFile);
		bits = buffer[1]<<8;
		bits |= buffer[0];

		//define format
		if(bits == 8)
		{
			if(channels == 1)
                bytesPerFrame = 1;
			else if(channels == 2)
                bytesPerFrame = 2;                
		}
		else if(bits == 16)
		{
			if(channels == 1)
                bytesPerFrame = 2;                
			else if(channels == 2)
                bytesPerFrame = 4;                
		}
		else
		{
			fprintf(stderr, "Incompatible format (%d, %d) :(\n", channels, bits);
			closeFile();
			return false;
		}


		//check that next sub-chunk is data
		fread(buffer, 1, 8, audioFile);
		if(buffer[0] != 'd' || buffer[1] != 'a' || buffer[2] != 't' || buffer[3] != 'a')
		{
			//IF it's not, there are more unexpected chunks, we'll find directly the "data" tag.
			std::cout << "not data chunk. let's search directly the data tag" << std::endl;

			fseek(audioFile, -7, SEEK_CUR);
			bool isData = false;
			byteCounter = ftell(audioFile);
			while(!isData && byteCounter < fileSize)
			{
				fread(buffer, 1, 1, audioFile);
				if(buffer[0]=='d')
				{
					fread(buffer,1,3, audioFile);
					if(buffer[0]=='a' && buffer[1]=='t' && buffer[2]=='a')
						isData = true;
					else
						fseek(audioFile, -3, SEEK_CUR);
				}
				byteCounter++;
			}
			if(isData)
				//pass through non-interesting bytes
				fread(buffer,1,4,audioFile);
			else
			{
				closeFile();
				fprintf(stderr, "Not 'data' \n");
				return false;
			}
		}


		headerSize = ftell(audioFile);
		std::cout << "header file size: " << headerSize << std::endl;
		
		audioSize = fileSize - headerSize;
		
		std::cout << "audio file size: " << audioSize << std::endl;

		setFrequency(frequency);


		return true;
	}


	bool AudioStreamSilentResource::initStreamFromWAV()
	{

		bufferIndex = 0;

		if(!openFile())
			return false;

		readHeader();


		return true;

	}

    

	bool AudioStreamSilentResource::readWAV()
	{
	

		bufferIndex = 0;
        
        allocBuffer();
        
        bool reading = true;



		while(reading)
		{
            reading = loadNextStreamInBuffer();
            
            handleProcessBuffer();
//            std::cout << "new chunk of data processed: "<< bufferIndex << " / " << audioSize / STREAMING_BUFFER_SIZE << std::endl;
            
            reading = handleEndLipSync(!reading);
		}
		
		deallocBuffer();
		
		std::cout << "stream read" << std::endl;


		closeFile();

		return true;
	}



	bool AudioStreamSilentResource::loadNextStreamInBuffer()
	{

		int totalChunks = audioSize / STREAMING_BUFFER_SIZE;
		if(bufferIndex > totalChunks)
			return false;

		long startOffset = bufferIndex*STREAMING_BUFFER_SIZE;

		if(bufferIndex == totalChunks)  //the audioFile does not have to be an exact multiple of bufferIndex
        {
			bufferSize = audioSize - (bufferSize*totalChunks);
            bufferData = (short*) realloc((void*)bufferData, bufferSize/2*sizeof(short*));
        }


		if(!readAudioStream((void*)bufferData, bufferSize, startOffset))
		{
			std::cout << "cannot read audio stream" << std::endl;
		   return false;
		}


		bufferIndex++;


		if(bufferIndex > totalChunks)
			return false;//there are no more data to enqueue.

		return true;

	}


	bool AudioStreamSilentResource::readAudioStream(void* buff, int bufferSize, int startOffset)
	{

		int byteCounter = 0;
		int offset= headerSize + startOffset;


		fseek(audioFile, offset, SEEK_SET);
		byteCounter = fread(buff, 1, bufferSize, audioFile);


		if(byteCounter != bufferSize)
		{
			std::cout << "unexpected size of stream audio data read" << std::endl;
			return false;
		}

		return true;

	}




	bool AudioStreamSilentResource::openFile()
	{

		audioFile = fopen(filename, "r");
		if(audioFile==NULL)
		{
			std::cout << "cannot open file" << std::endl;
			return false;
		}
		return true;

	}
    

	void AudioStreamSilentResource::closeFile()
	{
		if(audioFile)
			fclose(audioFile);
	}
    
    

}





