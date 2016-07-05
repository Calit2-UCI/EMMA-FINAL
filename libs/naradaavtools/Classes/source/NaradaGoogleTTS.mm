//
//  NaradaGoogleTTS.m
//  NaradaAVTools
//
//  Created by Enrique on 11/21/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "NaradaGoogleTTS.h"

#import <iostream>

using namespace std;

//NSString *queryURL = @"http://translate.google.com/translate_tts?ie=UTF-8&tl=";

@implementation NaradaGoogleTTS

//@synthesize ttsID = _ttsID;
//@synthesize text = _text;
//@synthesize language = _language;
//@synthesize delegate = _delegate;
//
//@synthesize receivedData = _receivedData;
//@synthesize pcmAudioPath = _pcmAudioPath;
//
//@synthesize last = _last;
//
//@synthesize sentences = _sentences;
//
- (id)initWithID:(int)ttsID textToSpeak:(NSString*)text filename:fileName language:(NSString*)lang isFemale:(BOOL)female andDelegate:(id<NaradaTTSDelegate>)delegate
{
//    if ((self = [super init])) {
//        self.ttsID = ttsID;
//        self.text = text;
//        self.language = lang;
//        self.delegate = delegate;
//        self.last = false;
//    }
//    return self;
    
    if ((self = [super initWithID:ttsID textToSpeak:text fileName:fileName language:lang isFemale:female andDelegate:delegate])) {
        self.queryURL = @"http://translate.google.com/translate_tts?ie=UTF-8&tl=[language]&q=[textToConvert]";
        self.queryURL = [self.queryURL stringByReplacingOccurrencesOfString:@"[language]" withString:self.language];
    }
    return self;
}

- (id)initWithID:(int)ttsID sentencesToSpeak:(NSArray*)sentences filename:fileName language:(NSString*)lang isFemale:(BOOL)female andDelegate:(id<NaradaTTSDelegate>)delegate
{
    if ((self = [super initWithID:ttsID sentencesToSpeak:sentences fileName:fileName language:lang isFemale:female andDelegate:delegate])) {
//        self.ttsID = ttsID;
//        self.language = lang;
//        self.delegate = delegate;
//        self.last = false;
//        self.sentences = sentences;
        self.queryURL = @"http://translate.google.com/translate_tts?ie=UTF-8&tl=[language]&q=[textToConvert]";
        self.queryURL = [self.queryURL stringByReplacingOccurrencesOfString:@"[language]" withString:self.language];
    }
    return self;
}
//
//
//+(NSArray *) chopSentences:(NSArray *)textArray withMaxLength:(int)maxlen
//{
//    //First chop. Punctuation
//    NSString *punctChars = @".,\\/?!;";
//    
//    NSMutableArray *dotArray = [self chopforPunctuation:(NSMutableArray *)textArray 
//                                      withCharsInString:punctChars];
//    
//    // 2. each element, rechop it counting chars
//    NSMutableArray *finalArray = [self chopforLength:(NSArray *)dotArray withMaxLength:maxlen];
//    
//    NSLog(@"%@", finalArray);
//    
//    return finalArray;
//}
//
//+(NSMutableArray *) chopforPunctuation:(NSMutableArray *)inArray withCharsInString:(NSString *)chars
//{
//    NSMutableArray *outArray;
//    
//    //for each character in chars
//    for (int i=0; i<[chars length]; i++) 
//    {
//        outArray = [NSMutableArray array];
//        //we get the current char
//        NSString *punct = [chars substringWithRange:NSMakeRange(i, 1)];
//        //for each string in the array
//        for (NSString *chop in inArray) 
//        {
//            //we separate the components in the current char
//            NSArray *components = [chop componentsSeparatedByString:punct];
//            
//            for (NSString *comp in components) {
//                if (![comp isEqualToString:@""] && ![comp isEqualToString:@" "]) {
//                    [outArray addObject:[comp stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]]];
//                }
//            }
//            
//            //            [outArray addObjectsFromArray:components];
//        }
//        //once we ended looping the inArray, we copy the array built (outArray) into the old one, to start looping again.
//        inArray = [outArray copy];
//    }
//    
//    NSLog(@"%@", outArray);
//    
//    return outArray;
//}
//
//+(NSMutableArray *) chopforLength:(NSArray *)inArray withMaxLength:(int)maxlen
//{
//    //    NSMutableArray *outArray = [NSMutableArray array];
//    NSMutableArray *outArray = [NSMutableArray array];
//    
//    int maxGoogle = maxlen;
//    
//    for (NSString *str in inArray) 
//    {
//        NSArray *words = [str componentsSeparatedByString:@" "];
//        NSMutableString *chain = [[NSMutableString alloc] initWithString:@""];
//        for (int i=0, count = 0; i<[words count]; i++)
//        {
//            NSString *word = [words objectAtIndex:i];
//            if ([word length] + count < maxGoogle) {
//                [chain appendString:word];
//                [chain appendString:@" "];
//                count += [word length];
//            }
//            else {
//                chain = (NSMutableString *)[chain stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
//                [outArray addObject:chain];
//                chain = [[NSMutableString alloc] initWithString:@""];
//                
//                count = 0; i--;
//                continue;
//            }
//        }
//        
//        chain = (NSMutableString *)[chain stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
//        
//        [outArray addObject:chain];
//        
//        chain = [[NSMutableString alloc] initWithString:@""];
//    }
//    
//    return outArray;
//}
//
//
//- (void)requestTTS
//{
//    cerr << self.ttsID << ": Creating TTS request" << endl;
//    
//    NSMutableString* requestStr = [[NSMutableString alloc] initWithString:@"http://translate.google.com/translate_tts?ie=UTF-8&tl="];
//    
////    NSString* language = AMLocalizedString(@"LANG_TTS", nil);
//    [requestStr appendString:self.language];
//    
//    [requestStr appendString:@"&q="];
//    
//    NSString * encodedString = (NSString *)CFURLCreateStringByAddingPercentEscapes(NULL, (CFStringRef)self.text, NULL, (CFStringRef)@"!*'();:@&=+$,/?%#[]", kCFStringEncodingUTF8 );
//    
//    [requestStr appendString:encodedString];
//    
//    printf("Text to say: %s\n", [requestStr UTF8String]);
//    
//    NSURLRequest * request = [NSURLRequest requestWithURL:[NSURL URLWithString:requestStr]
//                                              cachePolicy:NSURLRequestUseProtocolCachePolicy
//                                          timeoutInterval:60.0];
//    
//    NSURLConnection *theConnection =[[NSURLConnection alloc] initWithRequest:request delegate:self];
//    
//    if (theConnection) {
//        
//        // Create the NSMutableData to hold the received data.
//        
//        // receivedData is an instance variable declared elsewhere.
//        
//        self.receivedData = [[NSMutableData data] retain];
//            cerr << self.ttsID << ": request created" << endl;
//        
//    } else {
//        
//        // Inform the user that the connection failed.
//        
//        NSLog(@"The connection has failed\n");
//        
//    }
//
//}
//
//#pragma mark -
//#pragma mark HTTP request delegate methods (TTS)
//
//- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
//{
//    // This method is called when the server has determined that it
//    
//    // has enough information to create the NSURLResponse.
//    
//    
//    
//    // It can be called multiple times, for example in the case of a
//    
//    // redirect, so each time we reset the data.
//    
//    
//    
//    // receivedData is an instance variable declared elsewhere.
//    
//    NSLog(@"Did receive response!\n");
//    
//    [self.receivedData setLength:0];
//}
//
//- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
//{
//    
//    // Append the new data to receivedData.
//    
//    // receivedData is an instance variable declared elsewhere.
//    
//    NSLog(@"New data received %d\n", [data length]);
//    
//    [self.receivedData appendData:data];
//    
//}
//
//- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
//{
//    // release the connection, and the data object
//    
//    [connection cancel];
//    
//    [connection release];
//    
//    // receivedData is declared as a method instance elsewhere
//    
//    [self.receivedData release];
//    
//    // inform the user
//    
//    NSLog(@"Connection failed! Error - %@ %@",
//          [error localizedDescription],
//          [[error userInfo] objectForKey:NSURLErrorFailingURLStringErrorKey]);
//    
//    [self.delegate NaradaGoogleTTSDidFailWithError:error];
//    
//}
//
//- (void)connectionDidFinishLoading:(NSURLConnection *)connection
//{    
//    cerr << self.ttsID << ": Connection did finish loading" << endl;
//    
//    //Save the data just received into a path
//    NSMutableString * fileName = [[NSMutableString alloc] initWithString:@"outX.mp3"];
//    NSString * srcName = [fileName stringByReplacingOccurrencesOfString:@"X" withString:[NSString stringWithFormat:@"%d", self.ttsID]];
//    
//    NSString * dstName = [srcName stringByReplacingOccurrencesOfString:@"mp3" withString:@"pcm"];
//    
//    NSString * srcPath = [self getPathToDocuments:srcName];
//    NSString * dstPath = [self getPathToDocuments:dstName];    
//    
//    cerr << self.ttsID << ": Received data length for path " << [fileName UTF8String] << " is " << [self.receivedData length] << endl;
//    
//    
//    NSError *error = nil;
//    [self.receivedData writeToFile:srcPath options:NSDataWritingAtomic error:&error];
//    
//    cerr << self.ttsID << ": Doing conversion with source file: \n" << [srcPath UTF8String] << "\nAnd destination file: " << [dstPath UTF8String] << endl;
//    
//    [connection release];
//    
//    NaradaAudioConverter * nac = [[NaradaAudioConverter alloc] initWithSourcePath:srcPath 
//                                                               andDestinationPath:dstPath 
//                                                                    andSampleRate:16000 
//                                                                      andDelegate:self];
//    
//    
//    if ([self.receivedData length] <= 0) {
//        [self convertAudioDidFinish:nac];
//        return;
//    }
//    
//    [nac convertMP3toPCMWithAV];
//}
//
//-(void) convertAudioDidFinish:(NaradaAudioConverter *)converter
//{
//    cerr << self.ttsID << ": Conversion done" << endl;
//    
////    NSData * data = [NSData dataWithContentsOfFile:converter.destinationPath];
////    
////    NSString * wavFile = [converter.destinationPath stringByReplacingOccurrencesOfString:@"pcm" withString:@"wav"];
////    
////    [data writeToFile:wavFile atomically:YES];
//    
//    self.pcmAudioPath = converter.destinationPath;
//    
//    
////    [self.delegate performSelectorOnMainThread:@selector(NaradaGoogleTTSDidFinish:) 
////                                    withObject:self
////                                 waitUntilDone:YES];
//    
//    [self.delegate NaradaGoogleTTSDidFinish:self];
//    
//    
////    
////    NSString* wavFilePath = [self getPathToDocuments:@"test.wav"];
////    
////    NSData* pcmData = [[NSFileManager defaultManager] contentsAtPath:converter.destinationPath];
////    
////    cerr << "PCM length is: " << [mixedData length] << endl;
////    
////    [pcmData writeToFile:wavFilePath atomically:YES];
////    
////    [renderer addHeaderToWAVForPath: wavFilePath];
////    
////    cout << "Going to stop recording" << endl;
////    
////    [renderer stopRecording];
//}
//
//
//- (NSString *)getPathToDocuments:(NSString *)fileString
//{
//	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//	
//	return [ [paths objectAtIndex:0] stringByAppendingPathComponent:fileString ];
//}
//

@end
