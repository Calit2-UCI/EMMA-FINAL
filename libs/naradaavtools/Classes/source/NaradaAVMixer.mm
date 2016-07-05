//
//  Mixer.m
//  MyCookingAssistant
//
//  Created by Corbella on 30/11/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "NaradaAVMixer.h"

#import <iostream>

using namespace std;


@implementation NaradaAVMixer

@synthesize inAudioPath = _inAudioPath, inVideoPath = _inVideoPath, outVideoPath = _outVideoPath, delegate = _delegate;


- (id)initWithPathsInAudio:(const NSString *)inAudio inVideo:(const NSString *)inVideo outVideo:(const NSString *)outVideo andDelegate:(id<NaradaAVMixerDelegate>)theDelegate
{
	if (self = [super init]) {
		_inAudioPath = [NSString stringWithString:(NSString *)inAudio];
		_inVideoPath = [NSString stringWithString:(NSString *)inVideo];
		_outVideoPath = [NSString stringWithString:(NSString *)outVideo];
		_delegate = theDelegate;
	}
	return self;
}

#pragma mark Audio-Video Mixer

- (void)mixAudioVideo
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	NSError *error;
	
//	NSString *mainPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
	
//	NSString *audioPath = [mainPath stringByAppendingPathComponent:@"output.aac"];
//	NSString *videoPath = [mainPath stringByAppendingPathComponent:@"TestVideo.mp4"];
	
	cout << "In Audio: " << [_inAudioPath cStringUsingEncoding:1] << endl;
	cout << "In Video: " << [_inVideoPath cStringUsingEncoding:1] << endl;
	
	NSURL *audioURL = [[NSURL alloc] initFileURLWithPath:self.inAudioPath];
	NSURL *videoURL = [[NSURL alloc] initFileURLWithPath:self.inVideoPath];
	
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
	
//	NSString *exportPath = [mainPath stringByAppendingPathComponent:@"My Virtual Assistant.mp4"];
	NSURL *exportURL = [NSURL fileURLWithPath:self.outVideoPath];
	
	if ([[[NSFileManager alloc] init] fileExistsAtPath:self.outVideoPath]) {
		[[[NSFileManager alloc] init] removeItemAtPath:self.outVideoPath error:nil];
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
		
		NSError *error = [[NSError errorWithDomain:@"NaradaAVMixerErrorDomain" code:_assetExport.status userInfo:nil] retain];
		[self.delegate performSelectorOnMainThread:@selector(makeMovieFinishedWithError:) withObject:error waitUntilDone:YES];
	} ];
	
	[pool release];
}

- (void)dealloc
{
	[self.inAudioPath release];
	[self.inVideoPath release];
	[self.outVideoPath release];
	[super dealloc];
}


@end
