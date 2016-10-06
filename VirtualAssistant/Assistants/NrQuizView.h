//
//  NrQuizView.h
//  Calit2-Assistant
//
//  Created by Sergio Gago on 4/3/13.
//  Copyright (c) 2013 Narada Robotics S.L. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "NrMainViewController.h"
#import "NrCalendarMainViewController.h"

@interface NrQuizView : UIView

@property (nonatomic, strong) IBOutlet UILabel *question;
@property (nonatomic, strong) IBOutlet UILabel *answer1;
@property (nonatomic, strong) IBOutlet UILabel *answer2;
@property (nonatomic, strong) IBOutlet UILabel *answer3;
@property (nonatomic, strong) IBOutlet UILabel *answer4;
@property (nonatomic, strong) IBOutlet UILabel *progressText;
@property (nonatomic, retain) IBOutlet UIProgressView *progressBar;

@property (nonatomic, strong) NrCalendarMainViewController *mainViewController;

- (id)initWithMainViewController:(NrCalendarMainViewController *)controller;

- (IBAction)answer1Clicked:(id)sender;
- (IBAction)answer2Clicked:(id)sender;
- (IBAction)answer3Clicked:(id)sender;
- (IBAction)answer4Clicked:(id)sender;
- (IBAction)repeatClicked:(id)sender;
- (IBAction)exitClicked:(id)sender;

@end
