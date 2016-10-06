//
//  NrLocationsTableViewController.m
//  VirtualAssistant
//
//  Created by Enrique on 10/15/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NrLocationsTableViewController.h"

@interface NrLocationsTableViewController ()

@end

@implementation NrLocationsTableViewController

@synthesize navController;
@synthesize weatherLocMng;

- (id)initWithLocationManager:(NrWeatherLocationManager *)mng
{
    self = [super initWithStyle:UITableViewStyleGrouped];
    if (self) {
        weatherLocMng = mng;        
        self.navController = [[UINavigationController alloc] initWithRootViewController:self];
        self.title = NSLocalizedString(@"LOCATION_TABLEVIEW_TITLE", nil);
        
        UIBarButtonItem *backButton = [[UIBarButtonItem alloc] initWithTitle:NSLocalizedString(@"BACK", nil) style:UIBarButtonItemStyleBordered target:self action:@selector(backController)];
        
        self.navigationItem.leftBarButtonItem = backButton;
        self.navController.modalPresentationStyle = UIModalPresentationFormSheet;
        
        self.navController.navigationBar.tintColor = [UIColor colorWithRed:224.0f/255.0f green:161.0f/255.0f blue:34.0f/255.0f alpha:255.0f/255.0f];
        
    }
    return self;
}

- (void)backController
{
    [self dismissViewControllerAnimated:YES completion:^{
        NSLog(@"Successfully dismissed controller");
    }];
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    // Uncomment the following line to preserve selection between presentations.
    self.clearsSelectionOnViewWillAppear = YES;
 
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
    
    
    [self.tableView reloadData];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    return NSLocalizedString(@"LOCATION_CONFIRM_TITLE", nil);
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return self.weatherLocMng.possibleLocations.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    
    NrWeatherLocation * loc = [self.weatherLocMng.possibleLocations objectAtIndex:indexPath.row];
    
    if ([loc.stateName compare:@""] != NSOrderedSame) {
        cell.textLabel.text = [NSString stringWithFormat:@"%@ (%@), %@", loc.cityName, loc.stateName, loc.countryName];
    }
    else {
        cell.textLabel.text = [NSString stringWithFormat:@"%@, %@", loc.cityName, loc.countryName];
    }
    
    return cell;
}

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [self.weatherLocMng confirmCityByIndex:indexPath.row];
    [self dismissViewControllerAnimated:YES completion:^{
        NSLog(@"View controller successfully dismissed");
    }];
}

@end
