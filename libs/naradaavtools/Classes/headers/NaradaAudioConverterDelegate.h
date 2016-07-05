//
//  NaradaAudioConverterDelegate.h
//  NaradaAVTools
//
//  Created by Enrique on 11/10/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@class NaradaAudioConverter;

@protocol NaradaAudioConverterDelegate <NSObject>

- (void)convertAudioDidFinish:(NaradaAudioConverter *)converter;
- (void)convertAudio:(NaradaAudioConverter *)converter didFailWithError:(NSError *)error;

@end
