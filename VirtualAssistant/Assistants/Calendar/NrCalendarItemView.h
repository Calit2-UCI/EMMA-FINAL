//
//  NrCalendarItemView.h
//  VirtualAssistant
//
//  Created by Enrique on 10/2/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface NrCalendarItemView : UIControl

@property (nonatomic, strong) IBOutlet UILabel *dayName;
@property (nonatomic, strong) IBOutlet UILabel *eventCounter;
@property (nonatomic, strong) IBOutlet UIImageView *imgView;
@property (nonatomic, strong) IBOutlet UILabel *dateLabel;
@property (nonatomic) NSInteger itemID;

- (id)initWithID:(NSInteger)theItemID;


@end
