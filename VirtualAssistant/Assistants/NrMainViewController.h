//
//  NrMainViewController.h
//  VirtualAssistant
//
//  Created by Enrique on 10/8/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "NrMainViewControllerDelegate.h"
#import "NaradaTTSDelegate.h"
#import "NaradaAudioConverter.h"
#import "GLViewController.h"
#import "NaradaAudioResource.hpp"

#import "NrTableViewController.h"


#define notificationFilesReceived @"notificationFilesReceived"

enum nrMode {
    NR_MAIN,
    NR_STATION,
    NR_ACTIONS, /* EDITED */
    NR_DEVICE,
    NR_VIDEO,
    NR_OVERVIEW_TABLE,
    NR_SMART_TABLE
};

@interface NrMainViewController : UIViewController <
NaradaTTSDelegate,
NrMainViewControllerDelegate,
NaradaAudioConverterDelegate
>


@property (nonatomic, strong) UINavigationController *navController;
@property (nonatomic, strong) GLViewController *glController;
@property (strong, nonatomic) IBOutlet UILabel *TimerButton;





@property (nonatomic, readonly) UIView *coverView;
@property (nonatomic, strong) IBOutlet UIView *daysView;
@property (nonatomic, strong) IBOutlet UIView *assistantView;
@property (nonatomic, strong) IBOutlet UIView *detailView;
@property (nonatomic, strong) IBOutlet UIActivityIndicatorView *activityIndicator;
@property (nonatomic, strong) IBOutlet UIButton *shareButton;
@property (nonatomic, strong) IBOutlet UILabel *bottomCityNameLabel;
@property (nonatomic, strong) IBOutlet UIButton *bottomCityNameButton;
@property (nonatomic, strong) IBOutlet UIButton *moreInfoButton;
@property (nonatomic, strong) IBOutlet UIView *switchMoreInfoView;


@property (nonatomic, strong) IBOutlet UIButton *backButton;
@property (nonatomic, strong) IBOutlet UIButton *skipButton;
@property (nonatomic, strong) IBOutlet UIButton *exitButton;

@property (nonatomic, strong) UITableView *eventsTableView;

@property (nonatomic, assign) enum nrMode currentMode;

@property (nonatomic, assign) NSInteger selectedItem;

@property (nonatomic, strong) UIView *pointingBar;
@property (nonatomic, strong) UIView *contentsView;

@property (nonatomic, strong) NSMutableArray *pendingTTS;
@property (nonatomic, strong) NSMutableArray *filesToSpeak;
@property (nonatomic, assign) CGFloat initY;


// Details View For Weather
@property (nonatomic, strong) IBOutlet UIView *detailWeatherView;
@property (nonatomic, strong) IBOutlet UILabel *detailDayNameLabel;
@property (nonatomic, strong) IBOutlet UILabel *detailPredictionLabel;
@property (nonatomic, strong) IBOutlet UILabel *detailMinTempLabel;
@property (nonatomic, strong) IBOutlet UILabel *detailMaxTempLabel;
@property (nonatomic, strong) IBOutlet UILabel *detailWindSpeedLabel;
@property (nonatomic, strong) IBOutlet UILabel *detailHumidityLabel;
@property (nonatomic, strong) IBOutlet UILabel *detailFeelsLikeTempLabel;
@property (nonatomic, strong) IBOutlet UIImageView *detailWeatherImg;


@property (nonatomic, assign) BOOL shuttingUp;

- (void)loadPointingBarFromFrame:(CGRect)itemFrame;
- (void)movePointingBarToFrame:(CGRect)baseframe;

- (void)speakSentences:(NSArray *)sentences withMaxLength:(NSInteger)maxLength toFileName:(NSString *)fileName inLanguage:(NSString *)lang;

- (void)shutUpModel;

- (void)createCover;
- (BOOL)isCovered;
- (void)cover;
- (void)uncover;
- (void)shutUp;

@end
