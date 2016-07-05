//
//  ModelManager.h
//  BasicCharacter
//
//  Created by Enrique Oriol on 28/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <UIKit/UIKit.h>

#import <CoreGraphics/CGImage.h>
#import <AVFoundation/AVFoundation.h>


//#include <iostream>
//#include <exception>

#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <iostream>
#include <vector>


#import "PathsAndConstants.h"


@interface ModelManager : NSObject
{

//@private

// The pixel dimensions of the CAEAGLLayer
//GLint backingWidth;
//GLint backingHeight;


    NSString* wavPath;
    NSString* changeBackgroundPath;
    BOOL changeBackground;
    unsigned char menuState;
    float loadingPercentage;
    
    int width;
    int height;
    
    BOOL shouldDoTranslate;
    BOOL shouldWaitToTranslate;
    BOOL isTrackingRotAndPos;
    BOOL isPlayingTrackedRotAndPos;    
    
    int frame_count, margin_beats, nFrames, trackingPosition;
    Float64 duration;
    int hideCounter;
    BOOL startSession;
    std::vector<float> rotTrackX;
    std::vector<float> rotTrackY;
    std::vector<float> transTrackX;

    bool _shouldDoHigherPitch;
	
    int rotTrackFrameRateAdjust;
    int recording_counter;
    
//    std::string modelName;
//    std::string background;
    NSString* _modelName;
    NSString* _background;
    

    bool animateTranslation;
    int pendingFramesToAnimateTranslation;
    float animateTranslationXStep;
    float animateTranslationYStep;
    float animateTranslationFinalX;
    float animateTranslationFinalY;

    bool animateRotation;
    int pendingFramesToAnimateRotation;
    float animateRotationDegreesXStep;
    float animateRotationDegreesYStep;
    float animateRotationFinalDegreesX;
    float animateRotationFinalDegreesY;

}

-(ModelManager *)initWithName:(NSString*)name Background:(NSString*)background Width:(int)width Height:(int)height;

//-(void)initModelWithViewWidth:(int)width andHeight:(int)height;

-(void)initCharacter;

-(void)initCharacterWithPerspective:(BOOL) pers;

-(BOOL)isModelLoaded;

- (void) manageCover;

- (void)render;

- (void) resizeGLtoFitWidth:(int)width andHeight:(int)height;


-(void)startTrackingRotAndPos;

-(void)stopTrackingRotAndPos;

-(void)startPlayingTrackedRotAndPos;

-(void)stopPlayingTrackedRotAndPos;

-(void)startHideCounter;
//- (void)setMenuState:(unsigned char)state;
//- (unsigned char)getMenuState;

- (void)playAction:(NSString*) act;

- (void)playTemporaryAction:(NSString*) act withPath:(NSString*) path andDelay:(long) delay;

- (void)repeatAudio:(NSString*) act;
- (void)repeatAudioWithAbsoluteAudioPath:(NSString*)wavFilePath temporary:(BOOL) temp;
- (void)repeatOnMainThread:(NSString*)wavFilePath;
- (void)repeatTemporaryOnMainThread:(NSString*)wavFilePath;

- (void)hasFinishedSpeaking;

- (bool)playRecMovie;

- (bool)playRecAudio;

- (void)stopAction:(NSString *)actionName;
- (void)stopForegroundActions;

- (void)shutUp;



#pragma mark -
#pragma mark FrameRate functions

- (float)getRealFrameRate;
- (int)getFrameRate;
- (int)getNFrames;
- (void) restoreDefaultFrameRate;


#pragma mark -
#pragma mark Background functions


-(void)setDefaultBackground;

-(void)changeBackgroundImageFromResource:(NSString*) imageFile;


#pragma mark -
#pragma mark Audio functions


-(void)setDoPitchShift:(bool) shouldDo;

-(void)getDoPitchShift;

-(BOOL)calculateAudioDuration;

-(void)setSpeakRealTime:(bool)realTime;



#pragma mark -
#pragma mark Model Rotation and Motion

-(void)rotateToInit;

-(void)rotateToTrackedPosition:(int)index;

-(void)translateToTrackedPosition:(int)index;

-(void)manageRotAndPos;

-(void)trackRotation;

-(void)trackTranslation;

-(float)pullTrackRotX;

-(float)pullTrackRotY;

-(float)getTrackRotX:(int) position;

-(float)getTrackRotY:(int) position;

- (void)moveModelX:(float)incX y:(float)incY absolute:(BOOL)abs;

- (void)moveModelX:(float)incX absolute:(BOOL)abs;

- (void)moveModelY:(float)incY absolute:(BOOL)abs;

- (void)rotateModelX:(float)incX y:(float)incY absolute:(BOOL)abs;

-(int)getXCenterModel;

-(int)getYCenterModel;

- (void)moveCloser:(float)incZ absolute:(BOOL)abs;

- (float)modelPositionX;

- (float)modelPositionY;

- (float)modelPositionZ;

- (void) animateTranslationDuring:(float)seconds incX:(float)x incY:(float)y absolute:(BOOL)abs;

- (void) animateRotationDuring:(float)seconds degreesX:(float)degX degreesY:(float)degY absolute:(BOOL)abs;


- (void) manageAnimateTranslation;
- (void) manageAnimateRotation;




#pragma mark -
#pragma mark Path and Filename utilities

-(char*)getPathTo:(std::string) file;
-(char*)getPathFromNSStringTo:(NSString*) fileString;
-(NSString*)getNSPathFromNSStringTo:(NSString*) fileString;
-(NSString *)getPathToDocuments:(NSString *)fileString;
-(char*) getWritablePathTo:(std::string) file;
-(char*) getWritablePathToReadFrom:(NSString*) file;



#pragma mark -
#pragma mark -
#pragma mark Properties


@property (readonly, assign) bool isExporting;
@property (readonly, assign) bool isPlayingRecorded;


@property (nonatomic, copy) NSString* changeBackgroundPath;

@property (readonly, assign) int frame_count;
@property (readonly, assign) int margin_beats;
@property (readonly, assign) int nFrames;

@property (readonly, assign) Float64 duration;
@property (readonly, assign) int hideCounter;
@property (readonly, assign) BOOL startSession;

@property (nonatomic, assign) int width;
@property (nonatomic, assign) int height;
@property (readonly, assign) float loadingPercentage;

@property (readonly, assign) BOOL isTrackingRotAndPos;
@property (readonly, assign) BOOL isPlayingTrackedRotAndPos;

@property (nonatomic, assign) NSString* modelName;
@property (nonatomic, assign) NSString* background;




@end
