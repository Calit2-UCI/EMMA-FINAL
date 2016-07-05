//
//  NrViewController.m
//  VirtualAssistant
//
//  Created by Enrique on 9/27/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NrViewController.h"
#import "NrAppDelegate.h"
#import "NrCalendarMainViewController.h"

@interface NrViewController ()

@end

@implementation NrViewController

@synthesize glController = _glController;
@synthesize modelManager = _modelManager;

@synthesize mainView;

CGFloat initX, initY;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self)
    {
        self.glController = [[GLViewController alloc] init];
        [self.view addSubview:self.glController.view];
        
        [self copyAudiosToReadablePath];
        
        [self initModel];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
}

BOOL first = YES;

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [self.mainView removeFromSuperview];
    
    self.glController.view.frame = self.view.frame;
    [self.glController.view removeFromSuperview];
    [self.view addSubview:self.glController.view];
    [self.glController.glView startAnimation];
    
    [self.view addSubview:self.mainView];
    
    if (!first) {
        self.glController.view.alpha = 0.0f;
    }
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    if (!first) {
        
        [UIView animateWithDuration:0.3 animations:^{
            self.glController.view.alpha = 1.0f;
            [self.glController.modelManager moveModelX:initX y:initY absolute:YES];
        } completion:^(BOOL finished) {
            NSLog(@"Completed animation");
        }];
        
    }
    first = NO;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (NSString *)copyAudiosToReadablePath
{
    NSString *readablePath = [NSFileManager documentsPath];
    
    NSArray *filesNames = [NSArray arrayWithObjects:
                           @"events-unlock",
                           @"horoscope-unlock",
                           @"notification-disclaimer",
                           @"restaurants-unlock",
                           @"weather-unlock",
                           @"welcome-assistant",
                           @"enter-location",
                           @"car-unlock",
                           @"flights-unlock",
                           @"hotels-unlock",
                           @"not-implemented1",
                           @"not-implemented2",
                           @"not-implemented3",
                           @"pubs-unlock",
                           @"vote-unlock",
                           @"future-app-update",
                           nil];
    
    NSArray *fileExtensions = [NSArray arrayWithObjects:
                               @"wav", @"FBA",
                               nil];
    
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error = nil;
    
    for (NSString *filename in filesNames)
    {
        for (NSString *extension in fileExtensions) {
            NSString *fullfilepath = [readablePath stringByAppendingPathComponent:[filename stringByAppendingPathExtension:extension]];
            
            NSLog(@"Absolute File Path %@", fullfilepath);
            
            BOOL exists = [fileManager fileExistsAtPath:fullfilepath];
            
            if (exists) {
                [fileManager removeItemAtPath:fullfilepath error:&error];
                NSAssert2(error == nil, @"Error removing path %@, error: %@", fullfilepath, [error localizedDescription]);
            }
            
            //                NSString *defaultPath = [[NSBundle mainBundle] pathForResource:filename ofType:extension];
            NSString *defaultPath = [[[[NSBundle mainBundle].resourcePath stringByAppendingPathComponent:NSLocalizedString(@"LOCALIZATION_DIR", nil)] stringByAppendingPathComponent:filename] stringByAppendingPathExtension:extension];
            
            NSLog(@"Readable Path %@", readablePath);
            NSLog(@"Default Path %@", defaultPath);
            
            BOOL success = [fileManager copyItemAtPath:defaultPath toPath:fullfilepath error:&error];
            
            if (!success)
                NSAssert1(0, @"Failed to create writable file with message '%@'.", [error localizedDescription]);
            //            }
        }
    }
    
    return readablePath;
}


- (void)initModel
{
    [self setNotificationListeners];
    self.modelManager = [self.glController loadModelWithPath:@"reana.afm" AndBackground:@"office_white_hall.jpg" AndPerspective:YES];
    
    
    [self.glController.glView startAnimation];
}

- (void)createMainView
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        self.mainView = [[[NSBundle mainBundle] loadNibNamed:@"NrMainView_iPhone" owner:self options:nil] objectAtIndex:0];
    } else {
        self.mainView = [[[NSBundle mainBundle] loadNibNamed:@"NrMainView_iPad" owner:self options:nil] objectAtIndex:0];
    }
    
    self.mainView.mainController = self;
    
    [self.view addSubview:self.mainView];
}

- (void)setNotificationListeners
{
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleModelLoaded:) name:notificationModelCompletelyLoaded object:nil];
}

- (void)handleModelLoaded:(NSNotification *)notification
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        // IPHONE
        if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] == YES && [[UIScreen mainScreen] scale] == 2.00) {
            // RETINA DISPLAY
            initY = 100;
        }
        else {
            // NO RETINA DISPLAY
            initY = 50;
        }
    }
    else {
        // IPAD
        if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] == YES && [[UIScreen mainScreen] scale] == 2.00) {
            // RETINA DISPLAY
            initY = 200;
        }
        else {
            initY = 100;
        }
    }
    
    [self.modelManager moveModelY:initY absolute:NO];

    [self.modelManager rotateModelX:-6 y:0 absolute:NO];
    

    
    [self toCalendarManagerClicked:nil];
}

#pragma mark User Interface handlers

- (IBAction)toCalendarManagerClicked:(id)sender
{    

    
    NrCalendarMainViewController *calVC;
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        calVC = [[NrCalendarMainViewController alloc] initWithNibName:@"NrMainViewController_iPhone" bundle:nil];
    }
    else {
        calVC = [[NrCalendarMainViewController alloc] initWithNibName:@"NrMainViewController_iPad" bundle:nil];
    }
    
    [self presentViewController:calVC animated:NO completion:^{
        NSLog(@"Ended presenting view controlller");
    }];
}

@end
