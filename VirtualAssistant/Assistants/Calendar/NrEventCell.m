//
//  NrEventCell.m
//  VirtualAssistant
//
//  Created by Enrique on 10/3/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NrEventCell.h"

@implementation NrEventCell

@synthesize eventTitle;
@synthesize eventTime;

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        // Initialization code
    }
    return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
