//
//  NrDataHandler.h
//  UCIAssistant
//
//  Created by Christian Morte on 11/4/16.
//  Copyright © 2016 Narada Robotics S.L. All rights reserved.
//

#ifndef NrDataHandler_h
#define NrDataHandler_h

@interface NrDataHandler : NSObject


@property (nonatomic, strong) NSMutableDictionary* data;
@property (nonatomic, strong) NSDictionary *realStationNames,
                                           *bluetoothStationNames;
@property (nonatomic, strong) NSMutableArray *stationList;

-(void)updateData;

-(void)downloadDeviceData;
-(void)downloadStationData;

-(NSDictionary *)stationDataForStation:(NSString *)station;
-(NSDictionary *)deviceDataForDevice:(NSString *)deviceNumber atStation:(NSString *)station;

@end



#endif /* NrDataHandler_h */
