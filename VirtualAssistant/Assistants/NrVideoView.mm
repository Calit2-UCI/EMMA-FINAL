//
//  NrVideoView.m
//  Calit2-Assistant
//
//  Created by Sergio Gago on 4/26/13.
//  Copyright (c) 2013 Narada Robotics S.L. All rights reserved.
//

#import "NrVideoView.h"

@implementation NrVideoView

@synthesize videoCanvas, VideoNameLabel;
@synthesize mainViewController;

bool flag = NO;

- (id)initWithMainViewController:(NrCalendarMainViewController *)controller
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad ) {
        self = [[[NSBundle mainBundle] loadNibNamed:@"NrVideoView_iPad" owner:self options:nil] objectAtIndex:0];
    }
    if ((self)) {
        self.mainViewController = controller;
    }
    
    return self;
}


- (void) moviePlayBackDidFinish:(NSNotification*)notification
{
    //Eliminem la notificació del vector de notificacions (per temes de gestió de memòria)
    MPMoviePlayerController *player = [notification object];
    [[NSNotificationCenter defaultCenter]
     removeObserver:self
     name:MPMoviePlayerPlaybackDidFinishNotification
     object:player];
    
    //Aquest if es per assegurar-nos que el receptor de la notificació és un player
    //de vídeo (hauria de ser-ho sempre)
    if ([player respondsToSelector:@selector(setFullscreen:animated:)] && self.flag)
    {
        //Eliminem la vista del vídeo de la seva vista contenidora (videoCanvas)
        [player.view removeFromSuperview];
        [self StopAndHideVideoView:nil];
    }
}

- (IBAction)playMovie:(id)sender
{
    self.flag = YES;
    [self.mainViewController playMovieClicked];
}

- (IBAction)StopAndHideVideoView:(id)sender
{
    self.flag = NO;
    [self.mainViewController StopAndHideVideoViewClicked];
}

@end
