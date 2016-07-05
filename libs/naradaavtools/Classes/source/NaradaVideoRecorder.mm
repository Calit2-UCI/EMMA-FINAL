//
//  VideoRecorder.m
//  TestVideoRecording
//
//  Created by Corbella on 23/11/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "NaradaVideoRecorder.h"

#import "CoreMedia/CMTime.h"


#import <iostream>
#import <stdlib.h>

#define DEFAULT_BITRATE 800000

using namespace std;

@implementation NaradaVideoRecorder

@synthesize videoWriter = _videoWriter;
@synthesize writerInput = _writerInput;
@synthesize pixelAdaptor = _pixelAdaptor;

@synthesize outVideoPath = _outVideoPath;
@synthesize outBitRate = _outBitRate;
@synthesize frameRate = _frameRate;
@synthesize currentFrame = _currentFrame;

- (id)initWithOutPath:theVideoPath width:(int)width height:(int)height videoBitRate:(int)bitRate andFrameRate:(int)frameRate
{
	if (self = [super init]) {
		_outVideoPath = [[NSString stringWithString:theVideoPath] retain];
		backingWidth = width;
		backingHeight = height;
		if (bitRate == 0)
			_outBitRate = [NSNumber numberWithInt:DEFAULT_BITRATE];
		else 
			_outBitRate = [NSNumber numberWithInt:bitRate];
		_frameRate = frameRate;
		_currentFrame = 0;
	}
	return self;
}

- (unsigned char *)makeScreenShot
{
	int length = backingWidth * backingHeight * 4;
	
	GLubyte *pixel_buffer = (GLubyte *) malloc(length);
	
	//Read the pixels and store them in a buffer, raw format
	glReadPixels(0, 0, backingWidth, backingHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixel_buffer);
	
	unsigned char *pixel_buffer_reverted = (GLubyte *) malloc(length);
	
	int adjust = 0;
	
	//glReadPixels reads the pixels on the screen upside down, so we revert them. Plus we convert RGBA to ARGB
	for(int y=0; y<backingHeight; y++){
		for(int x=0; x<backingWidth*4; x++){
			//pixel indexes are adjusted to conform ARGB instead of RGBA
			if ((y * backingWidth * 4 + x) % 4 == 3) adjust = -3;
			else adjust = 1;
			
			pixel_buffer_reverted[(backingHeight - 1 - y) * backingWidth * 4 + x + adjust] =
			(unsigned char)pixel_buffer[y * 4 * backingWidth + x];
		}
	}
	free(pixel_buffer);
	return pixel_buffer_reverted;
}


- (CVPixelBufferRef)convertRawIntoPixelBuffer:(unsigned char *)pixel_buffer_reverted
{
	//Convert raw bytes to CVPixelBufferRef
	CVPixelBufferRef pixel_buffer_cv;
	CVReturn status;
	
	status = CVPixelBufferCreateWithBytes(kCFAllocatorDefault,			//Allocator
										  backingWidth, backingHeight,	//width, height
										  kCVPixelFormatType_32ARGB,	//pixelFormatType
										  pixel_buffer_reverted,		//void *baseAdress
										  backingWidth * 4,				//bytesPerRow
										  NULL, NULL, NULL,		//releaseCallBack, void *releaseRefCon, pixelBufferAttributes
										  &pixel_buffer_cv);			//outPutPixelBuffer
		
	return pixel_buffer_cv;
}


- (void)codeFrame
{
	unsigned char *raw_pixels = [self makeScreenShot];
	CVPixelBufferRef pixel_buffer = [self convertRawIntoPixelBuffer:raw_pixels];
	[self codeImage:pixel_buffer currentFrameCount:self.currentFrame atFrameRate:_frameRate];
	free(raw_pixels);
	CVPixelBufferRelease(pixel_buffer);
	_currentFrame ++;
}

 
- (void)startSession
{
	
	NSError *error;
	
	NSFileManager *fm = [[NSFileManager alloc] init];
	if ([fm fileExistsAtPath:self.outVideoPath]) {
		[fm removeItemAtPath:self.outVideoPath error:&error];
	}
	
	cout << "Out video Path: " << [self.outVideoPath cStringUsingEncoding:1] << endl;
	
	
	self.videoWriter = [[AVAssetWriter alloc] initWithURL:[NSURL fileURLWithPath:self.outVideoPath] 
											fileType:AVFileTypeMPEG4 
											   error:&error];
	
	NSParameterAssert(self.videoWriter);
	
	NSMutableDictionary *videoSettings = [[NSMutableDictionary alloc] init];
	[videoSettings setValue:AVVideoCodecH264 forKey:AVVideoCodecKey];
	
	//set Width and Height of the coded video
	[videoSettings setValue:[NSNumber numberWithInt:backingWidth] forKey:AVVideoWidthKey];
	[videoSettings setValue:[NSNumber numberWithInt:backingHeight] forKey:AVVideoHeightKey];
	
	//Another dictionary with compression properties
	NSMutableDictionary *properties = [[NSMutableDictionary alloc] init];
	//set the bitrate for the coded video
	[properties setValue:self.outBitRate forKey:AVVideoAverageBitRateKey];
	[properties setValue:AVVideoProfileLevelH264Main31 forKey:AVVideoProfileLevelKey];
	
	//merge both dictionaries
	[videoSettings setObject:properties forKey:AVVideoCompressionPropertiesKey];
	
	self.writerInput = [[[AVAssetWriterInput alloc] initWithMediaType:AVMediaTypeVideo 
													  outputSettings:videoSettings] retain];
	
	NSDictionary *bufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:
									  [NSNumber numberWithInt:kCVPixelFormatType_32ARGB], 
									  kCVPixelBufferPixelFormatTypeKey, nil];
	
	self.pixelAdaptor = [[[AVAssetWriterInputPixelBufferAdaptor alloc] initWithAssetWriterInput:self.writerInput	
															   sourcePixelBufferAttributes:bufferAttributes] retain];
	
	NSParameterAssert(self.pixelAdaptor);
	
	NSParameterAssert(self.writerInput);
	NSParameterAssert([self.videoWriter canAddInput:self.writerInput]);
	[self.videoWriter addInput:self.writerInput];
	
	//start a session
	BOOL start = [self.videoWriter startWriting];
	
	if (start == NO) {
		cout << "CANNOT START WRITING!!" << endl;
		[self EndSession];
		exit(-1);
	}
	
	[self.videoWriter startSessionAtSourceTime:kCMTimeZero];
}

- (void)codeImage:(CVPixelBufferRef)pixel_buffer currentFrameCount:(int64_t)f_count atFrameRate:(int)frameRate
{	
	if ([self.writerInput isReadyForMoreMediaData]){
		
		CMTime frameTime = CMTimeMake(1, frameRate);
		CMTime lastTime = CMTimeMake(f_count, frameRate);
		CMTime presentTime = CMTimeAdd(lastTime, frameTime);
		
		if (![self.pixelAdaptor appendPixelBuffer:pixel_buffer withPresentationTime:presentTime]) {
			cerr << "Append Pixel Buffer Returned NO" << endl;
			exit(-1);
		}
	}
	else {
		NSLog(@"error appending buffer");
	}
}


- (void)EndSession
{
	//Finish the session
	NSError *err;
	cout << "Ending Session" << endl;
	[self.writerInput markAsFinished];
	
	NSFileManager *fm = [[NSFileManager alloc] init];
	NSData *fileData;
	
	if ([self.videoWriter finishWriting] == NO){
		cout << "Could Not Finish Writing into File" << endl;
		err = self.videoWriter.error;
		cout << "Error Code " << [err code] << endl;
		cout << "Error Domain " << [err domain] << endl;
		cout << "Error UserInfo " << [err userInfo] << endl;
	}
	else {
		if( ![fm fileExistsAtPath:self.outVideoPath] )
			cout << "File Does Not Exist at path: "	<< self.outVideoPath << endl;
		if ( ![fm isReadableFileAtPath:self.outVideoPath] )
			cout << "File is Not Readable at path " << self.outVideoPath << endl;
		
		fileData = [fm contentsAtPath:self.outVideoPath];
		cout << "Size of the Video (only)" << [fileData length] << endl;
	}
}

- (void)dealloc
{
	[self.outVideoPath release];
	[self.pixelAdaptor release];
	[self.videoWriter release];
	[self.writerInput release];
	[super dealloc];
}

@end
