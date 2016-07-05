//
//  EAGLView.h
//  BasicCharacter
//
//  Created by Enrique Oriol on 9/20/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#import "ES1Renderer.h"

#import <CoreMotion/CMMotionManager.h>

// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView
{    
@private
//    id <ESRenderer> renderer;
	ES1Renderer *renderer;

    BOOL animating;
    BOOL displayLinkSupported;
    NSInteger animationFrameInterval;
    // Use of the CADisplayLink class is the preferred method for controlling your animation timing.
    // CADisplayLink will link to the main display and fire every vsync when added to a given run-loop.
    // The NSTimer class is used only as fallback when running on a pre 3.1 device where CADisplayLink
    // isn't available.
    id displayLink;
    NSTimer *animationTimer;
	
	int frameRate; //fps
	
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;
@property (nonatomic, retain) ES1Renderer* renderer;
@property (nonatomic,readonly) int frameRate;


- (void)deleteRenderer;
- (void)startAnimation;
- (void)stopAnimation;
- (void)drawView:(id)sender;
- (EAGLSharegroup*)getRendererContextSharegroup;
- (EAGLContext*)getContext;


@end
