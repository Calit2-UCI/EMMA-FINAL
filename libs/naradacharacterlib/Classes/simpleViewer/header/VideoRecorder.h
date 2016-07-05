//
//  VideoRecorder.h
//  TestVideoRecording
//
//  Created by Corbella on 23/11/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <AVFoundation/AVFoundation.h>
#import <CoreVideo/CVPixelBuffer.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>


@interface VideoRecorder : NSObject {

@private
	AVAssetWriter *videoWriter;
	AVAssetWriterInput *writerInput;
	AVAssetWriterInputPixelBufferAdaptor *pixelAdaptor;
	
	int backingWidth;
	int backingHeight;  
}

- (id)initWithWidth:(int)width height:(int)height;
- (unsigned char *)makeScreenShot;
- (CVPixelBufferRef)convertRawIntoPixelBuffer:(unsigned char *)pixel_buffer_reverted;
- (void)codeFrame:(int)frame_count atFrameRate:(int)frameRate;
- (void)startSession;
- (void)codeImage:(CVPixelBufferRef)pixel_buffer currentFrameCount:(int64_t)f_count atFrameRate:(int)frameRate;
- (void)EndSession;
- (NSString *)getPathToDocuments:(NSString *)fileString;

@property(readonly,retain) AVAssetWriter *videoWriter;
@property(readonly,retain) AVAssetWriterInput *writerInput;
@property(readonly,retain) AVAssetWriterInputPixelBufferAdaptor	*pixelAdaptor;

@property(readonly,retain) NSFileManager *fm;
@property(readonly,retain) NSData *fileData;

@end
