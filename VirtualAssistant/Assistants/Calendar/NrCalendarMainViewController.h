//
//  NrCalendarMainViewController.h
//  VirtualAssistant
//
//  Created by Enrique on 9/28/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "NrMainViewController.h"
#import "GLViewController.h"
#import "NrMainItemView.h"
#import "NrDeviceItemView.h"
#import "NrStationItemView.h"
#import "NaradaDownloader.h"


#import <MediaPlayer/MediaPlayer.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>

/*
 #define kLocationNameKey @"kLocationNameKey"
 #define kLocationCityCodeKey @"kCityCodeKey"
 #define kLocationZipCodeKey @"kZipCodeKey"
 #define kLocationCountryKey @"kCountryKey"
 #define kLocationStateKey @"kStateKey"
 #define kOpenInfoView @"kOpenInfoView"
 #define kApplicationUnlocked @"kApplicationUnlocked"
 #define notificationWakeFromLocalNotification @"notificationWakeFromLocalNotification"
 #define kVotedForHoroscope      @"kVotedForHoroscope"
 #define kVotedForRestaurants    @"kVotedForRestaurants"
 #define kVotedForPubs           @"kVotedForPubs"
 #define kVotedForFlights        @"kVotedForFlights"
 #define kVotedForHotels         @"kVotedForHotels"
 #define kVotedForCarRental      @"kVotedForCarRental"
 #ifdef __APPLE__
 #include "TargetConditionals.h"
 #endif
 enum voteOptions {
 NO_VOTE,
 HOROSCOPE,
 RESTAURANTS,
 PUBS,
 FLIGHTS,
 HOTELS,
 CARRENTAL
 };
 */


@interface NrCalendarMainViewController : NrMainViewController <
UITableViewDataSource,
UITableViewDelegate,
NSURLConnectionDataDelegate,
NaradaDownloaderDelegate
>


// UI elements

@property (nonatomic, strong) NSString *selectedDayName;

@property (nonatomic, strong) UIScrollView *scroll;

@property (nonatomic, strong) NSMutableArray *itemList;
@property (nonatomic) NSInteger stationChosen;
@property (nonatomic) NSString *currentStation;
@property (nonatomic) NSString * preStation;
@property (nonatomic) NSMutableDictionary *action_flags;
@property (nonatomic) NSString * lampValue;
@property (nonatomic) NSString * fanValue;
@property (strong, nonatomic) AVAudioPlayer *audioPlayer;

- (IBAction)mainItemViewClicked:(id)sender;
- (IBAction)deviceItemViewClicked:(id)sender;
- (IBAction)stationItemViewClicked:(id)sender;
- (IBAction)backButtonClicked:(id)sender;
- (IBAction)skipButtonClicked:(id)sender;
- (IBAction)exitButtonClicked:(id)sender;

- (void)loadMainPointingBar;
- (void)loadDevicePointingBar;

- (void)movePointingBarToItem:(NrMainItemView *)item;

// Menu-loading methods
- (void)loadScrollMainItem;
- (void)loadScrollDeviceItem;
- (void)loadWeatherIcons;

// Menu-filling methods
- (void)fillDayItemWithMainData:(NrMainItemView *)item;
- (void)fillDayItemWithDeviceData:(NrDeviceItemView *)item;


// Menu-changing methods
- (void)changeItemsTo:(NSInteger)itemID;
- (void)changeItemsToMain;
- (void)changeItemsToTest;
- (void)changeItemstoDevice;

- (void)hideWeatherButtons;

// Speak methods
-(void) speakAction:(NSString *)sentence;

//data update
-(NSData *) dataUpdated;
-(NSDictionary*)parseJSON: (NSData *) downloaded_data;
// Video-view methods
-(void) MoveModelOut;
-(void) MoveModelIn;
-(void) MoveModelOutWithDelay:(float)delay;
-(void) MoveModelInWithDelay:(float)delay;
-(void) ShowDetailView;
-(void) HideAndRemoveDetailView;
-(void) ShowDetailViewWithDelay:(float)delay;
-(void) HideAndRemoveDetailViewWithDelay:(float)delay;
-(void) LoadDetailViewWithWeatherImage;
-(void) speakLG;
-(void) speakLGWithDelay:(float)delay;
-(void) speakGBTM;
-(void) speakGood1WithDelay:(float)delay;
-(void)StopAndHideVideoViewClicked;
-(void)playMovieClicked;

// NotificationView methods

@end
