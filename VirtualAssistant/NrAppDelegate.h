//
//  NrAppDelegate.h
//  VirtualAssistant
//
//  Created by Enrique on 9/27/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import <UIKit/UIKit.h>


#define notificationUpdateKey @"Update"

@class NrViewController;

enum notificationType {
    NO_NOTIFICATION,
    EVENT_NOTIFICATION,
    HOROSCOPE_NOTIFICATION,
    WEATHER_NOTIFICATION
};

extern const NSString *notificationEventsKey;
extern const NSString *notificationHoroscopeKey;
extern const NSString *notificationWeatherKey;

@interface NrAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (strong, nonatomic) NrViewController *viewController;

@property (nonatomic, assign) enum notificationType wokenNotificationType;



@end
