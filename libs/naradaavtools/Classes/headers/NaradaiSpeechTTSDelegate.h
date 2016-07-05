//
//  NaradaiSpeechTTSDelegate.h
//  NaradaAVTools
//
//  Created by Enrique on 4/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@class NaradaiSpeechTTS

@protocol NaradaiSpeechTTSDelegate <NSObject>

- (void)NaradaiSpeechTTSDidFinish:(NaradaGoogleTTS *)ngtts;
- (void)NaradaiSpeechTTSDidFailWithError:(NSError *)error;

@end
