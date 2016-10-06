//
//  NrQuizView.m
//  Calit2-Assistant
//
//  Created by Sergio Gago on 4/3/13.
//  Copyright (c) 2013 Narada Robotics S.L. All rights reserved.
//

#import "NrQuizView.h"

@implementation NrQuizView

@synthesize answer1;
@synthesize answer2;
@synthesize answer3;
@synthesize answer4;
@synthesize question;
@synthesize progressText;
@synthesize mainViewController;
@synthesize progressBar;


- (id)initWithMainViewController:(NrCalendarMainViewController *)controller
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad ) {
        self = [[[NSBundle mainBundle] loadNibNamed:@"NrQuizView_iPad" owner:self options:nil] objectAtIndex:0];
    }
    if ((self)) {
        self.mainViewController = controller;        
    }
    
    return self;
}

- (IBAction)answer1Clicked:(id)sender
{
    NSLog(@"Hello Answer 1");
    [self.mainViewController answer1Clicked];
}

- (IBAction)answer2Clicked:(id)sender
{
    NSLog(@"Hello Answer 2");
    [self.mainViewController answer2Clicked];
}

- (IBAction)answer3Clicked:(id)sender
{
    NSLog(@"Hello Answer 3");
    [self.mainViewController answer3Clicked];
}

- (IBAction)answer4Clicked:(id)sender
{
    NSLog(@"Hello Answer 4");
    [self.mainViewController answer4Clicked];
}

- (IBAction)repeatClicked:(id)sender
{
    NSLog(@"Hello repeat");
    [self.mainViewController repeatClicked];
}

- (IBAction)exitClicked:(id)sender
{
    NSLog(@"Hello exit");
    [self.mainViewController exitClicked];
}

@end
