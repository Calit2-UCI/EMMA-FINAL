//
//  NrTableViewController.h
//  UCIAssistant
//
//  Created by Christian Morte on 5/19/16.
//  Copyright (c) 2016 Narada Robotics S.L. All rights reserved.
//

#import "NrSmartTableViewController.h"
#import "NrTableViewController.h"
#import "NrGridTableViewCell.h"
#import "NrCalendarMainViewController.h"
#import "NrSpeechGenerator.h"
#import "PNChart.h"

#define tableWidth 400
#define smartTableWidth 310
#define numberOfSmartTableColumns 2 // change if adding/deleting columns for smart table

#define smartTableCell1Width tableWidth / numberOfSmartTableColumns  // Smart table cells
#define smartTableCell2Width tableWidth / numberOfSmartTableColumns
#define smartTableMiddleRowCellWidth smartTableWidth / numberOfSmartTableColumns // middle row for smart table
#define smartTableCell3Width 0
#define smartTableCellHeight 44

NrCalendarMainViewController *calendarMainViewController;
NrSpeechGenerator *device_speech = [[NrSpeechGenerator alloc] init];

@implementation NrSmartTableViewController

UIColor *gray_color = [UIColor colorWithRed:0.5 green:0.5 blue:0.5 alpha:0.5];
UIColor *black_color = [UIColor colorWithRed:1 green:1 blue:1 alpha:0.5];

UISwitch *device_switch;
static dispatch_once_t onceToken;

#pragma mark -
#pragma mark Initialization

- (id)initWithDevice:(NSString *)device withViewController:(UIViewController *)viewController withData:(NSMutableDictionary *)station
{
    self = [super initWithData:station];
    if (self) {
        [self setViewController:viewController];
        [self setDevice:device];
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

#pragma mark -
#pragma mark Attribute-setter methods

- (void)setViewController:(UIViewController *)viewController
{
    calendarMainViewController = viewController;
}

- (void)setTableDataWithDevice:(NSString *)device
{
    self.device = device;
}

#pragma mark Attribute helpers
- (NSString *)device_name
{
    return [NSString stringWithFormat:@"%@", self.station_data[@"Devices"][self.device][@"Name"]];
}

#pragma mark -
#pragma mark TableView Methods

- (void) init_row:(NrGridTableViewCell *)cell atIndexPath:(NSIndexPath *)indexPath
{
    if (self.station_data != nil && self.station_data[@"Number of Devices"] != nil) {
        
        if (indexPath.row == 0) {
            cell.cell1.text = [NSString stringWithFormat:@"%@", self.station_data[@"Devices"][self.device][@"Name"]];
            if([cell.cell1.text isEqualToString:@"Lamp"]){
                dispatch_once(&onceToken, ^{
                    [self init_switch_for_cell:cell];
                });
            }
            else{
                [self init_switch_for_cell:cell];
            }
            
        } else if (indexPath.row == 1) {
            // Header column for 'brand'
            cell.cell1.text = @"Brand";
            
            // Vallue for 'brand' key
            cell.cell2.text = [NSString stringWithFormat:@"%@", self.station_data[@"Devices"][self.device][@"Brand"]];
            
        } else if (indexPath.row == 2) {
            // Header column for 'model'
            cell.cell1.text = @"Model";
            
            // Vallue for 'brand' key
            cell.cell2.text = [NSString stringWithFormat:@"%@", self.station_data[@"Devices"][self.device][@"Model"]];
            
        } else if (indexPath.row == 3) {
            // Header column for 'max consumption'
            cell.cell1.text = @"  Max consumption"; // added the space in the beginning as padding inside table
            
            // Value for 'max consumption' key
            cell.cell2.text = @"Test";
            
        } else if (indexPath.row == 4) {
            // Header column for 'max consumption'
            cell.cell1.text = [NSString stringWithFormat:@"Total usage last week: %@", @"0 W"];
        }
    }
}


- (void) update_row:(NrGridTableViewCell *)cell atIndexPath:(NSIndexPath *)indexPath
{
    if (self.station_data != nil && self.station_data[@"Number of Devices"] != nil) {
        
        if (indexPath.row == 0) {
            cell.cell1.text = [NSString stringWithFormat:@"%@", self.station_data[@"Devices"][self.device][@"Name"]];
            if([cell.cell1.text isEqualToString:@"Lamp"]){
                dispatch_once(&onceToken, ^{
                    [self init_switch_for_cell:cell];
                });
            }
            else{
                [self init_switch_for_cell:cell];
            }
            
        } else if (indexPath.row == 1) {
            // Value for 'brand' key
            cell.cell2.text = [NSString stringWithFormat:@"%@", self.station_data[@"Devices"][self.device][@"Brand"]];
            
        } else if (indexPath.row == 2) {
            // Vallue for 'brand' key
            cell.cell2.text = [NSString stringWithFormat:@"%@", self.station_data[@"Devices"][self.device][@"Model"]];
            
        } else if (indexPath.row == 3) {
            // Value for 'max consumption' key
            cell.cell2.text = @"100 W";
            
        } else if (indexPath.row == 4) {
            // Header column for 'max consumption'
            cell.cell1.text = [NSString stringWithFormat:@"Total usage last week: %@", @"0 W"];
        }
    }
}



- (void) update_table
{
    NSInteger num_of_devices = [self.station_data[@"Number of Devices"] integerValue];
    
    for (NSInteger i = 0; i < num_of_devices+1; ++i) {
        NSIndexPath *indexPath = [NSIndexPath indexPathForRow:i inSection:0];
        NrGridTableViewCell *cell = [self.tableView cellForRowAtIndexPath:indexPath];
        
        [self update_cell_data:cell atIndexPath:indexPath];
//        [cell.cell1 setTextColor:[UIColor whiteColor]];
    }
}

- (void) update_cell_data:(NrGridTableViewCell *)cell atIndexPath:(NSIndexPath *)indexPath
{
    // update the data inside the table here
    [self update_row:cell atIndexPath:indexPath];
}

- (void) change_cell_content:(NrGridTableViewCell *)cell for_device:(NSString *)device_key
{
    // find something to change cell content here
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
    return 5;
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"TableCell";
    
    NrGridTableViewCell *cell = (NrGridTableViewCell*)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[NrGridTableViewCell alloc] initWithMode:TABLE_DETAILED withStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
        cell.lineColor = [UIColor blackColor];
    }
    
    // Since we are drawing the lines ourself, we need to know which cell is the top cell in the table so that
    // we can draw the line on the top
    if (indexPath.row == 0) {
        cell.topCell = YES;
        
        cell.cell1.font = [UIFont fontWithName:@"Arial-BoldMT" size:32];
        [cell.cell1 setAdjustsFontSizeToFitWidth:YES];
        cell.cell1.textColor = [UIColor whiteColor];
        cell.cell1.backgroundColor = [UIColor blackColor];
        
        cell.cell1.frame = CGRectMake(0, 0, smartTableCell1Width+smartTableCell2Width, cell.cell1.frame.size.height);
        cell.cell2.backgroundColor = [UIColor clearColor];
        cell.cell2.frame = CGRectMake(0, 0, 0,0);
        
    } else if (indexPath.row == 4) {
        cell.topCell = NO;
        
        cell.cell1.font = [UIFont fontWithName:@"Arial-BoldMT" size:24];
        [cell.cell1 setAdjustsFontSizeToFitWidth:YES];
        cell.cell1.textColor = [UIColor blackColor];
        cell.cell1.backgroundColor = gray_color;
        cell.cell1.frame = CGRectMake(0, 0, smartTableCell1Width+smartTableCell2Width, cell.cell1.frame.size.height);
        
        cell.cell2.textColor = [UIColor blackColor];
        cell.cell2.backgroundColor = [UIColor clearColor];
        cell.cell2.frame = CGRectMake(0, 0, 0,0);
    }
    else {
        cell.topCell = NO;
        
        cell.cell1.backgroundColor = gray_color;
        cell.cell1.font = [UIFont fontWithName:@"Arial Rounded MT Bold" size:18];
        [cell.cell1 setAdjustsFontSizeToFitWidth:YES];
        [cell.cell2 setTextAlignment:NSTextAlignmentRight];
        
        cell.cell2.font = [UIFont fontWithName:@"Arial Rounded MT" size:24];
        [cell.cell2 setAdjustsFontSizeToFitWidth:YES];
        [cell.cell2 setTextAlignment:NSTextAlignmentCenter];
        
        cell.cell2.backgroundColor = gray_color;
        cell.cell1.frame = CGRectMake(0, 0, smartTableMiddleRowCellWidth+20, smartTableCellHeight);
        cell.cell2.frame = CGRectMake(smartTableMiddleRowCellWidth+20, 0, 2*smartTableMiddleRowCellWidth-20, smartTableCellHeight);
    }
    
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
    for (NSInteger i = 0; i < [self tableView:tableView numberOfRowsInSection:0]+1; ++i) {
        NSIndexPath *current_index = [NSIndexPath indexPathForRow:i inSection:0];
        NrGridTableViewCell *cell = [self.tableView cellForRowAtIndexPath:current_index];
        
        if (cell.cell1.textColor == [UIColor blackColor]) {
            [cell.cell1 setTextColor:[UIColor whiteColor]];
            [cell.cell2 setTextColor:[UIColor whiteColor]];
        }
    }
    
    NrGridTableViewCell *cell = [self.tableView cellForRowAtIndexPath:indexPath];
    [cell.cell1 setTextColor:[UIColor blackColor]];
    [cell.cell2 setTextColor:[UIColor blackColor]];
    [calendarMainViewController speakAction:[self sentenceForIndexPath:indexPath]];
}

# pragma mark -
# pragma mark Sentence Methods

-(NSString *)sentenceForIndexPath:(NSIndexPath *)indexPath
{
    // THESE ARE ALL SCRIPTED FOR NOW. EDIT WHEN WE CAN WORK WITH REAL DATA AND CAN FINALIZE SMART TABLE.
    if (indexPath.row == 3)
        return @"Here you can switch into your Energy Saver Mode. This mode will be developed very soon.";
    
    NrGridTableViewCell *cell = [self.tableView cellForRowAtIndexPath:indexPath];
    
    if ([self.device  isEqual:@"Device 1"]) {
        if (indexPath.row == 0) {           // top row
            return [NSString stringWithFormat:@"Your %@ is currently powered off. You can click on each option in your smart table to have me explain more information for the device.", [self device_name]];
        }
        else if (indexPath.row == 1) {     // middle rows
            return [NSString stringWithFormat:@"You are spending about $30 per month for your %@. This is incredible, you are saving more than 90 percent than the average population of lamp owners!", [self device_name]];
        }
        else if (indexPath.row == 2) {
            return @"Incredible! You are saving more money and energy than 90% of all lamp owners!";
        }
    }
    else if ([self.device isEqual:@"Device 2"]) {
        if (indexPath.row == 0) {           // top row
            return [NSString stringWithFormat:@"The status of the %@ is currently on. Go ahead and click the other options of the smart table to see more info for the device.", [self device_name]];
        }
        else if (indexPath.row == 1) {     // middle rows
            return [NSString stringWithFormat:@"I see that your %@ is currently on at an unusual time of the day. Since no one is really using the %@ right now, why don't we turn off the device during these times of the day? I can schedule the %@ to be turned off every 12AM to 5AM to save yourself some power.", [self device_name], [self device_name], [self device_name]];
        }
        else {                              // last row
            return [NSString stringWithFormat:@"By scheduling the times that your %@ is powered on and off, we can deduce your spendings by as much as 15 dollars per month! This can allow you to get a new %@ if you'd like!", [self device_name], [self device_name]];
        }
    }
    else if ([self.device isEqual:@"Device 3"]) {
        if (indexPath.row == 0) {           // top row
            return [NSString stringWithFormat:@"Your current spendings for your %@ is something that we can improve on for future usage. Let's review how we can make this better and how it can affect your energy usage for the device.", [self device_name]];
        }
        else if (indexPath.row == 1) {     // middle rows
            return [NSString stringWithFormat:@"You are currently spending about $44.05 per month for your %@. We can reduce this down to $40.05 per month by lowering the power usage of the device during the times when it is not used the most. I see that the %@ is least used during the wee hours of night around 2AM to 5AM. Let's enter Energy Saver Mode to do that.", [self device_name], [self device_name]];
        }
        else {                              // last row
            return [NSString stringWithFormat:@"We can reduce the spendings you have on your %@ by as much as $4 per month! With enough time, you can get yourself a brand new %@ in no time. Heck, maybe even give one to me!", [self device_name], [self device_name]];
        }
    }
}

#pragma mark -
#pragma mark Switch Handlers

- (void)init_switch_for_cell:(NrGridTableViewCell *)cell
{
    device_switch = [[UISwitch alloc] initWithFrame:CGRectZero];
    cell.accessoryView = device_switch;
    BOOL status = device_switch.isOn;
    if ([self.station_data[@"Devices"][self.device][@"Status"]  isEqual: @"Off"])
        status = NO;
    else
        status = YES;
//    if([calendarMainViewController.lampValue integerValue] > 0)
//        status = true;
//    else
//        status = false;
    if([self.station_data[@"Devices"][self.device][@"Name"] isEqualToString:@"Lamp"]){
            if([calendarMainViewController.lampValue integerValue] > 5)
                status = true;
            else
                status = false;
    }
    [device_switch setOn:status animated:NO];
    
    [device_switch addTarget:self action:@selector(switch_changed:) forControlEvents:UIControlEventTouchUpInside];
}

- (void)update_switch_for_cell:(NrGridTableViewCell *)cell
{
    device_switch = [[UISwitch alloc] initWithFrame:CGRectZero];
    cell.accessoryView = device_switch;
    BOOL status;
    status = device_switch.isOn;
    if([calendarMainViewController.lampValue integerValue] > 5)
                status = true;
            else
                status = false;
    //status = device_switch.isOn;
    [device_switch setOn:status animated:NO];
    
    [device_switch addTarget:self action:@selector(switch_changed:) forControlEvents:UIControlEventTouchUpInside];
}


- (void) switch_changed:(id)sender
{
    UISwitch* switch_sender = sender;
    if (switch_sender.on) {
        
        if([[self device_name] isEqualToString:@"Lamp"]){
        // turn on the device here
        NSLog(@"Sentence: %@", [device_speech turn_on_device_message:[self device_name]]);
        [calendarMainViewController speakAction:[device_speech turn_on_device_message:[self device_name]]];
        NSURL *url = [NSURL URLWithString:@"http://128.195.151.158/simhome/control/on/?device=Bulb&level=100"];
        
        NSURL *url1 = [NSURL URLWithString:@"http://128.195.151.158/simhome/db/?type=showpower&&dev=lamp"];
        NSData *data = [NSData dataWithContentsOfURL:url];
        NSString *ret = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        NSLog(@"ret=%@", ret);
        //TODOTODO
        }
        
    } else {
        // turn off the device here
         if([[self device_name] isEqualToString:@"Lamp"]){
        [calendarMainViewController speakAction:[device_speech turn_off_device_message:[self device_name]]];
        NSURL *url = [NSURL URLWithString:@"http://128.195.151.158/simhome/control/on/?device=Bulb&level=0"];
        NSData *data = [NSData dataWithContentsOfURL:url];
        NSString *ret = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        NSLog(@"ret=%@", ret);
         }
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


@end
