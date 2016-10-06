//
//  NrNotifSchedulerViewController.h
//  VirtualAssistant
//
//  Created by Enrique on 10/2/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import <UIKit/UIKit.h>

@class NrCalendarMainViewController;


@interface NrNotifSchedulerViewController : UIViewController

@property (nonatomic, strong) IBOutlet UIDatePicker *dPicker;
@property (nonatomic, strong) IBOutlet UITableView *notificationsTableView;
@property (nonatomic, strong) IBOutlet UIButton *addNotificationButton;
@property (nonatomic, strong) IBOutlet UIButton *removeNotificationButton;
@property (nonatomic, strong) IBOutlet UIButton *setDailyButton;
@property (nonatomic, strong) IBOutlet UIButton *setWorkingDaysButton;
@property (nonatomic, strong) IBOutlet UILabel *errorInsertingLabel;
@property (nonatomic, strong) IBOutlet UIView *disclaimerView;
@property (nonatomic, strong) IBOutlet UIButton *disclaimerButton;

@property (nonatomic, strong) UIView *coverView;

@property (nonatomic, strong) NrCalendarMainViewController * controller;
@property (nonatomic, strong) UINavigationController *navController;

@property (nonatomic, strong) NSMutableArray *eventsLocalNotifications;

- (id)initWithMainController:(NrCalendarMainViewController *)theController;

- (IBAction)addNotificationButtonClicked:(id)sender;
- (IBAction)removeNotificationButtonClicked:(id)sender;
- (IBAction)setDailyButtonClicked:(id)sender;
- (IBAction)setWorkingDaysButtonClicked:(id)sender;
- (IBAction)disclaimerButtonClicked:(id)sender;
- (IBAction)backButtonClicked:(id)sender;

- (void)addNotificationWithDate:(NSDate *)date;

@end
