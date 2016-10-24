//
//  GridTableViewCell.h
//  UCIAssistant
//
//  Created by Christian Morte on 5/3/16.
//  Copyright (c) 2016 Narada Robotics S.L. All rights reserved.
//

#ifndef UCIAssistant_GridTableViewCell_h
#define UCIAssistant_GridTableViewCell_h

#import <UIKit/UIKit.h>

enum table_mode {
    TABLE_OVERVIEW,
    TABLE_DETAILED
};

@interface NrGridTableViewCell : UITableViewCell {
    
    UIColor *lineColor;
    BOOL topCell;
    
    UILabel *cell1;
    UILabel *cell2;
    UILabel *cell3;
//    UILabel *cell4;
}

- (id)initWithMode:(enum table_mode)mode withStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier;

@property (nonatomic, retain) UIColor* lineColor;
@property (nonatomic) BOOL topCell;
@property (readonly) UILabel* cell1;
@property (readonly) UILabel* cell2;
@property (readonly) UILabel* cell3;
//@property (readonly) UILabel* cell4;

@end


#endif
