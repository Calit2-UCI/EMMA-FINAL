//
//  NrEventCell.h
//  VirtualAssistant
//
//  Created by Enrique on 10/3/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface NrEventCell : UITableViewCell

@property (nonatomic, strong) IBOutlet UILabel *eventTitle;
@property (nonatomic, strong) IBOutlet UILabel *eventTime;

@end
