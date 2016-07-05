//
//  NrDeviceItemView.h
//  UCIAssistant
//
//  Created by Christian Morte on 5/11/16.
//  Copyright (c) 2016 Narada Robotics S.L. All rights reserved.
//

#ifndef UCIAssistant_NrDeviceItemView_h
#define UCIAssistant_NrDeviceItemView_h

#import "NrMainItemView.h"

@interface NrDeviceItemView : NrMainItemView

@property (nonatomic, strong) IBOutlet UILabel *dayName;
@property (nonatomic, strong) IBOutlet UIImageView *imgView;

@end

#endif
