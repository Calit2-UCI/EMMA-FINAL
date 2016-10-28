//
//  NrBarChartViewController.h
//  UCIAssistant
//
//  Created by Christian Morte on 10/28/16.
//  Copyright © 2016 Narada Robotics S.L. All rights reserved.
//

#ifndef NrBarChartViewController_h
#define NrBarChartViewController_h

#import <UIKit/UIKit.h>
#import "PNChart.h"

@interface NrBarChartViewController : PNBarChart

/*
 Should be in format:
 {
    Weekly: {
                7/11: 100
                7/12: 200
                7/13: 300
                7/14: 400
            }
 }
 */
@property (nonatomic, strong) NSMutableDictionary* deviceData;


@property (nonatomic, strong) NSString* device;

-(BOOL)displayChart;
- (void)setData:(NSMutableDictionary *)deviceData withDevice:(NSString *)device;

@end

#endif /* NrBarChartViewController_h */
