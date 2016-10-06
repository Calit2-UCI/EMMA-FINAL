//
//  NaradaAudioRecorder.mm
//  NaradaAVTools
//
//  Created by Enrique Oriol on 05/11/10.
//  Copyright 2010 Narada Robotics. All rights reserved.
//

#import "NaradaAudioRecorder.h"


// AudioQueue callback function, called when an input buffers has been filled.
void handleInputBuffer ( void *audioData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer,
						const AudioTimeStamp *inStartTime, UInt32 inNumPackets, const AudioStreamPacketDescription *inPacketDesc)
{
	
	NaradaAudioRecorder *aRec = (NaradaAudioRecorder *)audioData;
	
	if (inNumPackets == 0 && aRec->getDataFormat().mBytesPerPacket != 0)
		inNumPackets = inBuffer->mAudioDataByteSize / aRec->getDataFormat().mBytesPerPacket;
	
	std::cout << "inumPackets: " << inNumPackets << " audiodatabytesize: " << inBuffer->mAudioDataByteSize  << std::endl;
	if(inNumPackets!=0)
	{
		
		if(aRec->getResource()->addFromCapture(inBuffer->mAudioData, inNumPackets, AL_FORMAT_MONO16, aRec->getDataFormat().mSampleRate, aRec->getCurrentPacket()))
			aRec->addCurrentPacket(inNumPackets);
		
	}
	
	if(aRec->hasExceededMaxSize())
		aRec->stopRunning();
	
	if (aRec->isRunning() == 0)
		return;
	
	
	AudioQueueEnqueueBuffer (inAQ,inBuffer, 0, NULL);		 
	

	
}





NaradaAudioRecorder::NaradaAudioRecorder(char* filename, int samplingRate, int nChannels, int bitsPerChannel, int maxSamples, char* filenameHighPitch)
{
    shouldDoHighPitch=false;
	resource = new NaradaAudioResource();
	if(filename)
		resource->setFilename(filename);
    
    if(filenameHighPitch)
		resource->setFilenameHighPitch(filenameHighPitch);
    //rarreaza 2012-01-20 pass the bitsperchannel to the bitsperSample field of NaradaResource, so it can operate on the data recieved at the "sample" level
    if (bitsPerChannel) {
        resource->setBitsPerSample(bitsPerChannel);
    }

	setup(samplingRate, nChannels, bitsPerChannel, maxSamples);

}




NaradaAudioRecorder::~NaradaAudioRecorder()
{
    AudioQueueDispose(mQueue, TRUE);
	if(resource)
		delete resource;
	resource = NULL;
}




void NaradaAudioRecorder::setup(int sRate, int nChannels, int bitsPerChannel, int maxSamples)
{
	mBufferedTime = 0.2;
    mIsRunning = false;
    mCurrentPacket = 0;
	
	maxSamplesPerAudioPermitted = maxSamples;
	
	//set up data format
	mDataFormat.mFormatID 	= kAudioFormatLinearPCM;
	mDataFormat.mSampleRate = sRate;
	mDataFormat.mChannelsPerFrame = nChannels;
	mDataFormat.mBitsPerChannel = bitsPerChannel;
	mDataFormat.mFramesPerPacket = 1;
	mDataFormat.mBytesPerPacket = mDataFormat.mBytesPerFrame = mDataFormat.mChannelsPerFrame * mDataFormat.mBitsPerChannel/8; 
	mDataFormat.mReserved = 0;
	
	mDataFormat.mFormatFlags = kLinearPCMFormatFlagIsPacked | kLinearPCMFormatFlagIsSignedInteger;
	
	//set audio buffers size
	mBufferByteSize = mDataFormat.mSampleRate * (mDataFormat.mBitsPerChannel / 8) * mBufferedTime;
}



bool NaradaAudioRecorder::hasExceededMaxSize()
{
	if(mCurrentPacket >= maxSamplesPerAudioPermitted)
		return true;
	else
		return false;
}




void NaradaAudioRecorder::startRecord()
{
	//set up audio queue
	OSStatus status = AudioQueueNewInput(&mDataFormat, handleInputBuffer, this, NULL, NULL, 0, &mQueue);
	//OSStatus status = AudioQueueNewInput(&mDataFormat, handleInputBuffer, this, CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &mQueue);
	if(status != 0)
		std::cout  << "cannot create mQueue" << std::endl; 
	if(status == 0)
	{
		std::cout  << "mQueue has been created" << std::endl; 
		//prepare audio queue buffers
		for(int i=0; i < kNumberRecordBuffers; i++)
		{
			status = AudioQueueAllocateBuffer(mQueue, mBufferByteSize, &mBuffers[i]);
			if(status != 0)
				std::cout  << "cannot allocate audio queue buffer" << std::endl; 

			status = AudioQueueEnqueueBuffer(mQueue, mBuffers[i], 0, NULL);	
			if(status != 0)
				std::cout  << "cannot enqueue audio queue buffer" << std::endl; 
			
		}
	
		mCurrentPacket = 0;
		mIsRunning = true;
		
		std::cout  << "going to start" << std::endl; 
		status = AudioQueueStart(mQueue, NULL);
	}
	if(status != 0)
		std::cout  << "cannot start recording, err. status= " << status << std::endl; 
	else
		std::cout  << "start recording" << std::endl; 
}


void NaradaAudioRecorder::stopRecord()
{
    // end recording
    mIsRunning = false;
    AudioQueueStop(mQueue, true);
	
	for(int i=0; i < kNumberRecordBuffers; i++)
	{
		AudioQueueFreeBuffer(mQueue, mBuffers[i]);
	}
	
	if(mCurrentPacket!=0){
        
//        if (shouldDoHighPitch) { //commented in order for high pitch to always be done, and user after decides if he wants to reproduce it in this pitch or the normal recording
//            resource->lowerStoredNumSamplesBy(2);// rarreaza 2012-01-20 to incorporate pitch shift
//        }
        
		resource->endAddingFromCapture();
    }

}

