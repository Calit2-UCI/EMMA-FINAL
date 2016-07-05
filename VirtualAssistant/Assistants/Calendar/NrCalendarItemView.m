//
//  NrCalendarItemView.m
//  VirtualAssistant
//
//  Created by Enrique on 10/2/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NrCalendarItemView.h"

@implementation NrCalendarItemView

@synthesize dayName;
@synthesize eventCounter;
@synthesize imgView;
@synthesize itemID;

- (id)initWithID:(NSInteger)theItemID
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        self = [[[NSBundle mainBundle] loadNibNamed:@"NrCalendarItemView_iPhone" owner:self options:nil] objectAtIndex:0];
    }
    else {
        self = [[[NSBundle mainBundle] loadNibNamed:@"NrCalendarItemView_iPad" owner:self options:nil] objectAtIndex:0];
    }
    
    if (self) {
        self.itemID = theItemID;
    }
    
    self.backgroundColor = [UIColor clearColor];
    
    return self;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
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
