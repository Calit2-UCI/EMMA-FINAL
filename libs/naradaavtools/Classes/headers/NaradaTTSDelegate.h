//
//  NaradaTTSDelegate.h
//  NaradaAVTools
//
//  Created by Enrique on 4/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@class NaradaTTS;

@protocol NaradaTTSDelegate <NSObject>

- (void)NaradaTTSDidFinish:(NaradaTTS *)ngtts;
- (void)NaradaTTSDidFailWithError:(NSError *)error;

- (void)naradaTTSDidFinishConvertingAudio:(NaradaTTS *)ngtts;
- (void)naradaTTS:(NaradaTTS *)ngtts didFailConvertingWithError:(NSError *)error;

@end
