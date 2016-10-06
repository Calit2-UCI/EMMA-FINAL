//
//  NrViewController.h
//  VirtualAssistant
//
//  Created by Enrique on 9/27/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GLViewController.h"
#import "EAGLView.h"
#import "NrMainView.h"

@protocol PassValueDelegate;
@class ModelManager;


@interface NrViewController : UIViewController
{
    GLViewController *_glController;
    ModelManager *_modelManager;
}

@property (strong, nonatomic) GLViewController *glController;
@property (strong, nonatomic) ModelManager *modelManager;
@property (nonatomic, strong) NrMainView *mainView;
@property (nonatomic) int count;
@property (nonatomic) NSString *currentStation;
@property (nonatomic) NSString *preStation;
@property (nonatomic, weak) id<PassValueDelegate> delegate;

- (NSString *)copyAudiosToReadablePath;
- (void)initModel;
- (void)createMainView;
- (void)setNotificationListeners;

// User Interface handlers

- (IBAction)toCalendarManagerClicked:(id)sender;
- (IBAction)toWeatherManagerClicked:(id)sender;



@end


@protocol PassValueDelegate <NSObject>

-(void) passValue:(NSString *) value;

@end