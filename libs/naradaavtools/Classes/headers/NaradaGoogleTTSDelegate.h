//
//  NaradaGoogleTTSDelegate.h
//  NaradaAVTools
//
//  Created by Enrique on 11/21/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@class NaradaGoogleTTS;

@protocol NaradaGoogleTTSDelegate <NSObject>

- (void)NaradaGoogleTTSDidFinish:(NaradaGoogleTTS *)ngtts;
- (void)NaradaGoogleTTSDidFailWithError:(NSError *)error;

@end
