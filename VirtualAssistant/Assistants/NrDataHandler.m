//
//  NrDataHandler.m
//  UCIAssistant
//
//  Created by Christian Morte on 11/4/16.
//  Copyright © 2016 Narada Robotics S.L. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NrDataHandler.h"

@implementation NrDataHandler

- (id)init
{
    self = [super init];
    
    if (self) {
        self.realStationNames = @{@"Station 1": @"Entertainment Room",
                                  @"Station 2": @"Kitchen",
                                 };
        self.bluetoothStationNames = @{@"beetroot" : @"Station 1",
                                       @"candy": @"Station 2",
                                       @"lemon": @"Station 3",
                                      };
        self.stationList = [[NSMutableArray alloc] initWithObjects:@"Station 1",@"Station 2",nil];
        
        self.data = [[NSMutableDictionary alloc] init];
        [self loadJSONFile];
    }
    return self;
}

#pragma mark -
#pragma mark Data Methods

-(void)updateData
{
    NSError *error;
    NSMutableArray *json;
    NSURL *url;
    NSData *data;
    
    /* TODO: Change NSData to NSURLConnection */
    for (id station in self.realStationNames) {
        for (id deviceNum in self.data[station][@"Devices"]) {
            
            NSString *deviceAPI = [self getDeviceAPINameForDevice:self.data[station][@"Devices"][deviceNum][@"Name"]];
            
            // Get the current power value for a device
            url = [NSURL URLWithString:[@"http://128.195.151.158/simhome/db/?type=showpower&&dev=" stringByAppendingString:deviceAPI]];
            data = [NSData dataWithContentsOfURL: url];
            
            if ([self dataIsValidWithData:data]) {    // check if data is null
                json = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:&error];
                // Update the power value inside the internal device dictionary
                [self.data[station][@"Devices"][deviceNum] setObject:[json valueForKey:@"showpower"] forKey:@"Watts"];
                NSLog(@"Current power value has been updated for %@!", self.data[station][@"Devices"][deviceNum][@"Name"]);
            }
            /* DELETE IN FINAL FORM, just want to use this to test out the pie chart */
            else {
                //
                [self.data[station][@"Devices"][deviceNum] setObject:@"100" forKey:@"Watts"];
            }
            
            
            // Get the daily power data
            url = [NSURL URLWithString:[NSString stringWithFormat:@"http://128.195.151.158/simhome/db/?type=showtable&table=%@_Daily_2016", deviceAPI]];
            data = [NSData dataWithContentsOfURL: url];
            
            if ([self dataIsValidWithData:data]) {
                json = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:&error];
                
                if ([[json valueForKey:@"showtable"] isKindOfClass:[NSString class]])
                    continue;
                
                [self.data[station][@"Devices"][deviceNum] setObject:[json valueForKey:@"showtable"] forKey:@"Daily"];
                NSLog(@"URL = %@, result = %@", [url absoluteString], [json valueForKey:@"showtable"]);
            }
        }
    }
}

-(NSDictionary *)stationDataForStation:(NSString *)station
{
    return [self.data objectForKey:station];
}

-(NSDictionary *)deviceDataForDevice:(NSString *)deviceNumber atStation:(NSString *)station
{
    for (id deviceNum in self.data[station][@"Devices"]) {
        if ( [deviceNum isEqualToString:deviceNumber] )
            return self.data[station][@"Devices"][deviceNum];
    }
    return nil;
}

#pragma mark -
#pragma mark Data Helper Methods

// Returns the Device API name form for a real device Name
// eg. "Apple TV" --> "AppleTV", "Blu-Ray" --> "BluRay"
// this should definitely be somewhere else, like a device class
-(NSString *)getDeviceAPINameForDevice:(NSString *)device
{
    return [[device stringByReplacingOccurrencesOfString:@" " withString:@""] stringByReplacingOccurrencesOfString:@"-" withString:@""];
}

-(BOOL)dataIsValidWithData:(NSData *)data
{
    return [data length] > 0;
}

#pragma mark -
#pragma mark JSON Methods

-(void)loadJSONFile{
    
    // Find the JSON file
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"EMMA" ofType:@"json"];
    NSError *error = nil;
    
    // Extract the JSON file
    NSData *JSONData = [NSData dataWithContentsOfFile:filePath options:NSDataReadingMappedIfSafe error:&error];
    
    self.data = [[self parseJSON:JSONData] mutableCopy];
    [self mutifyJsonDict:self.data];
}

- (NSDictionary*)parseJSON: (NSData *) downloaded_data
{
    NSDictionary *dict;
    
    NSError *json_error = nil;
    id json = [NSJSONSerialization JSONObjectWithData:downloaded_data options:0 error:&json_error];
    
    if (json_error) {                   // JSON file was malformed
        NSLog(@"Unable to parse JSON file! Please check format of file!");
    }
    
    if ([json isKindOfClass:[NSDictionary class]]) {    // JSON file can be translated to dictionary
        dict = json;
    } else {
        NSLog(@"JSON file unable to be converted to NSMutableDictionary");
    }
    
    return dict;
}

/*
 Create nested NSMutableDictionary from JSON file.
 This solves the problem of updating the data inside the dictionary.
 */
-(void)mutifyJsonDict:(NSMutableDictionary *)dict
{
    NSArray *keys = [dict allKeys];
    
    for (NSString* flag in keys) {
        if ([dict[flag] isKindOfClass:[NSDictionary class]]) {
            dict[flag] = [dict[flag] mutableCopy];
            [self mutifyJsonDict:dict[flag]];
        }
    }
}
@end
