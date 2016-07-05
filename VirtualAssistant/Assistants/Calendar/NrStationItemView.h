//
//  NrStationItemView.h
//  UCIAssistant
//
//  Created by Christian Morte on 5/29/16.
//  Copyright (c) 2016 Narada Robotics S.L. All rights reserved.
//

#ifndef UCIAssistant_NrStationItemView_h
#define UCIAssistant_NrStationItemView_h

#import "NrMainItemView.h"

@interface NrStationItemView : NrMainItemView

@property (nonatomic, strong) IBOutlet UILabel *dayName;
@property (nonatomic, strong) IBOutlet UIImageView *imgView;

@end

#endif
