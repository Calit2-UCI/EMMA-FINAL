//
//  AppTracker.h
//  MyCookingAssistant
//
//  Created by Enrique on 5/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

extern const NSString *kVersionKey;
extern const NSString *kNameKey;
extern const NSString *kEmailKey;
extern const NSString *kSpamKey;
extern const NSString *kIngredientsKey;
extern const NSString *kAddressKey;
extern const NSString *kCityKey;
extern const NSString *kRegionKey;
extern const NSString *kZipCodeKey;
extern const NSString *kCountryKey;
extern const NSString *kPerfume1Key;
extern const NSString *kPerfume2Key;
extern const NSString *kPerfume3Key;
extern const NSString *kPerfume4Key;
extern const NSString *kPerfume5Key;
extern const NSString *kIconKey;

@interface AppTracker : NSObject <NSURLConnectionDelegate>

+ (BOOL)saveUID;
+ (NSString *)getTimeStamp;

+ (void)sendSpamPersonalInfo:(NSDictionary *)info withDelegate:(id<NSURLConnectionDelegate>)delegate;
+ (void)sendIngredientsPersonalInfo:(NSDictionary *)info withDelegate:(id<NSURLConnectionDelegate>)delegate;

+ (void)sendVoteFor:(NSString *)vote withDelegate:(id<NSURLConnectionDelegate>)delegate;

+ (NSString *)encodedForURL:(NSString *)str;

@end
