//
//  NrNotifSchedulerViewController.m
//  VirtualAssistant
//
//  Created by Enrique on 10/2/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NrNotifSchedulerViewController.h"

#import "NrAppDelegate.h"
#import "NrViewController.h"
#import "NaradaCalendarManager.h"
#import "NrAppDelegate.h"
#import "NrCalendarMainViewController.h"
#import "NrAppDelegate.h"

@interface NrNotifSchedulerViewController ()

- (void)createCover;
- (BOOL)isCovered;
- (void)cover;
- (void)uncover;

- (void)deselect:(NSIndexPath *)indexPath;

@end

@implementation NrNotifSchedulerViewController

@synthesize controller;
@synthesize navController;
@synthesize dPicker;
@synthesize notificationsTableView;
@synthesize addNotificationButton;
@synthesize removeNotificationButton;
@synthesize errorInsertingLabel;
@synthesize disclaimerButton;
@synthesize disclaimerView;

@synthesize coverView;
@synthesize eventsLocalNotifications;


NSIndexPath *selectedIndexPath;
BOOL setDaily = NO, setWorking = NO;

- (id)initWithMainController:(NrCalendarMainViewController *)theController
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        self = [super initWithNibName:@"NrNotifSchedulerViewController_iPhone" bundle:nil];
    }
    else {
        self = [super initWithNibName:@"NrNotifSchedulerViewController_iPad" bundle:nil];
    }
    
    if (self)
    {
//        self.navController = theNavController;
//        
//        NrAppDelegate *delegate = [UIApplication sharedApplication].delegate;
//        NrViewController *viewController = delegate.viewController;
//        self.glController = viewController.glController;
        
        [self setNotificationListeners];
        
        [self createCover];
        
        self.controller = theController;
        
        self.navController = [[UINavigationController alloc] initWithRootViewController:self];
        self.title = NSLocalizedString(@"NOTIFSCHEDULERVIEW_TITLE", nil);
        
        UIBarButtonItem *backButton = [[UIBarButtonItem alloc] initWithTitle:NSLocalizedString(@"BACK", nil) style:UIBarButtonItemStyleBordered target:self action:@selector(backButtonClicked:)];
        
        self.navigationItem.leftBarButtonItem = backButton;
        self.navController.modalPresentationStyle = UIModalPresentationFormSheet;
        
        self.navController.navigationBar.tintColor = [UIColor colorWithRed:224.0f/255.0f green:161.0f/255.0f blue:34.0f/255.0f alpha:255.0f/255.0f];
        
        [self reloadEventNotifications];
    }
    
    return self;
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    self.disclaimerView.alpha = 0.0f;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    self.errorInsertingLabel.hidden = YES;
    [self.dPicker setMinimumDate:[NSDate date]];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    NrAppDelegate *appDelegate = [UIApplication sharedApplication].delegate;
    if (appDelegate.wokenNotificationType == EVENT_NOTIFICATION) {
        [self dismissViewControllerAnimated:YES completion:^{
            NSLog(@"Dismissed successfully");
        }];
    }
    else {
        NSError *ganerror;
        if (![[GANTracker sharedTracker] trackPageview:@"/app/notification" withError:&ganerror]) {
            NSLog(@"Error in tracking: %@", [ganerror localizedDescription]);
        }
    }
    
    [self setNotificationListenerToWake];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark -
#pragma mark User Interface Methods

- (IBAction)addNotificationButtonClicked:(id)sender
{
    @synchronized(self) {
		
		[self cover];
        
        [self addNotificationWithDate:self.dPicker.date];
        
//        [self.notificationsTableView reloadData];
        
        NSError *ganerror;
        if (![[GANTracker sharedTracker] trackEvent:@"notification" action:@"added" label:@"" value:-1 withError:&ganerror]) {
            NSLog(@"Error in tracking: %@", [ganerror localizedDescription]);
        }
        
        [self uncover];
    }
}

- (IBAction)removeNotificationButtonClicked:(id)sender
{
    @synchronized(self) {
        
        [self cover];
        
        if (selectedIndexPath) {
            
            self.eventsLocalNotifications = [NSMutableArray array];
            NSArray *scheduledNotifications = [[UIApplication sharedApplication] scheduledLocalNotifications];
            for (UILocalNotification *notification in scheduledNotifications) {
                if (![notification.userInfo objectForKey:notificationUpdateKey]) {
                    [self.eventsLocalNotifications addObject:notification];
                }
            }
            
            UILocalNotification *notif = [self.eventsLocalNotifications objectAtIndex:selectedIndexPath.row];
            
            for (UILocalNotification *notification in scheduledNotifications) {
                if (notification == notif) {
                    [[UIApplication sharedApplication] cancelLocalNotification:notif];
                    break;
                }
            }
            [self reloadEventNotifications];
            
            NSArray *indexPathsArray = [NSArray arrayWithObject:selectedIndexPath];
            
            [self.notificationsTableView beginUpdates];
            [self.notificationsTableView deleteRowsAtIndexPaths:indexPathsArray withRowAnimation:UITableViewRowAnimationLeft];
            [self.notificationsTableView endUpdates];
            
            [self deselect:selectedIndexPath];
            
            NSError *ganerror;
            if (![[GANTracker sharedTracker] trackEvent:@"notification" action:@"removed" label:@"" value:-1 withError:&ganerror]) {
                NSLog(@"Error in tracking: %@", [ganerror localizedDescription]);
            }
        }
        
        [self uncover];
    }
}

- (IBAction)setDailyButtonClicked:(id)sender
{
    if (setDaily) {
        UIImage *img = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"checkNo" ofType:@"png"]];
        NSAssert(img, @"Img for daily button is nil");
        [self.setDailyButton setBackgroundImage:img forState:UIControlStateNormal];
        
        setDaily = NO;
        self.setWorkingDaysButton.enabled = NO;
        
        img = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"checkNo" ofType:@"png"]];
        NSAssert(img, @"Img for working days button is nil");
        [self.setWorkingDaysButton setBackgroundImage:img forState:UIControlStateNormal];
        
        setWorking = NO;
        
        [self.dPicker setDatePickerMode:UIDatePickerModeDateAndTime];
        [self.dPicker setMinimumDate:[NSDate date]];
    }
    else {
        UIImage *img = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"checkYes-orange" ofType:@"png"]];
        NSAssert(img, @"Img for daily button is nil");
        [self.setDailyButton setBackgroundImage:img forState:UIControlStateNormal];
        
        setDaily = YES;
        self.setWorkingDaysButton.enabled = YES;
        
        [self.dPicker setDatePickerMode:UIDatePickerModeTime];
        [self.dPicker setMinimumDate:nil];
    }
}

- (IBAction)setWorkingDaysButtonClicked:(id)sender
{
    if (!setDaily)
        return;
    
    if (setWorking) {
        UIImage *img = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"checkNo" ofType:@"png"]];
        NSAssert(img, @"Img for working days button is nil");
        [self.setWorkingDaysButton setBackgroundImage:img forState:UIControlStateNormal];
        
        setWorking = NO;
    }
    else {
        UIImage *img = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"checkYes-orange" ofType:@"png"]];
        NSAssert(img, @"Img for working days button is nil");
        [self.setWorkingDaysButton setBackgroundImage:img forState:UIControlStateNormal];
        
        setWorking = YES;
    }
}

BOOL disclaimerActive = NO;

- (IBAction)disclaimerButtonClicked:(id)sender
{
    CGFloat finalAlpha;
    CGRect finalFrame;
    CGRect initframe = self.disclaimerView.frame;
    
    if (disclaimerActive) {
        finalAlpha = 0.0f;
        finalFrame = CGRectMake(finalFrame.origin.x, finalFrame.origin.y, 0, 0);
    }
    else {
        finalFrame = self.disclaimerView.frame;
//        self.disclaimerView.frame = CGRectMake(finalFrame.origin.x, finalFrame.origin.y, 0, 0);
        finalAlpha = 1.0f;
    }
    [UIView animateWithDuration:0.1 animations:^{
        self.disclaimerView.alpha = finalAlpha;
//        self.disclaimerView.frame = finalFrame;
    } completion:^(BOOL finished) {
        disclaimerActive = !disclaimerActive;
        self.disclaimerView.frame = initframe;
        
    }];
}

- (IBAction)backButtonClicked:(id)sender
{
    [self dismissViewControllerAnimated:YES completion:^{
        NSLog(@"Dismissed Scheduler");
        [self.controller.glController.glView startAnimation];
        [self.controller setNotificationListenerToWake];
    }];
}


#pragma mark -
#pragma mark Notification Setup Methods

- (void)setNotificationListenerToWake
{
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didWakeFromLocalNotification:) name:notificationWakeFromLocalNotification object:nil];
}

- (void)removeNotificationListenerToWake
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:notificationWakeFromLocalNotification object:nil];
}

- (void)didWakeFromLocalNotification:(NSNotification *)notification
{
    [self removeNotificationListenerToWake];
    [self backButtonClicked:nil];
    [self.controller didWakeFromLocalNotification:notification];
}

- (void)setNotificationListeners
{
//    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleFinishedSpeaking:) name:notificationFinishedSpeaking object:nil];
}

- (void)reloadEventNotifications
{
    self.eventsLocalNotifications = [NSMutableArray array];
    for (UILocalNotification *notification in [[UIApplication sharedApplication] scheduledLocalNotifications]) {
        if (![notification.userInfo objectForKey:notificationUpdateKey]) {
            [self.eventsLocalNotifications addObject:notification];
        }
    }
    
//    NSLog(@"Notification events: %@", self.eventsLocalNotifications);
    NSLog(@"Total Notifications: %@", [UIApplication sharedApplication].scheduledLocalNotifications);
    NSLog(@"Total Notifications: %@", [UIApplication sharedApplication].scheduledLocalNotifications);
}

BOOL errorLabelAnimating = NO;

- (void)addNotificationWithDate:(NSDate *)date
{
    NSCalendar *calendar = [NSCalendar autoupdatingCurrentCalendar];
    
    NSDateComponents *dateComponents = [calendar components:( NSYearCalendarUnit | NSMonthCalendarUnit | NSDayCalendarUnit | NSHourCalendarUnit | NSMinuteCalendarUnit | NSSecondCalendarUnit) fromDate:date];
    
    [dateComponents setSecond:0];
    
    NSDate *itemDate = [calendar dateFromComponents:dateComponents];
    
    NSInteger notifCountInit = [UIApplication sharedApplication].scheduledLocalNotifications.count;
//    NSInteger notifCountInit = self.eventsLocalNotifications.count;
    
    NSMutableArray *notificationsSet = [NSMutableArray array];
    
    if (setDaily && setWorking) {
        for (unsigned int wd = MONDAY; wd < SATURDAY; wd++) {
            NSDate *nextWeekday = [NaradaCalendarManager getNextWeekday:(weekday)wd];
            UILocalNotification *localNotif = [self scheduleLocalNotification:nextWeekday recurrent:NSWeekCalendarUnit];
            [notificationsSet addObject:localNotif];
        }
    }
    else if (setDaily && !setWorking) {
        UILocalNotification *localNotif = [self scheduleLocalNotification:itemDate recurrent:NSDayCalendarUnit];
        [notificationsSet addObject:localNotif];
    }
    else if (!setDaily) {
        UILocalNotification *localNotif = [self scheduleLocalNotification:itemDate recurrent:NSEraCalendarUnit];
        [notificationsSet addObject:localNotif];
    }
    
    [self reloadEventNotifications];
    
    NSMutableArray *indexPathsList = [NSMutableArray array];
    for (UILocalNotification *notif in notificationsSet)
    {
//        NSArray *scheduledNotifications = app.scheduledLocalNotifications;
        NSIndexPath *iPath = [NSIndexPath indexPathForRow:[self.eventsLocalNotifications indexOfObject:notif] inSection:0];
        [indexPathsList addObject:iPath];
    }
    
    
//    NSArray *notificationList = app.scheduledLocalNotifications;
    
    // If the notifications list has not changed size, don't add anything,
    // otherwise will crash, as it attempts to insert a cell to the table
    // without inserting it in the NSArray
    if (notifCountInit >= [UIApplication sharedApplication].scheduledLocalNotifications.count) {
        self.errorInsertingLabel.text = NSLocalizedString(@"ALREADY_INSERTED", nil);
        self.errorInsertingLabel.hidden = NO;
        
        self.addNotificationButton.enabled = NO;
        
        errorLabelAnimating = YES;
        [UIView animateWithDuration:2.5 animations:^{
            self.errorInsertingLabel.alpha = 0.0f;
        } completion:^(BOOL finished) {
            self.errorInsertingLabel.alpha = 1.0f;
            self.errorInsertingLabel.hidden = YES;
            errorLabelAnimating = NO;
            self.addNotificationButton.enabled = YES;
        }];
        return;
    }
    
    NSLog(@"Scheduled local notifications: %@", self.eventsLocalNotifications);
    
//    NSIndexPath *insertIndexPath = [NSIndexPath indexPathForRow:[notificationList indexOfObject:localNotif] inSection:0];
    
//    NSArray *indexPathsList = [NSArray arrayWithObject:insertIndexPath];
    
    NSLog(@"Index paths: %@", indexPathsList);
    
    [self.notificationsTableView beginUpdates];
    
    [self.notificationsTableView insertRowsAtIndexPaths:indexPathsList withRowAnimation:UITableViewRowAnimationRight];
    
    [self.notificationsTableView endUpdates];

//    [self.notificationsTableView reloadData];
}

- (NSIndexPath *)indexPathOfLocalNotification:(UILocalNotification *)notification
{
    NSArray *scheduledNotifications = [UIApplication sharedApplication].scheduledLocalNotifications;
    return [NSIndexPath indexPathForRow:[scheduledNotifications indexOfObject:notification] inSection:0];
}

- (UILocalNotification *)scheduleLocalNotification:(NSDate *)itemDate recurrent:(NSCalendarUnit)recurrent
{
    UILocalNotification *localNotif = [[UILocalNotification alloc] init];
    
    NSAssert(localNotif, @"Local notification just allocated is nil.");
    
    localNotif.fireDate = itemDate;
    localNotif.timeZone = [NSTimeZone defaultTimeZone];
    
    NSMutableString *notifTitle;
    if (recurrent == NSDayCalendarUnit) {
        notifTitle = [NSMutableString stringWithString:NSLocalizedString(@"NOTIF_DESC_DAILY", nil)];
        notifTitle = (NSMutableString *)[notifTitle stringByReplacingOccurrencesOfString:@"[WEEKDAY]" withString:[NaradaCalendarManager weekToNSString:[NaradaCalendarManager dateToWeekday:itemDate]]];
        
        localNotif.repeatInterval = NSDayCalendarUnit;
    }
    else if (recurrent == NSWeekCalendarUnit) {
        notifTitle = [NSMutableString stringWithString:NSLocalizedString(@"NOTIF_DESC_WEEKLY", nil)];
        notifTitle = (NSMutableString *)[notifTitle stringByReplacingOccurrencesOfString:@"[WEEKDAY]" withString:[NaradaCalendarManager weekToNSString:[NaradaCalendarManager dateToWeekday:itemDate]]];
        
        localNotif.repeatInterval = NSWeekCalendarUnit;
    }
    else {
        notifTitle = [NSMutableString stringWithString:NSLocalizedString(@"NOTIF_DESC_SINGLE", nil)];
        notifTitle = (NSMutableString *)[notifTitle stringByReplacingOccurrencesOfString:@"[WEEKDAY]" withString:[NaradaCalendarManager weekToNSString:[NaradaCalendarManager dateToWeekday:itemDate]]];
        localNotif.repeatInterval = 0;
    }
    
    localNotif.alertBody = (NSString *)notifTitle;
	// Set the action button
    localNotif.alertAction = @"View";
	
    localNotif.soundName = UILocalNotificationDefaultSoundName;
    localNotif.applicationIconBadgeNumber = 1;
	
	// Specify custom data for the notification
//    NSDictionary *infoDict = [NSDictionary dictionaryWithObject:@"someValue" forKey:@"someKey"];
//    localNotif.userInfo = infoDict;
    
    localNotif.userInfo = [NSDictionary dictionaryWithObject:@"Speak" forKey:notificationEventsKey];
    
    [[UIApplication sharedApplication] scheduleLocalNotification:localNotif];
    
    return localNotif;
}

#pragma mark -
#pragma mark Notification Table View Handlers

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    return NSLocalizedString(@"NOTIFICATIONS", nil);
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
//    return [[[UIApplication sharedApplication] scheduledLocalNotifications] count];
    return [self.eventsLocalNotifications count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *cellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:cellIdentifier];
    }
    
    UILocalNotification *notif = [self.eventsLocalNotifications objectAtIndex:indexPath.row];
    
    NSDateFormatter *form = [[NSDateFormatter alloc] init];
    switch (notif.repeatInterval) {
        case NSDayCalendarUnit:
            [form setDateFormat:@"HH:mm"];
            break;
        case NSWeekCalendarUnit:
            [form setDateFormat:@"HH:mm"];
            break;
            
        default:
            [form setDateFormat:@" YYYY/MM/dd - HH:mm"];
            break;
    }
    
    
    if (notif.repeatInterval == NSWeekCalendarUnit) {
        NSString *weekdayStr;
        weekday wkd = [NaradaCalendarManager dateToWeekday:notif.fireDate];
        switch (wkd) {
            case SUNDAY:
                weekdayStr = NSLocalizedString(@"SUNDAY", nil);
                break;
            case MONDAY:
                weekdayStr = NSLocalizedString(@"MONDAY", nil);
                break;
            case TUESDAY:
                weekdayStr = NSLocalizedString(@"TUESDAY", nil);
                break;
            case WEDNESDAY:
                weekdayStr = NSLocalizedString(@"WEDNESDAY", nil);
                break;
            case THURSDAY:
                weekdayStr = NSLocalizedString(@"THURSDAY", nil);
                break;
            case FRIDAY:
                weekdayStr = NSLocalizedString(@"FRIDAY", nil);
                break;
            case SATURDAY:
                weekdayStr = NSLocalizedString(@"SATURDAY", nil);
                break;
                
            default:
                break;
        }
        
        NSString *timeStr = [form stringFromDate:notif.fireDate];
        weekdayStr = [weekdayStr stringByAppendingFormat:@" %@", timeStr];
        
        [cell.detailTextLabel setText:weekdayStr];
    }
    else {
        [cell.detailTextLabel setText:[form stringFromDate:notif.fireDate]];
    }
    
    [cell.textLabel setText:notif.alertBody];
    
    return cell;
}


- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
//    selectedIndexPath = indexPath;
    
    if (selectedIndexPath && (selectedIndexPath.row == indexPath.row)) {
        [self deselect:selectedIndexPath];
    }
    else {
        selectedIndexPath = indexPath;
    }
}

- (void)deselect:(NSIndexPath *)indexPath
{
    [self.notificationsTableView deselectRowAtIndexPath:selectedIndexPath animated:YES];
    selectedIndexPath = nil;
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
    return !coverView.hidden;
}

- (void)cover
{
    NSAssert(coverView != nil, @"Cover: coverView is nil, so aborting program");
    coverView.hidden = NO;
}

- (void)uncover
{
    NSAssert(coverView != nil, @"Uncover: coverView is nil, so aborting program");
    coverView.hidden = YES;
}


@end
