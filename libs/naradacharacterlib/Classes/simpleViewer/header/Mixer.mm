//
//  Mixer.m
//  AviCuleNarada
//
//  Created by Corbella on 30/11/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "Mixer.h"

#import <AVFoundation/AVFoundation.h>
#import <CoreFoundation/CoreFoundation.h>

#import <AudioToolbox/AudioToolbox.h>

#import <iostream.h>

using namespace std;

extern OSStatus DoConvertFile(CFURLRef sourceURL, CFURLRef destinationURL, OSType outputFormat, Float64 outputSampleRate);

@implementation Mixer

@synthesize renderer;
@synthesize destinationFilePath;


#pragma mark Conversion to AAC

- (void)convertWavtoAAC
{
	NSArray  *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
	
	// create the URLs we'll use for source and destination
    //NSString *source = [[NSBundle mainBundle] pathForResource:@"test" ofType:@"wav"];
	NSString *source = [documentsDirectory stringByAppendingPathComponent:@"test.wav"];
	sourceURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)source, kCFURLPOSIXPathStyle, false);
	
	destinationFilePath = [documentsDirectory stringByAppendingPathComponent:@"output.aac"];    
	
	NSFileManager *fm = [[NSFileManager alloc] init];
	if ([fm fileExistsAtPath:destinationFilePath]) {
		[fm removeItemAtPath:destinationFilePath error:nil];
		cout << "Removing destination file path" << endl;
	}
	
	destinationURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)destinationFilePath, kCFURLPOSIXPathStyle, false);
	
	sampleRate = 0.0;
	
	NSData *audioData = [fm contentsAtPath:destinationFilePath];
	cout << "AudioDataSize: " << [audioData length] << endl;
	
	cout << "Sample Rate: " << sampleRate << endl;
	cout << "Source Path: " << [source cStringUsingEncoding:1] << endl;
	cout << "Destination Path: " << [destinationFilePath cStringUsingEncoding:1] << endl;
	
	[self setAudio];
	[self convertAudio];
	//[self unsetAudio];
	
	fm = [[NSFileManager alloc] init];
	
	audioData = [fm contentsAtPath:destinationFilePath];
	
	cout << "AudioDataSize: " << [audioData length] << endl;
	[fm release];

}

- (void)convertAudio
{    
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
    OSStatus error = DoConvertFile(sourceURL, destinationURL, kAudioFormatMPEG4AAC, sampleRate);
	
	NSData *fileData = [[[[NSFileManager alloc] init] autorelease] contentsAtPath:destinationFilePath];
	cout << "Audio Converted actual Size: " << [fileData length] << endl;
    
    if (error) {
        // delete output file if it exists since an error was returned during the conversion process
        if ([[NSFileManager defaultManager] fileExistsAtPath:destinationFilePath]) {
            [[NSFileManager defaultManager] removeItemAtPath:destinationFilePath error:nil];
        }
        
        printf("DoConvertFile failed! %ld\n", error);
	}
	
	AudioSessionSetActive(NO);
	printf("AudioSession deactivated\n");
    
    [pool release];
}



- (void)setAudio 
{    
	// Initialize and configure the audio session
	AudioSessionInitialize(NULL, NULL, NULL, self);
		
	// our default category -- we change this for conversion and playback appropriately
	//UInt32 audioCategory = kAudioSessionCategory_SoloAmbientSound;
	UInt32 audioCategory = kAudioSessionCategory_PlayAndRecord;
	AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(audioCategory), &audioCategory);
	
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

#pragma mark Audio-Video Mixer

- (void)mixAudioVideoWithController:(UIViewController *)controller
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	NSError *error;
	
	NSString *mainPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
	
	NSString *audioPath = [mainPath stringByAppendingPathComponent:@"output.aac"];
	NSString *videoPath = [mainPath stringByAppendingPathComponent:@"TestVideo.mp4"];
	
	NSURL *audioURL = [[NSURL alloc] initFileURLWithPath:audioPath];
	NSURL *videoURL = [[NSURL alloc] initFileURLWithPath:videoPath];
	
	//This is a class to represent timed AudioVisual media through URL
	AVURLAsset *audioAsset = [[AVURLAsset alloc ] initWithURL:audioURL options:nil];
	AVURLAsset *videoAsset = [[AVURLAsset alloc ] initWithURL:videoURL options:nil];
	
	NSParameterAssert(audioAsset);
	NSParameterAssert(videoAsset);
	
	//This class lets add and remove tracks (assets)
	AVMutableComposition *mixComposition = [AVMutableComposition composition];
	
	//We take a track for the audio
	AVMutableCompositionTrack *compositionAudioTrack = 
	[mixComposition addMutableTrackWithMediaType:AVMediaTypeAudio preferredTrackID:kCMPersistentTrackID_Invalid];
	
	//We take a track for the video
	AVMutableCompositionTrack *compositionVideoTrack = 
	[mixComposition addMutableTrackWithMediaType:AVMediaTypeVideo preferredTrackID:kCMPersistentTrackID_Invalid];
	
	
	BOOL ok = NO;
	
	//inserts the time range of the audio media (from t=0 (kCMTimeZero) to the audio duration (audioAsset.duration)
	//tracksWithMediaType from AVAsset returns an array of the asset tracks contained on it. Having only one, it is at index 0
	ok = [compositionAudioTrack insertTimeRange:CMTimeRangeMake(kCMTimeZero, audioAsset.duration) 
										ofTrack:[[audioAsset tracksWithMediaType:AVMediaTypeAudio] objectAtIndex:0] 
										 atTime:kCMTimeZero error:&error];
	
	if(!ok)
		cout << "cannot create composition audio track" << endl;
	cout << "composition audio made" << endl;
	
	//inserts the time range of the audio media (from t=0 (kCMTimeZero) to the video duration (videoAsset.duration)
	//tracksWithMediaType from AVAsset returns an array of the asset tracks contained on it. Having only one, it is at index 0
	ok = [compositionVideoTrack insertTimeRange:CMTimeRangeMake(kCMTimeZero, videoAsset.duration) 
										ofTrack:[[videoAsset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0] 
										 atTime:kCMTimeZero error:&error];
	
	if(!ok)
		cout << "cannot create composition video track" << endl;	
	cout << "composition video made" << endl;
	
	/*So here we have inserted both audio and video to a AVMutableCompositionTrack*/
	
	//This AssetExport transcodes the contents of the AVAssets to create an output of the form described by the preset
	//the preset PassThrough lets all tracks pass through. Maybe could also use AVAssetExportPreset{Low,Medium,Highest}Quality
	AVAssetExportSession *_assetExport = [[AVAssetExportSession alloc] initWithAsset:mixComposition 
																		  presetName:AVAssetExportPresetPassthrough];
	/*
	for (NSString* type in _assetExport.supportedFileTypes) {
		NSLog(@"type: %@", type);
	}
	 */
	
	NSString *exportPath = [mainPath stringByAppendingPathComponent:@"AviCule.mp4"];
	NSURL *exportURL = [NSURL fileURLWithPath:exportPath];
	
	if ([[[NSFileManager alloc] init] fileExistsAtPath:exportPath]) {
		[[[NSFileManager alloc] init] removeItemAtPath:exportPath error:nil];
	}
	
	_assetExport.outputFileType = @"public.mpeg-4";
	_assetExport.outputURL = exportURL;
	_assetExport.shouldOptimizeForNetworkUse = YES;
	
	//this method starts the asynchronous execution of the exportation in the path assigned
	
	[_assetExport exportAsynchronouslyWithCompletionHandler:^{
		
		bool isOk = NO;
		cout << "video has finished" << endl;
		switch (_assetExport.status) {
			case AVAssetExportSessionStatusUnknown:
				NSLog(@"AVAssetExportSessionStatusUnknown");
				break;
			case AVAssetExportSessionStatusExporting:
				NSLog(@"AVAssetExportSessionStatusExporting");
				break;
			case AVAssetExportSessionStatusCompleted:
				NSLog(@"AVAssetExportSessionStatusCompleted");
				[controller performSelectorOnMainThread:@selector (makeMovieFinishedWithError:) withObject:nil waitUntilDone:NO];
				isOk = YES;
				break;
			case AVAssetExportSessionStatusFailed:
				NSLog(@"AVAssetExportSessionStatusFailed");
				break;
			case AVAssetExportSessionStatusCancelled:
				NSLog(@"AVAssetExportSessionStatusCancelled");
				break;
			case AVAssetExportSessionStatusWaiting:
				NSLog(@"AVAssetExportSessionStatusWaiting");
				break;
			default:
				NSLog(@"default");
				break;
		}
		if(!isOk)
			cout << "ERROR WHILE EXPORTING VIDEO" << endl;
		
		
	} ];
	
	[pool release];
}


@end
