//
//  NaradaAudioConverter.h
//  NaradaAVTools
//
//  Created by Corbella on 27/01/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <AudioToolbox/AudioToolbox.h>

#include <AVFoundation/AVFoundation.h>

#include "NaradaAudioConverterDelegate.h"


@interface NaradaAudioConverter : NSObject {
    
    
    id <NaradaAudioConverterDelegate> _delegate;

	NSString* _sourcePath;
    NSString* _destinationPath;
	
	UInt64 _sampleRate;
    
    int convertID;
	
}


/**
 * This is the main initializer for the NaradaAVMixer class.
 * It requires the path for the audio source file (in wav format) and the desired path for the output in AAC
 * It is also required the sample rate for the output. 
 * Having sampleRate to 0 takes the same sample rate for the output as it is at input
 */
- (id)initWithSourcePath:(NSString *)theSourcePath 
	  andDestinationPath:(NSString *)theDestinationPath 
		   andSampleRate:(UInt32)theSampleRate 
             andDelegate:(id<NaradaAudioConverterDelegate>) delegate;

/**
 * This is the same as previous, but adding an ID.
 * this ID is useful for retrieving various converters at a time
 */
- (id)initWithID:(int)convertID andSourcePath:(NSString *)theSourcePath
andDestinationPath:(NSString *)theDestinationPath
   andSampleRate:(UInt32)theSampleRate
     andDelegate:(id<NaradaAudioConverterDelegate>)delegate;

/**
 * Converts a Wav file to AAC compressed format.
 * the source of the file to convert must be stored in the sourceURL 
 */
- (void)convertWavtoAAC;

/**
 * Converts a MP3 file to PCM uncompressed format.
 * the source of the file to convert must be stored in the sourceURL 
 */
- (void)convertMP3toPCM;

/**
 * Same as last one, but it does the conversion using AVFoundation
 */
- (void)convertMP3toPCMWithAV;

/**
 * This method removes the noise at the beginning of the pcm audio, by assigning the first samples to 0x0
 */
- (NSMutableData *)removeNoise:(NSMutableData *)inData;

//Delegate for the callback to catch once the conversion is done
@property (nonatomic, retain) id delegate;


// This is the source path at which the .wav file is to be taken
@property (nonatomic, copy) NSString* sourcePath;

//This is the destination path at which the output .aac is to be stored
@property (nonatomic, copy) NSString* destinationPath;

//This is the desired output sample rate of the .aac file. Setting this at 0.0 before conversion makes it equal to that of the source file
@property (nonatomic, assign) UInt64 sampleRate;

@property (nonatomic, assign) int convertID;


@end
