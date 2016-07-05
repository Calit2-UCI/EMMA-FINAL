//
//  NrWeatherMainViewController.h
//  VirtualAssistant
//
//  Created by Enrique on 10/10/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NrMainViewController.h"
#import "NaradaWeatherManager.h"

@interface NrWeatherMainViewController : NrMainViewController

@property (nonatomic, strong) NaradaWeatherManager *weatherManager;

@end
