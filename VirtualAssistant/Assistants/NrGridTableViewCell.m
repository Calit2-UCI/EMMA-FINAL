//
//  NrGridTableViewCell.m
//  UCIAssistant
//
//  Created by Christian Morte on 5/3/16.
//  Copyright (c) 2016 Narada Robotics S.L. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NrGridTableViewCell.h"

#define tableWidth 400
#define smartTableWidth 300
#define numberOfColumns 3   // change if adding/deleting columns for table
#define numberOfSmartTableColumns 2 // change if adding/deleting columns for smart table

#define cell1Width tableWidth / numberOfColumns  // Overview table cells
#define cell2Width tableWidth / numberOfColumns
#define cell3Width tableWidth / numberOfColumns
#define overviewTableCellHeight 44

#define smartTableCell1Width tableWidth / numberOfSmartTableColumns  // Smart table cells
#define smartTableCell2Width tableWidth / numberOfSmartTableColumns
#define smartTableCell3Width 0
#define smartTableMiddleRowCellWidth smartTableWidth / numberOfSmartTableColumns
#define smartTableCellHeight 44

@implementation NrGridTableViewCell

@synthesize lineColor, topCell, cell1, cell2, cell3;// cell4  ;

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
//    self.transform = CGAffineTransformMakeRotation(M_PI * 0.5);
    if (self) {
        
        topCell = NO;
        
        // Add labels for the three cells
        cell1 = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, cell1Width, overviewTableCellHeight)];
        cell1.textAlignment = UITextAlignmentCenter;
        cell1.backgroundColor = [UIColor clearColor]; // Important to set or lines will not appear
        [self addSubview:cell1];
        
        cell2 = [[UILabel alloc] initWithFrame:CGRectMake(cell1Width, 0, cell2Width, overviewTableCellHeight)];
        cell2.textAlignment = UITextAlignmentCenter;
        cell2.backgroundColor = [UIColor clearColor]; // Important to set or lines will not appear
        [self addSubview:cell2];
        
        cell3 = [[UILabel alloc] initWithFrame:CGRectMake(cell1Width+cell2Width, 0, cell3Width, overviewTableCellHeight)]; // Note - hardcoded 320 is not ideal; this can be done better
        cell3.textAlignment = UITextAlignmentCenter;
        cell3.backgroundColor = [UIColor clearColor]; // Important to set or lines will not appear
        [self addSubview:cell3];

//        cell4 = [[UILabel alloc] initWithFrame:CGRectMake(cell1Width+cell2Width+cell3Width, 0, 400-(cell1Width+cell2Width+cell3Width), cellHeight)]; // Note - hardcoded 320 is not ideal; this can be done better
//        cell4.textAlignment = UITextAlignmentCenter;
//        cell4.backgroundColor = [UIColor clearColor]; // Important to set or lines will not appear
//        [self addSubview:cell4];
        
    }
    return self;
}

- (id)initWithMode:(enum table_mode)mode withStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        
        topCell = NO;
        
        cell1 = [[UILabel alloc] initWithFrame:CGRectMake(0, 0,
                                                          mode == TABLE_OVERVIEW ? cell1Width : smartTableMiddleRowCellWidth,
                                                          mode == TABLE_OVERVIEW ? overviewTableCellHeight : smartTableCellHeight)];
        cell1.textAlignment = UITextAlignmentCenter;
        cell1.backgroundColor = [UIColor clearColor]; // Important to set or lines will not appear
        [self drawRect:cell1.frame onMode:mode];
        [self addSubview:cell1];
        
        cell2 = [[UILabel alloc] initWithFrame:CGRectMake(mode == TABLE_OVERVIEW ? cell1Width : smartTableMiddleRowCellWidth,
                                                          0,
                                                          mode == TABLE_OVERVIEW ? cell2Width : smartTableMiddleRowCellWidth+smartTableMiddleRowCellWidth,
                                                          mode == TABLE_OVERVIEW ? overviewTableCellHeight : smartTableCellHeight)];
        cell2.textAlignment = UITextAlignmentCenter;
        cell2.backgroundColor = [UIColor clearColor]; // Important to set or lines will not appear
        [self drawRect:cell2.frame onMode:mode];
        [self addSubview:cell2];
        
        if (mode == TABLE_OVERVIEW) {       // only have three columns for overview table
            cell3 = [[UILabel alloc] initWithFrame:CGRectMake(mode == TABLE_OVERVIEW ? cell1Width+cell2Width : 0,
                                                              0,
                                                              mode == TABLE_OVERVIEW ? cell3Width : 0,
                                                              mode == TABLE_OVERVIEW ? overviewTableCellHeight : 0)];
            cell3.textAlignment = UITextAlignmentCenter;
            cell3.backgroundColor = [UIColor clearColor]; // Important to set or lines will not appear
            [self drawRect:cell3.frame onMode:mode];
            [self addSubview:cell3];
        }
    }
    return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];
    
    // Configure the view for the selected state
}

- (void)drawRect:(CGRect)rect onMode:(enum table_mode)mode
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetStrokeColorWithColor(context, lineColor.CGColor);
    
    // CGContextSetLineWidth: The default line width is 1 unit. When stroked, the line straddles the path, with half of the total width on either side.
    // Therefore, a 1 pixel vertical line will not draw crisply unless it is offest by 0.5. This problem does not seem to affect horizontal lines.
    CGContextSetLineWidth(context, 2.0);
    
    // Add the vertical lines
    CGContextMoveToPoint(context,
                         mode == TABLE_OVERVIEW ? cell1Width+0.5 : smartTableMiddleRowCellWidth+0.5, 0);
    CGContextAddLineToPoint(context,
                            mode == TABLE_OVERVIEW ? cell1Width+0.5 : smartTableMiddleRowCellWidth, rect.size.height);
    
    CGContextMoveToPoint(context, mode == TABLE_OVERVIEW ? cell1Width+cell2Width+0.5 : 2*smartTableMiddleRowCellWidth + 0.5, 0);
    CGContextAddLineToPoint(context,
                            mode == TABLE_OVERVIEW ? cell1Width+cell2Width+0.5 : 2*smartTableMiddleRowCellWidth + 0.5, rect.size.height);
    
    CGContextMoveToPoint(context, mode == TABLE_OVERVIEW ? cell1Width+cell2Width+cell3Width+10 : 3*smartTableMiddleRowCellWidth + 10, 0);
    CGContextAddLineToPoint(context, mode == TABLE_OVERVIEW ? cell1Width+cell2Width+cell3Width+10 : 3*smartTableMiddleRowCellWidth + 10, rect.size.height);
    
    // Add bottom line
    CGContextMoveToPoint(context, 0, rect.size.height);
    CGContextAddLineToPoint(context, rect.size.width, rect.size.height-0.5);
    
    // If this is the topmost cell in the table, draw the line on top
    if (topCell)
    {
        CGContextMoveToPoint(context, 0, 0);
        CGContextAddLineToPoint(context, rect.size.width, 0);
    }
    
    // Draw the lines
    CGContextStrokePath(context); 
}

- (void)setTopCell:(BOOL)newTopCell
{
    topCell = newTopCell;
    [self setNeedsDisplay];
}

@end
