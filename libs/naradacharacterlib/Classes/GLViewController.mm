#import "GLViewController.h"
#import "EAGLView.h"

#import "ES1Renderer.h"

#import "math.h"

#import <iostream.h>



using namespace std;

CGPoint originPoint;

UITapGestureRecognizer *singleFingerDoubleTap;
UIPanGestureRecognizer *panGesture;
UIPinchGestureRecognizer *pinchGesture;

UIAccelerationValue x, y, z;
float last_angle;
BOOL accelerometerEnabled = NO;

float initYaw, initPitch;
BOOL initAcc;

enum states {
	STILL = 0,
	MOVE,
	ROTATE
};

states state = STILL;

@implementation GLViewController


@synthesize modelManager;
@synthesize glView;


@synthesize waitLabel;
@synthesize loading;
@synthesize loadingBar;
@synthesize gestureRecognizerView;
@synthesize loadingView;
@synthesize mainView;
@synthesize cover;

@synthesize playAudio;

@synthesize timeStamp=_timeStamp;

- (id)init
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        if ([UIScreen mainScreen].bounds.size.height == 568.0) {
            self = [super initWithNibName:@"GLViewController_iPhone5" bundle:nil];
        }
        else {
            self = [super initWithNibName:@"GLViewController" bundle:nil];
        }
    }
    else {
        self = [super initWithNibName:@"GLViewController_iPad" bundle:nil];
    }
    if (self) {
    }
    return self;
}


/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/


/*
 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
 // Return YES for supported orientations
	 return YES;
// return (interfaceOrientation == UIInterfaceOrientationPortrait);
 }
 */
 
- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
	[self becomeFirstResponder];
	self.view.multipleTouchEnabled = YES;
    [self.glView startAnimation];
}




// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	
    [super viewDidLoad];
    
    
    self.timeStamp = [NSNumber numberWithInt:0];
	
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) {
		//iPad specific settings should go here
	}
    
    self.view.autoresizesSubviews = YES;
	
    
    [self setModelManagerListeners];
    
}


- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc that aren't in use.
}


- (void)viewDidUnload {
    [super viewDidUnload];
    [self.glView stopAnimation];
    // Release any retained subviews of the main view.
}

- (void) changeSuperview:(UIView*) newSuperview doStartAnimation:(BOOL)start {
    [self.view removeFromSuperview];
    self.view.frame = CGRectMake(0, 0, newSuperview.frame.size.width, newSuperview.frame.size.height);
    [newSuperview addSubview:self.view];
    if(start)
        [self.glView startAnimation];

}



- (void)viewDidDisappear:(BOOL)animated
{
    [glView stopAnimation];
}










- (void)dealloc {
	
    [self releaseModelManagerListeners];
	[modelManager release];
	[loading release];
	[waitLabel release];

    [super dealloc];
}


#pragma mark timestamp handling methods

-(NSNumber*) getNowTimeStamp
{
    NSTimeInterval timeStamp = [[NSDate date] timeIntervalSince1970];
    // NSTimeInterval is defined as double
    NSNumber *timeStampObj = [NSNumber numberWithDouble: timeStamp];
    NSLog(@"stam created: %@",timeStampObj);
    return timeStampObj;
}

-(BOOL) stampWaitHasPassed{
    
    NSNumber *timeStampObj = [self getNowTimeStamp];    //see if 3 seconds have passed to make sure we record AFTER playing any animations
    if ( !([timeStampObj doubleValue]>=([self.timeStamp doubleValue]+ DEFAULT_N_SECONDS_TO_WAIT_AFTER_ANIMATION_PLAY)) ) {
        NSLog(@"wait returning NO %f >= %f",[timeStampObj doubleValue], [self.timeStamp doubleValue]+ DEFAULT_N_SECONDS_TO_WAIT_AFTER_ANIMATION_PLAY);
        return NO;
        
    }else{
        NSLog(@"wait returning YES %f >= %f", [timeStampObj doubleValue], [self.timeStamp doubleValue]+ DEFAULT_N_SECONDS_TO_WAIT_AFTER_ANIMATION_PLAY);
        return YES;
    }
}



#pragma mark New Show Methods



- (void)setStateInit
{
	mainView.hidden = YES;
	loadingView.hidden = YES;
}

- (void)setStateLoadingModel
{
	// Elements shown
	loadingView.hidden = NO;
	loading.hidden = NO;
	waitLabel.hidden = NO;
	
	// Elements hidden
	loadingBar.hidden = NO;
	
	// Other views, which are hidden
	mainView.hidden = YES;
//	initView.hidden = YES;
}

- (void)setStateReady
{
	// Elements Shown
	mainView.hidden = NO;
	gestureRecognizerView.hidden = NO;
    glView.hidden = NO;
	
	// Elements hidden
	playAudio.hidden = NO;
    
    CGRect initialFrame = loadingView.frame;
    CGRect finalFrame = CGRectMake(self.view.frame.origin.x, self.view.frame.size.height - loadingView.frame.origin.y, self.view.frame.size.width, self.view.frame.size.height);

    NSLog(@"Animating downwards");
    
    NSTimeInterval duration = 0.5, delay = 0.5;
    
    [UIView animateWithDuration:duration delay:delay options:nil animations:^{
        loadingView.frame = finalFrame;
        loadingView.alpha = 0.0f;
    } completion:^(BOOL finished) {
        loadingView.hidden = YES;
        loadingView.alpha = 1.0f;
        loadingView.frame = initialFrame;
    }];
    
}


- (void)setStatePlaying
{
	// Elements Shown
	mainView.hidden = NO;
	
	// Elements hidden
	playAudio.hidden = YES;
	gestureRecognizerView.hidden = YES;
	
	// Other views, which are hidden
    loadingView.hidden = YES;
}


- (void)showCover
{
	cover.hidden = NO;
}

- (void)hideCover
{
	cover.hidden = YES;
}



#pragma mark Other Setters



- (void)setModelManagerListeners
{
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleStateFinishedTimeForCover:) name:notificationJustFinishedCoverProtectionTime object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleModifiedLoadingBar:) name:notificationModifiedLoadingBar object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleFinishedSpeaking:) name:notificationFinishedSpeaking object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleFinishedPlayingTrackedRotAndPos:) name:notificationJustFinishedPlayingTrackedRotAndPos object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleStateEAGLViewResized:) name:notificationEAGLViewHasBeenResized object:nil];
    
    
    
}



- (void)releaseModelManagerListeners
{
    
    [[NSNotificationCenter defaultCenter] removeObserver:self name:notificationJustFinishedCoverProtectionTime object:nil];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self name:notificationModifiedLoadingBar object:nil];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self name:notificationFinishedSpeaking object:nil];  
    
    [[NSNotificationCenter defaultCenter] removeObserver:self name:notificationJustFinishedPlayingTrackedRotAndPos object:nil];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self name:notificationEAGLViewHasBeenResized object:nil];
    
    
    
}


-(void)handleStateEAGLViewResized:(NSNotification*)notification
{
    if([modelManager isModelLoaded])
    {
        [modelManager resizeGLtoFitWidth:self.glView.frame.size.width*glView.layer.contentsScale andHeight:self.glView.frame.size.height*glView.layer.contentsScale];
        [self centerXRecognizerViewOverModel:[modelManager getXCenterModel]];
    }
}


- (void)handleModifiedLoadingBar:(NSNotification *)notification
{
	float state = [[[notification userInfo] valueForKey:@"percentage"] floatValue];
	[loadingBar setProgress:state];
}




- (void)handleStateFinishedTimeForCover:(NSNotification *)notification
{
    [self hideCover];
}


- (void)handleFinishedSpeaking:(NSNotification *)notification
{
    [self setStateReady];
}

- (void)handleFinishedPlayingTrackedRotAndPos:(NSNotification *)notification
{
    [self centerXRecognizerViewOverModel:[modelManager getXCenterModel]];
}



-(void)doLoadingModelThread
{
	[self setStateLoadingModel];
}






#pragma mark User Interaction Methods

-(IBAction)clickPlayAudio:(id)sender
{
	@synchronized(self) {
        
        
		if([modelManager isModelLoaded])
		{
			[self showCover];
            [modelManager startHideCounter];
			[self setStatePlaying];
			
			
			[self disableAccelerometer];
			
            [modelManager playRecAudio];
            
            [modelManager startTrackingRotAndPos];

		}
	}
}

-(IBAction)clickTrackModel:(id)sender
{
	@synchronized(self) {
		if([modelManager isModelLoaded])
		{
            [modelManager startHideCounter];
            if(![modelManager isTrackingRotAndPos])
                [modelManager startTrackingRotAndPos];
            
            else
                [modelManager stopTrackingRotAndPos];
		}
	}
}

-(IBAction)clickStopTrackingModel:(id)sender
{
	@synchronized(self) {
        
		if([modelManager isModelLoaded])
		{
            [modelManager startHideCounter];
			
            [modelManager startPlayingTrackedRotAndPos];
            
		}
	}
}


-(ModelManager*)loadModelWithPath:(NSString*)modelName AndBackground:(NSString*)background
{
    [self loadModelWithPath:modelName AndBackground:background AndPerspective:FALSE];
}


-(ModelManager*)loadModelWithPath:(NSString*)modelName AndBackground:(NSString*)background AndPerspective:(BOOL)persp
{
    usePerspective = persp;

    modelManager = [[ModelManager alloc] initWithName:modelName Background:background Width:glView.frame.size.width*glView.layer.contentsScale Height:glView.frame.size.height*glView.layer.contentsScale];

    [self performSelectorInBackground:@selector(loadModelInBackground:) withObject:[self.glView getContext]];

    [self setStateLoadingModel];
    [self createGestureRecognizers:gestureRecognizerView];    
    [self hideCover];
    
    
    return modelManager;
}

-(void) loadModelInBackground:(EAGLContext*) ctx
{

    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    [EAGLContext setCurrentContext:ctx];

    [self.modelManager initCharacterWithPerspective:usePerspective];
    [self.glView.renderer setModelManager:self.modelManager];
    [self setStateReady];
    [self setModelToInitialPosition];
    
    [pool drain]; 
    
    [[NSNotificationCenter defaultCenter] postNotificationName:notificationModelCompletelyLoaded object:nil];
}

-(void)resetOpenGLContext:(EAGLContext*) ctx
{
    self.glView.renderer.context = ctx;
}


- (void)setModelToInitialPosition
{
	//The character must be automatically centered when initialize, so this line, now must be removed	
//	[modelManager rotateModelX:kInitYaw y:kInitPitch absolute:YES];
//    [modelManager rotateModelX:kInitYaw y:kInitPitch absolute:YES];
    
    CGRect newFrame= gestureRecognizerView.frame;
    newFrame.origin.x=glView.renderer.backingWidth/2 - gestureRecognizerView.frame.size.width/2;
    gestureRecognizerView.frame = newFrame; // initial position
    
}


- (void)centerXRecognizerViewOverModel:(int) x
{    
    CGRect newFrame= gestureRecognizerView.frame;
    newFrame.origin.x= x - gestureRecognizerView.frame.size.width/2;
    gestureRecognizerView.frame = newFrame; // initial position
}



- (void)showAlert:(NSString *)alertTitle message:(NSString *)alertMessage
{
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:alertTitle message:alertMessage delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
    dispatch_async(dispatch_get_main_queue(), ^{
        [alert show];
    });
}

UIBarButtonItem* selectedButton;

UIPopoverController* popController;


#pragma mark -
#pragma mark Image Interaction


- (void)pickDefaultBackground
{
	[modelManager setDefaultBackground];
}


#pragma mark -
#pragma mark User-Model interaction

#define kAccelerometerFrequency 100 //Hz
#define kFilteringFactor 0.25


- (void)enableAccelerometer
{	
	//NSString* imageStr = [[NSBundle mainBundle] pathForResource:@"bt_3d" ofType:@"png"];
	//UIImage* image = [UIImage imageWithContentsOfFile:imageStr];
//	perspectiveButton.currentImage = image;
	//[perspectiveButton setImage:image forState:nil];
	
	UIAccelerometer* theAccelerometer = [UIAccelerometer sharedAccelerometer];
	theAccelerometer.updateInterval = 1 / kAccelerometerFrequency;
	
	theAccelerometer.delegate = self;
	accelerometerEnabled = YES;
	initAcc = YES;

}
	
- (void)disableAccelerometer
{
	//NSString* imageStr = [[NSBundle mainBundle] pathForResource:@"bt_3d" ofType:@"png"];
	//UIImage* image = [UIImage imageWithContentsOfFile:imageStr];
//	perspectiveButton.currentImage = image;
	//[perspectiveButton setImage:image forState:nil];
	
	[UIAccelerometer sharedAccelerometer].delegate = nil;
//    [modelManager rotateModelX:0 y:-11 absolute:YES];
	accelerometerEnabled = NO;
	initAcc = YES;
}


- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration
{
	float yaw, pitch;
	
	if (initAcc) {
		initAcc = NO;
		x = acceleration.x;
		y = acceleration.y;
		z = acceleration.z;
		
		initYaw = -(floor(atan2(x,z) * 180 / M_PI));
		initPitch = floor(atan2(y,z) * 180 / M_PI);
		
		return;
	} 
	else {
		x = (acceleration.x * kFilteringFactor) + (x * (1.0 - kFilteringFactor));
		y = (acceleration.y * kFilteringFactor) + (y * (1.0 - kFilteringFactor));
		z = (acceleration.z * kFilteringFactor) + (z * (1.0 - kFilteringFactor));
		
		yaw = -(floor(atan2(x,z) * 180 / M_PI));
		pitch = floor(atan2(y,z) * 180 / M_PI);
		
		yaw -= (initYaw - kInitYaw);
		pitch -= (initPitch - kInitPitch);
	}
	
	//Set the angles to canonical (-180 .. 180)
	if (yaw > 180)			yaw -= 360;
	else if (yaw < -180)	yaw += 360;
	
	if (pitch > 180)		pitch -= 360;
	else if (pitch < -180)	pitch += 360;
	
		
	[modelManager rotateModelX:pitch y:yaw absolute:YES];
}


- (void)createGestureRecognizers:(UIView* )sView
{
	cout << "Creating gesture recognizers" << endl;
	//commented out until kika implements its own animations and it does not do the ones from BasicCharacter instead
	singleFingerDoubleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleDoubleTap:)];
	singleFingerDoubleTap.numberOfTapsRequired = 1;
	[sView addGestureRecognizer:singleFingerDoubleTap];
	[singleFingerDoubleTap release];
	
	
	panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanGesture:)];
	[sView addGestureRecognizer:panGesture];
	[panGesture release];
	
	panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanGesture:)];
	[gestureRecognizerView addGestureRecognizer:panGesture];
	[panGesture release];
	
	pinchGesture = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinchGesture:)];
	[sView addGestureRecognizer:pinchGesture];
	[pinchGesture release];
}

- (void)removeGestureRecognizers:(UIView *)sView
{
	[sView removeGestureRecognizer:singleFingerDoubleTap];
	[sView removeGestureRecognizer:panGesture];
	[sView removeGestureRecognizer:pinchGesture];
    [gestureRecognizerView removeGestureRecognizer:panGesture];
}


- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch* touch = [touches anyObject];
	originPoint = [touch locationInView:self.view];
	cout << "Origin Point: (" << originPoint.x << ", " << originPoint.y << ")" << endl;
}

int auxcounter = 0;

- (void)handleSingleDoubleTap:(UITapGestureRecognizer *)sender
{
	if(cover.hidden)
	{
		
		if([sender state]==UIGestureRecognizerStateEnded){
            self.timeStamp = [self getNowTimeStamp];
            [[NSNotificationCenter defaultCenter] postNotificationName:notificationGestureDetectedDoubleTap object:nil];

        }
	}
	
}

- (void)handlePanGesture:(UIPanGestureRecognizer *)sender
{
	if(cover.hidden)
	{
        [self handleMoveModel:sender];
	}
}

- (void)handleMoveModel:(UIPanGestureRecognizer *)sender
{
	CGPoint position = [sender locationInView:self.view];
	
	CGRect newFrame = sender.view.frame;
			
	newFrame.origin.x += position.x - originPoint.x;
	
	
	CGPoint centralPoint = CGPointMake(newFrame.origin.x + newFrame.size.width/2, 
									   newFrame.origin.y + newFrame.size.height/2);
	    
	if (fabs([sender velocityInView:self.view].x) > fabs([sender velocityInView:self.view].y)) {
        
		if (centralPoint.x>self.view.frame.size.width) {
            newFrame.origin.x=self.view.frame.size.width - newFrame.size.width/2;
            centralPoint.x = self.view.frame.size.width;
        }
        if (centralPoint.x<self.view.frame.origin.x) {
            newFrame.origin.x=self.view.frame.origin.x - newFrame.size.width/2;
            centralPoint.x = self.view.frame.origin.x;            
        }
        
        sender.view.frame = newFrame;
            
		[modelManager moveModelX:centralPoint.x absolute:YES];
        NSLog(@"%@",centralPoint.x);
        

	}
	else {
		[modelManager rotateModelX:(position.y - originPoint.y) * (kMaxPitch - kMinPitch) / newFrame.size.height  
                                 y:0
                                absolute:NO];
	}
    
	originPoint = position;
}


- (void)handleTouchInteractions:(UIPanGestureRecognizer *)sender
{
	if([sender state]==UIGestureRecognizerStateBegan)
	{
		panGestureNumChangesVelocitySignX = 0;
		panGestureNumChangesVelocitySignY = 0;
		panGestureNumChangesVelocitySignX = true;
		panGestureNumChangesVelocitySignX = false;
	}
	else if([sender state]==UIGestureRecognizerStateChanged)
	{
		
		CGPoint velocity = [sender velocityInView:gestureRecognizerView];
		if(velocity.x > 500)
		{
			if(!panGestureVelocityXIsPositive)
				panGestureNumChangesVelocitySignX++;
			panGestureVelocityXIsPositive = true;
		}
		else if(velocity.x < -500)
		{
			if(panGestureVelocityXIsPositive)
				panGestureNumChangesVelocitySignX++;
			panGestureVelocityXIsPositive = false;			
		}
		
		if(velocity.y > 500)
		{
			if(!panGestureVelocityYIsPositive)
				panGestureNumChangesVelocitySignY++;
			panGestureVelocityYIsPositive = true;
		}
		else if(velocity.y < -500)
		{
			if(panGestureVelocityYIsPositive)
				panGestureNumChangesVelocitySignY++;
			panGestureVelocityYIsPositive = false;			
		}
		
		if(panGestureNumChangesVelocitySignX>panGestureNumChangesVelocitySignY)
		{
			if(panGestureNumChangesVelocitySignX > 2){
                self.timeStamp = [self getNowTimeStamp];
                [[NSNotificationCenter defaultCenter] postNotificationName:notificationGestureDetectedScratchX object:nil];
            }
		}
		else if(panGestureNumChangesVelocitySignY > 2){
            self.timeStamp = [self getNowTimeStamp];
            [[NSNotificationCenter defaultCenter] postNotificationName:notificationGestureDetectedScratchX object:nil];
        }
		
	}
	else if([sender state]==UIGestureRecognizerStateEnded)
	{
		if(panGestureNumChangesVelocitySignX>panGestureNumChangesVelocitySignY)
		{
			if(panGestureNumChangesVelocitySignX > 2){
                self.timeStamp = [self getNowTimeStamp];
                [[NSNotificationCenter defaultCenter] postNotificationName:notificationGestureDetectedScratchX object:nil];
            }
		}
		else if(panGestureNumChangesVelocitySignY > 2){
            self.timeStamp = [self getNowTimeStamp];
                [[NSNotificationCenter defaultCenter] postNotificationName:notificationGestureDetectedScratchX object:nil];
        }
	}
}


- (void)handlePinchGesture:(UIPinchGestureRecognizer *)sender
{
    
	cout << "Pinching" << endl;
    [[NSNotificationCenter defaultCenter] postNotificationName:notificationGestureDetectedPinch object:nil];
//	CGFloat factor = [(UIPinchGestureRecognizer *)sender scale];
//	cout << "Pinch Factor: " << factor << endl ;	
//	double linFactor = log10((double)factor);
	
}





@end
