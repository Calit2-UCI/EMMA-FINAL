//
//  NrTableViewController.h
//  UCIAssistant
//
//  Created by Christian Morte on 5/3/16.
//  Copyright (c) 2016 Narada Robotics S.L. All rights reserved.
//

#import "NrTableViewController.h"
#import "NrGridTableViewCell.h"
#import "NrCalendarMainViewController.h"


#import "NrSpeechGenerator.h"

#define overviewTableCell1Width 205  // Smart table cells
#define overviewTableCell2Width 190
#define overviewTableCellHeight 60

NrSpeechGenerator *table_view_speech = [[NrSpeechGenerator alloc] init];

NrCalendarMainViewController *mainViewController;

@interface DeviceSwitch : UISwitch {
    NSString *device;
}
@property (nonatomic, readwrite, retain) NSString* device;
@end

@implementation DeviceSwitch
@synthesize device;
@end

@implementation NrTableViewController

UIColor *gray_row_color = [UIColor colorWithRed:0.5 green:0.5 blue:0.5 alpha:0.7];
UIColor *dark_gray_row_color = [UIColor colorWithRed:0.71 green:0.71 blue:0.71 alpha:0.7];

UIColor *on_color = [UIColor colorWithRed:0.3 green:1 blue:0 alpha:1];
UIColor *off_color = [UIColor colorWithRed:1 green:0 blue:0 alpha:1];
UIColor *unknown_color = [UIColor colorWithRed:0.63 green:0.63 blue:0.63 alpha:0.5];

NSString *station_title;
UISwitch *overview_switch;

#pragma mark -
#pragma mark Initialization

- (id)initWithViewController:(UIViewController *)viewController withData:(NSMutableDictionary *)station
{
    self = [super init];
    if (self) {
        [self setViewController:viewController];
        [self update_data:station];
    }
    return self;
}

- (id)initWithData:(NSMutableDictionary *)station
{
    self = [super init];
    if (self) {
        [self update_data:station];
    }
    return self;
}

#pragma mark -
#pragma mark View Handlers

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    // Turn off the tableView's default lines because we are drawing them all ourself
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    
//    self.mainSlider=[[UIScrollView alloc]init];
//    self.mainSlider.frame = CGRectMake(0, 0, 300, 200);  //set slider to fill the entire cell. You can also position it to take up only part of the cell (for example, the bottom half.
//    [self.mainSlider setContentSize:CGSizeMake(700, 60)]; // set its content size to be large enough to fit the label. You may need to calculate this on the fly.
//    
////    [self.view addSubview:self.mainSlider];
//    [self.mainSlider addSubview:self.tableView];
//    [self.view addSubview:self.view];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
}

- (void)set_station:(NSString *)station
{
    station_title = station;
}

- (void)setViewController:(UIViewController *)viewController
{
    mainViewController = viewController;
}

#pragma mark -
#pragma mark Dataset Methods

- (void) update_data:(NSMutableDictionary *)station
{
    self.station_data = [NSMutableDictionary dictionaryWithDictionary:station];
    [self.station_data setValue:station[@"Number of Devices"] forKey:@"Number of Devices"];
    //[self.station_data setValue:station[@"Total Energy Consumption"] forKey:@"Total Energy Consumption"];
    [self.station_data setValue:station[@"Total Usage"] forKey:@"Total Usage"];
    [self.station_data setValue:station[@"Devices"] forKey:@"Devices"];
        
    
    NSLog(@"Station data updated: %@", self.station_data);
}

- (NSDictionary *) get_data
{
    return self.station_data;
}

- (NSInteger) number_of_devices
{
    if (self.station_data != nil && self.station_data[@"Number of Devices"] != nil)
        return [self.station_data[@"Number of Devices"] integerValue];
    return 0;
}

#pragma mark -
#pragma mark TableView Methods

- (void) init_row:(NrGridTableViewCell *)cell atIndexPath:(NSIndexPath *)indexPath
{
    if (self.station_data != nil && self.station_data[@"Number of Devices"] != nil) {
        NSInteger num_of_devices = [self number_of_devices];
        
        if (indexPath.row == 0) {           // top row
            cell.cell1.text = station_title;
            cell.cell1.font = [UIFont fontWithName:@"Arial-BoldMT" size:32];
            [cell.cell1 setAdjustsFontSizeToFitWidth:YES];
            cell.cell1.textColor = [UIColor whiteColor];
            cell.cell1.backgroundColor = [UIColor blackColor];
            cell.cell1.frame = CGRectMake(0, 0, overviewTableCell1Width+overviewTableCell2Width, cell.cell1.frame.size.height);
        }
        else if (indexPath.row == 1) {
            cell.cell3.backgroundColor = cell.cell2.backgroundColor = cell.cell1.backgroundColor = gray_row_color;
            
            cell.cell1.text = @"Device";
            [cell.cell1 setFont:[UIFont fontWithName:@"Arial Rounded MT Bold" size:18]];
            [cell.cell1 setAdjustsFontSizeToFitWidth:YES];
            
            cell.cell2.text = @"Consumption";
            [cell.cell2 setFont:[UIFont fontWithName:@"Arial Rounded MT Bold" size:18]];
            [cell.cell2 setAdjustsFontSizeToFitWidth:YES];
            
            cell.cell3.text = @"Status";
            [cell.cell3 setFont:[UIFont fontWithName:@"Arial Rounded MT Bold" size:18]];
            [cell.cell3 setAdjustsFontSizeToFitWidth:YES];
        }
        else if (1 < indexPath.row && indexPath.row <= num_of_devices+2) {     // middle rows
            NSInteger device_number = indexPath.row-1;
            NSString *device_key = [NSString stringWithFormat:@"Device %d", device_number];
            
            if (indexPath.row % 2 == 0) {
                cell.cell3.backgroundColor = cell.cell2.backgroundColor = cell.cell1.backgroundColor = dark_gray_row_color;
            } else {
                cell.cell3.backgroundColor = cell.cell2.backgroundColor = cell.cell1.backgroundColor = gray_row_color;
            }
            
            DeviceSwitch* deviceSwitch = [[DeviceSwitch alloc] initWithFrame:CGRectZero];
            cell.accessoryView = deviceSwitch;
            
            NSString* status = self.station_data[@"Devices"][device_key][@"Status"];
            [deviceSwitch setDevice:self.station_data[@"Devices"][device_key][@"Name"]];
            [deviceSwitch setOn:[self switchIsOn:status] animated:YES];
            [deviceSwitch addTarget:self action:@selector(switch_changed:) forControlEvents:UIControlEventValueChanged];
        
            cell.cell1.text = self.station_data[@"Devices"][device_key][@"Name"];
            [cell.cell1 setAdjustsFontSizeToFitWidth:YES];
            
            //change here to change the power consumption
            cell.cell2.text = [NSString stringWithFormat:@"%@ W", self.station_data[@"Devices"][device_key][@"Watts"]];
            [cell.cell2 setAdjustsFontSizeToFitWidth:YES];
            
//            [self set_device_color:device_key for_cell:cell];
        }
    }
}

-(void) init_table
{
    NSInteger num_of_devices = [self number_of_devices];
    
    for (NSInteger i = 0; i < num_of_devices+2; ++i) {
        NSIndexPath *indexPath = [NSIndexPath indexPathForRow:i inSection:0];
        NrGridTableViewCell *cell = [self.tableView cellForRowAtIndexPath:indexPath];
        
        [self update_cell_data:cell atIndexPath:indexPath];
    }
}

- (void) update_table
{
    NSInteger num_of_devices = [self number_of_devices];
    
    for (NSInteger i = 0; i < num_of_devices+2; ++i) {
        NSIndexPath *indexPath = [NSIndexPath indexPathForRow:i inSection:0];
        NrGridTableViewCell *cell = [self.tableView cellForRowAtIndexPath:indexPath];
        
        [self update_cell_data:cell atIndexPath:indexPath];
    }
}

- (void) update_cell_data:(NrGridTableViewCell *)cell atIndexPath:(NSIndexPath *)indexPath
{
    if (self.station_data != nil && self.station_data[@"Number of Devices"] != nil) {
        NSInteger num_of_devices = [self number_of_devices];
        
        if (indexPath.row == 0) {           // top row
            cell.cell1.text = station_title;
            cell.cell1.font = [UIFont fontWithName:@"Arial-BoldMT" size:32];
            cell.cell1.textColor = [UIColor whiteColor];
            cell.cell1.backgroundColor = [UIColor blackColor];
            cell.cell1.frame = CGRectMake(0, 0, overviewTableCell1Width+overviewTableCell2Width, cell.cell1.frame.size.height);
        }
        if (1 < indexPath.row && indexPath.row <= num_of_devices+2) {     // middle rows
            NSInteger device_number = indexPath.row-1;
            NSString *device_key = [NSString stringWithFormat:@"Device %d", device_number];
            
            NSString* status = self.station_data[@"Devices"][device_key][@"Status"];
            
            DeviceSwitch *deviceSwitch = cell.accessoryView;
            [deviceSwitch setDevice:self.station_data[@"Devices"][device_key][@"Name"]];
            [deviceSwitch setOn:[self switchIsOn:status] animated:YES];
            
            [self change_cell_content:cell for_device:device_key];
//            [self set_device_color:device_key for_cell:cell];
        }
    }
}

- (void) change_cell_content:(NrGridTableViewCell *)cell for_device:(NSString *)device_key
{
    NSString *new_cell_1_text = self.station_data[@"Devices"][device_key][@"Name"];
    
    NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
    formatter.roundingIncrement = [NSNumber numberWithDouble:0.1];
    formatter.numberStyle = NSNumberFormatterDecimalStyle;
    
    NSString *new_cell_2_text = [NSString stringWithFormat:@"%@ W", [formatter stringFromNumber:[NSNumber numberWithFloat:[self.station_data[@"Devices"][device_key][@"Watts"] floatValue]]] ];
    
    
    if (cell.cell1.text != new_cell_1_text)
        cell.cell1.text = new_cell_1_text;
    
    if (cell.cell2.text != new_cell_2_text)
        cell.cell2.text = new_cell_2_text;
}

- (void)set_device_color:(NSString *)device_key for_cell:(NrGridTableViewCell *)cell
{
    if (self.station_data != nil && self.station_data[@"Devices"] != nil) {
        if ([self.station_data[@"Devices"][device_key][@"Status"]  isEqual: @"Off"]) {
            cell.cell1.textColor = off_color;
        }
        else if ([self.station_data[@"Devices"][device_key][@"Status"]  isEqual: @"On"]){
            cell.cell1.textColor = on_color;
        }
        else {
            cell.cell1.textColor = unknown_color;
            cell.cell2.textColor = unknown_color;
        }
    }
}

#pragma mark -
#pragma mark TableView Methods

/*
 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	// Return YES for supported orientations.
	return (interfaceOrientation == UIInterfaceOrientationPortrait);
 }
 */

// Customize the number of sections in the table view.
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if (self.station_data != nil && self.station_data[@"Number of Devices"] != nil)
        return [self.station_data[@"Number of Devices"] integerValue] + 2;
        // + 1 to account for title/top row
        // + 1 to account for header row

    return 0;
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"TableCell";
    
    NrGridTableViewCell *cell = (NrGridTableViewCell*)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[NrGridTableViewCell alloc] initWithMode:TABLE_OVERVIEW withStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
        cell.lineColor = [UIColor blackColor];
    }
    
    // Since we are drawing the lines ourself, we need to know which cell is the top cell in the table so that
    // we can draw the line on the top
    if (indexPath.row == 0)
        cell.topCell = YES;
    else
        cell.topCell = NO;
    
    // Configure the cell.
    [self init_row:cell atIndexPath:indexPath];
    
    return cell;
}


- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    /*
     <#DetailViewController#> *detailViewController = [[<#DetailViewController#> alloc] initWithNibName:@"<#Nib name#>" bundle:nil];
     // ...
     // Pass the selected object to the new view controller.
     [self.navigationController pushViewController:detailViewController animated:YES];
     [detailViewController release];
     */
    [mainViewController speakAction:[self sentenceForIndexPath:indexPath]];
}

# pragma mark -
# pragma mark Sentence Methods

-(NSString *)sentenceForIndexPath:(NSIndexPath *)indexPath
{
    NSInteger num_of_devices = [self number_of_devices];

    if (indexPath.row == 0)
        return [NSString stringWithFormat:@"Your overview table features devices and their kilowatts per hour. You can click on each device to have me explain their usage."];
    else if (1 < indexPath.row && indexPath.row <= num_of_devices+1) {
        NrGridTableViewCell *cell = [self tableView:self.tableView cellForRowAtIndexPath:indexPath];
        NSLog(@"Background color: %@", cell.cell1.backgroundColor);
        return [table_view_speech overview_device_info_message_with_device:cell.cell1.text
                                                          andCurrentStatus: [cell.cell1.textColor isEqual:on_color] ? @"On" : @"Off"
                                                             andPowerUsage:cell.cell2.text];
    }
    else {
        NrGridTableViewCell *cell = [self tableView:self.tableView cellForRowAtIndexPath:indexPath];
        return [NSString stringWithFormat:@"The total usage for this station is %@.", cell.cell2.text];
    }
    
    return @"This is an invalid row.";
}

#pragma mark -
#pragma mark Switch handlers

- (BOOL)switchIsOn:(NSString *)status
{
    if ([[status lowercaseString] isEqualToString:@"on"])
        return YES;
    else
        return NO;
}

- (void) switch_changed:(id)sender
{
    DeviceSwitch* switch_sender = sender;
    NSString *device = [switch_sender device];
    if (switch_sender.on) {
        // turn on all devices in station here
        NSURL *url = nil;
        if ([device isEqualToString:@"Coffee Maker"] || [device isEqualToString:@"PixarLamp"]){
            url = [NSURL URLWithString:[NSString stringWithFormat:@"http://128.195.151.158/simhome/control/switch/on/?device=%@&level=100", [self controlDeviceName:device]]];
            NSLog(@"%@",url);
        } else {
            url = [NSURL URLWithString:[NSString stringWithFormat:@"http://128.195.151.158/simhome/control/on/?device=%@&level=100", [self controlDeviceName:device]]];
            NSLog(@"%@",url);
            
        }
        
        
        NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url];
        [request setTimeoutInterval:5.0];
        
        [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue currentQueue] completionHandler:^(NSURLResponse *response, NSData *data, NSError *error) {
            
            if (data != nil && error == nil)
            {
                [mainViewController speakAction:[table_view_speech turn_on_device_message:device]];
            }
            else
            {
                [mainViewController speakAction:@"I was unable to turn on the device."];
            }
        }];
    } else {
        // turn off all devices in station here
        NSURL *url = nil;
        if ([device isEqualToString:@"Coffee Maker"] || [device isEqualToString:@"PixarLamp"]){
            url = [NSURL URLWithString:[NSString stringWithFormat:@"http://128.195.151.158/simhome/control/switch/off/?device=%@", [self controlDeviceName:device]]];
            NSLog(@"%@",url);
        } else {
            url = [NSURL URLWithString:[NSString stringWithFormat:@"http://128.195.151.158/simhome/control/off/?device=%@", [self controlDeviceName:device]]];
            NSLog(@"%@",url);

        }
                NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url];
        [request setTimeoutInterval:5.0];
        
        [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue currentQueue] completionHandler:^(NSURLResponse *response, NSData *data, NSError *error) {
            
            if (data != nil && error == nil)
            {
                [mainViewController speakAction:[table_view_speech turn_off_device_message:device]];
            }
            else
            {
                [mainViewController speakAction:@"I was unable to turn off the device."];
            }
        }];

        
    }

}

#pragma mark -
#pragma mark Memory Handlers

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Relinquish ownership any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    
    // Relinquish ownership of anything that can be recreated in viewDidLoad or on demand.
    // For example: self.myOutlet = nil;
}


#pragma mark pie chart
- (void)drawPieChart{

    NSArray *items = @[
                       [PNPieChartDataItem dataItemWithValue:35 color:PNFreshGreen description:@"LAMP"],
                       [PNPieChartDataItem dataItemWithValue:70 color:PNDeepGreen description:@"TV"],
                       ];
    self.pieChart = [[PNPieChart alloc] initWithFrame:CGRectMake(SCREEN_WIDTH /2.0 - 100, 135, 200.0, 200.0) items:items];
    self.pieChart.showOnlyValues = NO;
    self.pieChart.legendStyle = PNLegendItemStyleStacked;
    self.pieChart.legendFont = [UIFont boldSystemFontOfSize:12.0f];
    
//    UIView *legend = [self.pieChart getLegendWithMaxWidth:200];
//    [legend setFrame:CGRectMake(130, 350, legend.frame.size.width, legend.frame.size.height)];
//    [self.view addSubview:legend];
    
    //[self.view addSubview:self.pieChart];

}

-(void)removePieChart{
    [self.pieChart removeFromSuperview];
}

-(NSString *)controlDeviceName:(NSString *)device
{
    NSDictionary *controlDevices = @{@"Lamp" : @"Bulb",
                                     @"Coffee Maker" : @"Keurig",
                                     @"PixarLamp" : @"PixarLamp"
                                     };
    
    if (![[controlDevices objectForKey:device] isKindOfClass:[NSNull class]])
        return controlDevices[device];
    return device;
}


@end
