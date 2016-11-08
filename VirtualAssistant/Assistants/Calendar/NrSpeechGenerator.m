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
    self.welcome_messages = @[@"Welcome to Sim-Home!",
                              @"Hello and welcome to Sim-Home!",
                              @"Hi there. Welcome to Sim-Home.",
                              [NSString stringWithFormat:@"Good %@ and welcome to Sim-Home!", [self time_greeting]],
                              [NSString stringWithFormat:@"Hi there and welcome to Sim-Home. It is currently %@", [self current_date]],
                              [NSString stringWithFormat:@"Hello, welcome to Sim-Home. It is currently %@", [self current_date]],
                              [NSString stringWithFormat:@"Good %@. Glad to have you at Sim-Home. It is currently %@.", [self time_greeting], [self current_date]]
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
    // Arg1: length of station list
    self.connect_to_station_messages = @[@"Here are %d of the rooms in Sim-Home you can explore.",
                                         @"I see %d rooms that you can view. ",
                                         @"Okay, I have found %d rooms that you are able to connect to inside Sim-Home. ",
                                         @"Here are %d nearby areas of Sim-Home you can explore. ",
                                         @"Alright, I'm connecting you to %d areas you can view inside Sim-Home. ",
                                         @"I am now connecting you to %d Sim-Home rooms you can check out. "
                                           ];
    
    // Arg1: array-to-sentence for station list
    self.connected_stations_messages = @[@"",
                                         @"",
                                         @"These are the %@. ",
                                         @"I'm connected to the %@. ",
                                         @"I have found the %@. ",
                                         @"You can check out the %@. ",
                                         @"I see the %@ are connected. "
                                         ];
}

- (void)init_enter_overview_messages
{
    // Format with Station #
    self.enter_overview_messages = @[@"Here is the %@. You can select from the top bar what info you would like to see for the area. ",
                                     @"Here is your %@. ",
                                     @"This is your %@ area. ",
                                     @"Here is some information for the %@. ",
                                     @"Welcome to the %@ for Sim-Home. ",
                                     @"Welcome to the Sim-Home %@. Here are all the devices for the area. "
                                     ];
    
    self.overview_device_messages = @[@"I notice you have connected your %@ in here. ",
                                      @"You have your %@ all connected. ",
                                      @"I can see that your %@ are connected right now. ",
                                      @"Here you have your %@ connected in the area. "];
    
    // Arg1: station, Arg2: device, Arg3: Power
    self.device_with_most_usage_messages = @[@"The %2$@ has been using the most power for your %1$@ at about %3$d watts. ",
                                             @"Your %2$@ is using up the most power at %3$d watts for your %1$@ . ",
                                             @"I see that the %2$@ has been consuming the most power in your %1$@, and has been using up to %3$d watts. ",
                                             @"The %1$@'s most consuming device has been the %2$@ at %3$d watts. "];
}


- (void)init_return_to_overview_messages
{
    // Format with Station #
    self.return_to_overview_messages = @[@"Welcome back to your overview for the %@. These are all the devices in area. Use the pie chart and tables to interact and find out more information for each device.",
                                         @"Okay, here's the overview for the Sim-Home %@. You can interact with the devices in the area using the table and the pie chart. ",
                                         @"Here is the overview information for the %@ in Sim-home. Here you can see information for the current status of each device in the area. ",
                                         @"Welcome back to the Sim-Home %@ overview. Here is the table for the area. You can use the switches and pie chart to interact and figure out more information for each device. "
                                         ];
}

- (void)init_already_in_overview_messages
{
    // Format with Station #
    self.already_in_overview_messages = @[@"I see that you are already in the overview for your %@. You can click on the devices in the top bar to see more information for each device, or interact with the tables and pie chart currently displayed for you.",
                                          @"I already have your %@ information displayed here. You can choose more from the devices in the top bar to see each individual device.",
                                          @"The overview for the Sim-Home %@ is already displayed. You can power devices on and off using the switches on the table, or control them individually from the top bar.",
                                          @"Here is the overview for the Sim-Home %@ already displayed for you. You can control all devices in the area from here, or use the top bar ot view an individual device."
                                         ];
}

- (void)init_enter_smart_table_view_messages
{
    // Format with Device Name
    self.enter_smart_table_view_messages = @[@"Here is some information for your %@. ",
                                             @"Now displaying the data for the %@. ",
                                             @"Okay, this is Sim-Home's %@. ",
                                             @"Here is your %@. "
                                          ];
    
    self.smart_device_info_messages = @{
                                        // Living Room Devices
                                        @"TV": @[@"I see that your TV has been on throughout most of the night. You can use the auto-sleep feature present in most TV's to have it turn off whenever it is not being used.",
                                                 @"Your TV could improve its power usage by using the auto-sleep feature every three hours of inactivity.",
                                                 @"I can see that the TV has been using a lot of power during the night time. If you are sleeping during this time, I suggest you implement the auto-sleep feature to save more power in your home.",
                                                 @"",
                                                 @""
                                                 ],
                                        @"Apple TV": @[@"The Apple TV has been pretty consistent with its usage in the past week. You have been doing a great job at controlling its power usage.",
                                                       @"This device is a great compliment to the TV and conserves a good amount of power in stand-by mode.",
                                                       @"",
                                                       @""
                                                ],
                                        @"Blu-Ray": @[@"I see that your Blu-Ray player is used mostly during the evenings. To save more power for this device, I suggest unplugging it whenever it is not being used. This can help conserve more power in the living room.",
                                                      @"The Blu-Ray is one of the least consuming devices in the area overall, seeing that you only use it from time to time. As long as it is left unplugged instead of being left on stand by, you can save more energy.",
                                                      @"Your Blu-Ray is the optimal choice in the market in terms of saving energy. Compared to other media players in the market, your Blu-Ray device saves more than 30 percent than the market average.",
                                                      @"",
                                                      @""
                                                ],
                                        
                                        // Kitchen devices
                                        @"Coffee Maker": @[@"Your coffee maker is mostly active in the mornings between 6 to 9 AM. I suggest before someon leaves the house, they turn off the device to reduce the plug load usage in the kitchen.",
                                                           @"The coffee maker is mostly used during the mornings. To further improve its power usage, we can turn off or unplug the device after using it.",
                                                           @"I see that the coffee maker has been on or plugged in during the nights, which is unusual for its purpose. I suggest checking the status of the device if not being used to save more power in the kitchen.",
                                                           @"",
                                                           @""
                                                ],
                                        @"Microwave": @[@"Your microwave's usage is very sporadic throughout the day. Despite this, it is a very energy-safe device and saves more than 40% in energy than the average microwave in the market",
                                                        @"The microwave is one of the higher usage devices inside the kitchen area. However, compared to the average market brand, it is one of the least consuming brands in the market.",
                                                        @"",
                                                        @""],
                                        @"Rice Cooker": @[@"I can see that the rice cooker is sporadically on throughout the day, particularly in the mornings and evenings. Assuming this is used before meals, I suggest turning off the device from here after its usage.",
                                                          @"Your rice cooker has been consuming a fair amount of power, mostly in the mornings and evenings. From what I can see in the chart, shutting this device off during the afternoons can allow up to 30 dollars a month in savings.",
                                                          @"",
                                                          @""]
                                        };
}

- (void)init_turn_off_station_messages
{
    // Format with Station Name
    self.turn_off_station_messages =   @[@"Turning off all devices for %@.",
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
                                       @"Powering the %@ on.",
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
                                   @"Now displaying the menu.",
                                   @"",
                                   @""
                                   ];
}

- (void)init_back_to_station_menu_messages
{
    self.back_to_station_menu_messages = @[@"Okay, here's the areas of Sim-Home you can explore.",
                                           @"Going back to the list of Sim-Home rooms.",
                                           @"Alright, let's take you back to the Sim-Home areas you can monitor.",
                                           @"Here are the rooms of Sim-Home you can check out."
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

- (NSString *)connect_to_station_message_with_station_list:(NSMutableArray *)stationList
{
    NSUInteger random_index = [self random_index_for_array:self.connect_to_station_messages],
    random_index_1 = [self random_index_for_array:self.connected_stations_messages];
    
    
    return [[NSString stringWithFormat:[self.connect_to_station_messages objectAtIndex:random_index], [stationList count]] stringByAppendingString:[NSString stringWithFormat:[self.connected_stations_messages objectAtIndex:random_index_1], [self array_to_sentence_for_array:[self randomize_array:stationList]]]];
}

- (NSString *)enter_overview_message_with_station:(NSString *)station withStationData:(NSDictionary *)stationData
{
    NSUInteger random_index_1 = [self random_index_for_array:self.enter_overview_messages],
               random_index_2 = [self random_index_for_array:self.overview_device_messages];
    NSString *sentence_without_station = [self.enter_overview_messages objectAtIndex:random_index_1];
    
    NSString *overview_device_sentence = [self.overview_device_messages objectAtIndex:random_index_2];
    
    NSMutableArray *devices = [[NSMutableArray alloc] init];
    NSString *devices_for_overview;
    
    for (id device in [stationData objectForKey:@"Devices"]) {
        [devices addObject:stationData[@"Devices"][device][@"Name"]];
    }
    devices_for_overview = [self array_to_sentence_for_array:[self randomize_array:devices]];
    
    NSString *device_with_max_power = [self maxDeviceStringForStation:station withData:stationData];
    
    return [[[NSString stringWithFormat:sentence_without_station, station] stringByAppendingString:[NSString stringWithFormat:overview_device_sentence, devices_for_overview]] stringByAppendingString:device_with_max_power];
}



- (NSString *)return_to_overview_message_with_station:(NSString *)station withStationData:(NSDictionary *)stationData
{
    // Message: Return to overview, mention device with most usage
    
    NSUInteger random_index = [self random_index_for_array:self.return_to_overview_messages];
    NSString *sentence_without_station = [self.return_to_overview_messages objectAtIndex:random_index];
    
    NSString *device_with_max_power = [self maxDeviceStringForStation:station withData:stationData];
    
    return [[NSString stringWithFormat:sentence_without_station, station] stringByAppendingString:device_with_max_power];
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
    
    random_index = [self random_index_for_array:self.smart_device_info_messages[device]];
    NSString *smart_sentence_for_device = [self.smart_device_info_messages[device] objectAtIndex:random_index];
    
    return [[NSString stringWithFormat:sentence_without_device, device] stringByAppendingString:smart_sentence_for_device];
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
#pragma mark Sentence Helpers

- (NSString *)array_to_sentence_for_array:(NSArray *)array
{
    NSInteger length = [array count];
    if (length == 0)
        return @"";
    else if (length == 1)
        return array[0];
    
    NSString *result = array[0];
    for (NSInteger i = 1; i < length; i++)
        result = [[result stringByAppendingString: i == length-1 ? @", and " : @"," ]
                  stringByAppendingString:array[i]];
    return result;
}


-(NSString *)maxDeviceStringForStation:(NSString *)station withData:(NSDictionary *)stationData
{
    NSMutableArray *max_devices = [[NSMutableArray alloc] init];
    NSInteger max_usage = 0;
    
    for (id device in [stationData objectForKey:@"Devices"]) {
        NSInteger watts = [stationData[@"Devices"][device][@"Watts"] integerValue];
        if (watts > max_usage) {
            [max_devices setArray:@[]];
            [max_devices addObject:stationData[@"Devices"][device][@"Name"]];
            max_usage = [stationData[@"Devices"][device][@"Watts"] integerValue];
            
        } else if (watts == max_usage)
            [max_devices addObject:stationData[@"Devices"][device][@"Name"]];
    }
    
    // Arg1: station, Arg2: device, Arg3: Power
    NSUInteger random_index = [self random_index_for_array:self.device_with_most_usage_messages];
    
    if (max_usage == 0)
        return @"";
    
    if ([max_devices count] > 1)
        max_devices = [NSMutableArray arrayWithObject:[max_devices objectAtIndex:0]];
        
    NSString *device_with_max_power = [NSString stringWithFormat:[self.device_with_most_usage_messages objectAtIndex:random_index], station, [self array_to_sentence_for_array:max_devices], max_usage];
    
    return device_with_max_power;
}

/*
 "Device 2" : {
 "Name" : "TV",
 "Brand": "Samsung",
 "Model": "UN50JU6500F",
 "Type" : "Entertainment",
 "Total Usage/Week" : 25,
 "Watts" : 150,
 "Status" : "On",
 "Daily" : [[1471071541, 0.0014], [1471503540, 0.9144], [1474009140, 0.0613], [1474095540, 0.0645], [1474354740, 0.3375], [1475218740, 0.3087], [1475305140, 0.2157], [1475477940, 1.1247], [1475564340, 0.1357], [1475650740, 0.1629], [1475737140, 0.0014], [1475823540, 0.3894], [1476255540, 0.3611], [1476341940, 0.0041], [1476860341, 0.0063], [1477119540, 0.0875], [1477292340, 0.2849], [1477378740, 2.0783], [1477637940, 0.6834]]
 }
 */
-(NSString *)dayWithMostUsageForDeviceData:(NSDictionary *)deviceData
{
    NSMutableArray *max_devices = [[NSMutableArray alloc] init];
    NSInteger max_usage = 0;
    
    for (id device in [stationData objectForKey:@"Devices"]) {
        NSInteger watts = [stationData[@"Devices"][device][@"Watts"] integerValue];
        if (watts > max_usage) {
            [max_devices setArray:@[]];
            [max_devices addObject:stationData[@"Devices"][device][@"Name"]];
            max_usage = [stationData[@"Devices"][device][@"Watts"] integerValue];
            
        } else if (watts == max_usage)
            [max_devices addObject:stationData[@"Devices"][device][@"Name"]];
    }
    
    // Arg1: station, Arg2: device, Arg3: Power
    NSUInteger random_index = [self random_index_for_array:self.device_with_most_usage_messages];
    
    if (max_usage == 0)
        return @"";
    
    if ([max_devices count] > 1)
        max_devices = [NSMutableArray arrayWithObject:[max_devices objectAtIndex:0]];
    
    NSString *device_with_max_power = [NSString stringWithFormat:[self.device_with_most_usage_messages objectAtIndex:random_index], station, [self array_to_sentence_for_array:max_devices], max_usage];
    
    return device_with_max_power;
}



#pragma mark -
#pragma mark Randomizer Helpers

- (NSUInteger)random_index_for_array:(NSArray *)array
{
    NSUInteger random_index = arc4random() % [array count];
    return random_index;
}

- (NSArray *)randomize_array:(NSMutableArray *)array
{
    NSUInteger random_index;
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    while ([array count] > 0) {
        random_index = arc4random() % [array count];
        [result addObject:[array objectAtIndex:random_index]];
        [array removeObjectAtIndex:random_index];
    }
    return result;
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
