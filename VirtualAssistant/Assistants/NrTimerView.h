//
//  NrTimerView.h
//  GolfAssistant
//
//  Created by Sergio Gago on 5/27/13.
//  Copyright (c) 2013 Narada Robotics S.L. All rights reserved.
//
/*
#import <UIKit/UIKit.h>

#import "NaradaDownloader.h"
#import "NrMainViewController.h"
#import "NrCalendarMainViewController.h"
#import "NrMainViewController.h"


@interface NrTimerView : UIView <NaradaDownloaderDelegate>


@property (nonatomic, strong) IBOutlet UIButton *timerButton;
@property (nonatomic, strong) IBOutlet UILabel *timerCounterLabel;

@property (nonatomic, strong) NrCalendarMainViewController *mainViewController;

- (id)initWithMainViewController:(NrCalendarMainViewController *)controller;

- (IBAction)HideAndRemoveDetailView:(id)sender;
- (IBAction)timerButtonClicked:(id)sender;
- (void)startTimer;
- (void)stopTimer;


@end
