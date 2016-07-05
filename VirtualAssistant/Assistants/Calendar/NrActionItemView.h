//
//  NrWeatherItemView.h
//  VirtualAssistant
//
//  Created by Enrique on 10/16/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NrMainItemView.h"

@interface NrActionItemView : NrMainItemView

@property (nonatomic, strong) IBOutlet UILabel *shortPrediction;
@property (nonatomic, strong) IBOutlet UILabel *dateLabel;

@end
