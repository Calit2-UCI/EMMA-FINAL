//
//  NaradaDemoAppViewController.h
//  iPhoneNaradaDemo
//
//  Created by Corbella on 05/10/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <iAd/iAD.h>


#import "MessageUI/MFMailComposeViewController.h"

//#import "NaradaAVMixer.h"


#import "ModelManager.h"

#define DEFAULT_N_SECONDS_TO_WAIT_AFTER_ANIMATION_PLAY 4


@class EAGLView;



@interface GLViewController : UIViewController 
<
	UIAlertViewDelegate,
	UIAccelerometerDelegate,
	UINavigationControllerDelegate
> {
	
	IBOutlet UIButton *playAudio;
	
	IBOutlet UIActivityIndicatorView *loading;
	IBOutlet UILabel *waitLabel;
	
	IBOutlet UIProgressView *loadingBar;
	
	
	IBOutlet UIView *cover;
	IBOutlet UIView *gestureRecognizerView;
        
    
	EAGLView *glView;
    ModelManager *modelManager;
	
	int panGestureNumChangesVelocitySignX;
	int panGestureNumChangesVelocitySignY;
	BOOL panGestureVelocityXIsPositive;
	BOOL panGestureVelocityYIsPositive;
    BOOL usePerspective;

	UIImageView* imageView;
	
	IBOutlet UIView* mainView;
	IBOutlet UIView* loadingView;
    
    
    NSNumber* _timeStamp;
}

- (void)showAlert:(NSString *)alertTitle message:(NSString *)alertMessage;

- (void)setModelManagerListeners;

- (void)releaseModelManagerListeners;

-(void) loadModelInBackground:(EAGLContext*) ctx;
-(ModelManager*)loadModelWithPath:(NSString*)modelName AndBackground:(NSString*)background;
-(ModelManager*)loadModelWithPath:(NSString*)modelName AndBackground:(NSString*)background AndPerspective:(BOOL)persp;
- (IBAction)clickLoadModel:(id)sender;
- (IBAction)clickPlayAudio:(id)sender;

- (void) changeSuperview:(UIView*) newSuperview doStartAnimation:(BOOL)start;

- (void)alertView:(UIAlertView*)alertView clickButtonAtIndex:(NSInteger)buttonIndex;

- (void)showLoading;
- (void)showLoadModel;
-(void)doLoadingModelThread;

- (void)showCover;

- (void)hideAllButtons;
- (void)hideLoading;
- (void)hideLoadModel;

- (void)hideTitle;
- (void)hideCover;
- (void)hideAll;

- (void)showLoadingBar:(bool)show;
- (void)showLoadingProgress;


-(void)handleStateEAGLViewResized:(NSNotification*)notification;
- (void)handleMoveModel:(UIPanGestureRecognizer *)sender;
- (void)handleStateFinishedTimeForCover:(NSNotification *)notification;
- (void)handleModifiedLoadingBar:(NSNotification *)notification;
- (void)handleFinishedSpeaking:(NSNotification *)notification;
- (void)handleStateFinishedPlayingTrackedRotAndPos:(NSNotification *)notification;

- (void)startHideCounter;


- (void)handleTouchInteractions:(UIPanGestureRecognizer *)sender;

- (void)setModelToInitialPosition;
- (void)centerXRecognizerViewOverModel:(int) x;

- (void)setStateInit;
- (void)setStateLoadingModel;
- (void)setStateReady;
- (void)setStatePlaying;

- (void)disableAccelerometer;
- (void)enableAccelerometer;



@property (nonatomic, retain) ModelManager *modelManager;
@property (nonatomic, retain) IBOutlet EAGLView *glView;

@property (nonatomic, retain) IBOutlet UIActivityIndicatorView *loading;
@property (nonatomic, retain) IBOutlet UILabel *waitLabel;

@property (nonatomic,retain) IBOutlet UIView *gestureRecognizerView;

@property (nonatomic,retain) IBOutlet UIView *loadingView;
@property (nonatomic,retain) IBOutlet UIView *mainView;
@property (nonatomic,retain) IBOutlet UIView *cover;

@property (nonatomic, retain) IBOutlet UIProgressView *loadingBar;

@property (nonatomic, retain) IBOutlet UIButton *playAudio;

@property(nonatomic, retain)     NSNumber* timeStamp;
@property(nonatomic) CGPoint originPoint;

@end
