//
//  ES1Renderer.m
//  BasicCharacter
//
//  Created by Enrique Oriol on 9/20/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "ES1Renderer.h"


#include <CoreGraphics/CGImage.h>

#import <AVFoundation/AVFoundation.h>
#import <AVFoundation/AVAssetWriterInput.h>
#import <CoreVideo/CVPixelBuffer.h>
#import <UIKit/UIKit.h>

#import <AudioToolbox/AudioToolbox.h>



#define TIME_MARGIN 2  //seconds after the audio recording where video recording is also wanted



using namespace std;

@implementation ES1Renderer

@synthesize backingWidth;
@synthesize backingHeight;
@synthesize context = _context;



// Create an OpenGL ES 1.1 context
- (id)init
{
    if ((self = [super init]))
    {
		
        self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];

        if (!self.context || ![EAGLContext setCurrentContext:self.context])
        {
            [self release];
            return nil;
        }
		
        // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
        glGenFramebuffersOES(1, &defaultFramebuffer);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
        
		//Create color renderbuffer object.
		glGenRenderbuffersOES(1, &colorRenderbuffer); 
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);
		
		//create depth renderbuffer object.
		glGenRenderbuffersOES(1, &depthRenderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
		
    }

    return self;
}


- (void)setModelManager:(ModelManager*)mMng
{
    modelManager = mMng;
}




- (void)render
{
    
    
    [EAGLContext setCurrentContext:self.context];
    
        
	if([modelManager isModelLoaded])
        [modelManager render];        		
	else
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
	[self.context presentRenderbuffer:GL_RENDERBUFFER_OES];
	
    
//    [modelManager manageVideoRecording];
    
	[modelManager manageCover];
    
}



- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer
{	
//	if(![modelManager isModelLoaded]) //to avoid extra allocations each time that we change views
//	{
//        [modelManager resizeGLtoFitWidth:layer.bounds.size.width andHeight:layer.bounds.size.height];
    

    [self deleteFrameBuffers];
    [self createFrameBuffers];
    return [self allocateFrameBuffers:layer];

//	}
	
}


-(BOOL)allocateFrameBuffers:(CAEAGLLayer *)layer
{
    NSLog(@"resizing to fit: %f, %f", layer.bounds.size.width*layer.contentsScale, layer.bounds.size.height*layer.contentsScale);
    
    // Allocate color buffer backing based on the current layer size
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [self.context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer];
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    //Allocate depth buffer backing based on the current layer size
    glGenRenderbuffersOES(1, &depthRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
    glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    
    NSLog(@"backing width, height are:%d, %d ", backingWidth, backingHeight);
    
    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
    {
        NSLog(@"Failed to make complete framebuffer object %d", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }
    return YES;
}


- (void)deleteFrameBuffers
{
    //we need a valid and current context to access any OpenGL methods
    if (self.context) {
        [EAGLContext setCurrentContext:self.context];
        
        //if the default framebuffer has been set, delete it.
        if (defaultFramebuffer) {
            glDeleteFramebuffersOES(1, &defaultFramebuffer);
            defaultFramebuffer = 0;
        }
        
        //same for the renderbuffers, if they are set, delete them
        if (colorRenderbuffer) {
            glDeleteRenderbuffersOES(1, &colorRenderbuffer);
            colorRenderbuffer = 0;
        }
        
        if (depthRenderbuffer) {
            glDeleteRenderbuffersOES(1, &depthRenderbuffer);
            depthRenderbuffer = 0;
        }
    }
}


- (void)createFrameBuffers
{
    // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
    glGenFramebuffersOES(1, &defaultFramebuffer);
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
    
    //Create color renderbuffer object.
    glGenRenderbuffersOES(1, &colorRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);
    
    //create depth renderbuffer object.
    glGenRenderbuffersOES(1, &depthRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    
    
}





- (void)dealloc
{
	
    // Tear down GL
    if (defaultFramebuffer)
    {
        glDeleteFramebuffersOES(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }

    if (colorRenderbuffer)
    {
        glDeleteRenderbuffersOES(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }
	
	if (depthRenderbuffer)
    {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }

    // Tear down context
    if ([EAGLContext currentContext] == self.context)
        [EAGLContext setCurrentContext:nil];

    [self.context release];
    self.context = nil;

    [super dealloc];
}


- (void)deleteRenderer
{
		
    // Tear down GL
    if (defaultFramebuffer)
    {
        glDeleteFramebuffersOES(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }
	
    if (colorRenderbuffer)
    {
        glDeleteRenderbuffersOES(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }
	
	if (depthRenderbuffer)
    {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
	
    // Tear down context
    if ([EAGLContext currentContext] == self.context)
        [EAGLContext setCurrentContext:nil];
	
    [self.context release];
    self.context = nil;
	
    [super dealloc];
}



@end
