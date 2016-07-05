//
//  NaradaAudioRecorder.h
//  NaradaAVTools
//
//  Created by Enrique Oriol on 05/11/10.
//  Copyright 2010 Narada Robotics. All rights reserved.
//




#ifndef __NARADAAUDIORECORDER_HEADER_FILE
#define __NARADAAUDIORECORDER_HEADER_FILE



#include <AudioToolbox/AudioToolbox.h>
#include "NaradaAudioResource.hpp"


#import <iostream>


#include <OpenAL/al.h>
#include <OpenAL/alc.h>



#define kNumberRecordBuffers    3
#define kSamplingRate	16000
#define kNumberOfChannels	1
#define kBitsPerChannel	16
#define kMaxSamplesPerAudioPermited 160000//10 seconds





static void handleInputBuffer ( void *audioData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer,
						const AudioTimeStamp *inStartTime, UInt32 inNumPackets, 
						const AudioStreamPacketDescription *inPacketDesc);


class NaradaAudioResource;


class NaradaAudioRecorder 
{
	
public:

	
	NaradaAudioRecorder(char* filename= NULL, int samplingRate=kSamplingRate, int nChannels=kNumberOfChannels, 
						int bitsPerChannel=kBitsPerChannel, int maxSamples=kMaxSamplesPerAudioPermited, char* filenameHighPitch= NULL);
	
	
	~NaradaAudioRecorder();
	

	
	void	startRecord();
	
	void	stopRecord();    

	

	bool hasExceededMaxSize();	
	
	AudioStreamBasicDescription getDataFormat()
	{
		return mDataFormat;
	}
	
	SInt64 getCurrentPacket()
	{
		return mCurrentPacket;
	}
	
	bool isRunning()
	{
		return mIsRunning;
	}
	
	void stopRunning()
	{
		mIsRunning = false;
	}
	
	
	void addCurrentPacket(UInt32 sum)
	{
		mCurrentPacket += sum;
	}
	
	
	NaradaAudioResource* getResource()
	{
		return resource;
	}
    
    //rarreaza 2012-01-23 changes to discern if hight pitch voice should be done
    void setDoHigherPitch(bool shouldDo)
    {
        shouldDoHighPitch=shouldDo;
    }
    
    bool getDoHigherPitch()
    {
        return shouldDoHighPitch;
    }
	
	
private:
		
	AudioStreamBasicDescription mDataFormat;
	
	AudioQueueRef           mQueue;
	
	AudioQueueBufferRef     mBuffers[kNumberRecordBuffers];
	
	SInt64                  mCurrentPacket; // current packet number in record file
	
	UInt32					mBufferByteSize;
	
	bool                    mIsRunning;
	
	float					mBufferedTime;
	
	int						maxSamplesPerAudioPermitted;
	
	NaradaAudioResource*	resource;
    
    bool                    shouldDoHighPitch;
	
	
	
	void setup(int sRate, int nChannels, int bitsPerChannel, int maxSamples);


};

#endif

