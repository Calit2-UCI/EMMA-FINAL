//
//  NrSpeechSentences.m
//  UCIAssistant
//
//  Created by Christian Morte on 5/23/16.
//  Copyright (c) 2016 Narada Robotics S.L. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NrSpeechGenerator.h"

@implementation NrSpeechGenerator

@synthesize welcome_messages;

#pragma mark-
#pragma mark Initialization methods

- (id)init
{
    self = [super init];
    
    if (self) {
        [self init_welcome_messages];
        [self init_request_device_info_messages];
        [self init_overview_device_info_messages];
        [self init_connect_to_station_messages];
        [self init_enter_overview_messages];
        [self init_return_to_overview_messages];
        [self init_already_in_overview_messages];
        [self init_enter_smart_table_view_messages];
        [self init_turn_off_station_messages];
        [self init_turn_on_station_messages];
        [self init_turn_off_device_messages];
        [self init_turn_on_device_messages];
        [self init_back_to_menu_messages];
        [self init_back_to_station_menu_messages];
    }
    
    return self;
}

- (void)init_welcome_messages
{
    self.welcome_messages = @[@"Hello guest!",
                              @"Hello and welcome to Sim-Home!",
                              @"Hello guest! Welcome to Sim-Home.",
                              [NSString stringWithFormat:@"Good %@ and welcome to Sim-Home!", [self time_greeting]],
                              [NSString stringWithFormat:@"Welcome to Sim-Home in this wonderful %@!", [self time_greeting]],
                              [NSString stringWithFormat:@"Hello guest! It is currently %@", [self current_date]],
                              [NSString stringWithFormat:@"Hello guest! It is currently %@", [self current_date]],
                              [NSString stringWithFormat:@"Good %@ guest! It is currently %@", [self time_greeting], [self current_date]]
                              ];
}

- (void)init_request_device_info_messages
{
    self.request_device_info_messages = @[@"Would you like me to give you more information about the device?",
                                 @"Do you want to hear more about this device?",
                                 @"Are you interested in hearing more about the device?",
                                 @"Would you like to hear more about the device?"
                                 ];
}

- (void)init_overview_device_info_messages
{
    // Arguments: Device (1), Status (2), Power (3)
    self.overview_device_info_messages = @[@"Your %1$@ is currently turned %2$@ and using an average of %3$@.",
                                           @"Your %1$@ is powered %2$@ right now.",
                                           @"The %1$@ is currently %2$@ and averaging about %3$@.",
                                           @"The %1$@ is currently %2$@ and uses %3$@ on average."
                                           ];
}

- (void)init_connect_to_station_messages
{
    self.connect_to_station_messages = @[@"Connecting to all nearby stations.",
                                         @"Let's connect to the stations.",
                                         @"Seeing your nearby stations.",
                                         @"Here are all your nearby stations.",
                                         @"These are the stations nearby you.",
                                         @"I've foudn these stations nearby."
                                           ];
}

- (void)init_enter_overview_messages
{
    // Format with Station #
    self.enter_overview_messages = @[@"This is your overview table for %@.",
                                     @"Welcome to the overview table for %@.",
                                     @"Now displaying the overview table for %@.",
                                     @"Here is your overview table. Here are all the devices in %@.",
                                     @"Welcome to %@. Here are all the devices for the area."
                                     ];
}

- (void)init_return_to_overview_messages
{
    // Format with Station #
    self.return_to_overview_messages = @[@"Welcome back to your overview table. These are all the devices in %@.",
                                         @"I am now taking you back to the overview table for %@.",
                                         @"Here is the information for the devices in your current station.",
                                         @"Taking you back to the overview for %@."
                                         ];
}

- (void)init_already_in_overview_messages
{
    // Format with Station #
    self.already_in_overview_messages = @[@"I see that you are already in your overview table.",
                                          @"Currently displaying to you all the devices for %@.",
                                          @"Your overview table is already displayed on the screen.",
                                          @"Here is the overview table already presented for %@."
                                         ];
}

- (void)init_enter_smart_table_view_messages
{
    // Format with Device Name
    self.enter_smart_table_view_messages = @[@"Showing the smart table view for the %@.",
                                             @"Here is the smart table view for your %@.",
                                             @"Displaying the smart table view for the %@.",
                                             @"Now displaying the smart table view for your %@."
                                          ];
}

- (void)init_turn_off_station_messages
{
    // Format with Station Name
    self.turn_off_station_messages = @[@"Turning off all devices for %@.",
                                             @"Now turning off the power in %@.",
                                             @"OK, let's shut off all the power in this station.",
                                             @"Turning off all the power in this station.",
                                             @"Now shutting off the power in %@.",
                                             @"Going to shut on all the power in this station."
                                             ];
}

- (void)init_turn_on_station_messages
{
    // Format with Station Name
    self.turn_on_station_messages = @[@"Turning on all devices for %@.",
                                       @"Now turning on the power in %@.",
                                       @"OK, let's power on all the devices in this station.",
                                       @"Powering on all the devices in this station.",
                                       @"Now powering the %@.",
                                       @"Going to power on all the devices in this station."
                                       ];
}

- (void)init_turn_off_device_messages
{
    // Format with device Name
    self.turn_off_device_messages = @[@"Turning off the %@.",
                                       @"Shutting down the %@.",
                                       @"OK, let's turn off the %@.",
                                       @"Shutting off this device.",
                                       @"Going to shut off the %@.",
                                       @"Powering the %@ off."
                                       ];
}

- (void)init_turn_on_device_messages
{
    // Format with device Name
    self.turn_on_device_messages = @[@"Now turning on the %@.",
                                       @"Powering the %@.",
                                       @"OK, let's turn on the %@.",
                                       @"Now about to power this device.",
                                       @"Going to power on the %@.",
                                       @"Powering the %@ on."
                                       ];
}

- (void)init_back_to_menu_messages
{
    self.back_to_menu_messages = @[@"Going back to the menu.",
                                   @"Now taking you back to the menu.",
                                   @"Okay, let's go back to the menu.",
                                   @"Now displaying the menu."
                                   ];
}

- (void)init_back_to_station_menu_messages
{
    self.back_to_station_menu_messages = @[@"Going back to the list of connected stations.",
                                           @"Now taking you back to the station menu.",
                                           @"Okay, let's go back to your connected stations.",
                                           @"Here is a list of the connected stations."
                                   ];
}

#pragma mark -
#pragma mark Speech Handlers

- (NSString *)welcome_message
{
    NSUInteger random_index = [self random_index_for_array:self.welcome_messages];
    return [self.welcome_messages objectAtIndex:random_index];
}

- (NSString *)request_device_info_message
{
    NSUInteger random_index = [self random_index_for_array:self.request_device_info_messages];
    return [self.request_device_info_messages objectAtIndex:random_index];
}

- (NSString *)overview_device_info_message_with_device:(NSString *)device andCurrentStatus:(NSString *)status
                                         andPowerUsage:(NSString *) power
{
    NSUInteger random_index = [self random_index_for_array:self.overview_device_info_messages];
    NSString *sentence_without_parameters = [self.overview_device_info_messages objectAtIndex:random_index];
    
    return [NSString stringWithFormat:sentence_without_parameters, device, status, power];
}

- (NSString *)connect_to_station_message
{
    NSUInteger random_index = [self random_index_for_array:self.connect_to_station_messages];
    return [self.connect_to_station_messages objectAtIndex:random_index];
}

- (NSString *)enter_overview_message_with_station:(NSString *)station
{
    NSUInteger random_index = [self random_index_for_array:self.enter_overview_messages];
    NSString *sentence_without_station = [self.enter_overview_messages objectAtIndex:random_index];
    
    return [NSString stringWithFormat:sentence_without_station, station];
}

- (NSString *)return_to_overview_message_with_station:(NSString *)station
{
    NSUInteger random_index = [self random_index_for_array:self.return_to_overview_messages];
    NSString *sentence_without_station = [self.return_to_overview_messages objectAtIndex:random_index];
    
    return [NSString stringWithFormat:sentence_without_station, station];
}

- (NSString *)already_in_overview_message_with_station:(NSString *)station
{
    NSUInteger random_index = [self random_index_for_array:self.already_in_overview_messages];
    NSString *sentence_without_station = [self.already_in_overview_messages objectAtIndex:random_index];
    
    return [NSString stringWithFormat:sentence_without_station, station];
}

- (NSString *)enter_smart_table_view_message_with_device:(NSString *)device
{
    NSUInteger random_index = [self random_index_for_array:self.enter_smart_table_view_messages];
    NSString *sentence_without_device = [self.enter_smart_table_view_messages objectAtIndex:random_index];
    
    return [NSString stringWithFormat:sentence_without_device, device];
}

- (NSString *)turn_off_station_message:(NSString *)station
{
    NSUInteger random_index = [self random_index_for_array:self.turn_off_station_messages];
    NSString *sentence_without_station = [self.turn_off_station_messages objectAtIndex:random_index];
    
    return [NSString stringWithFormat:sentence_without_station, station];
}

- (NSString *)turn_on_station_message:(NSString *)station
{
    NSUInteger random_index = [self random_index_for_array:self.turn_on_station_messages];
    NSString *sentence_without_station = [self.turn_on_station_messages objectAtIndex:random_index];
    
    return [NSString stringWithFormat:sentence_without_station, station];
}

- (NSString *)turn_off_device_message:(NSString *)device
{
    NSUInteger random_index = [self random_index_for_array:self.turn_off_device_messages];
    NSString *sentence_without_device = [self.turn_off_device_messages objectAtIndex:random_index];
    
    return [NSString stringWithFormat:sentence_without_device, device];
}

- (NSString *)turn_on_device_message:(NSString *)device
{
    NSUInteger random_index = [self random_index_for_array:self.turn_on_device_messages];
    NSString *sentence_without_device = [self.turn_on_device_messages objectAtIndex:random_index];
    
    return [NSString stringWithFormat:sentence_without_device, device];
}

- (NSString *)back_to_menu_message
{
    NSUInteger random_index = [self random_index_for_array:self.back_to_menu_messages];
    return [self.back_to_menu_messages objectAtIndex:random_index];
}

- (NSString *)back_to_station_menu_message
{
    NSUInteger random_index = [self random_index_for_array:self.back_to_station_menu_messages];
    return [self.back_to_station_menu_messages objectAtIndex:random_index];
}

#pragma mark -
#pragma mark Randomizer Helpers

- (NSUInteger)random_index_for_array:(NSArray *)array
{
    NSUInteger random_index = arc4random() % [array count];
    return random_index;
}

#pragma mark -
#pragma mark Date Helpers

- (NSString *)current_date
{
    NSDate * current_datetime = [NSDate date];
    
    NSDateFormatter *date_formatter = [[NSDateFormatter alloc] init];
    [date_formatter setDateFormat:@"hh:mm a"];
    
    return [date_formatter stringFromDate:current_datetime];
}

- (NSString *)time_greeting
{
    NSString *current_date = [self current_date];
    NSString* hour = [current_date substringToIndex:2];
    
    if ([current_date containsString:@"PM"]) {
        if ([@[@"12", @"1", @"2", @"3", @"4", @"5"] containsObject:hour])
            return @"afternoon";
    }
    else {
        if ([@[@"05", @"06", @"07", @"08", @"09", @"10", @"11"] containsObject:hour])
            return @"morning";
    }
    return @"evening";
}


@end