//
//  NrSpeechSentences.h
//  UCIAssistant
//
//  Created by Christian Morte on 5/23/16.
//  Copyright (c) 2016 Narada Robotics S.L. All rights reserved.
//

#ifndef UCIAssistant_NrSpeechSentences_h
#define UCIAssistant_NrSpeechSentences_h

#import <UIKit/UIKit.h>

@interface NrSpeechGenerator : NSObject


@property (nonatomic, retain) NSArray *welcome_messages; // Does not have name capabilities yet

@property (nonatomic, retain) NSArray *request_device_info_messages;
@property (nonatomic, retain) NSArray *overview_device_info_messages;

@property (nonatomic, retain) NSArray *connect_to_station_messages;
@property (nonatomic, retain) NSArray *connected_stations_messages;

@property (nonatomic, retain) NSArray *enter_overview_messages;           // from main menu / video view
@property (nonatomic, retain) NSArray *overview_device_messages;
@property (nonatomic, retain) NSDictionary *device_on_messages_for_count;
@property (nonatomic, retain) NSDictionary *device_off_messages_for_count;
@property (nonatomic, retain) NSArray *device_with_most_usage_messages;
@property (nonatomic, retain) NSArray *enter_piechart_messages;


@property (nonatomic, retain) NSArray *return_to_overview_messages;       // from smart table view
@property (nonatomic, retain) NSArray *already_in_overview_messages;       // from overview table

@property (nonatomic, retain) NSArray *enter_smart_table_view_messages;

@property (nonatomic, retain) NSArray *turn_off_station_messages;
@property (nonatomic, retain) NSArray *turn_on_station_messages;

@property (nonatomic, retain) NSArray *turn_off_device_messages;
@property (nonatomic, retain) NSArray *turn_on_device_messages;

@property (nonatomic, retain) NSArray *back_to_menu_messages;
@property (nonatomic, retain) NSArray *back_to_station_menu_messages;


- (NSString *)welcome_message;
- (NSString *)find_station;
- (NSString *)request_device_info_message;
- (NSString *)overview_device_info_message_with_device:(NSString *)device andCurrentStatus:(NSString *)status andPowerUsage:(NSString *) power;

- (NSString *)connect_to_station_message_with_station_list:(NSMutableArray *)stationList;

- (NSString *)enter_overview_message_with_station:(NSString *)station withStationData:(NSDictionary *)stationData;
- (NSString *)return_to_overview_message_with_station:(NSString *)station withStationData:(NSDictionary *)stationData;
- (NSString *)already_in_overview_message_with_station:(NSString *)station;

- (NSString *)enter_smart_table_view_message_with_device:(NSString *)device;

- (NSString *)turn_off_station_message:(NSString *)station;
- (NSString *)turn_on_station_message:(NSString *)station;

- (NSString *)turn_off_device_message:(NSString *)device;
- (NSString *)turn_on_device_message:(NSString *)device;

- (NSString *)back_to_menu_message;
- (NSString *)back_to_station_menu_message;

@end

#endif
