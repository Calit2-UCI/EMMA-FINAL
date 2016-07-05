//
//  NaradaiSpeechTTS.m
//  NaradaAVTools
//
//  Created by Enrique on 4/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "NaradaiSpeechTTS.h"

NSString *apiKey = @"18cf918d69404c654f6a7fb037302c67";

#define kVoiceUSEnglishFemale @"usenglishfemale"
#define kVoiceUSEnglishMale @"usenglishmale"
#define kVoiceEURSpanishishFemale @"eurspanishfemale"
#define kVoiceEURSpanishishMale @"eurspanishmale"

@implementation NaradaiSpeechTTS

- (id)initWithID:(int)ttsID textToSpeak:(NSString*)text fileName:(NSString *)fileName language:(NSString*)lang isFemale:(BOOL)female andDelegate:(id<NaradaTTSDelegate>) delegate
{
    if ((self = [super initWithID:ttsID textToSpeak:text fileName:fileName language:lang isFemale:female andDelegate:delegate])) {
        self.queryURL = @"http://api.ispeech.org/api/rest?apikey=[apiKey]&action=convert&voice=[voice]&text=[textToConvert]&speed=[speed]";
        
        self.queryURL = [self.queryURL stringByReplacingOccurrencesOfString:@"[apiKey]" withString:apiKey];
        
        NSString *voice;
        if (female) {
            if ([self.language compare:@"en"] == NSOrderedSame) {
                voice = kVoiceUSEnglishFemale;
            }
            else if ([self.language compare:@"es"] == NSOrderedSame) {
                voice = kVoiceEURSpanishishFemale;
            }
        }
        else {
            if ([self.language compare:@"en"] == NSOrderedSame) {
                voice = kVoiceUSEnglishMale;
            }
            else if ([self.language compare:@"es"] == NSOrderedSame) {
                voice = kVoiceEURSpanishishMale;
            }
        }
        self.queryURL = [self.queryURL stringByReplacingOccurrencesOfString:@"[voice]" withString:voice];
        
        self.queryURL = [self.queryURL stringByReplacingOccurrencesOfString:@"[speed]" withString:@"0"];
    }
    return self;
}

- (id)initWithID:(int)ttsID sentencesToSpeak:(NSArray*)sentences fileName:(NSString *)fileName language:(NSString*)lang isFemale:(BOOL)female andDelegate:(id<NaradaTTSDelegate>)delegate
{
    if ((self = [super initWithID:ttsID sentencesToSpeak:sentences filename:fileName language:lang isFemale:female andDelegate:delegate])) {
        self.queryURL = @"http://api.ispeech.org/api/rest?apikey=[apiKey]&action=convert&voice=[voice]&text=[textToConvert]";
        
        self.queryURL = [self.queryURL stringByReplacingOccurrencesOfString:@"[apiKey]" withString:apiKey];
    }
    return self;
}

@end
