//
//  NrMainViewControllerDelegate.h
//  VirtualAssistant
//
//  Created by Enrique on 10/8/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol NrMainViewControllerDelegate <NSObject>

- (void)handleWillBeginSpeaking;
- (void)handleDidFinishSpeaking;

@end
