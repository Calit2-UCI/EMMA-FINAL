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

-(id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        
        NSInteger titleFontSize = 18, axisFontSize = 12;
        
        // Set the title bar
        self.titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(axisFontSize, 0, frame.size.width-axisFontSize, titleFontSize)];
        [self.titleLabel setText:@"Device Usage Over Past Week"];
        [self.titleLabel setFont:[UIFont fontWithName:@"Arial Rounded MT Bold" size:titleFontSize]];
        [self.titleLabel setTextColor:[UIColor whiteColor]];
        [self.titleLabel setTextAlignment:NSTextAlignmentCenter];
        [self.titleLabel setBackgroundColor:[UIColor darkGrayColor]];
        [self.titleLabel setAlpha:0.85];
        [self.titleLabel setAdjustsFontSizeToFitWidth:YES];
        
        self.xAxisLabel = [[UILabel alloc] initWithFrame:CGRectMake(axisFontSize, frame.size.height-axisFontSize, frame.size.width-axisFontSize, axisFontSize)];
        [self.xAxisLabel setText:@"Days of Recorded Activity"];
        [self.xAxisLabel setFont:[UIFont fontWithName:@"Arial Rounded MT Bold" size:axisFontSize]];
        [self.xAxisLabel setTextColor:[UIColor whiteColor]];
        [self.xAxisLabel setTextAlignment:NSTextAlignmentCenter];
        [self.xAxisLabel setBackgroundColor:[UIColor darkGrayColor]];
        [self.xAxisLabel setAlpha:0.85];
        [self.xAxisLabel setAdjustsFontSizeToFitWidth:YES];
        
        self.yAxisLabel = [[UILabel alloc] initWithFrame:CGRectMake(-frame.size.width/4 - 44, frame.size.height/2-6, frame.size.height, axisFontSize)];
        
        // Rotate the yAxisLabel to be vertical
        self.yAxisLabel.transform = CGAffineTransformMakeRotation( ( 270 * M_PI ) / 180 );
        
        [self.yAxisLabel setText:@"Power (Watts)"];
        [self.yAxisLabel setFont:[UIFont fontWithName:@"Arial Rounded MT Bold" size:axisFontSize]];
        [self.yAxisLabel setTextColor:[UIColor whiteColor]];
        [self.yAxisLabel setTextAlignment:NSTextAlignmentCenter];
        [self.yAxisLabel setBackgroundColor:[UIColor darkGrayColor]];
        [self.yAxisLabel setAlpha:0.85];
        [self.yAxisLabel setAdjustsFontSizeToFitWidth:YES];
        
        [self setFrame:CGRectMake(axisFontSize, titleFontSize, frame.size.width-axisFontSize, frame.size.height - self.titleLabel.frame.size.height - axisFontSize)];
        self.isShowNumbers = NO;
    }
    return self;
}

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
    [self setXLabels:[self unixToStringArrayFromArray:[weeklyData objectAtIndex:0]]]; // Dates
    
    
    // Draw the chart
    [self strokeChart];
    
    // X-label fix: for some reason, the x-labels will only display if you do this
    [self setXLabels:[self unixToStringArrayFromArray:[weeklyData objectAtIndex:0]]];
    return YES; // we displayed the chart
}

-(void)displayChartLabelsInView:(UIView *)view
{
    [view addSubview:self.titleLabel];
    [view addSubview:self.xAxisLabel];
    [view addSubview:self.yAxisLabel];
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
        NSNumber *timeStamp = [NSNumber numberWithDouble:timeStampVal];
        
        // Skip if the value is null
        if ([self.deviceData[@"Daily"][index][1] isKindOfClass:[NSNull class]])
            continue;
        
        NSNumber *value = [NSNumber numberWithFloat:[self.deviceData[@"Daily"][index][1] floatValue]];
        
        [result setObject:value forKey:timeStamp];
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

// Returns an array of dates ["7/11", "7/12", ...] from a list of UNIX timestamps
-(NSArray *)unixToStringArrayFromArray:(NSArray *)array
{
    NSMutableArray *result = [[NSMutableArray alloc] init];
    for (NSInteger i = 0; i < [array count]; i++) {
        NSTimeInterval timeStamp = [[array objectAtIndex:i] doubleValue];
        NSString *date = [self dateForTimeStamp:timeStamp];
        
        [result addObject:date];
    }
    return result;
}

-(NSArray *)lastSevenDaysOfDeviceUsageWithData:(NSDictionary *)data
{
    NSArray *keys = [[data allKeys] sortedArrayUsingSelector:@selector(compare:)];
    
    NSMutableArray *values = [[NSMutableArray alloc] init];
    for (id key in keys)
        [values addObject:[data objectForKey:key]];
    
    if ([data count] < 7)
        return @[ keys, values ];
        
    return @[ [keys subarrayWithRange:NSMakeRange([keys count]-7, 7)] , [values subarrayWithRange:NSMakeRange([keys count]-7, 7)] ];
}

@end
