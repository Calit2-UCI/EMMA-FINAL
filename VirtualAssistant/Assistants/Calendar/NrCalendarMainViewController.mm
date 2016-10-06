//
//  NrCalendarMainViewController.m
//  VirtualAssistant
//
//  Created by Enrique on 9/28/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NaradaDownloader.h"
#import "NaradaDownloaderDelegate.h"
#import "NrCalendarMainViewController.h"
#import "NrAppDelegate.h"

#import "NrVideoView.h"
#import "NrTableViewController.h"
#import "NrSmartTableViewController.h"
#import "NrGridTableViewCell.h"


#import "BeaconDetails.h"
#import "BeaconDetailsCloudFactory.h"
#import "CachingContentFactory.h"
#import "ProximityContentManager.h"

//TODO:back gesture


#import "NrSpeechGenerator.h"

@interface NrCalendarMainViewController ()<ProximityContentManagerDelegate>

- (void)makeTableVisible;
- (BOOL)canSpeak;

- (void)removeAudios;
@property (nonatomic) ProximityContentManager *proximityContentManager;
//@property (nonatomic) NrViewController *nrViewController;
@end

@implementation NrCalendarMainViewController

@synthesize selectedDayName;
@synthesize itemList;
@synthesize scroll;
//@synthesize hud;
@synthesize backButton;
@synthesize skipButton;
@synthesize exitButton;
@synthesize action_flags;
@synthesize lampValue;
@synthesize fanValue;
//@synthesize TimerButton;


// *** App members ***
BOOL appLocked = YES;
UIAlertView *cityAlertView, *buyAlertView, *facebookAlert, *voteOptionAlert, *pendingNotificatonAlert;

BOOL postImmediately = NO;
BOOL fromLoadProfile = NO;
BOOL moreShown = NO;
BOOL pendingNotification = NO;
BOOL started;
BOOL timerStarted = false;
BOOL canClickBack = YES;
int counter;
int count = 0;
NrSpeechGenerator *speech_generator;
UITapGestureRecognizer *singleFingerTap;

//  *** Action members ***
//TODO
//use local file instead google drive
NSString* json_link = @"https://drive.google.com/a/uci.edu/uc?export&confirm=no_antivirus&id=0B2OE3bpKOXc6SnNkZHNDNHMyX2s";
NSMutableDictionary* new_flags;
NSURL *url2 = [NSURL URLWithString:@"http://128.195.151.158/simhome/db/?type=showpower&&dev=Fan"];
NSURL *url1 = [NSURL URLWithString:@"http://128.195.151.158/simhome/db/?type=showpower&&dev=Lamp"];

//NSString* json_link = @"https://drive.google.com/file/d/0B1gAXFnVaLD8eUVSZ2tTbUluSHc/view?usp=sharing";

BOOL can_speak = YES;// Bool for if model can speak
NSInteger speech_counter = 0;                                         // For creating unique FBA files
NSTimer *timer;                                                 // Timer instance
NSInteger update_inteveral = 2;                             // Intervals for when to update action flags

NSString* old_time = @"";
BOOL toUpdate = true;//a flag to check if we need to update the value for table.

//  *** VideoView members ***
BOOL processing = NO;
BOOL videoViewDisplayed = NO;
NrVideoView *videoView;

// *** TableView members ***
NrTableViewController *tableViewController;
NrSmartTableViewController *pieChartController;
NrSmartTableViewController *smartTableViewController;
BOOL tableViewDisplayed = NO;

// *** ChartView Members ***


// *** DeviceItemView members ***
//09/01/2016

NSArray *device_names = @[@"TV",@"Lamp",@"Fridge",@"Refigerator",@"Macbook",@"CoffeeMaker",@"3DPrinter",@"Fan",@"BluRay",@"AppleTV",@"Tablet",@"RiceCooker",@"Toaster",@"Appliance"];

// *** StationItemView members ***
NSMutableArray *station_list = [NSMutableArray arrayWithObjects:@"Station 1",@"Station 2",@"Station 3",nil];
NSString *current_station = @"Station 2";
NSString *station_locked = @"Station 1";

NSDictionary *stationConverter = @{@"beetroot" : @"Station 1",
                                   @"candy": @"Station 2",
                                   @"lemon": @"Station 3",
                                   };





#pragma mark -
#pragma mark Initialization

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        
        self.currentMode = NR_MAIN;
        self.currentDegree = CR_OUT;
    }
    return self;
    
}

#pragma mark -
#pragma mark View Functions

- (void)viewDidLoad
{
    [super viewDidLoad];
    action_flags = [NSMutableDictionary dictionary];
    self.lampValue = @"0.0";
    self.fanValue = @"0.0";
    [self loadScrollMainItem];
    [self loadMainPointingBar];
    
    self.skipButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.view addSubview:skipButton];
    self.skipButton.alpha = 0.0f;
    
    self.exitButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.view addSubview:exitButton];
    self.exitButton.alpha = 0.0f;
//    self.proximityContentManager = [[ProximityContentManager alloc]
//                                    initWithBeaconIDs:@[
//                                                        [[BeaconID alloc] initWithUUIDString:@"B9407F30-F5F8-466E-AFF9-25556B57FE6D" major:10575 minor:30159],
//                                                        [[BeaconID alloc] initWithUUIDString:@"B9407F30-F5F8-466E-AFF9-25556B57FE6D" major:12315 minor:42375],
//                                                        [[BeaconID alloc] initWithUUIDString:@"B9407F30-F5F8-466E-AFF9-25556B57FE6D" major:21412 minor:5512]
//                                                        ]
//                                    beaconContentFactory:[[CachingContentFactory alloc] initWithBeaconContentFactory:[BeaconDetailsCloudFactory new]]];
//    self.proximityContentManager.delegate = self;
    //[self.proximityContentManager startContentUpdates];
    
//    NrViewController *beaconValueController = [[NrViewController alloc] init];
//    beaconValueController.delegate = self;
    NSLog(@"this is ");
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    self.assistantView.alpha = 0.0f;
    
    self.daysView.alpha = 0.0f;
    self.activityIndicator.hidden = YES;
    
    if (self.currentMode == NR_MAIN) {
        self.backButton = [UIButton buttonWithType:UIButtonTypeSystem];
        [self.view addSubview:backButton];
        self.backButton.alpha = 0.0f;
        
    }
    
//    self.switchMoreInfoView.hidden = YES;
//    [self setIAPNotificationListeners];
    self.detailView.alpha = 0.0f;
    self.detailView.hidden = NO;
    
    appLocked = NO;
   
}


- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];

    
    self.currentMode = NR_MAIN;
    [self.glController changeSuperview:self.assistantView doStartAnimation:YES];
    
    speech_generator = [[NrSpeechGenerator alloc] init];
    
    tableViewController = [[NrTableViewController alloc] init];
    [tableViewController setViewController:self];
    
    pieChartController = [[NrSmartTableViewController alloc] init];
    [pieChartController setViewController:self];
    
    smartTableViewController = [[NrSmartTableViewController alloc] init];
    [smartTableViewController setViewController:self];
    
    [UIView animateWithDuration:0.3 animations:^{
        self.assistantView.alpha = 1.0f;
        self.daysView.alpha = 1.0f;
    } completion:^(BOOL finished) {
        NSLog(@"Assistant view shown");
    }];
    
    
    [self loadJSONFile];
    //where to start timer
    [self startTimer];
    
    [self speakAction:[speech_generator welcome_message]];
    //NSLog(@"here is delegate, %d",self.stationChosen);
    //delegate doesn't work.
}

/*
 Create nested NSMutableDictionary from JSON file.
 This solves the problem of updating the data inside the dictionary.
 */
-(void)mutableJSONDict:(NSMutableDictionary *)dict
{
    NSArray *keys = [dict allKeys];
    
    for (NSString* flag in keys) {
        if ([dict[flag] isKindOfClass:[NSDictionary class]]) {
//            NSLog(@"dictionary found! -> %@", dict[flag]);
            dict[flag] = [dict[flag] mutableCopy];
            [self mutableJSONDict:dict[flag]];
        }
    }
}

-(void)loadJSONFile{
    //[self downloadJSON];
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"EMMA" ofType:@"json"];
    NSError *error = nil;
    NSData *JSONData = [NSData dataWithContentsOfFile:filePath options:NSDataReadingMappedIfSafe error:&error];
    new_flags = [[self parseJSON:JSONData] mutableCopy];
    [self mutableJSONDict:new_flags];
    
    // If the file changed, replace old contents of action flags w/new content
    
    // Replace contents of current action flags w/new ones
    for (NSString* flag in new_flags) {
        [action_flags setObject:new_flags[flag] forKey:flag];
    }
    
    [tableViewController update_data:action_flags[current_station]];
    //[pieChartController update_data:action_flags[current_station]];
    if (self.currentMode == NR_OVERVIEW_TABLE)
        [tableViewController update_table];
    else if (self.currentMode == NR_SMART_TABLE)
        [smartTableViewController update_table];
    
    [self do_actions];
    //NSLog(@"String downloaded: %@", JSONData);

}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
}

#pragma mark -
#pragma mark Button Methods

- (IBAction)backButtonClicked:(id)sender
{
    if (canClickBack) {
        canClickBack = NO;
        if (self.currentMode != NR_MAIN) {
            if (self.currentMode == NR_STATION) {
                [self changeItemsToMain];
                [self shutUp];
                [self speakAction:[speech_generator back_to_menu_message]];
            }
            else {
                [self cover];
                [self hideDetailSubview];
                [self shutUp];
            }
        }
    }
}

- (IBAction)skipButtonClicked:(id)sender
{
    @synchronized(self) {
        if (!can_speak) {
            if(self.currentMode == NR_VIDEO){
                [self StopAndHideVideoViewClicked];
                
            }
            else
                [self shutUp];
            
            can_speak = YES;
        }
    }
}

- (IBAction)exitButtonClicked:(id)sender
{
    @synchronized(self) {
        exit(0);
        }
}


// no use
- (void)hideWeatherButtons
{
    [UIView animateWithDuration:0.5 animations:^{
        //        self.bottomCityNameLabel.alpha = 0.0f;
        self.backButton.alpha = 0.0f;
        //        self.switchMoreInfoView.alpha = 0.0f;
    }];
}

#pragma mark Detail View Functions

-(void) ShowDetailView
{
    [self.assistantView addSubview:self.detailView];
    
    [self MoveModelOut];
    [UIView animateWithDuration:0.5 animations:^{
        self.detailView.alpha = 1.0f;
    } completion:^(BOOL finished) {
        NSLog(@"Showing DetailView");
    }];
}

-(void) HideAndRemoveDetailView
{
    [self MoveModelIn];
    [UIView animateWithDuration:0.5 animations:^{
        [self adjustDetailViewToOverviewTable];
        self.detailView.alpha = 0.0f;
    } completion:^(BOOL finished) {
        NSLog(@"Hiding DetailView");
    }];
    
    if (self.currentMode == NR_VIDEO)
        [videoView removeFromSuperview];
    
    [self.detailWeatherView removeFromSuperview];
    [self.detailWeatherImg removeFromSuperview];
    [self.detailDayNameLabel removeFromSuperview];
    
}

-(void) ShowDetailViewWithDelay:(float)delay
{
    [self performSelector:@selector(ShowDetailView) withObject:nil afterDelay:delay];
}

-(void) HideAndRemoveDetailViewWithDelay:(float)delay
{
    [self performSelector:@selector(HideAndRemoveDetailView) withObject:nil afterDelay:delay];
}

- (void)hideDetailSubview
{
    // here is how to handle detail views
    if (self.currentMode == NR_VIDEO) {
        videoView.flag = NO;
        [self StopAndHideVideoViewClicked];
        [self changeItemsToMain];
    }
    else if (self.currentMode == NR_STATION) {
        [self speakAction:[speech_generator back_to_station_menu_message]];
        [self changeItemsToMain];
    }
    else if (self.currentMode == NR_OVERVIEW_TABLE) {
        [self backFromOverviewTable];
        [self changeItemsToStation];
    }
    else if (self.currentMode == NR_SMART_TABLE) {
        [self backFromSmartTable];
        [self changeItemsToStation];
    }
    
    [self hideWeatherButtons];
}

#pragma mark Helper View Functions

- (void)appearViewWithDuration:(NSTimeInterval)duration onView:(UIView *)view
            withCompletionMessage:(NSString *)message
{
    [UIView animateWithDuration:duration animations:^{
        view.alpha = 1.0f;
    } completion:^(BOOL finished) {
        NSLog(message);
    }];
}

- (void)appearViewWithDuration:(NSTimeInterval)duration onView:(UIView *)view
{
    [UIView animateWithDuration:duration animations:^{
        view.alpha = 1.0f;
    } completion:^(BOOL finished) {
        NSLog(@"Showing view");
    }];
}

- (void)disappearViewWithDuration:(NSTimeInterval)duration onView:(UIView *)view
            withCompletionMessage:(NSString *)message
{
    [UIView animateWithDuration:duration animations:^{
        view.alpha = 0.0f;
    } completion:^(BOOL finished) {
        NSLog(message);
    }];
}

- (void)disappearViewWithDuration:(NSTimeInterval)duration onView:(UIView *)view
{
    [UIView animateWithDuration:duration animations:^{
        view.alpha = 0.0f;
    } completion:^(BOOL finished) {
        NSLog(@"Hiding view");
    }];
}

#pragma mark -
#pragma mark Pointing Bar Functions

- (void)loadMainPointingBar
{
    NrMainItemView *item = [[[NSBundle mainBundle] loadNibNamed:@"NrMainItemView_iPhone" owner:self options:nil] objectAtIndex:0];
    [self loadPointingBarFromFrame:item.frame];
}

- (void)loadStationPointingBar
{
    NrMainItemView *item = [[[NSBundle mainBundle] loadNibNamed:@"NrStationItemView_iPad" owner:self options:nil] objectAtIndex:0];
    [self loadPointingBarFromFrame:item.frame];
}

- (void)loadDevicePointingBar
{
    NrMainItemView *item = [[[NSBundle mainBundle] loadNibNamed:@"NrDeviceItemView_iPad" owner:self options:nil] objectAtIndex:0];
    [self loadPointingBarFromFrame:item.frame];
}

- (void)movePointingBarToItem:(NrMainItemView *)item
{
    [self movePointingBarToFrame:item.frame];
}

#pragma mark -
#pragma mark Menu Item Functions

// Obtains the file name for the picture to be provided for the Device Item.
// If the device type is recognized by the application, just return the lowercase version of it.
//                                                          eg. Entertainment -> entertainment.png
// If the device is of a "Generic" type or is unrecognized, return a generic device picture.
- (NSString *)device_picture_path:(NSString *)name
{
    
    if ([device_names containsObject:name])
        return name;
    else
        return @"share-new";
}

#pragma mark Load Item Methods

- (void)loadScrollMainItem
{
    self.scroll = [[UIScrollView alloc] initWithFrame:CGRectMake(0, 0, self.daysView.frame.size.width, self.daysView.frame.size.height)];
    
    
    //09/01/2016 here to change how many things to show on mainbar.
    NSInteger numberOfItems = 2;
    
    NrMainItemView *sampleLittleView = [[NrMainItemView alloc] initWithID:0];
    self.contentsView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, sampleLittleView.frame.size.width * numberOfItems, self.daysView.frame.size.height)];
    
    self.itemList = [NSMutableArray array];
    
    for (int i = 0; i < numberOfItems; i++) {
        NrMainItemView *littleView = [[NrMainItemView alloc] initWithID:i];
        
        
        littleView.backgroundColor = [UIColor clearColor];
        
        
        littleView.frame = CGRectMake(littleView.frame.size.width * i, 0, littleView.frame.size.width, littleView.frame.size.height);
        
        [self.contentsView addSubview:littleView];
        
        [self fillDayItemWithMainData:littleView];
        
        [self.itemList addObject:littleView];
    }
    //TODO 09092016
    self.contentsView.alpha = 1.0f;
    scroll.contentSize = self.contentsView.frame.size;
    [scroll addSubview:self.contentsView];
    
    NSLog(@"Content Size: %@", NSStringFromCGSize(scroll.contentSize));
    NSLog(@"days view Size: %@", NSStringFromCGSize(scroll.frame.size));
    [self.daysView addSubview:scroll];
}

- (void)loadScrollDeviceItem
{
    self.scroll = [[UIScrollView alloc] initWithFrame:CGRectMake(0, 0, self.daysView.frame.size.width, self.daysView.frame.size.height)];
    //    scroll.pagingEnabled = YES;
    
    NSInteger numberOfItems = [action_flags[current_station][@"Number of Devices"] integerValue] + 2; // + 1 to account for overview tab and station view
    //    [self loadCalendarEvents];
    
    
    NrDeviceItemView *sampleLittleView = [[NrDeviceItemView alloc] initWithID:0];
    self.contentsView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, sampleLittleView.frame.size.width * numberOfItems, self.daysView.frame.size.height)];
    
    //    self.daysView.backgroundColor = [UIColor clearColor];
    //    self.contentsView.backgroundColor = [UIColor redColor];
    
    self.itemList = [NSMutableArray array];
    
    for (int i = 0; i < numberOfItems; i++) {
        NrDeviceItemView *littleView = [[NrDeviceItemView alloc] initWithID:i];
        
        if (i%2 == 0) {
            littleView.backgroundColor = [UIColor clearColor];
        }
        
        littleView.frame = CGRectMake(littleView.frame.size.width * i, 0, littleView.frame.size.width, littleView.frame.size.height);
        
        [self.contentsView addSubview:littleView];
        
        [self fillDayItemWithDeviceData:littleView];
        
        
        [self.itemList addObject:littleView];
    }
    
    scroll.contentSize = self.contentsView.frame.size;
    [scroll addSubview:self.contentsView];
    
    NSLog(@"Content Size: %@", NSStringFromCGSize(scroll.contentSize));
    NSLog(@"Awesome view Size: %@", NSStringFromCGSize(scroll.frame.size));
    [self.daysView addSubview:scroll];
}

- (void)loadScrollStationItem
{
    self.scroll = [[UIScrollView alloc] initWithFrame:CGRectMake(0, 0, self.daysView.frame.size.width, self.daysView.frame.size.height)];
    //    scroll.pagingEnabled = YES;
    
    NSInteger numberOfItems = [station_list count];
    
    NrStationItemView *sampleLittleView = [[NrStationItemView alloc] initWithID:0];
    self.contentsView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, sampleLittleView.frame.size.width * numberOfItems, self.daysView.frame.size.height)];
    
    //    self.daysView.backgroundColor = [UIColor clearColor];
    //    self.contentsView.backgroundColor = [UIColor redColor];
    
    self.itemList = [NSMutableArray array];
    
    for (int i = 0; i < numberOfItems; i++) {
        NrStationItemView *littleView = [[NrStationItemView alloc] initWithID:i];
        
        if (i%2 == 0) {
            littleView.backgroundColor = [UIColor clearColor];
        }
        
        littleView.frame = CGRectMake(littleView.frame.size.width * i, 0, littleView.frame.size.width, littleView.frame.size.height);
        
        [self.contentsView addSubview:littleView];
        
        [self fillDayItemWithStationData:littleView];
        
        
        [self.itemList addObject:littleView];
    }
    
    scroll.contentSize = self.contentsView.frame.size;
    [scroll addSubview:self.contentsView];
    
    NSLog(@"Content Size: %@", NSStringFromCGSize(scroll.contentSize));
    NSLog(@"Awesome view Size: %@", NSStringFromCGSize(scroll.frame.size));
    [self.daysView addSubview:scroll];
    can_speak = YES;
    canClickBack = YES;
    [self uncover];
}

# pragma mark Fill Item Methods

//09/01/2016
//add help button
//leave blank
- (void)fillDayItemWithMainData:(NrMainItemView *)item
{
    //Here we'll present the main data (i.e. Calendar, Weather, etc.)
    switch (item.itemID) {
        case 0:
            //item.dayName.text = @"Introduction/Tutorial";
            item.dayName.text = @"";
            item.dayName.font = [UIFont fontWithName:@"ArialMT" size:16];
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"intro" ofType:@"png"]];
            break;
        case 1:
            //item.dayName.text = @"Connect to Stations";
            item.dayName.text = @"";
            item.dayName.font = [UIFont fontWithName:@"ArialMT" size:16];
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"connect6" ofType:@"png"]];
            break;
            //TODO need to implement method for help
//        case 2:
//            //item.dayName.text = @"Help";
//            item.dayName.text = @"";
//            item.dayName.font = [UIFont fontWithName:@"ArialMT" size:16];
//            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"help" ofType:@"png"]];
//            break;
        default:
            item.dayName.text = @"Help";
            item.dayName.font = [UIFont fontWithName:@"ArialMT" size:16];
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"help" ofType:@"png"]];
            break;
    }
}

//09/01/2016
//this part if for devices
//TODO: universal path creation
- (void)fillDayItemWithDeviceData:(NrDeviceItemView *)item
{
    // Device
    switch (item.itemID) {
        case 0:
            //using current staions icons.
            //item.dayName.text = current_station;
            item.dayName.text = @"";
            //item.dayName.text = action_flags[current_station][@"Devices"][@"Device 1"][@"Name"];
            //NSLog(@"%@",item.dayName.text);
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:current_station ofType:@"png"]];
            break;
        case 1:
            //item.dayName.text = @"Overview";
            item.dayName.text = @"";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"Overview" ofType:@"png"]];
            break;
        case 2:
            //item.dayName.text = action_flags[current_station][@"Devices"][@"Device 1"][@"Name"];
            item.dayName.text = @"";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:[self device_picture_path:action_flags[current_station][@"Devices"][@"Device 1"][@"Name"]] ofType:@"png"]];
            break;
        case 3:
           // item.dayName.text = action_flags[current_station][@"Devices"][@"Device 2"][@"Name"];
            item.dayName.text = @"";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:[self device_picture_path:action_flags[current_station][@"Devices"][@"Device 2"][@"Name"]] ofType:@"png"]];
            break;
        case 4:
            // item.dayName.text = action_flags[current_station][@"Devices"][@"Device 2"][@"Name"];
            item.dayName.text = @"";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:[self device_picture_path:action_flags[current_station][@"Devices"][@"Device 3"][@"Name"]] ofType:@"png"]];
            break;
//        case 3:
//            item.dayName.text = action_flags[current_station][@"Devices"][@"Device 3"][@"Name"];
//            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:[self device_picture_path:action_flags[current_station][@"Devices"][@"Device 3"][@"Name"]] ofType:@"png"]];
//            break;
//        case 4:
//            item.dayName.text = action_flags[current_station][@"Devices"][@"Device 4"][@"Name"];
//            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:[self device_picture_path:action_flags[current_station][@"Devices"][@"Device 4"][@"Name"]] ofType:@"png"]];
//            break;
//        case 5:
//            item.dayName.text = action_flags[current_station][@"Devices"][@"Device 5"][@"Name"];
//            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:[self device_picture_path:action_flags[current_station][@"Devices"][@"Device 5"][@"Name"]] ofType:@"png"]];
//            break;
        default:
            //item.dayName.text = action_flags[current_station][@"Devices"][[NSString stringWithFormat:@"Device %d", item.itemID]][@"Name"];
            item.dayName.text = @"";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:[self device_picture_path:action_flags[current_station][@"Devices"][[NSString stringWithFormat:@"Device %d", item.itemID]][@"Name"]] ofType:@"png"]];
            break;
    }
}

- (void)fillDayItemWithStationData:(NrStationItemView *)item
{
    // Station
    switch (item.itemID) {
        case 0:
            //item.dayName.text = [station_list objectAtIndex:0];
            item.dayName.text = @"";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:[station_list objectAtIndex:0] ofType:@"png"]];
            break;
        case 1:
            //item.dayName.text = [station_list objectAtIndex:0];
            item.dayName.text = @"";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:[station_list objectAtIndex:1] ofType:@"png"]];
            break;
        case 2:
            //item.dayName.text = [station_list objectAtIndex:0];
            item.dayName.text = @"";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:[station_list objectAtIndex:2] ofType:@"png"]];
            break;
        default:
            item.dayName.text = [station_list objectAtIndex:item.itemID];
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"station" ofType:@"png"]];
            break;
    }
}

# pragma mark Change Item methods

- (void)changeItemsTo:(NSInteger)itemID
{
    switch (itemID) {
        case 0:
            [self changeItemsToMain];
            break;
        case 1:
            [self changeItemsToStation];
            break;
        default:
            [self changeItemstoDevice];
    }
}

- (void)changeItemsToMain
{
    self.currentMode = NR_MAIN;
    
    [UIView animateWithDuration:0.3 animations:^{
        self.scroll.alpha = 0.0f;
    } completion:^(BOOL finished) {
        NSLog(@"Animation fade out completed");
        [self loadScrollMainItem];
        self.scroll.alpha = 0.0f;
        [UIView animateWithDuration:0.3 animations:^{
            self.scroll.alpha = 1.0f;
        } completion:^(BOOL finished) {
            NSLog(@"Animation fade in completed");
            [self.pointingBar removeFromSuperview];
            [self loadMainPointingBar];
            [self.contentsView addSubview:self.pointingBar];
            self.activityIndicator.hidden = YES;
            [self uncover];
            
//            NSError *error;
//            if (![[GANTracker sharedTracker] trackPageview:@"/app/main" withError:&error]) {
//                NSLog(@"Error in tracking: %@", [error localizedDescription]);
//            }
        }];
    }];
}


- (void)changeItemstoDevice
{
    self.currentMode = NR_OVERVIEW_TABLE;
    
    [UIView animateWithDuration:0.3 animations:^{
        self.scroll.alpha = 0.0f;
    } completion:^(BOOL finished) {
        NSLog(@"Animation fade out completed");
        [self loadScrollDeviceItem];
        self.scroll.alpha = 0.0f;
        [UIView animateWithDuration:0.3 animations:^{
            self.scroll.alpha = 1.0f;
        } completion:^(BOOL finished) {
            NSLog(@"Animation fade in completed");
            [self.pointingBar removeFromSuperview];
            [self loadDevicePointingBar];
            [self.contentsView addSubview:self.pointingBar];
            self.activityIndicator.hidden = YES;
            [self uncover];
        }];
    }];
}

- (void)changeItemsToStation
{
    self.currentMode = NR_STATION;
    
    [UIView animateWithDuration:0.3 animations:^{
        self.scroll.alpha = 0.0f;
    } completion:^(BOOL finished) {
        NSLog(@"Animation fade out completed");
        [self loadScrollStationItem];
        self.scroll.alpha = 0.0f;
        [UIView animateWithDuration:0.3 animations:^{
            self.scroll.alpha = 1.0f;
        } completion:^(BOOL finished) {
            NSLog(@"Animation fade in completed");
            [self.pointingBar removeFromSuperview];
            [self loadStationPointingBar];
            [self.contentsView addSubview:self.pointingBar];
            self.activityIndicator.hidden = YES;
            [self uncover];
        }];
    }];
}

# pragma mark Item Clicked Methods


- (IBAction)mainItemViewClicked:(id)sender
{
    if (can_speak && self.currentMode != NR_VIDEO) {
        @synchronized(self) {
            [self cover];
            
            if (processing) {
                [self uncover];
                return;
            }
            processing = YES;
            
            NrMainItemView *calItem = (NrMainItemView *)sender;
            NSLog(@"Clicked at item %d", calItem.itemID);
            
            self.selectedItem = calItem.itemID;
            
            [self movePointingBarToItem:calItem];
            
            self.activityIndicator.hidden = NO;
            [self.activityIndicator startAnimating];
            
            switch (calItem.itemID) {
                case 0:
                    [self loadvideo];
                    break;
                case 1:
//                    if(self.currentMode ==  NR_VIDEO){
//                        [self StopAndHideVideoViewClicked];
//                    }
                    
                    [self speakAction:[speech_generator connect_to_station_message]];
                    [self changeItemsTo:calItem.itemID];
                    
                    //start scan bluetooth
                     //[self.proximityContentManager startContentUpdates];
                    break;
                case 2:
                    //TODO
                    //leave blank for help tap
                    
                    break;
                default:
                    [self changeItemsTo:0];
                    break;
            }
            
        }
    } else
        NSLog(@"I can't speak!");
    
}

- (IBAction)deviceItemViewClicked:(id)sender
{
    if(can_speak){
    @synchronized(self) {
        [self cover];
        
        if (processing) {
            [self uncover];
            return;
        }
        processing = YES;
        
        NrDeviceItemView *calItem = (NrDeviceItemView *)sender;
        NSLog(@"Clicked at item %d", calItem.itemID);
        
        self.selectedItem = calItem.itemID;
        
        [self movePointingBarToItem:calItem];
        
        self.activityIndicator.hidden = NO;
        [self.activityIndicator startAnimating];
        
        switch (calItem.itemID) {
            case 0:
                if (self.currentMode == NR_SMART_TABLE)
                    [self hideSmartTable];
                [self loadSmartTableForDevice:@"pieChart"];
                break;
            case 1:                                         // Overview table
                [self showOverviewTable];
                break;
            case 2:                                         // Device 1
                if (self.currentMode == NR_SMART_TABLE)
                    [self hideSmartTable];
                [self loadSmartTableForDevice:@"Device 1"];
                break;
            case 3:                                         // Device 2
                if (self.currentMode == NR_SMART_TABLE)
                    [self hideSmartTable];
                [self loadSmartTableForDevice:@"Device 2"];
                break;
            case 4:                                         // Device 3
                if (self.currentMode == NR_SMART_TABLE)
                    [self hideSmartTable];
                [self loadSmartTableForDevice:@"Device 3"];
                break;
            default:                                        // Other devices
                if (self.currentMode == NR_SMART_TABLE)
                    [self hideSmartTable];
                NSString *device = [NSString stringWithFormat:@"Device %d", calItem.itemID];
                [self loadSmartTableForDevice:device];
                break;
        }
        
    }
    }
}

- (IBAction)stationItemViewClicked:(id)sender
{
    if(can_speak){
    @synchronized(self) {
        [self cover];
        
        if (processing) {
            [self uncover];
            return;
        }
        processing = YES;
        
        NrStationItemView *calItem = (NrStationItemView *)sender;
        NSLog(@"Clicked at item %d", calItem.itemID);
        
        self.selectedItem = calItem.itemID;
        
        [self movePointingBarToItem:calItem];
        
        self.activityIndicator.hidden = NO;
        [self.activityIndicator startAnimating];
        
        switch (calItem.itemID) {
            case 0:
                // First station detected
                //Can have a concurrency problem
                current_station = [station_list objectAtIndex:calItem.itemID];
                station_locked = current_station;
                [self showOverviewTable];
                    [self changeItemsTo:2];// Change into DeviceItemView
                break;
            case 1:
                // First station detected
                //Can have a concurrency problem
                current_station = [station_list objectAtIndex:calItem.itemID];
                station_locked = current_station;
                [self showOverviewTable];
                [self changeItemsTo:2];// Change into DeviceItemView
                break;
            case 2:
                // First station detected
                //Can have a concurrency problem
                current_station = [station_list objectAtIndex:calItem.itemID];
                station_locked = current_station;
                [self showOverviewTable];
                [self changeItemsTo:2];// Change into DeviceItemView
                break;
            default:
                current_station = [station_list objectAtIndex:calItem.itemID];
                station_locked = current_station;
                [self showOverviewTable];
                [self changeItemsTo:2];
                break;
        }
        
    }
    }
}



- (void)stationItemViewWithAlert:(NSString *) station
{
    @synchronized(self) {
        [self cover];
        
        if (processing) {
            [self uncover];
            return;
        }
        processing = YES;
        self.activityIndicator.hidden = NO;
        [self.activityIndicator startAnimating];
        current_station = station;
        
        station_locked = current_station;
        
        [self showOverviewTable];
        [self changeItemsTo:2];
        
        //NrStationItemView *calItem = (NrStationItemView *)sender;
        //NSLog(@"Clicked at item %d", calItem.itemID);
        
        //self.selectedItem = calItem.itemID;
        
        //[self movePointingBarToItem:calItem];
    }
}

#pragma mark -
#pragma mark Move Model Functions


//09/01/2016
//TODO: use some degree to check the status
-(void) MoveModelOut
{
    @synchronized (self) {
        if(self.currentDegree != CR_IN){
            canClickBack = NO;
            [self cover];
            float duration = 1.2;
            float translation = 250;
            if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] == YES && [[UIScreen mainScreen] scale] == 2.00) {
                translation = 500;
            }
            
            float rotation = -30;
            
            [self.glController.modelManager animateTranslationDuring:duration incX:-translation incY:0 absolute:NO];
            [self.glController.modelManager animateRotationDuring:duration degreesX:0 degreesY:-rotation absolute:NO];
            self.currentDegree = CR_IN;}
    }
}

-(void) MoveModelIn
{
    @synchronized (self) {
        if(self.currentDegree != CR_OUT){
            canClickBack = NO;
            float duration = 1.2;
            float translation = -250;
            float rotation = +30;
            if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] == YES && [[UIScreen mainScreen] scale] == 2.00) {
                translation = -500;
            }
            
            [self.glController.modelManager animateTranslationDuring:duration incX:-translation incY:0 absolute:NO];
            [self.glController.modelManager animateRotationDuring:duration degreesX:0 degreesY:-rotation absolute:NO];
            self.currentDegree = CR_OUT;}
    }
    
}

-(void) MoveModelOutWithDelay:(float)delay
{
    [self performSelector:@selector(MoveModelOut) withObject:nil afterDelay:delay];
}

-(void) MoveModelInWithDelay:(float)delay
{
    [self performSelector:@selector(MoveModelIn) withObject:nil afterDelay:delay];
    
}

#pragma mark -
#pragma mark Timer Functions

- (void)startTimer
{
    // Starts timer w/interval of n seconds to call function "increaseTimerCount"
    if (![timer isValid]) {
        timerStarted = true;
        
        timer = [NSTimer scheduledTimerWithTimeInterval:update_inteveral target:self selector:@selector(updateStation) userInfo:nil repeats:YES];
        toUpdate = (!toUpdate);
        [timer fire];
    }
}

- (void)stopTimer
{
    // Stops timer
    if ([timer isValid]) {
        timerStarted = false;
        
        [timer invalidate];
        timer = nil;
        counter = 0;
    }
    //    self.timerCounterLabel.text = @"0";
    
}

- (void)increaseTimerCount
{
    // Function that gets called within timed-intervals
    
    //    self.TimerButton.text = [NSString stringWithFormat:@"%d", counter];
    
    if (started) {
        counter ++;
    }
    
    [self updateStation];
}

#pragma mark -
#pragma mark Downloader Functions

// Edited by Christian Morte - CalPlug - SimHome - 4/17/16

- (void)downloadJSON
{
    // Downloads test file from public server
    NSLog(@"Downloading JSON file from %@", json_link);
    //[self stationUpdated:self.currentStation];
    //[self pushAlertButton:self.currentStation];
    // Initialize downloader with URL of file
    
    NaradaDownloader *json_downloader = [[NaradaDownloader alloc] initWithURLString:json_link andDelegate:self];
    
    // Requests download, displays if connection failed or succeeded
    //[downloader requestDownload];
    
    [json_downloader requestDownload];
    
    //NSLog(@"here is %d",self.stationChosen);
    
    /*
    switch (self.stationChosen) {
        case 0:
             current_station = @"Station 1";
            break;
        case 1:
             current_station = @"Station 2";
            break;
        case 2:
             current_station = @"Station 3";
            break;
        default:
            current_station = @"Station 1";
            break;
    }
     */
    //station_list = [NSMutableArray arrayWithObjects:current_station, nil];
}

- (NSDictionary*)parseJSON: (NSData *) downloaded_data
{
    NSDictionary *dict;
    

    NSError *json_error = nil;
    id json = [NSJSONSerialization JSONObjectWithData:downloaded_data options:0 error:&json_error];
    
    if (json_error) {                   // JSON file was malformed
        NSLog(@"Unable to parse JSON file! Please check format of file!");
    }
    
    if ([json isKindOfClass:[NSDictionary class]]) {    // JSON file can be translated to dict
        dict = json;
    } else {
        NSLog(@"JSON file unable to be converted to NSMutableDictionary");
    }
    
    //NSLog(@"Dictionary created: %@", dict);
    
    return dict;
}



- (void)naradaDownlaoderDidFinishDownloading:(NaradaDownloader *)downloader
{
    NSLog(@"Did download successfully!!");
    NSLog(@"String downloaded: %@", downloader.receivedString);
    
    // create new set of action flags
    NSMutableDictionary* new_flags = [[self parseJSON:downloader.receivedData] mutableCopy];
    [self mutableJSONDict:new_flags];
    
    // If the file changed, replace old contents of action flags w/new content
    if ( [self timeHasChanged: new_flags] ) {
        NSLog(@"THE TIME CHANGED!");
        
        // Replace contents of current action flags w/new ones
        for (NSString* flag in new_flags) {
            [action_flags setObject:new_flags[flag] forKey:flag];
        }
        
        [tableViewController update_data:action_flags[current_station]];
        if (self.currentMode == NR_OVERVIEW_TABLE)
            [tableViewController update_table];
        else if (self.currentMode == NR_SMART_TABLE)
            [smartTableViewController update_table];
        
        [self do_actions];
    }
}

- (void)naradaDownloader:(NaradaDownloader *)downloader didFailWithError:(NSError *)error
{
    NSLog(@"Downloader failed with error: %@", error.description);
}

- (void) updateStation{
    [self stationUpdated:self.currentStation];
    //station_list = [NSMutableArray arrayWithObjects:current_station, nil];
    
//    NSData *data = [NSData dataWithContentsOfURL:url1];
//    NSString *ret = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
//    NSLog(@"ret=%@", ret);
    
    NSError *error;;
    NSData *data = [NSData dataWithContentsOfURL: url1];
    NSMutableArray *json = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:&error];
    lampValue = [json valueForKey:@"showpower"];
    [action_flags[current_station][@"Devices"][@"Device 1"] setObject:lampValue forKey:@"Watts"];
    
    // Here is an example of changing the data inside the dictionary!
//    [action_flags[current_station][@"Devices"][@"Device 1"] setObject:@"Hello world!" forKey:@"Name"];
    
    data = [NSData dataWithContentsOfURL: url2];
    json = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:&error];
    fanValue = [json valueForKey:@"showpower"];
    [action_flags[current_station][@"Devices"][@"Device 2"] setObject:lampValue forKey:@"Watts"];
    
    if (self.currentMode == NR_OVERVIEW_TABLE)
        [tableViewController update_table];
    else if (self.currentMode == NR_SMART_TABLE)
        [smartTableViewController update_table];
    
}

#pragma mark -
#pragma mark Action Functions

- (BOOL)timeHasChanged:(NSDictionary *) new_flags
{
    // returns true if time has changed from RIVA.txt
    NSString *time = [new_flags objectForKey:@"Time"];
    
    if ( ![time isEqual:old_time] ) {
        old_time = time;
        return true;
    }
    
    return false;
}

- (void)speakAction:(NSString *)sentence
{
    NSLog(@"Model can speak: %hhd", can_speak);
    
    if (can_speak) {
        NSString* filename = [NSString stringWithFormat:@"%@%d", @"speakTestAction", ++speech_counter];
        
        NSArray *sentences = [NSArray arrayWithObjects: sentence, nil];
        [self speakSentences:sentences withMaxLength:400 toFileName:filename inLanguage:NSLocalizedString(@"LANG_TTS", nil)];
    }
}

- (void)do_actions
{
    // add more actions from set of action flags to make model do more stuff
    if ([action_flags[@"EMMA"][@"Video"][@"Trigger"] isEqual:@1])
        [self loadvideo];
    
    else if ([action_flags[@"EMMA"][@"Speak"][@"Trigger"] isEqual:@1])
        [self speakAction:action_flags[@"EMMA"][@"Speak"][@"TTS"]];
}

#pragma mark -
#pragma mark General Speak Functions

- (void)speakToSpeakSentences
{
        NSLog(@"Files to speak: %@", self.filesToSpeak);
        
        [self.glController.modelManager playTemporaryAction:[self.filesToSpeak objectAtIndex:0] withPath:[NSFileManager documentsPath] andDelay:0];
        [self.filesToSpeak removeObjectAtIndex:0];

}

- (void)addToSpeakFile:(NSString *)filename
{
    NSString *file = [[NSFileManager documentsPath] stringByAppendingPathComponent:filename];
    NSAssert1([[NSFileManager defaultManager] fileExistsAtPath:file], @"File does not exist! File: %@", file);
    [self.filesToSpeak addObject:filename];
}

- (void)speakFirstOption
{
    // Ading new Audio locally through hard drive
    self.filesToSpeak = [NSMutableArray array];
    [self addToSpeakFile:@"welcome-assistant.wav"];
//  [self addToSpeakFile:@"file1b.wav"];
//  [self addToSpeakFile:@"file1c.wav"];
    [self speakToSpeakSentences];

}

- (void)speakSecondOption
{
    // Uses text-to-speech from ispeech.org to translate string into speech
    NSArray *sentences = [NSArray arrayWithObjects:
                          @"Here you can manage your calendar in order to schedule various energy usage for your appliances.",
                          nil];
    [self speakSentences:sentences withMaxLength:400 toFileName:@"speakTest2" inLanguage:NSLocalizedString(@"LANG_TTS", nil)];
}

#pragma mark Speak Video functions

- (void)speakVideo
{
    NSArray *sentences = [NSArray arrayWithObjects:
                          action_flags[@"EMMA"][@"Video"][@"TTS"],
                          nil];
    NSLog(@"%@",sentences);
    [self speakSentences:sentences withMaxLength:400 toFileName:@"Video" inLanguage:NSLocalizedString(@"LANG_TTS", nil)];
}

- (void)speakGBTM
{
    [self speakAction:[speech_generator back_to_menu_message]];

}

#pragma mark -
#pragma mark Video gesture

- (void)createGesture:(UIView* )sView
{
    
    singleFingerTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleDoubleTap:)];
    singleFingerTap.numberOfTapsRequired = 1;
    [sView addGestureRecognizer:singleFingerTap];
}

- (void)removeGesture:(UIView *)sView
{
    [sView removeGestureRecognizer:singleFingerTap];
    
}


- (void)handleSingleDoubleTap:(UITapGestureRecognizer *)sender
{
    if([sender state]==UIGestureRecognizerStateEnded){
        [[NSNotificationCenter defaultCenter] postNotificationName:notificationGestureDetectedDoubleTap object:nil];
    }
    
}



#pragma mark -
#pragma mark Video handler

- (void)adjustDetailViewToVideoView
{
    CGRect newBounds = CGRectMake(self.detailView.frame.origin.x, self.detailView.frame.origin.y,
                                  self.detailView.frame.size.width, 360);
    [self.detailView setFrame:newBounds];
}

- (void) loadvideo
{
    @synchronized (self) {
        
    if (!videoViewDisplayed) {
        videoViewDisplayed = YES;
        
        [videoView removeFromSuperview];
        videoView = [[NrVideoView alloc] initWithMainViewController:self];
        
        [self adjustDetailViewToVideoView];
        [self.detailView addSubview:videoView];
        
        if (self.currentMode == NR_MAIN)
            [self ShowDetailViewWithDelay:1];
        
        [self performSelector:@selector(speakVideo) withObject:nil afterDelay:0.25];
        self.currentMode = NR_VIDEO;
    }
    can_speak = YES;
    canClickBack = YES;
    }
    
}

//09/01/2016
//change the video name
- (void)playMovieClicked
{
    @synchronized(self){
        
        [self cover];
        if (processing) {
            [self uncover];
            return;
        }
    if(singleFingerTap == nil){
        
        self.currentMode = NR_VIDEO;
        processing = NO;
        //Creem un NSURL amb el path del video. En el nostre cas és el path a resources,
        //però podria ser una url http://www....
         NSURL *videoURL = [[NSBundle mainBundle] URLForResource:@"SimHome Prototype v3" withExtension:@"mp4"];

        //Creem el controlador del vídeo i li passem la URL que haurà de reproduir
        videoView.moviePlayer =  [[MPMoviePlayerController alloc]
                                  initWithContentURL:videoURL];
        
        //Aquesta crida és opcional. Serveix per definir una funció que cridarà quan el vídeo
        //hagi finalitzat. Cridarà la funció moviePlayBalkDidFinish:
        [[NSNotificationCenter defaultCenter] addObserver:videoView.self selector:@selector(moviePlayBackDidFinish:) name:MPMoviePlayerPlaybackDidFinishNotification object:videoView.moviePlayer];
        
        //ControlStyle defineix l'estil dels botons (play, pause, volum, etc.)
        //MPMovieControlStyleNone treu els controls.
        videoView.moviePlayer.controlStyle = MPMovieControlStyleNone;
        videoView.moviePlayer.movieSourceType = MPMovieSourceTypeStreaming;
        videoView.moviePlayer.shouldAutoplay = NO;
        
        //Assignem al vídeo una mida (width, height) per a què coincideixi amb el UIView
        //on volem que es reprodueixi.
        [[videoView.moviePlayer view] setFrame:[videoView.self.videoCanvas bounds]];
        
        //Inserim el video en el videoCanvas
        [videoView.self.videoCanvas addSubview:videoView.moviePlayer.view];
        
        //Posem la propietat fullscreen del video a NO. D'aquesta manera no sortim de la
        //vista actual.
        [videoView.moviePlayer setFullscreen:NO animated:NO];
        [videoView.moviePlayer play];
        can_speak = YES;
        [self createGesture:videoView.self.gestureRecognizer];
        }
    }
    can_speak = YES;
    [self uncover];
}

- (void)StopAndHideVideoViewClicked
{
    @synchronized(self){
        
        [self cover];
        if (processing) {
            [self uncover];
            return;
        }
        processing = NO;
        
        [videoView.moviePlayer stop];
        [self HideAndRemoveDetailView];
        [self speakAction:[speech_generator back_to_menu_message]];
        /*
         if(_moviePlayer.playbackState == MPMoviePlaybackStatePlaying){
         NSLog(@"Yes Playing");
         }
         else{
         [self.mainViewController speakGBTM];
         NSLog(@"Not Playing");
         }
         */
        self.currentMode = NR_MAIN;
        videoViewDisplayed = NO;
        [self uncover];
        can_speak = YES;
        singleFingerTap = nil;
    }
    
    can_speak = YES;
}
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if(can_speak && self.currentMode == NR_VIDEO){
        videoView.flag = YES;
        [self playMovieClicked];
    }
}

-(void) changeToStation{
    if(can_speak && self.currentMode == NR_VIDEO){
        videoView.flag = YES;
        [self playMovieClicked];
    }
    if (canClickBack) {
        canClickBack = NO;
        if (self.currentMode != NR_MAIN) {
            if (self.currentMode == NR_STATION) {
                [self changeItemsToMain];
                [self shutUp];
                //[self speakAction:[speech_generator back_to_menu_message]];
            }
            else {
                [self cover];
                [self hideDetailSubview];
                [self shutUp];
            }
        }
    }
    
}


#pragma mark -
#pragma mark Table View handler

- (void) speakInfo
{
    NSArray *sentences = [NSArray arrayWithObjects:
                          action_flags[@"EMMA"][@"Info"][@"TTS"],
                          nil];
    [self speakSentences:sentences withMaxLength:400 toFileName:@"speakInfo" inLanguage:NSLocalizedString(@"LANG_TTS", nil)];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return 1;
}


# pragma mark Chartview Methods

-(void) showChart{
    
}


- (void)hideChartView
{

}

/*
- (void)loadSmartTableForDevice:(NSString *)device
{
    @synchronized(self){
        [self speakAction:[speech_generator enter_smart_table_view_message_with_device:
                           action_flags[current_station][@"Devices"][device][@"Name"]]];
        [self disappearViewWithDuration:0.5 onView:tableViewController.tableView];
        [tableViewController.tableView performSelector:@selector(removeFromSuperview) withObject:nil afterDelay:0.5];
        
        [smartTableViewController update_data:action_flags[current_station]];
        [smartTableViewController setDevice:device];
        
        [smartTableViewController performSelector:@selector(update_table) withObject:nil afterDelay:0.5];
        
        smartTableViewController.view.frame = self.detailView.bounds;
        smartTableViewController.view.backgroundColor = [UIColor clearColor];
        [self performSelector:@selector(displaySmartTable) withObject:nil afterDelay:0.5];
        
        self.currentMode = NR_SMART_TABLE;
    }
}
*/
- (void)backFromChartView
{
   
}


- (void)adjustDetailViewToChartView
{
    CGRect newBounds = CGRectMake(self.detailView.frame.origin.x, self.detailView.frame.origin.y,
                                  self.detailView.frame.size.width, 225);
    [self.detailView setFrame:newBounds];
}

# pragma mark Overview Table Methods

- (void)adjustDetailViewToOverviewTable
{
    CGRect newBounds = CGRectMake(self.detailView.frame.origin.x, self.detailView.frame.origin.y,
                                  self.detailView.frame.size.width, 225);
    [self.detailView setFrame:newBounds];
}

- (void)displayOverviewTable
{
    tableViewController.tableView.alpha = 0.0f;
    
    [self adjustDetailViewToOverviewTable];
    [UIView animateWithDuration:0.5 animations:^ {
        tableViewController.tableView.frame = CGRectMake(0, 0, self.detailView.frame.size.width, self.detailView.frame.size.height);
        [self.detailView addSubview:tableViewController.tableView];
    }];
    [self appearViewWithDuration:1.0 onView:tableViewController.tableView];
}

- (void)hideOverviewTable
{
    [self disappearViewWithDuration:0.1 onView:tableViewController.tableView];
    [tableViewController.tableView performSelector:@selector(removeFromSuperview) withObject:nil afterDelay:0.1];
}

- (void)loadOverviewTable
{
    [self hideSmartTable];
    if (self.currentMode != NR_OVERVIEW_TABLE) {
        [tableViewController set_station:station_locked];
        [tableViewController update_data:action_flags[station_locked]];
        
        if (self.currentMode == NR_VIDEO && videoViewDisplayed) {
            [videoView.moviePlayer stop];
            [videoView removeFromSuperview];
            videoViewDisplayed = NO;
        } else if (self.currentMode == NR_SMART_TABLE) {
            [self hideSmartTable];
        }
        
        tableViewDisplayed = YES;
        
        tableViewController.view.frame = self.detailView.bounds;
        [self displayOverviewTable];
        
        if (self.currentMode == NR_STATION) {
            [self ShowDetailViewWithDelay:0.5];
        }
        
        self.currentMode = NR_OVERVIEW_TABLE;
    }
}

- (void)backFromOverviewTable
{
    @synchronized(self){
        [self hideOverviewTable];
        [self HideAndRemoveDetailView];
        [self speakAction:[speech_generator back_to_station_menu_message]];
        
        self.currentMode = NR_MAIN;
        tableViewDisplayed = NO;

        [self uncover];
    }
}

- (void)showOverviewTable
{
    //station_locked = current_station;
    if (self.currentMode == NR_OVERVIEW_TABLE) {
        [self speakAction:[speech_generator already_in_overview_message_with_station:station_locked]];
    }
    else if (self.currentMode == NR_SMART_TABLE) {
        [self loadOverviewTable];
        [self speakAction:[speech_generator return_to_overview_message_with_station:station_locked]];
    }
    else if (self.currentMode == NR_STATION) {
        [self loadOverviewTable];
        [self speakAction:[speech_generator enter_overview_message_with_station:station_locked]];
    }
    else {
        [self loadOverviewTable];
        [self speakAction:@"This is your overview table."];
    }
}

# pragma mark Smart Table Methods

- (void)adjustDetailViewToSmartTable
{
    CGRect newBounds = CGRectMake(self.detailView.frame.origin.x, self.detailView.frame.origin.y,
                                  self.detailView.frame.size.width, 190);
    [self.detailView setFrame:newBounds];
}

- (void)displaySmartTable
{
    smartTableViewController.tableView.alpha = 0.0f;
    [self adjustDetailViewToSmartTable];
    [UIView animateWithDuration:0.5 animations:^ {
        smartTableViewController.tableView.frame = CGRectMake(0,0, self.detailView.frame.size.width, self.detailView.frame.size.height);
        [self.detailView addSubview:smartTableViewController.tableView];
    }];
    [self appearViewWithDuration:0.5 onView:smartTableViewController.tableView];
}

- (void)hideSmartTable
{
    
    [self disappearViewWithDuration:0.5 onView:smartTableViewController.tableView];
    [smartTableViewController.tableView performSelector:@selector(removeFromSuperview) withObject:nil afterDelay:0.5];
    [self disappearViewWithDuration:0.5 onView:smartTableViewController.pieChart];
    [smartTableViewController.pieChart performSelector:@selector(removeFromSuperview) withObject:nil afterDelay:0.5];

}


//TODO
//so far it is like a shit
// here maybe need to change currentstation to station locked
- (void)loadSmartTableForDevice:(NSString *)device
{
    @synchronized(self){
        
        [self disappearViewWithDuration:0.5 onView:tableViewController.tableView];
        [tableViewController.tableView performSelector:@selector(removeFromSuperview) withObject:nil afterDelay:0.5];
        
    if(![device isEqualToString:@"pieChart"]){
        [self speakAction:[speech_generator enter_smart_table_view_message_with_device:
                           action_flags[current_station][@"Devices"][device][@"Name"]]];
        
        [smartTableViewController update_data:action_flags[current_station]];
        [smartTableViewController setDevice:device];
    
        [smartTableViewController performSelector:@selector(update_table) withObject:nil afterDelay:0.5];
        }
        else
        {
            [self speakAction:[speech_generator enter_piechart_message_with_station:
                               current_station]];
            
            [smartTableViewController update_data:action_flags[current_station]];
            [smartTableViewController setDevice:device];
            
            [smartTableViewController performSelector:@selector(update_table) withObject:nil afterDelay:0.5];
        }

        
        smartTableViewController.view.frame = self.detailView.bounds;
        smartTableViewController.view.backgroundColor = [UIColor clearColor];
        
        if(![device isEqualToString:@"pieChart"]){
            [self performSelector:@selector(displaySmartTable) withObject:nil afterDelay:0.5];
             self.currentMode = NR_SMART_TABLE;
        }
        else{
//            smartTableViewController.tableView.alpha = 0.0f;
//            [self adjustDetailViewToSmartTable];
//            [UIView animateWithDuration:0.5 animations:^ {
//                smartTableViewController.tableView.frame = CGRectMake(0,0, self.detailView.frame.size.width, self.detailView.frame.size.height);
//                [self.detailView addSubview:smartTableViewController.tableView];
//            }];
//            [self appearViewWithDuration:0.5 onView:smartTableViewController.tableView];
            
            
            smartTableViewController.tableView.alpha = 0.0f;
            [self adjustDetailViewToSmartTable];
            //TODO
            NSArray *items;
            if([station_locked  isEqual: @"Station 1"]){
            items = @[
                               [PNPieChartDataItem dataItemWithValue:35 color:PNFreshGreen description:@"Lamp"],
                               [PNPieChartDataItem dataItemWithValue:70 color:PNDeepGreen description:@"Fan"],
                               [PNPieChartDataItem dataItemWithValue:70 color:PNLightGreen description:@"Appliance"]
                               ];
            }
            else if ([station_locked  isEqual: @"Station 2"]){
                items = @[
                          [PNPieChartDataItem dataItemWithValue:20 color:PNFreshGreen description:@"AppleTV"],
                          [PNPieChartDataItem dataItemWithValue:150 color:PNDeepGreen description:@"TV"],
                          [PNPieChartDataItem dataItemWithValue:30 color:PNLightGreen description:@"BluRay"]
                          ];
            
            }
            else if ([station_locked  isEqual: @"Station 3"]){
                items = @[
                                   [PNPieChartDataItem dataItemWithValue:900 color:PNFreshGreen description:@"RiceCooker"],
                                   [PNPieChartDataItem dataItemWithValue:300 color:PNDeepGreen description:@"CoffeeMaker"],
                                   [PNPieChartDataItem dataItemWithValue:500 color:PNLightGreen description:@"Toaster"]
                                   ];
            }
            
            //smartTableViewController.pieChart = [[PNPieChart alloc] initWithFrame:CGRectMake(SCREEN_WIDTH /2.0 - 100, 135, 200.0, 200.0) items:items];
           
            [UIView animateWithDuration:0.5 animations:^ {
                NSLog(@"%f",self.detailView.frame.size.height);
                NSLog(@"%f",self.detailView.frame.size.width);
                //smartTableViewController.pieChart = [[PNPieChart alloc] initWithFrame:CGRectMake(0,0, self.detailView.frame.size.height, self.detailView.frame.size.height) items:items];
                smartTableViewController.pieChart = [[PNPieChart alloc] initWithFrame:CGRectMake(SCREEN_WIDTH /2.0 - 230, 105, self.detailView.frame.size.height + 50,self.detailView.frame.size.height + 50) items:items];
                [self.detailView addSubview:smartTableViewController.pieChart];
            }];
            smartTableViewController.pieChart.showOnlyValues = NO;
            smartTableViewController.pieChart.descriptionTextFont  = [UIFont fontWithName:@"Avenir-Medium" size:15.0];
            //smartTableViewController.pieChart.legendStyle = PNLegendItemStyleStacked;
            //smartTableViewController.pieChart.legendFont = [UIFont boldSystemFontOfSize:12.0f];
            
            [self appearViewWithDuration:0.5 onView:smartTableViewController.pieChart];
             self.currentMode = NR_SMART_TABLE;
        }
        
    }
}

- (void)backFromSmartTable
{
    @synchronized(self){
        [self hideSmartTable];
        [self HideAndRemoveDetailView];
        [self speakAction:[speech_generator back_to_station_menu_message]];
        
        self.currentMode = NR_MAIN;
        tableViewDisplayed = NO;
        
    }
}


#pragma mark -
#pragma mark Handlers for the Gesture Recognizer

- (void)handleSwipeLeftFrom:(UIGestureRecognizer *)recognizer
{
    NSLog(@"Did Swipe Left");
    [self MoveModelOut];
}

- (void)handleSwipeRightFrom:(UIGestureRecognizer *)recognizer
{
    [self MoveModelIn];
    NSLog(@"Did Swipe Right");
    
}

-(void) handleTap:(UIGestureRecognizer *) recognizer
{

    NSLog(@"did tape here Gilbert");
}

#pragma mark -
#pragma mark Finish Speaking Handler

- (void)handleWillBeginSpeaking
{
    processing = NO;
    self.activityIndicator.hidden = YES;
    NSLog(@"I am about to speak");
    can_speak = NO;
    canClickBack = NO;
    self.backButton.userInteractionEnabled = NO;
    speech_counter = arc4random() % 999;
}



- (void)handleDidFinishSpeaking
{
    processing = NO;
    self.activityIndicator.hidden = YES;
    [self uncover];
    NSLog(@"I just spoke");
    can_speak = YES;
    canClickBack = YES;
    self.backButton.userInteractionEnabled = YES;
}


#pragma mark -
#pragma mark Beacon Callback

//- (void)proximityContentManager:(ProximityContentManager *)proximityContentManager didUpdateContent:(id)content {
//    
//    
//    BeaconDetails *beaconDetails = content;
////    count++;
////    NSLog(@"%d",count);
//    if (beaconDetails) {
//        //     self.view.backgroundColor = beaconDetails.backgroundColor;
//        NSLog(@"%@",beaconDetails.beaconName);
//        
//    } else {
//        self.view.backgroundColor = BeaconDetails.neutralColor;
//        NSLog(@"No beacons");
//        
//    }
//}
#pragma mark -
#pragma mark Beacon Delegate
//- (void) passValue:(NSString *)value{
//
//    NSLog(@"here is the delegate value,%@",value);
//    
//}

#pragma mark -
#pragma mark data updated
-(NSData *) dataUpdated{
    NSURL *url = [NSURL URLWithString:@"http://128.195.151.158/simhome/db/?type=showtable&table=Lamp_Hourly_04192015"];
    NSData *data = [NSData dataWithContentsOfURL:url];
    NSLog(@"ret=%@", data);
    return data;
}


#pragma alert
#pragma mark -


-(void) stationUpdated:(NSString *) station {
        NSLog(@"data updated! the station is %@",station);
        if ([_currentStation isEqualToString:station] ) {
            count++;
            if(count < 4)
            {
                NSLog(@"count is %d, less than 4s",count);
            }
            else{
                NSLog(@"enough");
                count = 0;
                if (![_preStation isEqualToString:station] ) {
    //                [self pushAlertButton:station];
//                    if([station isEqualToString:@"beetroot"]){
//                         calVC.stationChosen = 0;
//                    }
//                    if([station isEqualToString:@"candy"]){
//                        calVC.stationChosen = 1;
//                    }
//                    if([station isEqualToString:@"lemon"]){
//                        calVC.stationChosen = 2;
//                    }
                    self.currentStation = station;
                    [self pushAlertButton:station];
                    _preStation = station;
                }
            }
        } else {
            count = 1;
            self.currentStation = station;
            NSLog(@"New Stations");
        }

    
}
- (void)makeAlertController:(UIAlertController *)ac{
    
    [ac addAction:[UIAlertAction
                   actionWithTitle:@"Yes"
                   style:UIAlertActionStyleDefault
                   handler:^(UIAlertAction *action) {
                       [self yesButtonPushed:self.currentStation];
                   }]];
    
    [ac addAction:[UIAlertAction
                   actionWithTitle:@"Cancel"
                   style:UIAlertActionStyleCancel
                   handler:^(UIAlertAction *action) {
                       [self cancelButtonPushed];
                   }]];
    [self presentViewController:ac animated:YES completion:nil];
}

- (void)cancelButtonPushed {
    NSLog(@"Cancel");
}
- (void)yesButtonPushed:(NSString *) station{
    NSLog(@"YEs!");
    @synchronized (self) {
    self.currentMode = NR_STATION;
    [self stationItemViewWithAlert:stationConverter[station]];
    }
}

- (void)pushAlertButton:(NSString *) station{
    
    if(self.currentMode != NR_VIDEO){
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:[NSString stringWithFormat:@"%@ is detected!",stationConverter[station]] message:nil preferredStyle:UIAlertControllerStyleAlert];
        //[self shutUp];
        [self speakAction:[speech_generator find_station]];
        
        [self makeAlertController:alertController];
        
    }
}


@end