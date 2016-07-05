//
//  NrTableViewController.h
//  UCIAssistant
//
//  Created by Christian Morte on 5/3/16.
//  Copyright (c) 2016 Narada Robotics S.L. All rights reserved.
//

#ifndef UCIAssistant_NrTableView_h
#define UCIAssistant_NrTableView_h

#import <UIKit/UIKit.h>

@interface NrTableViewController : UITableViewController

@property (nonatomic, strong) NSMutableDictionary* station_data;

@property (nonatomic, strong) UIScrollView* mainSlider;

- (id)initWithData:(NSMutableDictionary *)station;

- (void)set_station:(NSString *)station;

- (void) update_data:(NSDictionary *)station;

- (void) update_table;

- (void)setViewController:(UIViewController *)viewController;

- (NSDictionary *) get_data;

@end

#endif //UCIAssistant_NrTableView_h
