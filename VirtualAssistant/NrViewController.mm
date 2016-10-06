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
#import "BeaconDetails.h"
#import "BeaconDetailsCloudFactory.h"
#import "CachingContentFactory.h"
#import "ProximityContentManager.h"

@interface NrViewController ()<ProximityContentManagerDelegate>
@property (nonatomic) ProximityContentManager *proximityContentManager;

@end


@protocol BeaconValueDelegate <NSObject>

- (void) passValue:(NSString *) value;

@end

@implementation NrViewController

@synthesize glController = _glController;
@synthesize modelManager = _modelManager;
NrCalendarMainViewController *calVC;
@synthesize mainView;

CGFloat initX, initY;
BOOL first = YES;
int station;

id<PassValueDelegate> strongDelegate;


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
    
//    self.proximityContentManager = [[ProximityContentManager alloc]
//                                    initWithBeaconIDs:@[
//                                                        [[BeaconID alloc] initWithUUIDString:@"B9407F30-F5F8-466E-AFF9-25556B57FE6D" major:10575 minor:30159],
//                                                        [[BeaconID alloc] initWithUUIDString:@"B9407F30-F5F8-466E-AFF9-25556B57FE6D" major:12315 minor:42375],
//                                                        [[BeaconID alloc] initWithUUIDString:@"B9407F30-F5F8-466E-AFF9-25556B57FE6D" major:21412 minor:5512]
//                                                        ]
//                                    beaconContentFactory:[[CachingContentFactory alloc] initWithBeaconContentFactory:[BeaconDetailsCloudFactory new]]];
//    self.proximityContentManager.delegate = self;
//    
//    [self.proximityContentManager startContentUpdates];
//    strongDelegate = self.delegate;
//    station = 0;
//    _count = 0;
//    _currentStation =@"candy";
//    _preStation = nil;
}


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
    self.proximityContentManager = [[ProximityContentManager alloc]
                                    initWithBeaconIDs:@[
                                                        [[BeaconID alloc] initWithUUIDString:@"B9407F30-F5F8-466E-AFF9-25556B57FE6D" major:10575 minor:30159],
                                                        [[BeaconID alloc] initWithUUIDString:@"B9407F30-F5F8-466E-AFF9-25556B57FE6D" major:12315 minor:42375],
                                                        [[BeaconID alloc] initWithUUIDString:@"B9407F30-F5F8-466E-AFF9-25556B57FE6D" major:21412 minor:5512]
                                                        ]
                                    beaconContentFactory:[[CachingContentFactory alloc] initWithBeaconContentFactory:[BeaconDetailsCloudFactory new]]];
    self.proximityContentManager.delegate = self;
    
    [self.proximityContentManager startContentUpdates];
    strongDelegate = self.delegate;
    station = 0;
    _count = 0;
    _currentStation =@"candy";
    _preStation = nil;

}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}



//useless,cuz we don't even use it.
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
    self.modelManager = [self.glController loadModelWithPath:@"reana.afm" AndBackground:@"Background6.png" AndPerspective:YES];
    
    
    [self.glController.glView startAnimation];
}

- (void)createMainView
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        self.mainView = [[[NSBundle mainBundle] loadNibNamed:@"NrMainView_iPhone" owner:self options:nil] objectAtIndex:0];
    } else {
        self.mainView = [[[NSBundle mainBundle] loadNibNamed:@"NrMainView_iPad" owner:self options:nil] objectAtIndex:0];
    }
    
    //interesting code.
    
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
    

    //to initiaze the IBAction.
    [self toCalendarManagerClicked:nil];
}

#pragma mark User Interface handlers

- (IBAction)toCalendarManagerClicked:(id)sender
{    

    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        calVC = [[NrCalendarMainViewController alloc] initWithNibName:@"NrMainViewController_iPhone" bundle:nil];
    }
    else {
        calVC = [[NrCalendarMainViewController alloc] initWithNibName:@"NrMainViewController_iPad" bundle:nil];
    }
    
    //calVC.stationChosen = station;
    calVC.currentStation = @"beetroot";
    
    [self presentViewController:calVC animated:NO completion:^{
        NSLog(@"Ended presenting view controlller");
    }];
}

- (void)proximityContentManager:(ProximityContentManager *)proximityContentManager didUpdateContent:(id)content {

    
    BeaconDetails *beaconDetails = content;
    if (beaconDetails) {
        NSLog(@"%@",beaconDetails.beaconName);
////        self.currentStation = beaconDetails.beaconName;
//        if([beaconDetails.beaconName isEqualToString:@"beetroot"])
//        {
//            //calVC.stationChosen = 0;
//            
//        }
////        [strongDelegate passValue:beaconDetails.beaconName];
//        if([beaconDetails.beaconName isEqualToString:@"candy"])
//        {
//            //calVC.stationChosen = 1;
//                    }
//        if([beaconDetails.beaconName isEqualToString:@"lemon"])
//        {
//            //calVC.stationChosen = 2;
//                    }
//        //[self dataUpdate:beaconDetails.beaconName];
        
        //[self dataUpdate:beaconDetails.beaconName];
        calVC.currentStation = beaconDetails.beaconName;
        
    } else {
        NSLog(@"Not beacons");
        //calVC.stationChosen = 0;
//        self.currentStation = nil;
    }
}


#pragma mark-
#pragma alert

//-(void) dataUpdate:(NSString *) station{
//    NSLog(@"data updated! the station is %@",station);
//    if ([_currentStation isEqualToString:station] ) {
//        _count++;
//        if(_count < 4)
//        {
//            NSLog(@"count is %d, less than 10s",_count);
//        }
//        else{
//            NSLog(@"enough");
//            _count = 0;
//            if (![_preStation isEqualToString:station] ) {
////                [self pushAlertButton:station];
//                if([station isEqualToString:@"beetroot"]){
//                     calVC.stationChosen = 0;
//                }
//                if([station isEqualToString:@"candy"]){
//                    calVC.stationChosen = 1;
//                }
//                if([station isEqualToString:@"lemon"]){
//                    calVC.stationChosen = 2;
//                }
//                calVC.currentStation = station;
//                _preStation = station;
//            }
//        }
//    } else {
//        _count = 1;
//        _currentStation = station;
//        NSLog(@"New Stations");
//    }
//}

//- (void)makeAlertController:(UIAlertController *)ac{
//    
//    [ac addAction:[UIAlertAction
//                   actionWithTitle:@"Yes"
//                   style:UIAlertActionStyleDefault
//                   handler:^(UIAlertAction *action) {
//                       [self yesButtonPushed];
//                   }]];
//    
//    [ac addAction:[UIAlertAction
//                   actionWithTitle:@"Cancel"
//                   style:UIAlertActionStyleCancel
//                   handler:^(UIAlertAction *action) {
//                       [self cancelButtonPushed];
//                   }]];
//    [self presentViewController:ac animated:YES completion:nil];
//}
//
//- (void)cancelButtonPushed {
//    NSLog(@"Cancel");
//}
//- (void)yesButtonPushed{
//    NSLog(@"YEs!");
//}
//
//- (void)pushAlertButton:(NSString *) station{
//    
//    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:[NSString stringWithFormat:@"%@ is detected!",station] message:nil preferredStyle:UIAlertControllerStyleAlert];
//    
//    [self makeAlertController:alertController];
//}
@end
