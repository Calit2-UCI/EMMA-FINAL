//
//  VideoRecorder.m
//  TestVideoRecording
//
//  Created by Corbella on 23/11/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "VideoRecorder.h"

#import "CoreMedia/CMTime.h"


#import <iostream>
#import <stdlib.h>

#define MAX_FRAMES 90

unsigned char *pixels;

using namespace std;

@implementation VideoRecorder

@synthesize videoWriter;
@synthesize writerInput;
@synthesize pixelAdaptor;

- (id)initWithWidth:(int)width height:(int)height
{
	videoWriter = nil;
	writerInput = nil;
	pixelAdaptor = nil;
	backingWidth = width;
	backingHeight = height;
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
	
	size_t dataSize = CVPixelBufferGetDataSize(pixel_buffer_cv);
	
	return pixel_buffer_cv;
}


- (void)codeFrame:(int)current_frame atFrameRate:(int)frameRate
{
	unsigned char *raw_pixels = [self makeScreenShot];
	CVPixelBufferRef pixel_buffer = [self convertRawIntoPixelBuffer:raw_pixels];
	[self codeImage:pixel_buffer currentFrameCount:current_frame atFrameRate:frameRate];
	free(raw_pixels);
	CVPixelBufferRelease(pixel_buffer);
}

 
- (void)startSession
{
	
	NSError *error;
	
	NSFileManager *fm = [[NSFileManager alloc] init];
	
	NSString *path = [self getPathToDocuments:@"TestVideo.mp4"];
	
	if ([fm fileExistsAtPath:path]) {
		BOOL remove = [fm removeItemAtPath:path error:&error];
		if (remove == NO) {
			cout << "Error Removing: " << [error code] << endl;
		}
	}
	
	cout << [path cStringUsingEncoding:1] << endl;
	
	
	videoWriter = [[AVAssetWriter alloc] initWithURL:[NSURL fileURLWithPath:path] 
											fileType:AVFileTypeMPEG4 
											   error:&error];
	
	NSParameterAssert(videoWriter);
	
	
	NSMutableDictionary *videoSettings = [[NSMutableDictionary alloc] init];
	[videoSettings setValue:AVVideoCodecH264 forKey:AVVideoCodecKey];
	
	//set Width and Height of the coded video
	[videoSettings setValue:[NSNumber numberWithInt:backingWidth] forKey:AVVideoWidthKey];
	[videoSettings setValue:[NSNumber numberWithInt:backingHeight] forKey:AVVideoHeightKey];
	
	//Another dictionary with compression properties
	NSMutableDictionary *properties = [[NSMutableDictionary alloc] init];
	//set the bitrate for the coded video
	[properties setValue:[NSNumber numberWithInt:800000] forKey:AVVideoAverageBitRateKey];
	[properties setValue:AVVideoProfileLevelH264Main31 forKey:AVVideoProfileLevelKey];
	
	//merge both dictionaries
	[videoSettings setObject:properties forKey:AVVideoCompressionPropertiesKey];
	
	writerInput = [[[AVAssetWriterInput alloc] initWithMediaType:AVMediaTypeVideo 
													  outputSettings:videoSettings] retain];
	
	NSDictionary *bufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:
									  [NSNumber numberWithInt:kCVPixelFormatType_32ARGB], 
									  kCVPixelBufferPixelFormatTypeKey, nil];
	
	pixelAdaptor = [[[AVAssetWriterInputPixelBufferAdaptor alloc] initWithAssetWriterInput:writerInput	
															   sourcePixelBufferAttributes:bufferAttributes] retain];
	
	NSParameterAssert(pixelAdaptor);
	
	NSParameterAssert(writerInput);
	NSParameterAssert([videoWriter canAddInput:writerInput]);
	[videoWriter addInput:writerInput];
	
	//start a session
	BOOL start = [videoWriter startWriting];
	
	if (start == NO) {
		cout << "CANNOT START WRITING!!" << endl;
		[self EndSession];
		exit(-1);
	}
	
	[videoWriter startSessionAtSourceTime:kCMTimeZero];
}

- (void)codeImage:(CVPixelBufferRef)pixel_buffer currentFrameCount:(int64_t)f_count atFrameRate:(int)frameRate
{	
	if ([writerInput isReadyForMoreMediaData]){
		
		CMTime frameTime = CMTimeMake(1, frameRate);
		CMTime lastTime = CMTimeMake(f_count, frameRate);
		CMTime presentTime = CMTimeAdd(lastTime, frameTime);
		
		if (![pixelAdaptor appendPixelBuffer:pixel_buffer withPresentationTime:presentTime]) {
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
	[writerInput markAsFinished];
	
	NSString *path = [self getPathToDocuments:@"TestVideo.mp4"];
	NSFileManager *fm = [[NSFileManager alloc] init];
	NSData *fileData;
	
	if ([videoWriter finishWriting] == NO){
		cout << "Could Not Finish Writing into File" << endl;
		err = videoWriter.error;
		cout << "Error Code " << [err code] << endl;
		cout << "Error Domain " << [err domain] << endl;
		cout << "Error UserInfo " << [err userInfo] << endl;
	}
	else {
		if( ![fm fileExistsAtPath:path] )
			cout << "File Does Not Exist at path: "	<< path << endl;
		if ( ![fm isReadableFileAtPath:path] )
			cout << "File is Not Readable at path " << path << endl;
		
		fileData = [fm contentsAtPath:path];
		cout << "Size of the Video (only)" << [fileData length] << endl;
	}
	
	[writerInput release];
	[videoWriter release];
	[pixelAdaptor release];
	[fm release];
	//[fileData release];

}

- (NSString *)getPathToDocuments:(NSString *)fileString
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	
	return [ [paths objectAtIndex:0] stringByAppendingPathComponent:fileString ];
}


@end
