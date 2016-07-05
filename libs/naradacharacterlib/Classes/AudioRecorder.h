//
//  untitled.h
//  iPhoneNaradaDemo
//
//  Created by Corbella on 05/11/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//




#ifndef __AUDIORECORDER_HEADER_FILE
#define __AUDIORECORDER_HEADER_FILE



#include <AudioToolbox/AudioToolbox.h>
#include "AudioResource.hpp"

#import <iostream>

//OpenAL libraries
#if defined(MAC) || defined(IPHONE)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif


#define kNumberRecordBuffers    3
#define kSamplingRate	16000
#define kNumberOfChannels	1
#define kBitsPerChannel	16
#define kMaxSamplesPerAudioPermited 160000//10 seconds


using namespace VisageSDK;


static void handleInputBuffer ( void *audioData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer,
						const AudioTimeStamp *inStartTime, UInt32 inNumPackets, 
						const AudioStreamPacketDescription *inPacketDesc);


class AudioRecorder 
{
	
public:
	AudioRecorder(AudioResource *res);
	
	~AudioRecorder();
	

	
	void            startRecord();
	
	void            stopRecord();       

	
	UInt64          startTime;
	

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
	
	
	AudioResource* getResource()
	{
		return resource;
	}
	
	
private:
		
	AudioStreamBasicDescription mDataFormat;
	
	AudioQueueRef               mQueue;
	
	AudioQueueBufferRef         mBuffers[kNumberRecordBuffers];
	
	SInt64                      mCurrentPacket; // current packet number in record file
	
	UInt32						mBufferByteSize;
	
	bool                     mIsRunning;
	
	float						mBufferedTime;
	
	AudioResource*				resource;
	
	


};

#endif

