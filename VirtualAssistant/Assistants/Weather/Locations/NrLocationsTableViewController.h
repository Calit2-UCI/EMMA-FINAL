//
//  NrLocationsTableViewController.h
//  VirtualAssistant
//
//  Created by Enrique on 10/15/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "NrWeatherLocationManager.h"

@interface NrLocationsTableViewController : UITableViewController

@property (nonatomic, retain) UINavigationController *navController;
@property (nonatomic, retain) NrWeatherLocationManager * weatherLocMng;

- (id)initWithLocationManager:(NrWeatherLocationManager *)mng;

@end
