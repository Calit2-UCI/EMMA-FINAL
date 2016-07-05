//
//  NrDeviceItemView.m
//  UCIAssistant
//
//  Created by Christian Morte on 5/11/16.
//  Copyright (c) 2016 Narada Robotics S.L. All rights reserved.
//


#import "NrDeviceItemView.h"

@implementation NrDeviceItemView

- (id)initWithID:(NSInteger)theItemID
{
    // Uncomment this piece if we plan to import into iPhone
    
//    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
//        self = [[[NSBundle mainBundle] loadNibNamed:@"NrActionItemView_iPhone" owner:self options:nil] objectAtIndex:0];
//    }
//    else {
//        self = [[[NSBundle mainBundle] loadNibNamed:@"NrDeviceItemView_iPad" owner:self options:nil] objectAtIndex:0];
//    }
    
    self = [[[NSBundle mainBundle] loadNibNamed:@"NrDeviceItemView_iPad" owner:self options:nil] objectAtIndex:0];
    
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