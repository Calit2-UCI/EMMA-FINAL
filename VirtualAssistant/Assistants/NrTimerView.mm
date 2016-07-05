//
//  Nr.m
//  GolfAssistant
//
//  Created by Sergio Gago on 5/27/13.
//  Copyright (c) 2013 Narada Robotics S.L. All rights reserved.
//
/*
#import "NrTimerView.h"
#import "NrMainViewController.h"

@implementation NrTimerView

@synthesize timerButton = _timerButton;
@synthesize timerCounterLabel = _timerCounterLabel;

BOOL started;
NSTimer *timer;
int counter;
int CurrentData = 0;

- (id)initWithMainViewController:(NrCalendarMainViewController *)controller
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad ) {
        self = [[[NSBundle mainBundle] loadNibNamed:@"NrTimerView_iPad" owner:self options:nil] objectAtIndex:0];
    }
    if ((self)) {
        self.mainViewController = controller;
    }
    
    return self;
}

- (IBAction)HideAndRemoveDetailView:(id)sender
{
    NSLog(@"Hello Answer 1");
    [self.mainViewController HideAndRemoveDetailView];
}

- (void)viewDidLoad
{
    //  [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    started = NO;
    counter = 0;
    self.timerCounterLabel.text = @"0";
}

- (void)didReceiveMemoryWarning
{
    //   [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)timerButtonClicked:(id)sender
{
    if (started) {
        started = NO;
        [self.timerButton setTitle:@"Start" forState:UIControlStateNormal];
        [self stopTimer];
    }
    else {
        started = YES;
        [self.timerButton setTitle:@"Stop" forState:UIControlStateNormal];
        [self startTimer];
    }
}

- (void)increaseTimerCount
{
    self.timerCounterLabel.text = [NSString stringWithFormat:@"%d", counter];
    
    if (started) {
        counter ++;
    }
    
    
    NaradaDownloader *downloader = [[NaradaDownloader alloc] initWithURLString:@"http://128.195.185.104:8080/musicglove/resources/saves/temp/RIVA_log.txt" andDelegate:sselg;
    [downloader requestDownload];
}

- (void)startTimer
{
    timer = [NSTimer scheduledTimerWithTimeInterval:10 target:self selector:@selector(increaseTimerCount) userInfo:nil repeats:YES];
    [timer fire];
}

- (void)stopTimer
{
    [timer invalidate];
    counter = 0;
    self.timerCounterLabel.text = @"0";
}

#pragma mark -
#pragma mark Delegate methods for NaradaDownloader

/* Golf PLatform
 - (void)naradaDownlaoderDidFinishDownloading:(NaradaDownloader *)downloader
 {
 NSLog(@"Did finish successfully!!");
 NSLog(@"String downloaded: %@", downloader.receivedString);
 
 //MoCaData.txt is something like:"TrialNum:n1,IV:e1,IA:e2,PCP:e3,Feedback_1:f1,Feedback_2:f2,Feedback_3:f3"
 
 NSString *MoCaData = downloader.receivedString;
 NSArray *parsedArray = [MoCaData componentsSeparatedByString:@","];
 NSInteger TrialNum, Feedback, Coaching;
 //    NSString *sentence = @"";
 for (NSString *valueStr in parsedArray) {
 //Separem els components per dos punts
 NSArray *keyValArray = [valueStr componentsSeparatedByString:@":"];
 NSString *keyword = [keyValArray objectAtIndex:0];
 //Si el primer valor és Position, desa el segon valor a la variable position, com a NSInteger
 if ([keyword isEqualToString:@"TrialNum"]) {
 TrialNum = [[keyValArray objectAtIndex:1] intValue];
 }
 //Si el primer valor és Strength, desa el segon valor a la variable strength, com a NSInteger
 else if ([keyword isEqualToString:@"Feedback"]) {
 Feedback = [[keyValArray objectAtIndex:1] intValue];
 }
 //Si el primer valor és Speed, desa el segon valor a la variable speed, com a NSInteger
 else if ([keyword isEqualToString:@"Coaching"]) {
 Coaching = [[keyValArray objectAtIndex:1] intValue];
 }
 
 }
 NSLog(@"Current trial number is %ld", (long)CurrentTrial);
 if (CurrentTrial == TrialNum){
 NSLog(@"Waiting");
 NSLog(@"Trial number is %ld", (long)TrialNum);
 }
 else {
 NSLog(@"Action required!!");
 CurrentTrial = TrialNum;
 NSLog(@"Trial number is %ld", (long)TrialNum);
 //FEEDBACK
 if (Feedback == 1){
 [self.mainViewController speakIV1];
 }
 if (Feedback == 2){
 [self.mainViewController speakIV2];
 }
 if (Feedback == 3){
 [self.mainViewController speakIV3];
 }
 if (Feedback == 4){
 [self.mainViewController speakIA1];
 }
 if (Feedback == 5){
 [self.mainViewController speakIA2];
 }
 if (Feedback == 6){
 [self.mainViewController speakPCP1];
 }
 if (Feedback == 7){
 [self.mainViewController speakPCP2];
 }
 
 
 // COACHING
 if (Coaching == 1){
 [self.mainViewController performSelector:@selector(speakHM) withObject:nil afterDelay:7];
 }
 if (Coaching == 2){
 [self.mainViewController performSelector:@selector(speakSH) withObject:nil afterDelay:7];
 }
 if (Coaching == 3){
 [self.mainViewController performSelector:@selector(speakSR1) withObject:nil afterDelay:7];
 }
 if (Coaching == 4){
 [self.mainViewController performSelector:@selector(speakSR2) withObject:nil afterDelay:7];
 }
 if (Coaching == 5){
 [self.mainViewController performSelector:@selector(speakAA1) withObject:nil afterDelay:7];
 }
 if (Coaching == 6){
 [self.mainViewController performSelector:@selector(speakAA2) withObject:nil afterDelay:7];
 }
 if (Coaching == 7){
 [self.mainViewController performSelector:@selector(speakST1) withObject:nil afterDelay:7];
 }
 if (Coaching == 8){
 [self.mainViewController performSelector:@selector(speakST2) withObject:nil afterDelay:7];
 }
 else {
 NSLog(@"No data detected!!");
 CurrentTrial = TrialNum;    };
 }
 
 }
 
 */
/* MUSICGLOVE PLatform

- (void)naradaDownlaoderDidFinishDownloading:(NaradaDownloader *)downloader
{
    NSLog(@"Did finish successfully!!");
    NSLog(@"String downloaded: %@", downloader.receivedString);
    
    //RIVA_log.txt is something like:"NewData:0;ENCOURAGEMENT:0;WORST_GRIP:0;BEST_GRIP:0"
    
    NSString *MGLogData = downloader.receivedString;
    NSArray *parsedArray = [MGLogData componentsSeparatedByString:@";"];
    NSInteger NewData;
    NSString *ENCOURAGEMENT_WAV = @"noaudio";
    NSString *WORST_GRIP_WAV =  @"noaudio";
    NSString *BEST_GRIP_WAV =  @"noaudio";
    NSString *ENCOURAGEMENT =  @"noaudio";
    NSString *WORST_GRIP =  @"noaudio";
    NSString *BEST_GRIP =  @"noaudio";
    
    
    for (NSString *valueStr in parsedArray) {
        //split the components by column
        NSArray *keyValArray = [valueStr componentsSeparatedByString:@":"];
        NSString *keyword = [keyValArray objectAtIndex:0];
        
        if ([keyword isEqualToString:@"NewData"]) {
            NewData = [[keyValArray objectAtIndex:1] intValue];
        }
        
        else if ([keyword isEqualToString:@"ENCOURAGEMENT"]) {
            ENCOURAGEMENT = [keyValArray objectAtIndex:1];
        }
        
        else if ([keyword isEqualToString:@"WORST_GRIP"]) {
            WORST_GRIP = [keyValArray objectAtIndex:1];
        }
        
        else if ([keyword isEqualToString:@"BEST_GRIP"]) {
            BEST_GRIP = [keyValArray objectAtIndex:1];
        }
        
    }
    NSLog(@"CurrentData number is %ld", (long)CurrentData);
    NSLog(@"NewData number is %ld", (long)NewData);
    if (NewData == CurrentData or NewData == 0){
        NSLog(@"Waiting");
    }
    else {
        
        /*
         NSLog(@"Text to speech is %@", TTS);
         NSArray *sentences = [NSArray arrayWithObjects:
         @"Hello how are you?", @"I am fine, thanks!",
         nil];
         [self.mainViewController speakSentences:sentences withMaxLength:200 toFileName:@"speakexp1" inLanguage:NSLocalizedString(@"LANG_TTS", nil)];
         */
        
        //    processing = NO;
        //    [self performSelector:@selector(speakExperiment) withObject:nil afterDelay:1];
/*
        CurrentData = NewData;
        
        if ([ENCOURAGEMENT isEqualToString:@"1"]) {
            ENCOURAGEMENT_WAV = @"ENCOURAGEMENT_1.wav";
        }
        
        else if ([ENCOURAGEMENT isEqualToString:@"2"]) {
            ENCOURAGEMENT_WAV = @"ENCOURAGEMENT_2.wav";
        }
        
        else if ([ENCOURAGEMENT isEqualToString:@"3"]) {
            ENCOURAGEMENT_WAV = @"ENCOURAGEMENT_3.wav";
        }
        
        else if ([ENCOURAGEMENT isEqualToString:@"4"]) {
            ENCOURAGEMENT_WAV = @"ENCOURAGEMENT_4.wav";
        }
        
        else if ([ENCOURAGEMENT isEqualToString:@"5"]) {
            ENCOURAGEMENT_WAV = @"ENCOURAGEMENT_5.wav";
        }
        else if ([ENCOURAGEMENT isEqualToString:@"6"]) {
            ENCOURAGEMENT_WAV = @"ENCOURAGEMENT_6.wav";
        }
        else {
            ENCOURAGEMENT_WAV = @"noaudio";
        }
        
        
        NSLog(@"ENCOURAGEMENT_WAV is %@", ENCOURAGEMENT_WAV);
        
        //Blue Grip
        
        if ([WORST_GRIP isEqualToString:@"1_B"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_1_Blue.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"2_B"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_2_Blue.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"3_B"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_3_Blue.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"4_B"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_4_Blue.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"5_B"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_5_Blue.wav";
        }
        
        //Green Grip
        
        else if ([WORST_GRIP isEqualToString:@"1_G"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_1_Green.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"2_G"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_2_Green.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"3_G"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_3_Green.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"4_G"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_4_Green.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"5_G"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_5_Green.wav";
        }
        
        //Red Grip
        
        else if ([WORST_GRIP isEqualToString:@"1_P"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_1_Purple.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"2_P"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_2_Purple.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"3_P"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_3_Purple.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"4_P"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_4_Purple.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"5_P"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_5_Purple.wav";
        }
        
        
        //Red Grip
        
        else if ([WORST_GRIP isEqualToString:@"1_R"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_1_Red.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"2_R"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_2_Red.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"3_R"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_3_Red.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"4_R"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_4_Red.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"5_R"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_5_Red.wav";
        }
        
        
        //Yellow Grip
        
        else if ([WORST_GRIP isEqualToString:@"1_Y"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_1_Yellow.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"2_Y"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_2_Yellow.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"3_Y"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_3_Yellow.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"4_Y"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_4_Yellow.wav";
        }
        
        else if ([WORST_GRIP isEqualToString:@"5_Y"]) {
            WORST_GRIP_WAV = @"WORST_GRIP_5_Yellow.wav";
        }
        
        //No feedback for worst grip
        
        else {
            WORST_GRIP_WAV = @"noaudio";
        }
        
        NSLog(@"WORST_GRIP_WAV is %@", WORST_GRIP_WAV);
        
        //BEST GRIP
        //Blue Grip
        
        if ([BEST_GRIP isEqualToString:@"1_B"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_1_Blue.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"2_B"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_2_Blue.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"3_B"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_3_Blue.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"4_B"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_4_Blue.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"5_B"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_5_Blue.wav";
        }
        
        //Green Grip
        
        else if ([BEST_GRIP isEqualToString:@"1_G"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_1_Green.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"2_G"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_2_Green.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"3_G"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_3_Green.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"4_G"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_4_Green.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"5_G"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_5_Green.wav";
        }
        
        //Purple Grip
        
        else if ([BEST_GRIP isEqualToString:@"1_P"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_1_Purple.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"2_P"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_2_Purple.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"3_P"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_3_Purple.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"4_P"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_4_Purple.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"5_P"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_5_Purple.wav";
        }
        
        //Red Grip
        
        else if ([BEST_GRIP isEqualToString:@"1_R"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_1_Red.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"2_R"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_2_Red.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"3_R"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_3_Red.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"4_R"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_4_Red.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"5_R"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_5_Red.wav";
        }
        
        //Yellow Grip
        
        else if ([BEST_GRIP isEqualToString:@"1_Y"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_1_Yellow.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"2_Y"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_2_Yellow.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"3_Y"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_3_Yellow.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"4_Y"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_4_Yellow.wav";
        }
        
        else if ([BEST_GRIP isEqualToString:@"5_Y"]) {
            BEST_GRIP_WAV = @"BEST_GRIP_5_Yellow.wav";
        }
        
        //No feedback for BEST grip
        
        else {
            BEST_GRIP_WAV = @"noaudio";
        }
        
        NSLog(@"BEST_GRIP_WAV is %@", BEST_GRIP_WAV);
        
        
        if ([ENCOURAGEMENT_WAV isEqualToString:@"noaudio"]){
            if ([WORST_GRIP_WAV isEqualToString:@"noaudio"]){
                if ([BEST_GRIP_WAV isEqualToString:@"noaudio"]){
                    //do nothing
                }
                else {
                    [self speak1Item:(BEST_GRIP_WAV)];
                }
            }
            else {
                if ([BEST_GRIP_WAV isEqualToString:@"noaudio"]){
                    [self speak1Item:(WORST_GRIP_WAV)];
                }
                else {
                    //    self speakWORST_GRIP_&_BEST_GRIP;
                }
            }
        }
        
        else {
            if ([WORST_GRIP_WAV isEqualToString:@"noaudio"]){
                if ([BEST_GRIP_WAV isEqualToString:@"noaudio"]){
                    [self speak1Item:(ENCOURAGEMENT_WAV)];
                }
                else {
                    [self speak2Item:(ENCOURAGEMENT_WAV):(BEST_GRIP_WAV)];
                }
            }
            else {
                if ([BEST_GRIP_WAV isEqualToString:@"noaudio"]){
                    [self speak2Item:(ENCOURAGEMENT_WAV):(WORST_GRIP_WAV)];
                }
                else {
                    [self speak3Item:(ENCOURAGEMENT_WAV):(WORST_GRIP_WAV):(BEST_GRIP_WAV)];
                }
            }
        }
    }
}


-(void) speak1Item:(NSString*)FileName
{
    NSString *basepath = NSTemporaryDirectory();
    self.mainViewController.filesToSpeak = [NSMutableArray arrayWithObjects:FileName, nil];
    for (NSString * file in self.mainViewController.filesToSpeak) {
        NSAssert1([[NSFileManager defaultManager] fileExistsAtPath:[NSTemporaryDirectory() stringByAppendingPathComponent:file]], @"File does not exist, %@", file);
    }
    if ([self.mainViewController.filesToSpeak count] > 0) {
        [self.mainViewController.glController.modelManager playTemporaryAction:[self.mainViewController.filesToSpeak objectAtIndex:0] withPath:basepath andDelay:0];
        [self.mainViewController.filesToSpeak removeObjectAtIndex:0];
    }
}

-(void) speak2Item:(NSString*)FileName1:(NSString*)FileName2
{
    NSString *basepath = NSTemporaryDirectory();
    self.mainViewController.filesToSpeak = [NSMutableArray arrayWithObjects:FileName1,FileName2, nil];
    for (NSString * file in self.mainViewController.filesToSpeak) {
        NSAssert1([[NSFileManager defaultManager] fileExistsAtPath:[NSTemporaryDirectory() stringByAppendingPathComponent:file]], @"File does not exist, %@", file);
    }
    if ([self.mainViewController.filesToSpeak count] > 0) {
        [self.mainViewController.glController.modelManager playTemporaryAction:[self.mainViewController.filesToSpeak objectAtIndex:0] withPath:basepath andDelay:0];
        [self.mainViewController.filesToSpeak removeObjectAtIndex:0];
    }
}

-(void) speak3Item:(NSString*)FileName1:(NSString*)FileName2:(NSString*)FileName3
{
    NSString *basepath = NSTemporaryDirectory();
    self.mainViewController.filesToSpeak = [NSMutableArray arrayWithObjects:FileName1,FileName2,FileName3, nil];
    for (NSString * file in self.mainViewController.filesToSpeak) {
        NSAssert1([[NSFileManager defaultManager] fileExistsAtPath:[NSTemporaryDirectory() stringByAppendingPathComponent:file]], @"File does not exist, %@", file);
    }
    if ([self.mainViewController.filesToSpeak count] > 0) {
        [self.mainViewController.glController.modelManager playTemporaryAction:[self.mainViewController.filesToSpeak objectAtIndex:0] withPath:basepath andDelay:0];
        [self.mainViewController.filesToSpeak removeObjectAtIndex:0];
    }
}

- (void)naradaDownloader:(NaradaDownloader *)downloader didFailWithError:(NSError *)error
{
    UIAlertView *alert = [[UIAlertView alloc] init];
    alert.title = @"Error";
    alert.message = error.localizedDescription;
    alert.delegate = self;
    [alert addButtonWithTitle:@"Ok"];
    [alert show];
}

@end
*/
