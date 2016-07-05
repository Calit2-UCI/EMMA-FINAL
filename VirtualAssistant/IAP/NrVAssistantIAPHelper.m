//
//  NrVAssistantIAPHelper.m
//  VirtualAssistant
//
//  Created by Enrique on 10/16/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NrVAssistantIAPHelper.h"

@implementation NrVAssistantIAPHelper

static NrVAssistantIAPHelper * _sharedHelper;

+ (NrVAssistantIAPHelper *)sharedHelper
{
    if (_sharedHelper != nil) {
        return _sharedHelper;
    }
    _sharedHelper = [[NrVAssistantIAPHelper alloc] init];
    return _sharedHelper;
}

- (id)init
{
    NSSet * productIdentifiers = [NSSet setWithObjects:
                                  @"com.naradarobotics.MyVirtualAssistant.unlock",
                                  nil];
    self = [super initWithProductIdentifiers:productIdentifiers];
    if (self) {
        
    }
    return self;
}

@end
