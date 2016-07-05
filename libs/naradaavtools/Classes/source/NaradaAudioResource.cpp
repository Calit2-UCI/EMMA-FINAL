//============================================================================
// Name        : NaradaAudioResource.cpp
// Author      : Enrique Oriol
// Version     :
// Copyright   : Narada Robotics
// Description : Manage Audio streams and raw audio, play and i/o to wav file
//============================================================================

//
//  NaradaAudioResource.cpp
//  NaradaAVTools
//
//  Created by Enrique Oriol on 05/11/10.
//  Copyright 2010 Narada Robotics. All rights reserved.
//


#include <iostream>
#include <stdlib.h>
#include "NaradaAudioResource.hpp"



	NaradaAudioResource::NaradaAudioResource()
	{
		context = NULL;
		init();
	}

	NaradaAudioResource::~NaradaAudioResource()
	{

		if(filename)
		{
			free(filename);
			filename = NULL;
		}
		
		deleteSource();

		if(context)
			delete context;

	}



	NaradaAudioResource::NaradaAudioResource(NaradaAudioContext *ctx)
	{
		context = ctx;
		init();

	}


	NaradaAudioResource::NaradaAudioResource(char* file)
	{
		context = new NaradaAudioContext(false);
		init();
		setFilename(file);
		loadFromWAV(filename);

	}

	NaradaAudioResource::NaradaAudioResource(NaradaAudioContext *ctx, char* file)
	{
		context = ctx;
		init();
		setFilename(file);
		loadFromWAV(filename);

	}


	void NaradaAudioResource::init()
	{
		//Init audio source and dest.
	//TODO IDEA: bind sourcePos and orientation to 3D model mouth
		// Position of the source sound.
		for(int i=0; i<3; i++)
		{
			SourcePos[i] = 0.0;
			SourceVel[i] = 0.0;
			ListenerPos[i] = 0.0;
			ListenerVel[i] = 0.0;
			ListenerOri[i] = 0.0;
			ListenerOri[i+3] = 0.0;
		}
		ListenerOri[2] = -1.0;
		ListenerOri[4] = 1.0;
		loop = AL_FALSE;
		filename = NULL;
        // rarreaza 2012-01-24 initialization of new propertiez to get high pitch
        dataHighPitch=NULL;
        sizeHighPitch=0;

		std::cout << "audio resource initialized" << std::endl;

	}




	//setters

	void NaradaAudioResource::setFormat(ALenum form)
	{
		format = form;
	}

	void NaradaAudioResource::setSize(ALsizei dim)
	{
		size = dim;
	}

	void NaradaAudioResource::setFrequency(ALsizei f)
	{
		freq = f;
	}

	void NaradaAudioResource::setData(ALCvoid* dat)
	{
		data = dat;
	}

	void NaradaAudioResource::setFilename(char* file)
	{
		int lengh = strlen(file) + 1;
		filename = (char*)malloc(lengh * sizeof(char));
		filename = strcpy(filename, (const char*)file);
		std::cout << "filename set: " << filename << std::endl;
	}



	//getters

	ALuint* NaradaAudioResource::getSource()
	{
		return &alSource;
	}

	ALenum NaradaAudioResource::getFormat()
	{
		return format;
	}

	ALsizei NaradaAudioResource::getSize()
	{
		return size;
	}

	ALsizei NaradaAudioResource::getFrequency()
	{
		return freq;
	}

	char* NaradaAudioResource::getFilename()
	{
		return filename;
	}

	ALCvoid* NaradaAudioResource::getData()
	{
		return data;
	}

//rarreaza 2012-01-24 getters/setters of new properties for pitch shift
    void NaradaAudioResource::setSizeHighPitch(ALsizei dim)
    {
        sizeHighPitch = dim;
    }

    void NaradaAudioResource::setDataHighPitch(ALCvoid* dat)
    {
        dataHighPitch = dat;
    }

    ALsizei NaradaAudioResource::getSizeHighPitch()
    {
        return sizeHighPitch;
    }

    ALCvoid* NaradaAudioResource::getDataHighPitch()
    {
        return dataHighPitch;
    }


    void NaradaAudioResource::setFilenameHighPitch(char* file)
    {
        int lengh = strlen(file) + 1;
        filenameHighPitch = (char*)malloc(lengh * sizeof(char));
        filenameHighPitch = strcpy(filenameHighPitch, (const char*)file);
        std::cout << "filename for high pitch file set: " << filenameHighPitch << std::endl;
    }

// e.o.n.c


	void NaradaAudioResource::copyData(ALCvoid* dat)
	{
		data = (ALCvoid*)malloc(size);
		memcpy((void*)data, (void*)dat, size);
	}


	void NaradaAudioResource::addData(ALCvoid* dat, ALsizei increaseSize, bool firstTime)
	{
        //std::cout<<"increasedSize: "<<increaseSize;
        //std::cout<<"data: "<<data;
		if(firstTime)
		{
			data = (ALCvoid*)malloc(increaseSize);
			size = 0;
		}
		else
			data = (ALCvoid*)realloc(data, (size + increaseSize));

		memcpy((void*)((unsigned char*)data + size), (void*)dat, increaseSize);
		size += increaseSize;

	}


	void NaradaAudioResource::bindListener()
	{
		alListenerfv(AL_POSITION,    ListenerPos);
		alListenerfv(AL_VELOCITY,    ListenerVel);
		alListenerfv(AL_ORIENTATION, ListenerOri);
	}



	void NaradaAudioResource::deleteSource()
	{
		if(alIsSource(alSource))
			alDeleteSources(1, &alSource);
	}


	void NaradaAudioResource::deleteData()
	{
		if(data)
			free(data);
	}


    void NaradaAudioResource::deleteDataHighPitch()
    {
        if(dataHighPitch)
            free(dataHighPitch);
        dataHighPitch=NULL;
    }


	void NaradaAudioResource::bindSourceAtt()
	{
		alSourcef (alSource, AL_PITCH,    1.0f     );
		alSourcef (alSource, AL_GAIN,     1.0f     );
		alSourcefv(alSource, AL_POSITION, SourcePos);
		alSourcefv(alSource, AL_VELOCITY, SourceVel);
		alSourcei (alSource, AL_LOOPING,  loop     );

	}
	


	ALboolean NaradaAudioResource::writeToWAV(char* file)
	{

		setFilename(file);



		//TAKE CARE: asume we are working on little-endian sistem
		FILE* fd;
		unsigned char buffer[44];
		int channels, bits;
		int auxInt;
		ALshort auxShort;

		switch(format)
		{
		case AL_FORMAT_MONO8:
			channels = 1;
			bits = 8;
			break;

		case AL_FORMAT_STEREO8:
			channels = 2;
			bits = 8;
			break;

		case AL_FORMAT_MONO16:
			channels = 1;
			bits = 16;
			break;

		case AL_FORMAT_STEREO16:
			channels = 2;
			bits = 16;
			break;
		default:
			std::cout << "unknown format" << std::endl;
		}



		fd = fopen(filename, "w");
		if(fd==NULL)
		{
			std::cout << "cannot open file: "<< filename << std::endl;
			exit(1);
		}



		//PREPARE WAV CANONICAL HEADER

		//create riff chunk descriptor
		buffer[0] = 'R';
		buffer[1] = 'I';
		buffer[2] = 'F';
		buffer[3] = 'F';

		auxInt = 36 + size;// * bits / 8; MODIFIED BY KAIK
		memcpy((void*)&buffer[4], (void*)&auxInt, 4);

		buffer[8] = 'W';
		buffer[9] = 'A';
		buffer[10] = 'V';
		buffer[11] = 'E';


		//create fmt sub-chunk
		buffer[12] = 'f';
		buffer[13] = 'm';
		buffer[14] = 't';
		buffer[15] = ' ';

		auxInt = 16;
		memcpy((void*)&buffer[16], (void*)&auxInt, 4);

		//audio format (always PCM = 1 -> 10 -from least signif. byte to most one)
		auxShort = 1;
		memcpy((void*)&buffer[20], (void*)&auxShort, 2);

		//number of channels
		auxShort = channels;
		memcpy((void*)&buffer[22], (void*)&auxShort, 2);

		//frequency sampling
		auxInt = freq;
		memcpy((void*)&buffer[24], (void*)&auxInt, 4);

		//bitrate
		auxInt = freq * channels * (bits / 8);
		memcpy((void*)&buffer[28], (void*)&auxInt, 4);

		//block align
		auxShort =channels * (bits/8);
		memcpy((void*)&buffer[32], (void*)&auxShort, 2);

		//bits per sample
		auxShort = bits;
		memcpy((void*)&buffer[34], (void*)&auxShort, 2);



		//create data sub-chunk

		buffer[36] = 'd';
		buffer[37] = 'a';
		buffer[38] = 't';
		buffer[39] = 'a';

		//data size
		auxInt = size;// * (bits/8); MODIFIED BY KAIK
		memcpy((void*)&buffer[40], (void*)&auxInt, 4);

		//write WAV header
		fwrite(buffer, 1, 44, fd);

		//write WAV data
		fwrite(data, 1, auxInt, fd);


		fclose(fd);

		return true;


	}


//rarreaza 2012-01-24 new writeTowav for high pitch

ALboolean NaradaAudioResource::writeToWAVHighPitch(char* file)
{
    
    setFilenameHighPitch(file);
    
    
    
    //TAKE CARE: asume we are working on little-endian sistem
    FILE* fd;
    unsigned char buffer[44];
    int channels, bits;
    int auxInt;
    ALshort auxShort;
    
    switch(format)
    {
		case AL_FORMAT_MONO8:
			channels = 1;
			bits = 8;
			break;
            
		case AL_FORMAT_STEREO8:
			channels = 2;
			bits = 8;
			break;
            
		case AL_FORMAT_MONO16:
			channels = 1;
			bits = 16;
			break;
            
		case AL_FORMAT_STEREO16:
			channels = 2;
			bits = 16;
			break;
		default:
			std::cout << "unknown format" << std::endl;
    }
    
    
    
    fd = fopen(filenameHighPitch, "w");
    if(fd==NULL)
    {
        std::cout << "cannot open fileHighPitch: "<< filenameHighPitch << std::endl;
        exit(1);
    }
    
    
    
    //PREPARE WAV CANONICAL HEADER
    
    //create riff chunk descriptor
    buffer[0] = 'R';
    buffer[1] = 'I';
    buffer[2] = 'F';
    buffer[3] = 'F';
    
    auxInt = 36 + sizeHighPitch;// * bits / 8; MODIFIED BY KAIK
    memcpy((void*)&buffer[4], (void*)&auxInt, 4);
    
    buffer[8] = 'W';
    buffer[9] = 'A';
    buffer[10] = 'V';
    buffer[11] = 'E';
    
    
    //create fmt sub-chunk
    buffer[12] = 'f';
    buffer[13] = 'm';
    buffer[14] = 't';
    buffer[15] = ' ';
    
    auxInt = 16;
    memcpy((void*)&buffer[16], (void*)&auxInt, 4);
    
    //audio format (always PCM = 1 -> 10 -from least signif. byte to most one)
    auxShort = 1;
    memcpy((void*)&buffer[20], (void*)&auxShort, 2);
    
    //number of channels
    auxShort = channels;
    memcpy((void*)&buffer[22], (void*)&auxShort, 2);
    
    //frequency sampling
    auxInt = freq;
    memcpy((void*)&buffer[24], (void*)&auxInt, 4);
    
    //bitrate
    auxInt = freq * channels * (bits / 8);
    memcpy((void*)&buffer[28], (void*)&auxInt, 4);
    
    //block align
    auxShort =channels * (bits/8);
    memcpy((void*)&buffer[32], (void*)&auxShort, 2);
    
    //bits per sample
    auxShort = bits;
    memcpy((void*)&buffer[34], (void*)&auxShort, 2);
    
    
    
    //create data sub-chunk
    
    buffer[36] = 'd';
    buffer[37] = 'a';
    buffer[38] = 't';
    buffer[39] = 'a';
    
    //data size
    auxInt = sizeHighPitch;// * (bits/8); MODIFIED BY KAIK
    memcpy((void*)&buffer[40], (void*)&auxInt, 4);
    
    //write WAV header
    fwrite(buffer, 1, 44, fd);
    
    //write WAV data
    fwrite(dataHighPitch, 1, auxInt, fd);
    
    
    fclose(fd);
    
    return true;
    
    
}



	void NaradaAudioResource::wavLoader()
	{
		//TAKE CARE: asume we are working on little-endian sistem
		FILE* fd;
		long bufferSize;
		unsigned char* buffer;
		int channels, bits;
		ALuint frequency;


		fd = fopen(filename, "r");
		if(fd==NULL)
		{
			std::cout << "cannot open file" << std::endl;
			exit(1);
		}


		fseek(fd, 0, SEEK_END);
		bufferSize = ftell(fd);
		std::rewind(fd);

		buffer = (unsigned char*)malloc(sizeof(unsigned char)*bufferSize);

		//pass through first sub-chunk
		fread(buffer, 1, 12, fd);

		//check if we are reading fmt sub-chunk or not
		fread(buffer, 1, 8, fd);
		if(buffer[0] != 'f' || buffer[1] != 'm' || buffer[2] != 't' || buffer[3] != ' ')
		{
			fprintf(stderr, "oops! Not 'fmt ' \n");
			fclose(fd);
			exit(1);
		}

		//check if WAV has PCM format (default)
		fread(buffer, 1, 2, fd);
		if(buffer[1] != 0 || buffer[0] != 1)
		{
			fprintf(stderr, "Not PCM \n");
			fclose(fd);
			exit(1);
		}

		//get channel count
		fread(buffer, 1, 2, fd);
		channels  = buffer[1]<<8;
		channels |= buffer[0];

		//get sample frequency
		fread(buffer, 1, 4, fd);
		frequency  = buffer[3]<<24;
		frequency |= buffer[2]<<16;
		frequency |= buffer[1]<<8;
		frequency |= buffer[0];

		//passing throught bytes per second and block size
		fread(buffer, 1, 6, fd);

		//get bit depth. 8 or 16
		fread(buffer, 1, 2, fd);
		bits = buffer[1]<<8;
		bits |= buffer[0];

		//define format
		setFormat(0);
		if(bits == 8)
		{
			if(channels == 1)
				setFormat(AL_FORMAT_MONO8);
			else if(channels == 2)
				setFormat(AL_FORMAT_STEREO8);
		}
		else if(bits == 16)
		{
			if(channels == 1)
				setFormat(AL_FORMAT_MONO16);
			else if(channels == 2)
				setFormat(AL_FORMAT_STEREO16);
		}
		else
		{
			fprintf(stderr, "Incompatible format (%d, %d) :(\n", channels, bits);
			fclose(fd);
			exit(1);
		}


		//check that next sub-chunk is data
		fread(buffer, 1, 8, fd);
		if(buffer[0] != 'd' || buffer[1] != 'a' || buffer[2] != 't' || buffer[3] != 'a')
		{
			//IF it's not, there are more unexpected chunks, we'll find directly the "data" tag.
			std::cout << "not data chunk. let's search directly the data tag" << std::endl;

			fseek(fd, -7, SEEK_CUR);
			bool isData = false;
			long counter = ftell(fd);
			while(!isData && counter < bufferSize)
			{
				fread(buffer, 1, 1, fd);
				if(buffer[0]=='d')
				{
					fread(buffer,1,3, fd);
					if(buffer[0]=='a' && buffer[1]=='t' && buffer[2]=='a')
						isData = true;
					else
						fseek(fd, -3, SEEK_CUR);
				}
				counter++;
			}
			if(isData)
				//pass through non-interesting bytes
				fread(buffer,1,4,fd);
			else
			{
				fclose(fd);
				fprintf(stderr, "Not 'data' \n");
				exit(1);
			}
		}


		setSize(fread(buffer, 1, bufferSize, fd));

		setFrequency(frequency);

		setData((ALCvoid*)buffer);


		fclose(fd);

	}




	ALboolean NaradaAudioResource::loadFromWAV(char* file)
	{

		filename = file;

		context->loadWAVonContext(this);

		context->bindAudioToBuffer(this);

		bindSourceAtt();

		bindListener();


		if (alGetError() == AL_NO_ERROR)
		   return AL_TRUE;

		std::cout << "Cannot load WAV file" << std::endl;
		deleteSource();

		return AL_FALSE;
	}




	ALboolean NaradaAudioResource::addFromCapture(ALCvoid* data, ALsizei size, ALenum format, ALsizei frequency, int currentPos)
	{
		bool firstTime = false;
		if(currentPos==0)
		{
			setFormat(format);
			setFrequency(frequency);
			firstTime = true;

		}
		switch (format) {
			case AL_FORMAT_MONO8:
				break;
			case AL_FORMAT_MONO16:
				size = 2*size;
				break;
			case AL_FORMAT_STEREO8:
				break;
			case AL_FORMAT_STEREO16:
				size = 2*size;
				break;
			default:
				break;
		}

		addData(data, size, firstTime);


		// Do another error check and return.
		if (alGetError() == AL_NO_ERROR)
			return AL_TRUE;

		std::cout << "Cannot add data" << std::endl;

		return AL_FALSE;
	}


	ALboolean NaradaAudioResource::endAddingFromCapture()
	{

		writeToWAV(filename);
        
        if (dataHighPitch!=NULL && sizeHighPitch!=0) {
            writeToWAVHighPitch(filenameHighPitch);
        }

		context = new NaradaAudioContext(false);

		 context->loadCaptureOnContext(this);

		if (alGetError() != AL_NO_ERROR)
			std::cout << "Error capturing on context" << std::endl;

		 context->bindAudioToBuffer(this);

		if (alGetError() != AL_NO_ERROR)
			std::cout << "Error binding audio to buffer" << std::endl;

		bindSourceAtt();

		if (alGetError() != AL_NO_ERROR)
			std::cout << "Error binding sources" << std::endl;

		bindListener();

		if (alGetError() != AL_NO_ERROR)
			std::cout << "Error binding listener" << std::endl;

		 // Do another error check and return.
		 if (alGetError() == AL_NO_ERROR)
		 return AL_TRUE;

		 std::cout << "Error ending capture" << std::endl;
		 deleteSource();

		 return AL_FALSE;

	}





	//ALboolean NaradaAudioResource::loadFromCapture(AudioCapture* capture)
//	{
//
//	setSize(capture->getSize());
//
//	//setData(capture->getData());
//	copyData(capture->getData());
//
//	setFormat(capture->getFormat());
//
//	setFrequency(capture->getFrequency());
//
//	std::cout << "val. of size: " << size << std::endl;
//
//	context->loadCaptureOnContext(this);
//
//	context->bindAudioToBuffer(this);
//
//	bindSourceAtt();
//
//	bindListener();
//
//
//	// Do another error check and return.
//	if (alGetError() == AL_NO_ERROR)
//	   return AL_TRUE;
//
//	std::cout << "Cannot load WAV file" << std::endl;
//	deleteSource();
//
//	return AL_FALSE;
//	}
	
	
	
	bool NaradaAudioResource::isPlaying()
	{
		ALint play;
		alGetSourcei(alSource, AL_SOURCE_STATE, &play);

		if (play != AL_PLAYING)
			return false;
		else
			return true;
	}
	
	
	void NaradaAudioResource::play()
	{
		alSourcePlay(alSource);
	}
	
	
	void NaradaAudioResource::stop()
	{
		alSourceStop(alSource);
	}
	
	
	void NaradaAudioResource::rewind()
	{
		alSourceRewind(alSource);
	}

//rarreaza 2012-01-19 added in order to implement the new pitch changing implementation

void NaradaAudioResource::lowerStoredNumSamplesBy( int factor )
{
    int step=factor;
    //code to get new data with less samples to be played at same frequency
    if ( (bitsPerSample % 8*sizeof(unsigned char) )!=0 ) {//check if value is valid
        std::cout<<"ERROR: number of bits per channel provided is not a multiple of 8 (unsigned char size)";
        return;
    }
    //if valid, get it, and it should be integer and correspond to the number of chars per sample
    int nCharsPerSample=bitsPerSample/8*sizeof(unsigned char);
    
    if ( (size % nCharsPerSample)!=0 ) {//check if value is valid
        std::cout<<"ERROR: size in chars per channel provided is not a multiple of nCharsPerSAmple ";
        return;
    }

    int nSamples=size/nCharsPerSample;//total number of samples in data buffer
    
    ALsizei newSize=0;
    ALvoid * newData=NULL;
    bool firstTime=true;
    for (int i=0; i<nSamples; i+=step) {//now copy every "step" samples FROM data to variable newData, and keep increasing newSize along with it
		if(firstTime)
		{
			newData = (ALCvoid*)malloc(nCharsPerSample);
			newSize = 0;
            firstTime=false;
		}
		else{
			newData = (ALCvoid*)realloc(newData, (newSize + nCharsPerSample));
        }
        
		memcpy((void*)((unsigned char*)newData + newSize), (void*)((unsigned char*)data + i*nCharsPerSample), nCharsPerSample);
		newSize += nCharsPerSample;
    }
    
    //now that we have store in newData and newSize the re-sampled values, we can then store them into the global variables data and size respectively, or just assign them to new ones
//    ALCvoid* auxData=data;//aux pointer not to create a memory leak
//    data=newData;
//    size=newSize;
//    free(auxData); //free previous data memory allocation.
//    auxData=NULL;
    dataHighPitch=newData;
    sizeHighPitch=newSize;
    
}

int NaradaAudioResource::getBitsPerSample()
{
    return bitsPerSample;
}

void NaradaAudioResource::setBitsPerSample(int bitsPS)
{
    bitsPerSample=bitsPS;
}


