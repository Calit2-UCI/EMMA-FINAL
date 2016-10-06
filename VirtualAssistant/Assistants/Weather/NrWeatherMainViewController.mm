//
//  NrWeatherMainViewController.m
//  VirtualAssistant
//
//  Created by Enrique on 10/10/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NrWeatherMainViewController.h"

@interface NrWeatherMainViewController ()

@end

@implementation NrWeatherMainViewController

@synthesize weatherManager;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
    
    weatherManager = [[NaradaWeatherManager alloc] init];
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
