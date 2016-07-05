//
//  NaradaTTS.m
//  NaradaAVTools
//
//  Created by Enrique on 4/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "NaradaTTS.h"

#import <iostream>

using namespace std;


@implementation NaradaTTS

@synthesize ttsID = _ttsID;
@synthesize text = _text;
@synthesize language = _language;
@synthesize isFemale = _isFemale;
@synthesize delegate = _delegate;

@synthesize queryURL = _queryURL;

@synthesize receivedData = _receivedData;
@synthesize pcmAudioPath = _pcmAudioPath;

@synthesize last = _last;

@synthesize sentences = _sentences;

@synthesize fileName = _fileName;

- (id)initWithID:(int)ttsID textToSpeak:(NSString*)text fileName:(NSString *)fileName language:(NSString*)lang isFemale:(BOOL)female andDelegate:(id<NaradaTTSDelegate>)delegate
{
    if ((self = [super init])) {
        self.ttsID = ttsID;
        self.fileName = fileName;
        self.text = text;
        self.language = lang;
        self.delegate = delegate;
        self.last = false;
        self.isFemale = female;
        didFailLast = false;
    }
    return self;
}

- (id)initWithID:(int)ttsID sentencesToSpeak:(NSArray*)sentences fileName:(NSString *)fileName language:(NSString*)lang isFemale:(BOOL)female andDelegate:(id<NaradaTTSDelegate>)delegate
{
    if ((self = [super init])) {
        self.ttsID = ttsID;
        self.fileName = fileName;
        self.language = lang;
        self.delegate = delegate;
        self.last = false;
        self.sentences = sentences;
        self.isFemale = female;
    }
    return self;
}


+(NSArray *) chopSentences:(NSArray *)textArray withMaxLength:(int)maxlen
{
    if (maxlen == 0) {
        NSLog(@"Verses post chop: %@", textArray);
        return textArray;
    }
    NSLog(@"Verses pre chop: %@", textArray);
    
//    textArray = [NSArray arrayWithObjects:@"Hello. hello my dear, friend of the party in the corner every night in the morning to the people right here", @"Goodbye. goodbye my dear. friend of the party in the corner every night. in the morning", nil];
    
    NSMutableArray *textArrayMutable = [NSMutableArray array];
    for (NSString *str in textArray) {
        str = [str stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
        [textArrayMutable addObject:str];
    }
    
    NSLog(@"Verses post trimming: %@", textArrayMutable);
    
    
    NSArray *punctChars = [NSArray arrayWithObjects:@".", @",", @"\\", @"/", @"?", @"!", @";", nil];
//    NSMutableArray *textArrayMutable = [textArray mutableCopy];
    NSMutableArray *choppedPunctArray = [NSMutableArray array];
    
    for (NSString *punctChar in punctChars) {
        for (NSString *sentence in textArrayMutable) {
            if ([sentence length] >= maxlen) {
                [choppedPunctArray addObjectsFromArray:(NSArray *)[self chopforPunctuation:sentence withChar:punctChar]];
                NSLog(@"%@", choppedPunctArray);
            }
            else {
                [choppedPunctArray addObject:sentence];
            }
        }
        textArrayMutable = choppedPunctArray;
        choppedPunctArray = [NSMutableArray array];
    }
    
    textArrayMutable = [self chopforLength:textArrayMutable withMaxLength:maxlen];
    
    NSLog(@"Verses post chop: %@", textArrayMutable);
    
    return [NSArray arrayWithArray:(NSArray *)textArrayMutable];
}

+(BOOL)sentences:(NSArray *)sentences areShorterThan:(int)maxlen
{
    for (NSString *sentence in sentences) {
        if ([sentence length] > maxlen) {
            return NO;
        }
    }
    return YES;
}
                                  
+(NSMutableArray *) chopforPunctuation:(NSString *)sentence withChar:(NSString *)charStr
{
    //we separate the components in the current char
    NSArray *components = [sentence componentsSeparatedByString:charStr];
    
    NSMutableArray *outArray = [NSMutableArray array];
    
    for (NSString *comp in components) {
        if (![comp isEqualToString:@""] && ![comp isEqualToString:@" "]) {
            NSString *trimmedStr = [comp stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
            [outArray addObject:[trimmedStr stringByAppendingString:charStr]];
        }
    }
    
    return outArray;
}

+(NSMutableArray *) chopforPunctuation:(NSString *)sentence withCharsInString:(NSString *)chars andMaxLen:(int)maxlen
{    
    
//    //for each character in chars
//    for (int i=0; i<[chars length]; i++) 
//    {
//        //we get the current char
//        NSString *punct = [chars substringWithRange:NSMakeRange(i, 1)];
//        //we separate the components in the current char
//        NSArray *components = [inSentence componentsSeparatedByString:punct];
//        
//        for (NSString *comp in components) {
//            if (![comp isEqualToString:@""] && ![comp isEqualToString:@" "]) {
//                [outArray addObject:[comp stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]]];
//            }
//        }
//        
//        //            [outArray addObjectsFromArray:components];
//        }
//        //once we ended looping the inArray, we copy the array built (outArray) into the old one, to start looping again.
//        inArray = [outArray copy];
//    }
//    
//    NSLog(@"%@", outArray);
//    
//    return outArray;
}

+ (NSMutableArray *) chopforLength:(NSArray *)inArray withMaxLength:(int)maxlen
{
    //    NSMutableArray *outArray = [NSMutableArray array];
    NSMutableArray *outArray = [NSMutableArray array];
    
    int maxGoogle = maxlen;
    
    for (NSString *str in inArray) 
    {
        NSArray *words = [str componentsSeparatedByString:@" "];
        NSMutableString *chain = [[NSMutableString alloc] initWithString:@""];
        for (int i=0, count = 0; i<[words count]; i++)
        {
            NSString *word = [words objectAtIndex:i];
            if ([word length] + count < maxGoogle) {
                [chain appendString:word];
                [chain appendString:@" "];
                count += [word length] +1;
            }
            else {
                chain = (NSMutableString *)[chain stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
                [outArray addObject:chain];
                chain = [[NSMutableString alloc] initWithString:@""];
                
                count = 0; i--;
                continue;
            }
        }
        
        chain = (NSMutableString *)[chain stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
        
        [outArray addObject:chain];
        
        chain = [[NSMutableString alloc] initWithString:@""];
    }
    
    return outArray;
}

+ (NSArray *) chopSentencesArray:(NSArray *)textArray withMaxLength:(int)maxlen
{
    NSMutableArray *mArray = [NSMutableArray array];
    for (NSString *sentence in textArray) {
        [mArray addObjectsFromArray:[NaradaTTS chopSentence:sentence withMaxLength:maxlen forIndex:0 toArray:[NSMutableArray array]]];
    }
    
    return [mArray copy];
}

+ (NSArray *)chopSentence:(NSString *)sentence withMaxLength:(int)maxlen forIndex:(int)index toArray:(NSMutableArray *)queueArray
{
    if (sentence.length <= maxlen || maxlen == 0) {
        return [NSArray arrayWithObject:sentence];
    }
    
    NSArray *sepChars = [NSArray arrayWithObjects:@"!", @"?", @".", @",", @" ", nil];
    
    while (index<sepChars.count) {
        NSMutableArray *splitCharsArray = [[sentence componentsSeparatedByString:[sepChars objectAtIndex:index]] mutableCopy];
        //        NSLog(@"Split chars array by char: %@: %@", [sepChars objectAtIndex:index], splitCharsArray);
        
        if (splitCharsArray.count == 1) {
            index++;
            continue;
        }
        
        NSMutableArray *mArray = [NSMutableArray array];
        for (NSString *str in splitCharsArray) {
            if (![str isEqualToString:@""]) {
                [mArray addObject:[[str stringByAppendingString:[sepChars objectAtIndex:index]] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]]];
            }
        }
        //        NSLog(@"mArray = %@", mArray);
        
        NSMutableString *resStr = [NSMutableString stringWithString:@""];
        while (mArray.count > 0) {
            //            NSLog(@"count: %d", mArray.count);
            //            NSLog(@"Reading mArray: %@", mArray);
            NSString *str = [mArray objectAtIndex:0];
            if (str.length > maxlen && [resStr isEqualToString:@""]) {
                [self chopSentence:str withMaxLength:maxlen forIndex:index +1 toArray:queueArray];
                //                NSArray *recursiveArray = [self chopSentence:str withMaxLength:maxlen forIndex:index +1 toArray:queueArray];
                //                NSLog(@"Recursive Array: %@", recursiveArray);
                //                NSLog(@"mArray is: %@", mArray);
                [mArray removeObjectAtIndex:0];
                continue;
            }
            else if (resStr.length + str.length > maxlen) {
                [queueArray addObject:resStr];
                resStr = [NSMutableString stringWithString:@""];
                //                NSLog(@"QueueArray: %@", queueArray);
                continue;
            }
            else if (resStr.length + str.length <= maxlen){
                [resStr appendFormat:@" %@", str];
                [mArray removeObjectAtIndex:0];
                //                NSLog(@"ResStr: %@", resStr);
                //                NSLog(@"mArray: %@", mArray);
                
                if (mArray.count == 0) {
                    [queueArray addObject:resStr];
                    //                    NSLog(@"Return Array: %@", queueArray);
                    return queueArray;
                }
            }
        }
        
        return queueArray;
    }
    
    return [queueArray copy];
}


- (void)requestTTS
{
    cerr << self.ttsID << ": Creating TTS request" << endl;
    
//    NSMutableString* requestStr = [[NSMutableString alloc] initWithString:queryURL];
    
    //    NSString* language = AMLocalizedString(@"LANG_TTS", nil);
//    [requestStr appendString:self.language];
//    [requestStr appendString:@"&q="];
    
//    queryURL = [queryURL stringByReplacingOccurrencesOfString:@"[language]" withString:self.language];
    
    
    NSString * encodedString = (NSString *)CFURLCreateStringByAddingPercentEscapes(NULL, (CFStringRef)self.text, NULL, (CFStringRef)@"!*'();:@&=+$,/?%#[]", kCFStringEncodingUTF8 );
    
//    [requestStr appendString:encodedString];
    self.queryURL = [self.queryURL stringByReplacingOccurrencesOfString:@"[textToConvert]" withString:encodedString];
    
    
    printf("Text to say: %s\n", [self.queryURL UTF8String]);
    
    NSURLRequest * request = [NSURLRequest requestWithURL:[NSURL URLWithString:self.queryURL]
                                              cachePolicy:NSURLRequestUseProtocolCachePolicy
                                          timeoutInterval:60.0];
    
    NSURLConnection *theConnection =[[NSURLConnection alloc] initWithRequest:request delegate:self];
    
    if (theConnection) {
        
        // Create the NSMutableData to hold the received data.
        
        // receivedData is an instance variable declared elsewhere.
        
        self.receivedData = [[NSMutableData data] retain];
        cerr << self.ttsID << ": request created" << endl;
        
    } else {
        
        // Inform the user that the connection failed.
        
        NSLog(@"The connection has failed\n");
        
    }
    
}

- (void)requestConversion
{
    NSLog(@"fileName is: %@", self.fileName);
    NSLog(@"id is %d", self.ttsID);
    //Save the data just received into a path derived from its name. For example, to saturday0.mp3
    NSString *srcName = [NSString stringWithFormat:@"%@%d.%@", self.fileName, self.ttsID, @"mp3"];
    NSString *dstName = [NSString stringWithFormat:@"%@%d.%@", self.fileName, self.ttsID, @"pcm"];
    
    
    //    NSMutableString * fileName = [[NSMutableString alloc] initWithString:@"outX.mp3"];
    //    NSString * srcName = [fileName stringByReplacingOccurrencesOfString:@"X" withString:[NSString stringWithFormat:@"%d", self.ttsID]];
    //
    //    NSString * dstName = [srcName stringByReplacingOccurrencesOfString:@"mp3" withString:@"pcm"];
    
#ifndef USE_TEMPORARY_DIR
    NSString * srcPath = [self getPathToDocuments:srcName];
    NSString * dstPath = [self getPathToDocuments:dstName];
#else
    NSString *srcPath = [NSTemporaryDirectory() stringByAppendingPathComponent:srcName];
    NSString *dstPath = [NSTemporaryDirectory() stringByAppendingPathComponent:dstName];
#endif
    
    cerr << self.ttsID << ": Received data length for path " << [self.fileName UTF8String] << " is " << [self.receivedData length] << endl;
    
    
    NSError *error = nil;
    [self.receivedData writeToFile:srcPath options:NSDataWritingAtomic error:&error];
    
    cerr << self.ttsID << ": Doing conversion with source file: \n" << [srcPath UTF8String] << "\nAnd destination file: " << [dstPath UTF8String] << endl;
    
    
    NaradaAudioConverter * nac = [[NaradaAudioConverter alloc] initWithSourcePath:srcPath
                                                               andDestinationPath:dstPath
                                                                    andSampleRate:16000
                                                                      andDelegate:self];
    
    
    if ([self.receivedData length] <= 0) {
        [self convertAudioDidFinish:nac];
        return;
    }
    
    [nac convertMP3toPCMWithAV];
}

#pragma mark -
#pragma mark HTTP request delegate methods (TTS)

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    // This method is called when the server has determined that it
    
    // has enough information to create the NSURLResponse.
    
    
    
    // It can be called multiple times, for example in the case of a
    
    // redirect, so each time we reset the data.
    
    
    
    // receivedData is an instance variable declared elsewhere.
    
    NSLog(@"Did receive response!\n");
    
    [self.receivedData setLength:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    
    // Append the new data to receivedData.
    
    // receivedData is an instance variable declared elsewhere.
    
    NSLog(@"New data received %d\n", [data length]);
    
    [self.receivedData appendData:data];
    
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    // release the connection, and the data object
    
    [connection cancel];
    
    [connection release];
    
    // receivedData is declared as a method instance elsewhere
    
    [self.receivedData release];
    
    // inform the user
    
    NSLog(@"Connection failed! Error - %@ %@",
          [error localizedDescription],
          [[error userInfo] objectForKey:NSURLErrorFailingURLStringErrorKey]);
    
    [self.delegate NaradaTTSDidFailWithError:error];
    
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{    
    cerr << self.ttsID << ": Connection did finish loading" << endl;
    
    [connection release];
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.delegate NaradaTTSDidFinish:self];
    });
}

-(void) convertAudioDidFinish:(NaradaAudioConverter *)converter
{
    cerr << self.ttsID << ": Conversion done" << endl;
    
    self.pcmAudioPath = converter.destinationPath;
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.delegate naradaTTSDidFinishConvertingAudio:self];
    });
}

- (void)convertAudio:(NaradaAudioConverter *)converter didFailWithError:(NSError *)error
{
    NSLog(@"Conversion failed with error: %@", [error localizedDescription]);
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.delegate naradaTTS:self didFailConvertingWithError:error];
    });
}


- (NSString *)getPathToDocuments:(NSString *)fileString
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	
	return [ [paths objectAtIndex:0] stringByAppendingPathComponent:fileString ];
}

@end
