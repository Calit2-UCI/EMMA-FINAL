//
//  NrSmartTableViewController.h
//  UCIAssistant
//
//  Created by Christian Morte on 5/19/16.
//  Copyright (c) 2016 Narada Robotics S.L. All rights reserved.
//

#ifndef UCIAssistant_NrSmartTableViewController_h
#define UCIAssistant_NrSmartTableViewController_h

#import <UIKit/UIKit.h>
#import "NrTableViewController.h"
#import "PNBarChart.h"

@interface NrSmartTableViewController : NrTableViewController

@property (nonatomic, strong) NSMutableDictionary* station_data;

@property (nonatomic, strong) NSString* device;
@property (nonatomic) PNPieChart *pieChart;
@property (nonatomic) PNBarChart *barChart;

- (id)initWithDevice:(NSString *)device withViewController:(UIViewController *)viewController withData:(NSMutableDictionary *)station;

- (void)setTableDataWithDevice:(NSString *)device;

@end

#endif
