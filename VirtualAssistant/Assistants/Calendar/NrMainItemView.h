//
//  NrMainItemView.h
//  VirtualAssistant
//
//  Created by Enrique on 10/16/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface NrMainItemView : UIControl

@property (nonatomic, strong) IBOutlet UILabel *dayName;
@property (nonatomic, strong) IBOutlet UIImageView *imgView;
@property (nonatomic, assign) NSInteger itemID;

- (id)initWithID:(NSInteger)theItemID;

@end
