//
//  NrMainViewController.m
//  VirtualAssistant
//
//  Created by Enrique on 10/8/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NrMainViewController.h"

#import "NaradaiSpeechTTS.h"
#import "NrAppDelegate.h"
#import "NrViewController.h"
#import "EAGLView.h"

#import "NSFileManager+Regular.h"

@interface NrMainViewController ()

@end

@implementation NrMainViewController

@synthesize navController;
@synthesize coverView;
@synthesize assistantView;
@synthesize detailView;
@synthesize shareButton;
@synthesize daysView;
@synthesize glController;
@synthesize activityIndicator;
@synthesize pointingBar;
@synthesize pendingTTS;
@synthesize filesToSpeak;
@synthesize selectedItem;
@synthesize contentsView;
@synthesize currentMode;


@synthesize shuttingUp = _shuttingUp;


int nextIndex;
BOOL speaking;
BOOL userInteractionEnabled;
BOOL shuttingUp = NO;

CGFloat pointingBarYMargin = 2;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        
        NrAppDelegate *delegate = [UIApplication sharedApplication].delegate;
        NrViewController *viewController = delegate.viewController;
        self.glController = viewController.glController;
        
        [self setNotificationListeners];
        
        self.initY = [self.glController.modelManager modelPositionY];
        
        self.pendingTTS = [NSMutableArray array];
        
        self.selectedItem = -1;
        
        self.shuttingUp = NO;
        
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    [self createCover];
    [self setGestureRecognizerListeners];
    self.assistantView.backgroundColor = [UIColor clearColor];
    
    
}



- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    [self.glController.glView stopAnimation];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)loadPointingBarFromFrame:(CGRect)itemFrame
{
    CGFloat itemWidth = itemFrame.size.width;
    CGFloat itemHeight = 5;
    
    
    self.pointingBar = [[UIView alloc] initWithFrame:CGRectMake(self.view.frame.size.width, self.daysView.frame.origin.y + self.daysView.frame.size.height + pointingBarYMargin, itemWidth * 0.5, itemHeight)];
    
    self.pointingBar.backgroundColor = [UIColor colorWithRed:224.0f/255.0f green:161.0f/255.0f blue:34.0f/255.0f alpha:255.0f/255.0f];
    
    self.pointingBar.alpha = 0.0f;
    
    
    [self.contentsView addSubview:self.pointingBar];
}

- (void)movePointingBarToFrame:(CGRect)baseframe
{
    CGRect finalFrame = CGRectMake(baseframe.origin.x + (baseframe.size.width - self.pointingBar.frame.size.width)/2,
                                   baseframe.origin.y + baseframe.size.height + pointingBarYMargin,
                                   self.pointingBar.frame.size.width,
                                   self.pointingBar.frame.size.height);
    
    if (self.pointingBar.alpha == 0.0f) {
        self.pointingBar.frame = finalFrame;
        [UIView animateWithDuration:0.3 animations:^{
            self.pointingBar.alpha = 1.0f;
        }];
    }
    else {
        [UIView animateWithDuration:0.3 animations:^{
            self.pointingBar.frame = finalFrame;
        }];
    }
}

#pragma mark -
#pragma mark Notification Setup Methods

- (void)setNotificationListeners
{
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleFinishedSpeaking:) name:notificationFinishedSpeaking object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleDidFailSpeaking:withErrorInfo:) name:notificationDidFailSpeaking object:nil];
    
}

#pragma mark -
#pragma mark Gesture Recognizer Handlers

- (void)setGestureRecognizerListeners
{
    UISwipeGestureRecognizer *swipeLeft = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeLeftFrom:)];
    swipeLeft.direction = UISwipeGestureRecognizerDirectionLeft;
    
    UISwipeGestureRecognizer *swipeRight = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeRightForm:)];
    swipeRight.direction = UISwipeGestureRecognizerDirectionRight;
    
    UISwipeGestureRecognizer *swipeUp = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeUpForm:)];
    swipeUp.direction = UISwipeGestureRecognizerDirectionUp;
    
    UIPanGestureRecognizer *panHorizontal = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanGesture:)];

    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTap:)];
    [self.detailView addGestureRecognizer:tap];
    // [self.view addGestureRecognizer:swipeUp];
    //    [self.detailView addGestureRecognizer:panHorizontal];
}

#pragma mark -
#pragma mark TTS delegate methods

int totalTTS = 0;

- (void)speakSentences:(NSArray *)sentences withMaxLength:(NSInteger)maxLength toFileName:(NSString *)fileName inLanguage:(NSString *)lang
{
    NSArray *choppedEvents = [NaradaTTS chopSentences:sentences withMaxLength:maxLength];
    
    int count = 0;
    nextIndex = count;
    totalTTS = [choppedEvents count];
    
    for (NSString *event in choppedEvents) {
        NSLog(@"Event title: %@", event);
        
        NaradaTTS *gTTS = [[NaradaiSpeechTTS alloc] initWithID:count textToSpeak:event fileName:fileName language:lang isFemale:YES andDelegate:self];
        
        NSLog(@"Created TTS #%d", count);
        
        if (count == [choppedEvents count]) {
            gTTS.last = YES;
        }
        [gTTS requestTTS];
        count ++;
    }
}

- (BOOL)canSpeak
{
    return !speaking;
}

// Finished downloading the audio (in mp3)
- (void)NaradaTTSDidFinish:(NaradaTTS *)ngtts
{
    NSLog(@"Appending TTS #%d", ngtts.ttsID);
    [self.pendingTTS addObject:ngtts];
    
    if ([self.pendingTTS count] == totalTTS) {
        
        
        NSLog(@"Next index is: %d", nextIndex);
        for (NaradaTTS *ngtts in self.pendingTTS) {
            if (ngtts.ttsID == nextIndex) {
                [ngtts requestConversion];
                [self.pendingTTS removeObject:ngtts];
                break;
            }
        }
        nextIndex ++;
        
        return;
    } else {
        return;
    }
    
    
    NSLog(@"TTS #%d did finish", ngtts.ttsID);
    
    if ([self canSpeak] && ngtts.ttsID == nextIndex)
    {
        ngtts.last ? nextIndex = 0 : nextIndex ++;
        speaking = YES;
        
        [ngtts requestConversion];
        
        return;
    }
    
    NSLog(@"Appending TTS #%d", ngtts.ttsID);
    [self.pendingTTS addObject:ngtts];
    
    NSLog(@"Size of PendingTTS is %d", [self.pendingTTS count]);
    
    NSAssert(self.pendingTTS, @"PendingTTS is nil");
}

- (void)NaradaTTSDidFailWithError:(NSError *)error
{
    @synchronized(self) {
        if (error == nil) {
            return;
        }
        
        speaking = NO;
        
        NSLog(@"Error occurred while doing TTS, error: %@", [error localizedDescription]);
        
        UIAlertView *errorAlertView = [[UIAlertView alloc] init];
        errorAlertView.title = NSLocalizedString(@"ERROR", nil);
        errorAlertView.message = [error localizedDescription];
        [errorAlertView addButtonWithTitle:NSLocalizedString(@"ACCEPT", nil)];
        
        [errorAlertView show];
        
        [self uncover];
        self.activityIndicator.hidden = YES;
        
        [self.pendingTTS removeAllObjects];
        
        //       [self handleDidFailSpeaking];
    }
}

- (void)naradaTTS:(NaradaTTS *)ngtts didFailConvertingWithError:(NSError *)error
{
    // If the error was due to audio size equals to 0, request it again.
    if (error.code == -758) {
        [ngtts requestConversion];
    }
}


// Finished converting tts to pcm
- (void)naradaTTSDidFinishConvertingAudio:(NaradaTTS *)ngtts
{
    NSLog(@"Speaking TTS #%d", ngtts.ttsID);
    NSLog(@"Text to speak is %@", ngtts.text);
    
    //Speak
    NSString *wavFilePath = [self addHeaderToWavFile:ngtts.pcmAudioPath];
    
    [self.glController.modelManager repeatAudioWithAbsoluteAudioPath:wavFilePath temporary:YES];
    [self handleWillBeginSpeaking];
}

- (NSString *)addHeaderToWavFile:(NSString *)pcmFilePath
{
    NSString *wavFilePath = [pcmFilePath stringByReplacingOccurrencesOfString:@".pcm" withString:@".wav"];
    
    NSFileManager *fm = [NSFileManager defaultManager];
    NSData *data = [fm contentsAtPath:pcmFilePath];
    
    const void *bytes = [data bytes];
    int dataSize = [data length];
    
    NaradaAudioResource *audioResource = new NaradaAudioResource();
    
    audioResource->addFromCapture((void*)bytes, dataSize/2, AL_FORMAT_MONO16, 16000, 0);
    audioResource->writeToWAV((char *)[wavFilePath UTF8String]);
    
    delete audioResource;
    
    NSError *error;
    if (![fm removeItemAtPath:pcmFilePath error:&error]) {
        NSLog(@"Error removing PCM path: %@", [error localizedDescription]);
    }
    
    return wavFilePath;
}

- (void)handleDidFailSpeaking:(NSNotification *)notification withErrorInfo:(NSDictionary *)info
{
    NSError * error = [[notification userInfo] objectForKey:@"error"];
    [self performSelectorOnMainThread:@selector(NaradaTTSDidFailWithError:) withObject:error waitUntilDone:YES];
}

- (void)handleFinishedSpeaking:(NSNotification *)notification
{
    NSLog(@"Model did finish Speaking");
    NSLog(@"Next index is %d", nextIndex);
    speaking = NO;
    NSLog(@"Pending TTS is %d", [self.pendingTTS count]);
    
    if ([self.pendingTTS count] && !self.shuttingUp) {
        
        for (NaradaTTS *ngtts in self.pendingTTS)
        {
            if (ngtts.ttsID == nextIndex ) {
                NSLog(@"TTS #%d present", ngtts.ttsID);
                NSLog(@"Text to speak is %@", ngtts.text);
                
                ngtts.last ? nextIndex = 0 : nextIndex ++;
                speaking = YES;
                
                [ngtts requestConversion];
                [self.pendingTTS removeObject:ngtts];
                
                return;
            }
        }
    }
    else if ([self.filesToSpeak count] && !self.shuttingUp) {
        NSLog(@"BUG: Files to Speak: %@", self.filesToSpeak);
        //        [self.glController.modelManager repeatAudioWithAbsoluteAudioPath:[self.filesToSpeak objectAtIndex:0] temporary:YES];
        
        NSDate *future = [NSDate dateWithTimeIntervalSinceNow:3];
        [NSThread sleepUntilDate:future];
        
        [self.glController.modelManager playTemporaryAction:[self.filesToSpeak objectAtIndex:0] withPath:NSTemporaryDirectory() andDelay:0];
        if ([self.filesToSpeak count])
            [self.filesToSpeak removeObjectAtIndex:0];
    }
    else {
        NSLog(@"Finished speaking all");
        [self handleDidFinishSpeaking];
        self.shuttingUp = NO;
    }
}

- (void)speakNextSentence:(id)sender
{
    [self.glController.modelManager playTemporaryAction:[self.filesToSpeak objectAtIndex:0] withPath:NSTemporaryDirectory() andDelay:0];
    if ([self.filesToSpeak count])
        [self.filesToSpeak removeObjectAtIndex:0];
}

- (void)shutUpModel
{
    [self.glController.glView stopAnimation];
}



#pragma mark -
#pragma mark Methods for the Coverview

- (void)createCover
{
    coverView = [[UIView alloc] initWithFrame:self.view.frame];
    coverView.backgroundColor = [UIColor clearColor];
    coverView.hidden = YES;
    [self.view addSubview:coverView];
}

- (BOOL)isCovered
{
    return !self.coverView.hidden;
}

- (void)cover
{
    NSAssert(self.coverView != nil, @"Cover: coverView is nil, so aborting program");
    self.coverView.hidden = NO;
}

- (void)uncover
{
    NSAssert(self.coverView != nil, @"Uncover: coverView is nil, so aborting program");
    self.coverView.hidden = YES;
}

- (void)shutUp
{
    self.shuttingUp = YES;
    [self.glController.modelManager shutUp];
}

@end
