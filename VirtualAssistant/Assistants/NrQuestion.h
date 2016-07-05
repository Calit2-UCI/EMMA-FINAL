//
//  NrQuestion.h
//  Calit2-Assistant
//
//  Created by Sergio Gago on 4/3/13.
//  Copyright (c) 2013 Narada Robotics S.L. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NrQuestion : NSObject

@property (nonatomic, strong) NSString *explanation;
@property (nonatomic, strong) NSString *question;
@property (nonatomic, strong) NSString *answer1;
@property (nonatomic, strong) NSString *answer2;
@property (nonatomic, strong) NSString *answer3;
@property (nonatomic, strong) NSString *answer4;
@property (nonatomic, assign) int correctAnswer;

- (id)initWithExplanation:(NSString *)explanation andQuestion:(NSString *)question andAnswer1:(NSString *)answer1 andAnswer2:(NSString *)answer2 andAnswer3:(NSString *)answer3 andAnswer4:(NSString *)answer4 andCorrectAnswer:(int)correctAnswer;

@end
