//
//  NaradaTTS.h
//  NaradaAVTools
//
//  Created by Enrique on 4/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "NaradaAudioConverter.h"
#import "NaradaTTSDelegate.h"
#import "NaradaAudioConverterDelegate.h"

@interface NaradaTTS : NSObject
<
NaradaAudioConverterDelegate, 
NSURLConnectionDelegate //Google translator (TTS) 
>
{
    int _ttsID;
    NSString * _text;
    NSString * _language;
    BOOL _isFemale;
    
    NSObject<NaradaTTSDelegate> * _delegate;
    
    NSMutableData * _receivedData;
    
    NSString * _pcmAudioPath;
    
    BOOL _last;
    
    NSArray *_sentences;
    
    
    NSString *_queryURL;
    
    BOOL didFailLast;
    
    NSString *_fileName;
}

@property (nonatomic, assign) int ttsID;
@property (nonatomic, copy) NSString * text;
@property (nonatomic, copy) NSString * language;
@property (nonatomic, assign) BOOL isFemale;
@property (nonatomic, retain) NSObject<NaradaTTSDelegate> * delegate;

@property (nonatomic, retain) NSMutableData * receivedData;

@property (nonatomic, copy) NSString * pcmAudioPath;

@property (nonatomic, copy) NSString *queryURL;

@property (nonatomic, assign) BOOL last;

@property (nonatomic, retain) NSArray *sentences;

@property (retain) NSString *fileName;

/**
 * This is the main constructor. It must be specified an ID, which is the order to be said (ie. first the avatar will speak ID no 0, then no 1 and so on. 
 * Then it must be specified the text to speak, and finally a delegate, which will take the request and speak it when possible.
 */
- (id)initWithID:(int)ttsID textToSpeak:(NSString*)text fileName:(NSString *)fileName language:(NSString*)lang isFemale:(BOOL)female andDelegate:(id<NaradaTTSDelegate>)delegate;

- (id)initWithID:(int)ttsID sentencesToSpeak:(NSArray*)sentences fileName:(NSString *)fileName language:(NSString*)lang isFemale:(BOOL)female andDelegate:(id<NaradaTTSDelegate>)delegate;

/**
 * Requests a File containing the audio for the text set in the constructor
 */
- (void)requestTTS;

+(NSString *)getPathToDocuments:(NSString *)fileString;

/** Divides the sentences into small pieces to be sent to the Google TTS, wich will convert them into audio files */
+(NSArray *) chopSentences:(NSArray *)textArray withMaxLength:(int)maxlen;

/** Divides an array of sentences into pieces regarding puntuation marks, such as dots, commas, etc. */
+(NSMutableArray *) chopforPunctuation:(NSString *)sentence withCharsInString:(NSString *)chars;

+(NSMutableArray *) chopforPunctuation:(NSString *)sentence withChar:(NSString *)charStr;

/** Divides an array of sentences (NSStrings) into pieces regarding its length, with a maximum specified */
+(NSMutableArray *) chopforLength:(NSArray *)inArray withMaxLength:(int)maxlen;

+ (NSArray *) chopSentencesArray:(NSArray *)textArray withMaxLength:(int)maxlen;
+ (NSArray *)chopSentence:(NSString *)sentence withMaxLength:(int)maxlen forIndex:(int)index toArray:(NSMutableArray *)queueArray;

+(id)initWithID:(int)ttsID sentencesToSpeak:(NSArray*)sentences language:(NSString*)lang andDelegate:(id<NaradaTTSDelegate>) delegate;

+(BOOL)sentences:(NSArray *)sentences areShorterThan:(int)maxlen;

/**
 * Requests to convert the mp3 to pcm
 */
- (void)requestConversion;


@end
