//
//  Mixer.h
//  AviCuleNarada
//
//  Created by Corbella on 30/11/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "ESRenderer.h"


@interface Mixer : NSObject {

	id <ESRenderer> renderer;
	
	NSString *destinationFilePath;
    CFURLRef sourceURL;
    CFURLRef destinationURL;
	
	UInt64 sampleRate;
	
}

- (void)convertWavtoAAC;
- (void)convertAudio;
- (void)setAudio;
- (void)mixAudioVideoWithController:(UIViewController *)controller;


@property (nonatomic, retain) id <ESRenderer> renderer;
@property (nonatomic, retain) NSString *destinationFilePath;

@end
