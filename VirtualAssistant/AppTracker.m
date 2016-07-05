//
//  AppTracker.m
//  MyCookingAssistant
//
//  Created by Enrique on 5/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "AppTracker.h"

NSString *formKey = @"dGJzTzR4TWdmdjAtX2hlb2gwRDRmTUE6MQ";
NSString *idName = @"com.naradarobotics.uid";
NSString *timeStampName = @"com.naradarobotics.timestamp";

@implementation AppTracker

+ (BOOL)saveUID
{
    UIPasteboard *appPasteBoard = [UIPasteboard pasteboardWithName:idName create:NO];
    NSLog(@"%@", [appPasteBoard string]);
    
    if (![appPasteBoard string]) {
        CFUUIDRef uuidRef = CFUUIDCreate(kCFAllocatorDefault);
        NSString *uuidString = (__bridge NSString *)CFUUIDCreateString(NULL, uuidRef);
        
        appPasteBoard = [UIPasteboard pasteboardWithName:idName create:YES];
        appPasteBoard.persistent = YES;
        
        [appPasteBoard setString:uuidString];
        
        NSLog(@"%@", [appPasteBoard string]);
        return YES;
    }
    else {
        appPasteBoard.persistent = YES;
        return NO;
    }
}

+ (NSString *)getTimeStamp
{
    NSDateFormatter *form = [[NSDateFormatter alloc] init];
    NSLocale *formatterLocale = [[NSLocale alloc] initWithLocaleIdentifier:@"en_GB"];
    [form setLocale:formatterLocale];
    [form setDateFormat:@"YYYY/MM/dd HH:mm:ss"];
    
    return [form stringFromDate:[NSDate date]];
}

+ (NSString *)encodedForURL:(NSString *)str
{
    return (__bridge NSString *)CFURLCreateStringByAddingPercentEscapes(NULL, (CFStringRef)str, NULL, (CFStringRef)@"!*'();:@&=+$,/?%#[]", kCFStringEncodingUTF8);
}


//+ (void)sendSpamPersonalInfo:(NSDictionary *)info withDelegate:(id<NSURLConnectionDelegate>)delegate
//{
//    NSMutableString *urlMStr = [NSMutableString stringWithFormat:@"https://spreadsheets.google.com/formResponse?formkey=%@&ifq", formSpamKey];
//    
//    UIPasteboard *appPasteBoard = [UIPasteboard pasteboardWithName:idName create:YES];
//    NSString *uid = [appPasteBoard string];
//    
//    [urlMStr appendFormat:@"&entry.0.single=%@", uid];
//    [urlMStr appendFormat:@"&entry.15.single=%@", [AppTracker encodedForURL:[info objectForKey:kVersionKey]]];
//    [urlMStr appendFormat:@"&entry.1.single=%@", [AppTracker encodedForURL:[info objectForKey:kNameKey]]];
//    [urlMStr appendFormat:@"&entry.2.single=%@", [AppTracker encodedForURL:[info objectForKey:kEmailKey]]];
//    [urlMStr appendFormat:@"&entry.4.single=%@", [AppTracker encodedForURL:[info objectForKey:kPerfume1Key]]];
//    [urlMStr appendFormat:@"&entry.6.single=%@", [AppTracker encodedForURL:[info objectForKey:kPerfume2Key]]];
//    [urlMStr appendFormat:@"&entry.8.single=%@", [AppTracker encodedForURL:[info objectForKey:kPerfume3Key]]];
//    [urlMStr appendFormat:@"&entry.10.single=%@", [AppTracker encodedForURL:[info objectForKey:kPerfume4Key]]];
//    [urlMStr appendFormat:@"&entry.12.single=%@", [AppTracker encodedForURL:[info objectForKey:kPerfume5Key]]];
//    [urlMStr appendFormat:@"&entry.14.single=%@", [AppTracker encodedForURL:[info objectForKey:kIconKey]]];
//    
//    [urlMStr appendFormat:@"&submit=Submit"];
//    
//    NSLog(@"Sending info to URL %@",urlMStr);
//
//    NSURL *url = [NSURL URLWithString:urlMStr];
//    
//    NSURLRequest *theRequest = [NSURLRequest requestWithURL:url 
//                                                cachePolicy:NSURLRequestUseProtocolCachePolicy 
//                                            timeoutInterval:60.0];
//    
//    NSURLConnection *theConnection = [[NSURLConnection alloc] initWithRequest:theRequest delegate:delegate];
//    
//    if (theConnection) {
//        NSLog(@"Connected successfully");
//    }
//    else {
//        NSLog(@"Connection failed");
//    }
//}
//
//+ (void)sendIngredientsPersonalInfo:(NSDictionary *)info withDelegate:(id<NSURLConnectionDelegate>)delegate
//{
//    NSMutableString *urlMStr = [NSMutableString stringWithFormat:@"https://spreadsheets.google.com/formResponse?formkey=%@&ifq", formIngredientsKey];
//    
//    UIPasteboard *appPasteBoard = [UIPasteboard pasteboardWithName:idName create:YES];
//    NSString *uid = [appPasteBoard string];
//    
//    [urlMStr appendFormat:@"&entry.0.single=%@", uid];
//    [urlMStr appendFormat:@"&entry.24.single=%@", [AppTracker encodedForURL:[info objectForKey:kVersionKey]]];
//    [urlMStr appendFormat:@"&entry.1.single=%@", [AppTracker encodedForURL:[info objectForKey:kNameKey]]];
//    [urlMStr appendFormat:@"&entry.2.single=%@", [AppTracker encodedForURL:[info objectForKey:kEmailKey]]];
//    [urlMStr appendFormat:@"&entry.4.single=%@", [AppTracker encodedForURL:[info objectForKey:kAddressKey]]];
//    [urlMStr appendFormat:@"&entry.6.single=%@", [AppTracker encodedForURL:[info objectForKey:kCityKey]]];
//    [urlMStr appendFormat:@"&entry.8.single=%@", [AppTracker encodedForURL:[info objectForKey:kRegionKey]]];
//    [urlMStr appendFormat:@"&entry.10.single=%@", [AppTracker encodedForURL:[info objectForKey:kZipCodeKey]]];
//    [urlMStr appendFormat:@"&entry.12.single=%@", [AppTracker encodedForURL:[info objectForKey:kCountryKey]]];
//    [urlMStr appendFormat:@"&entry.14.single=%@", [AppTracker encodedForURL:[info objectForKey:kPerfume1Key]]];
//    [urlMStr appendFormat:@"&entry.16.single=%@", [AppTracker encodedForURL:[info objectForKey:kPerfume2Key]]];
//    [urlMStr appendFormat:@"&entry.18.single=%@", [AppTracker encodedForURL:[info objectForKey:kPerfume3Key]]];
//    [urlMStr appendFormat:@"&entry.20.single=%@", [AppTracker encodedForURL:[info objectForKey:kPerfume4Key]]];
//    [urlMStr appendFormat:@"&entry.23.single=%@", [AppTracker encodedForURL:[info objectForKey:kPerfume5Key]]];
//    [urlMStr appendFormat:@"&entry.22.single=%@", [AppTracker encodedForURL:[info objectForKey:kIconKey]]];
//    
//    [urlMStr appendFormat:@"&submit=Submit"];
//    
//    NSLog(@"Sending info to URL %@",urlMStr);
//    
//    NSURL *url = [NSURL URLWithString:urlMStr];
//    
//    NSURLRequest *theRequest = [NSURLRequest requestWithURL:url 
//                                                cachePolicy:NSURLRequestUseProtocolCachePolicy 
//                                            timeoutInterval:60.0];
//    
//    NSURLConnection *theConnection = [[NSURLConnection alloc] initWithRequest:theRequest delegate:delegate];
//    
//    if (theConnection) {
//        NSLog(@"Connected successfully");
//    }
//    else {
//        NSLog(@"Connection failed");
//    }
//}


+ (void)sendVoteFor:(NSString *)vote withDelegate:(id<NSURLConnectionDelegate>)delegate
{
    NSMutableString *urlMStr = [NSMutableString stringWithFormat:@"https://spreadsheets.google.com/formResponse?formkey=%@", formKey];
    
    UIPasteboard *appPasteBoard = [UIPasteboard pasteboardWithName:idName create:YES];
    NSString *uid = [appPasteBoard string];
    
    [urlMStr appendFormat:@"&entry.0.single=%@", uid];
    [urlMStr appendFormat:@"&entry.1.single=%@", vote];
    
    NSLog(@"Sending info to URL %@",urlMStr);
    
    NSURL *url = [NSURL URLWithString:urlMStr];
    
    NSURLRequest *theRequest = [NSURLRequest requestWithURL:url
                                                cachePolicy:NSURLRequestUseProtocolCachePolicy
                                            timeoutInterval:60.0];
    
    NSURLConnection *theConnection = [[NSURLConnection alloc] initWithRequest:theRequest delegate:delegate];
    
    if (theConnection) {
        NSLog(@"Connected successfully");
    }
    else {
        NSLog(@"Connection failed");
    }
}



@end
