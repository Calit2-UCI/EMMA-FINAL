//
//  NrWeatherItemView.m
//  VirtualAssistant
//
//  Created by Enrique on 10/16/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NrWeatherItemView.h"

@implementation NrWeatherItemView

- (id)initWithID:(NSInteger)theItemID
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        self = [[[NSBundle mainBundle] loadNibNamed:@"NrWeatherItemView_iPhone" owner:self options:nil] objectAtIndex:0];
    }
    else {
        self = [[[NSBundle mainBundle] loadNibNamed:@"NrWeatherItemView_iPad" owner:self options:nil] objectAtIndex:0];
    }
    
    if (self) {
        self.itemID = theItemID;
    }
    
    self.backgroundColor = [UIColor clearColor];
    return self;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end
