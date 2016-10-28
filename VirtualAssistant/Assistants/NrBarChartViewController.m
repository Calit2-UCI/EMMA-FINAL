//
//  NrBarChartViewController.m
//  UCIAssistant
//
//  Created by Christian Morte on 10/28/16.
//  Copyright © 2016 Narada Robotics S.L. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NrBarChartViewController.h"

@implementation NrBarChartViewController

-(BOOL)displayChart
{
    NSDictionary *data = [self dailyPowerValues];
    
    // We don't want to show the bar graph if there are now values to use
    if ([data count] == 0)
        return NO;  // we did not display the chart
    
    NSArray *weeklyData = [self lastSevenDaysOfDeviceUsageWithData:data];
    
    // Show the chart border
    [self setShowChartBorder:YES];
    
    // Set chart margin top
    [self setChartMarginTop:10];
    
    
    // Set colors
    [self setLabelTextColor:[UIColor whiteColor]];
    [self setBarBackgroundColor:[UIColor clearColor]];
    [self setBackgroundColor:[UIColor darkGrayColor]];
    [self setAlpha:0.85];
    
    // Set Data
    
    [self setYValues:[weeklyData objectAtIndex:1]]; // Power values
    [self setXLabels:[weeklyData objectAtIndex:0]]; // Dates
    
    
    // Draw the chart
    [self strokeChart];
    
    // X-label fix: for some reason, the x-labels will only display if you do this
    [self setXLabels:[weeklyData objectAtIndex:0]];
    
    return YES; // we displayed the chart
}

-(void)setData:(NSMutableDictionary *)deviceData withDevice:(NSString *)device
{
    self.device = device;
    self.deviceData = deviceData;
}

/* Returns a dictionary in the form:
    {
         7/11: 100
         7/12: 200
         7/13: 300
         7/14: 400
     }
 */
-(NSDictionary *)dailyPowerValues
{
    NSMutableDictionary *result = [[NSMutableDictionary alloc] init];
    NSArray *values = self.deviceData[@"Daily"];
    
    for (NSInteger index = 0; index < [values count]; index++) {
        double timeStampVal = [self.deviceData[@"Daily"][index][0] doubleValue];
        NSTimeInterval timeStamp = timeStampVal;
        
        NSString *date = [self dateForTimeStamp:timeStamp];
        
        // Skip if the value is null
        if ([self.deviceData[@"Daily"][index][1] isKindOfClass:[NSNull class]])
            continue;
        
        NSNumber *value = [NSNumber numberWithFloat:[self.deviceData[@"Daily"][index][1] floatValue]];
        
        [result setObject:value forKey:date];
    }
    
    return result;
}

-(NSString *)dateForTimeStamp:(NSTimeInterval)timeStamp
{
    NSDate *date = [NSDate dateWithTimeIntervalSince1970:timeStamp];
    
    NSDateFormatter *formatter= [[NSDateFormatter alloc] init];
    [formatter setLocale:[NSLocale currentLocale]];
    [formatter setDateFormat:@"MM/dd"];
    
    NSString *dateString = [formatter stringFromDate:date];
    return dateString;
}

-(NSArray *)lastSevenDaysOfDeviceUsageWithData:(NSDictionary *)data
{
    NSArray *keys = [data allKeys],
            *values = [data allValues];
    
    if ([data count] < 7)
        return @[ keys, values ];
        
    return @[ [keys subarrayWithRange:NSMakeRange([keys count]-7, 7)] , [values subarrayWithRange:NSMakeRange([keys count]-7, 7)] ];
    
    
    
}

@end
