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


@interface NaradaVideoRecorder : NSObject {

	AVAssetWriter* _videoWriter;
	AVAssetWriterInput* _writerInput;
	AVAssetWriterInputPixelBufferAdaptor* _pixelAdaptor;
	
	int backingWidth;
	int backingHeight;
	
	const NSString* _outVideoPath;
	
	NSNumber* _outBitRate;
	
	int _frameRate;
	int _currentFrame;
}

- (unsigned char *)makeScreenShot;
- (CVPixelBufferRef)convertRawIntoPixelBuffer:(unsigned char *)pixel_buffer_reverted;

/**
 * Makes a screenshot of the glpixels and appends it to the .mp4-coded video
 */
- (void)codeFrame;

- (void)startSession;
- (void)codeImage:(CVPixelBufferRef)pixel_buffer currentFrameCount:(int64_t)f_count atFrameRate:(int)frameRate;
- (void)EndSession;


@property(nonatomic,retain) AVAssetWriter* videoWriter;
@property(nonatomic,retain) AVAssetWriterInput* writerInput;
@property(nonatomic,retain) AVAssetWriterInputPixelBufferAdaptor* pixelAdaptor;

/**
 * This is the mail initializer. You must provide the height and width of the screen in pixels, the bit rate at which you desire 
 * the video to be encoded and the frame rate at which the video will be coded
 */
- (id)initWithOutPath:(NSString *)theVideoPath width:(int)width height:(int)height videoBitRate:(int)bitRate andFrameRate:(int)frameRate;

/**
 * This specifies the desired path to store the video
 */
@property(nonatomic, retain) NSString* outVideoPath;

/**
 * This states the level of compression of the resultant video
 */
@property(nonatomic, retain) NSNumber* outBitRate;

/**
 * This is the frame rate at which the video will be made, that is, the number of frames per second.
 */
@property(readonly) int frameRate;

@property(readonly) int currentFrame;

@end
