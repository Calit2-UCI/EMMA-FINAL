//
//  NrAppDelegate.m
//  VirtualAssistant
//
//  Created by Enrique on 9/27/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NrAppDelegate.h"

#import "NrViewController.h"

#import "NrCalendarMainViewController.h"
#import <EstimoteSDK/EstimoteSDK.h>

#import <UIKit/UIKit.h>

@implementation NrAppDelegate


@synthesize wokenNotificationType;

const NSString *notificationEventsKey = @"Events";
const NSString *notificationHoroscopeKey = @"Horoscope";
const NSString *notificationWeatherKey = @"Weather";


UIAlertView * pushAlert;
NSURL *updateURL;
NSString *plistFile;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{

    
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    

    
    // Override point for customization after application launch.
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        self.viewController = [[NrViewController alloc] initWithNibName:@"NrViewController_iPhone" bundle:nil];
    } else {
        self.viewController = [[NrViewController alloc] initWithNibName:@"NrViewController_iPad" bundle:nil];
    }
    self.window.rootViewController = self.viewController;
    [self.window makeKeyAndVisible];
    
    [ESTConfig setupAppID:@"simhomecalplug-gmail-com-s-j54" andAppToken:@"362c81bd0e6a2f3c970fa5bd08c88e6c"];

    return YES;
}



- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error
{
    NSLog(@"Failed to register for push, with error %@", [error localizedDescription]);
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo
{
    NSString *urlStr = @"itms-apps://itunes.com/apps/myvirtualassistant";
    updateURL = [NSURL URLWithString:urlStr];

    NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithContentsOfFile:plistFile];
    [dict setObject:[NSNumber numberWithBool:YES] forKey:@"UpdateAvailable"];
    
    [self showUpdateAvailable];
}

- (void)showUpdateAvailable
{
    pushAlert = [[UIAlertView alloc] init];
    pushAlert.title = NSLocalizedString(@"UPDATE_AVAILABLE_TITLE", nil);
    pushAlert.message = NSLocalizedString(@"UPDATE_AVAILABLE_MSG", nil);
    pushAlert.delegate = self;
    [pushAlert addButtonWithTitle:NSLocalizedString(@"REMIND_LATER", nil)];
    [pushAlert addButtonWithTitle:NSLocalizedString(@"UPDATE", nil)];
    [pushAlert show];
}

- (NSURL *)extractURLOfString:(NSString *)message
{
    NSString *urlStr;
    @try {
        NSError *error;
        NSRegularExpression *regexp = [NSRegularExpression regularExpressionWithPattern:@"((?:https?\\:\\/\\/|www\\.)(?:[-a-z0-9]+\\.)*[-a-z0-9]+.*)" options:0 error:&error];
        
        NSAssert(regexp, @"Regexp is nil");
        NSTextCheckingResult *result = [regexp firstMatchInString:message options:0 range:NSMakeRange(0, [message length])];
        
        NSRange range = [result rangeAtIndex:1];
        
        urlStr = [message substringWithRange:range];
        NSLog(@"URL string: %@", urlStr);
        
    }
    @catch (NSException *exception) {
        urlStr = nil;
    }
    
    return [NSURL URLWithString:urlStr];
}

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    NSMutableArray *notifList = (NSMutableArray *)[UIApplication sharedApplication].scheduledLocalNotifications;
    if (alertView == pushAlert) {
        switch (buttonIndex) {
            case 0: //REMIND ME LATER
            {
                for (UILocalNotification *notification in notifList) {
                    if ([notification.userInfo objectForKey:notificationUpdateKey]) {
                        return;
                    }
                }
                [self scheduleUpdateLocalNotification];
                break;
            }
            case 1: //UPDATE
            {
                NSString *urlStr = @"itms-apps://itunes.com/apps/myvirtualassistant";
                updateURL = [NSURL URLWithString:urlStr];
                if (![[UIApplication sharedApplication] openURL:updateURL]) {
                    NSLog(@"Failed to open URL, %@", [updateURL description]);
                }
                else {
                    for (UILocalNotification *notification in notifList) {
                        if ([notification.userInfo objectForKey:notificationUpdateKey]) {
                            [[UIApplication sharedApplication] cancelLocalNotification:notification];
                        }
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    
    [self.viewController.glController.glView stopAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    
    [self.viewController.glController.glView startAnimation];
    [UIApplication sharedApplication].applicationIconBadgeNumber = 0;
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}



- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
{
    //
}

- (UILocalNotification *)scheduleUpdateLocalNotification
{
    UILocalNotification *localNotif = [[UILocalNotification alloc] init];
    
    NSString *title = NSLocalizedString(@"LOCAL_NOTIFICATION_UPDATE", nil);
    NSDate *itemDate = [NSDate date];
    NSCalendarUnit recurrent = NSWeekCalendarUnit;
    
    NSAssert(localNotif, @"Local notification just allocated is nil.");
    
    localNotif.fireDate = itemDate;
    localNotif.timeZone = [NSTimeZone defaultTimeZone];
    localNotif.alertBody = title;
    localNotif.repeatInterval = recurrent;
    
	// Set the action button
    localNotif.alertAction = @"View";
	
    localNotif.soundName = UILocalNotificationDefaultSoundName;
    localNotif.applicationIconBadgeNumber = 1;
    
    localNotif.userInfo = [NSDictionary dictionaryWithObject:@"Update" forKey:notificationUpdateKey];
    
    [[UIApplication sharedApplication] scheduleLocalNotification:localNotif];
    
    return localNotif;
}

- (NSString *)plistCopy
{
    NSFileManager *fm = [NSFileManager defaultManager];
    NSString *plistOriginalPath = [[NSBundle mainBundle] pathForResource:@"updates" ofType:@"plist"];
    
    NSString *plistFile = [[NSFileManager documentsPath] stringByAppendingPathComponent:@"updates.plist"];
    
    if([fm fileExistsAtPath:plistOriginalPath]) {
        NSLog(@"File already exists, file %@", plistFile);
        return plistFile;
    }
    
    NSError *error;
    BOOL success = [fm copyItemAtPath:plistOriginalPath toPath:[NSFileManager documentsPath] error:&error];
    
    NSAssert2(success, @"Could not copy item from path %@ to path %@", plistOriginalPath, [NSFileManager documentsPath]);
    
    return [[NSFileManager documentsPath] stringByAppendingPathComponent:@"updates.plist"];
}

@end
