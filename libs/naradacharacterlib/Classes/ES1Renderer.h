//
//  ES1Renderer.h
//  BasicCharacter
//
//  Created by Enrique Oriol on 9/20/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "ESRenderer.h"



#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#import <CoreGraphics/CGImage.h>
#import <AVFoundation/AVFoundation.h>

#import "ModelManager.h"






@interface ES1Renderer : NSObject <ESRenderer>
{
	
@private
    EAGLContext *_context;

    // The pixel dimensions of the CAEAGLLayer
    GLint backingWidth;
    GLint backingHeight;

    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view
    GLuint defaultFramebuffer, colorRenderbuffer, depthRenderbuffer;
	
    ModelManager *modelManager;
    
    int duration;
    
	
	
}

-(void)createFrameBuffers;
-(void)deleteFrameBuffers;
-(BOOL)allocateFrameBuffers:(CAEAGLLayer *)layer;


- (void)setModelManager:(ModelManager*)modelManager;
- (void)render;
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;
- (void)deleteRenderer;

@property (nonatomic, assign) GLint backingWidth;
@property (nonatomic, assign) GLint backingHeight;
@property (nonatomic, retain) EAGLContext * context;


//- (UIImage *)saveIntoCGImagePixels:(GLubyte *)pixel_buffer_reverted;


@end
