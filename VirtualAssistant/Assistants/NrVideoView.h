//
//  NrVideoView.h
//  Calit2-Assistant
//
//  Created by Sergio Gago on 4/26/13.
//  Copyright (c) 2013 Narada Robotics S.L. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "NrMainViewController.h"
#import "NrCalendarMainViewController.h"
#import <MediaPlayer/MediaPlayer.h>

@interface NrVideoView : UIView

@property (nonatomic, strong) IBOutlet UIView *videoCanvas;
@property (nonatomic, strong) MPMoviePlayerController *moviePlayer;
@property (nonatomic, strong) IBOutlet UILabel *VideoNameLabel;

@property (nonatomic, strong) NrCalendarMainViewController *mainViewController;

@property bool flag;

- (id)initWithMainViewController:(NrCalendarMainViewController *)controller;

- (IBAction)playMovie:(id)sender;

- (IBAction)StopAndHideVideoView:(id)sender;


@end
