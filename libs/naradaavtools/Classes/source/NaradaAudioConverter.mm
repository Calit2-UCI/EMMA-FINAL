//
//  NaradaAudioConverter.mm
//  NaradaAVTools
//
//  Created by Jordi Serra on 27/01/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "NaradaAudioConverter.h"

#import "NaradaAudioResource.hpp"

#import <UIKit/UIDevice.h>

#import <iostream>
using namespace std;

extern OSStatus DoConvertFile(CFURLRef sourceURL, CFURLRef destinationURL, OSType outputFormat, Float64 outputSampleRate);

@implementation NaradaAudioConverter

@synthesize convertID, sourcePath, destinationPath, sampleRate, delegate = _delegate;

- (id)initWithSourcePath:(NSString *)theSourcePath 
	  andDestinationPath:(NSString *)theDestinationPath 
		   andSampleRate:(UInt32)theSampleRate 
             andDelegate:(id<NaradaAudioConverterDelegate>)delegate
{
	if (self = [super init]) {
		self.sourcePath = [NSString stringWithString:theSourcePath];
		self.destinationPath = [NSString stringWithString:theDestinationPath];
		self.sampleRate = theSampleRate;
        self.delegate = delegate;
	}
	return self;
}

- (id)initWithID:(int)convertID andSourcePath:(NSString *)theSourcePath
	  andDestinationPath:(NSString *)theDestinationPath
		   andSampleRate:(UInt32)theSampleRate
             andDelegate:(id<NaradaAudioConverterDelegate>)delegate
{
    self = [self initWithSourcePath:theSourcePath andDestinationPath:theDestinationPath andSampleRate:theSampleRate andDelegate:delegate];
    if (self) {
        self.convertID = convertID;
    }
    return self;
}


- (void)setAudio 
{    
	// Initialize and configure the audio session
	AudioSessionInitialize(NULL, NULL, NULL, self);
	
	// our default category -- we change this for conversion and playback appropriately
	UInt32 audioCategory = kAudioSessionCategory_PlayAndRecord;
	AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(audioCategory), &audioCategory);
	
	//This is set force hearing the sound in the loud speakers
	UInt32 speakers = 1;
	AudioSessionSetProperty(kAudioSessionProperty_OverrideCategoryDefaultToSpeaker, sizeof(speakers), &speakers);
	
	// the session must be active for offline conversion including after an  an audio interruption
	AudioSessionSetActive(true);
	
}

- (void)unsetAudio
{
	AudioSessionSetActive(false);
	AudioSessionRemovePropertyListenerWithUserData(kAudioSessionProperty_AudioRouteChange, NULL, self);
}

- (void)convertWavtoAAC
{	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	[self setAudio];
	
	CFURLRef sourceURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)self.sourcePath, kCFURLPOSIXPathStyle, false);
	CFURLRef destinationURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)self.destinationPath, kCFURLPOSIXPathStyle, false);
	
    OSStatus error = DoConvertFile(sourceURL, destinationURL, kAudioFormatMPEG4AAC, sampleRate);
	
	NSData *fileData = [[[[NSFileManager alloc] init] autorelease] contentsAtPath:self.destinationPath];
	cout << "Audio Converted actual Size: " << [fileData length] << endl;
    
    if (error) {
        // delete output file if it exists since an error was returned during the conversion process
        if ([[NSFileManager defaultManager] fileExistsAtPath:self.destinationPath]) {
            [[NSFileManager defaultManager] removeItemAtPath:self.destinationPath error:nil];
        }
        printf("DoConvertFile failed! %ld\n", error);
	}
	
	[self unsetAudio];
	AudioSessionSetActive(NO);
	printf("AudioSession deactivated\n");
	
    [pool release];
}

- (void)convertMP3toPCM
{	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	[self setAudio];
	
	CFURLRef sourceURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)self.sourcePath, kCFURLPOSIXPathStyle, false);
	CFURLRef destinationURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)self.destinationPath, kCFURLPOSIXPathStyle, false);
	
    OSStatus error = DoConvertFile(sourceURL, destinationURL, kAudioFormatLinearPCM, sampleRate);
	
	NSData *fileData = [[[[NSFileManager alloc] init] autorelease] contentsAtPath:self.destinationPath];
	cout << "Audio Converted actual Size: " << [fileData length] << endl;
    
    if (error) {
        // delete output file if it exists since an error was returned during the conversion process
        if ([[NSFileManager defaultManager] fileExistsAtPath:self.destinationPath]) {
            [[NSFileManager defaultManager] removeItemAtPath:self.destinationPath error:nil];
        }
        printf("DoConvertFile failed! %ld\n", error);
	}
	
	[self unsetAudio];
	AudioSessionSetActive(NO);
	printf("AudioSession deactivated\n");
	
    [pool release];
    
    
}

- (void)convertMP3toPCMWithAV
{

    //Extracted from http://www.subfurther.com/blog/2010/12/13/from-ipod-library-to-pcm-samples-in-far-fewer-steps-than-were-previously-necessary/
    
    cout << "Path is: " << [sourcePath UTF8String] << endl;
    NSURL *assetURL = [NSURL fileURLWithPath:sourcePath];
    NSData *dat = [NSData dataWithContentsOfURL:assetURL];
    NSLog(@"Data length: %d", [dat length]);
    
    AVURLAsset *asset = [AVURLAsset URLAssetWithURL:assetURL options:nil];
    
    if (asset == nil) {
        cerr << "Not created asset. Ending app";
        exit(197);
    }
    
	NSError *assetError;
    AVAssetReader *assetReader = [[AVAssetReader alloc] initWithAsset:asset error:&assetError];
	if (assetError) {
		NSLog (@"asset reader error: %@", assetError);
        NSArray *args = [NSArray arrayWithObjects:self, assetError, nil];
        [self.delegate performSelectorOnMainThread:@selector(convertAudio:didFailWithError:) withObject:args waitUntilDone:NO];
		return;
	}
    
    
    AVAssetReaderOutput *assetReaderOutput =
	[[AVAssetReaderAudioMixOutput assetReaderAudioMixOutputWithAudioTracks:asset.tracks
                                                             audioSettings: nil] 
     retain];
    
    if (! [assetReader canAddOutput: assetReaderOutput]) {
        NSLog (@"can't add reader output... die!");
        return;
    }
    [assetReader addOutput: assetReaderOutput];
    
    
    if ([[NSFileManager defaultManager] fileExistsAtPath:destinationPath]) {
        [[NSFileManager defaultManager] removeItemAtPath:destinationPath
                                                   error:nil];
    }
    NSURL *exportURL = [NSURL fileURLWithPath:destinationPath];
    
    AVAssetWriter *assetWriter = [[AVAssetWriter assetWriterWithURL:exportURL 
                                                           fileType:AVFileTypeCoreAudioFormat 
                                                              error:&assetError] 
                                  retain];
    if (assetError) {
        NSLog (@"asset writer error: %@", assetError);
        NSArray *args = [NSArray arrayWithObjects:self, assetError, nil];
        [self.delegate performSelectorOnMainThread:@selector(convertAudio:didFailWithError:) withObject:args waitUntilDone:NO];
        return;
    }
    
    AudioChannelLayout channelLayout;
    memset(&channelLayout, 0, sizeof(AudioChannelLayout));
    channelLayout.mChannelLayoutTag = kAudioChannelLayoutTag_Mono;
    
    NSDictionary *outputSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithInt:kAudioFormatLinearPCM], AVFormatIDKey,
                                    [NSNumber numberWithFloat:16000.0], AVSampleRateKey,    
                                    [NSNumber numberWithInt:1], AVNumberOfChannelsKey, 
                                    [NSData dataWithBytes:&channelLayout length:sizeof(AudioChannelLayout)],AVChannelLayoutKey, 
                                    [NSNumber numberWithInt:16], AVLinearPCMBitDepthKey, 
                                    [NSNumber numberWithBool:NO], AVLinearPCMIsNonInterleaved, 
                                    [NSNumber numberWithBool:NO],AVLinearPCMIsFloatKey, 
                                    [NSNumber numberWithBool:NO], AVLinearPCMIsBigEndianKey, 
                                    nil];
    
    AVAssetWriterInput *assetWriterInput = [[AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeAudio outputSettings:outputSettings] retain];
    
    if ([assetWriter canAddInput:assetWriterInput]) {
        [assetWriter addInput:assetWriterInput];
    } else {
        NSLog (@"can't add asset writer input... die!");
        return;
    }
    assetWriterInput.expectsMediaDataInRealTime = NO;
    
    [assetWriter startWriting];
    [assetReader startReading];
    AVAssetTrack *soundTrack = [asset.tracks objectAtIndex:0];
    CMTime startTime = CMTimeMake (0, soundTrack.naturalTimeScale);
    [assetWriter startSessionAtSourceTime: startTime];
    
    NSLog(@"Time Range is %lld", assetReader.timeRange.duration.value);
    
    
    __block UInt64 convertedByteCount = 0;
    dispatch_queue_t mediaInputQueue = dispatch_queue_create("mediaInputQueue", NULL);
    [assetWriterInput requestMediaDataWhenReadyOnQueue:mediaInputQueue usingBlock: ^
     {
         int counter = 0;
         while (assetWriterInput.readyForMoreMediaData) {
             CMSampleBufferRef nextBuffer =
             [assetReaderOutput copyNextSampleBuffer];
             if (nextBuffer) {
                 // append buffer
                 std::cout << counter << "|";
                 counter ++;
                 [assetWriterInput appendSampleBuffer: nextBuffer];
                 CFRelease(nextBuffer);
                 
             } else {
                 switch (assetReader.status) {
                     case AVAssetReaderStatusCompleted:
                         NSLog(@"Status is completed!");
                         break;
                     case AVAssetReaderStatusUnknown:
                         NSLog(@"Status is Unknown");
                         break;
                     case AVAssetReaderStatusReading:
                         NSLog(@"Status is Reading!");
                         break;
                     case AVAssetReaderStatusFailed:
                         NSLog(@"Status is Failed!");
                         break;
                     case AVAssetReaderStatusCancelled:
                         NSLog(@"Status is cancelled!");
                         break;
                     default:
                         NSLog(@"It should not be entering here!");
                         break;
                 }
                 
                 
                 // done!
                 std::cout << std::endl;
                 [assetWriterInput markAsFinished];
                 
                 NSString *version = [[UIDevice currentDevice] systemVersion];
                 if ([version intValue] < 6.0) {
                     [assetWriter finishWriting];
                 }
                 else {
                     [assetWriter finishWritingWithCompletionHandler:^{
                         NSLog(@"Finished Writing AVAssetWriter");
                     }];
                 }
                 
                 dispatch_release(mediaInputQueue);
                 
                 [assetReader cancelReading];
                 
                 NSError *outputFileAttrsError = nil;
                 NSDictionary *outputFileAttributes = 
                 [[NSFileManager defaultManager] attributesOfItemAtPath:destinationPath error:&outputFileAttrsError];
                 
                 NSAssert2(outputFileAttributes != nil, @"Error getting the file at path %@, error: $@", destinationPath, [outputFileAttrsError localizedDescription]);
                 
                 NSLog (@"done. file size is %llu", [outputFileAttributes fileSize]);
                 
                 if ([outputFileAttributes fileSize] == 0) {
                     NSMutableDictionary *details = [NSMutableDictionary dictionary];
                     [details setValue:@"Audio output is 0" forKey:NSLocalizedDescriptionKey];
                     NSError *fileZeroError = [[NSError alloc] initWithDomain:@"com.naradarobotics.converterErrorDomain" code:-758 userInfo:details];
//                     NSArray *args = [NSArray arrayWithObjects:self, fileZeroError, nil];
                     
                     dispatch_async(dispatch_get_main_queue(), ^{
                         [self.delegate convertAudio:self didFailWithError:fileZeroError];
                     });
                     
//                     [self.delegate performSelectorOnMainThread:@selector(convertAudio:didFailWithError:) withObject:args waitUntilDone:YES];
                     break;
                 }
                 
//                 NSAssert([outputFileAttributes fileSize] > 0, @"Error converting from mp3 to pcm. File size is 0!");
//                 NSNumber *doneFileSize = [NSNumber numberWithLong:
//                                           [outputFileAttributes fileSize]];
//                 [self performSelectorOnMainThread:@selector(updateCompletedSizeLabel:)
//                                        withObject:doneFileSize
//                                     waitUntilDone:NO];
                 
                 NSMutableData *pcmData = [NSMutableData dataWithContentsOfFile:self.destinationPath];
                 
                 NSAssert([pcmData length] > 0, @"Length of converted data is 0!");
                 
                 pcmData = [self removeNoise:pcmData];
//                 pcmData = [self lenthenAudio:pcmData];
                 [pcmData writeToFile:self.destinationPath atomically:YES];
                 
                 
//                 dispatch_async(dispatch_get_main_queue(), ^{
//                     [self.delegate convertAudioDidFinish:self];
//                 });
                 
                 [self.delegate performSelectorOnMainThread:@selector(convertAudioDidFinish:) 
                                                 withObject:self
                                              waitUntilDone:YES];

                 
                 // release a lot of stuff
                 [assetReader release];
                 [assetReaderOutput release];
                 [assetWriter release];
                 [assetWriterInput release];
                 [destinationPath release];
                 
                 
                 break;
             }
             
         }
         return;
	 }];
	NSLog (@"bottom of convertTapped:");
    
}

- (NSMutableData *)removeNoise:(NSMutableData *)inData
{
    int sampleFormat = 2; //bytes per sample
    int sampleRate = 16000; //samples per second
    float nSeconds = 0.14;
    
    int length = int(nSeconds *sampleRate * sampleFormat);
    Byte *buffer = (Byte *)inData.mutableBytes;
    
    for (int i=0; i<length; i++) {
        buffer[i] = 0x0;
    }
    
    return [NSMutableData dataWithBytes:buffer length:[inData length]];
}

- (NSMutableData *)lenthenAudio:(NSMutableData *)inData
{
    int sampleFormat = 2; //bytes per sample
    int rate = 16000; //samples per second
    float nSeconds = 200;
    
    int length = int(nSeconds *rate * sampleFormat);
    Byte *newBytes = new Byte[length];
    for (int i=0; i<length; i++) {
        newBytes[i] = 0x0;
    }
//    NSData *newData = [NSData dataWithBytes:newBytes length:length *sizeof(Byte)];
    
    [inData appendBytes:newBytes length:length];
    
    return inData;
    
//    
//    Byte *buffer = (Byte *)inData.mutableBytes;
//    
//    int oldLength = inData.length;
//    int newLength = (oldLength + length);
//    
//    realloc(buffer, newLength * sizeof(Byte));
//    
//    for (int i=oldLength; i<newLength; i++) {
//        buffer[i] = 0x0;
//    }
//    
//    return [NSMutableData dataWithBytes:buffer length:[inData length]];
}

//- (void)addHeaderToWAVFromSourceFile:(NSString *)pcmSourceFile toDestinationFile:(NSString *)wavDestinationFile
//{
//    NSFileManager *fm = [NSFileManager defaultManager];
//    NSData *data = [fm contentsAtPath:pcmSourceFile];
//    const void *bytes = [data bytes];
//    NSUInteger dataSize = [data length];
//    
//    AudioResource
//}



- (void)dealloc
{
	[self.sourcePath release];
//	[self.destinationPath release];
	[super dealloc];
}

@end
