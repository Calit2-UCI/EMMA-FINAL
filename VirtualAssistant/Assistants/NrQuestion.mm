//
//  NrQuestion.m
//  Calit2-Assistant
//
//  Created by Sergio Gago on 4/3/13.
//  Copyright (c) 2013 Narada Robotics S.L. All rights reserved.
//

#import "NrQuestion.h"

@implementation NrQuestion

@synthesize explanation = _explanation;
@synthesize question = _question;
@synthesize answer1 = _answer1;
@synthesize answer2 = _answer2;
@synthesize answer3 = _answer3;
@synthesize answer4 = _answer4;
@synthesize correctAnswer = _correctAnswer;

- (id)initWithExplanation:(NSString *)explanation andQuestion:(NSString *)question andAnswer1:(NSString *)answer1 andAnswer2:(NSString *)answer2 andAnswer3:(NSString *)answer3 andAnswer4:(NSString *)answer4 andCorrectAnswer:(int)correctAnswer
{
    if ((self = [super init])) {
        self.explanation = explanation;
        self.question = question;
        self.answer1 = answer1;
        self.answer2 = answer2;
        self.answer3 = answer3;
        self.answer4 = answer4;
        self.correctAnswer = correctAnswer;
    }
    return self;
}

@end
