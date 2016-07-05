//
//  Mixer.h
//  MyCookingAssistant
//
//  Created by Corbella on 30/11/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <AVFoundation/AVFoundation.h>
#import <CoreFoundation/CoreFoundation.h>
#import "NaradaAVMixerDelegate.h"


@interface NaradaAVMixer : NSObject {
	
	id <NaradaAVMixerDelegate> _delegate;

	NSString* _inAudioPath;
	NSString* _inVideoPath;
	NSString* _outVideoPath;

}

- (id)initWithPathsInAudio:(const NSString *)inAudio 
                   inVideo:(const NSString *)inVideo 
                  outVideo:(const NSString *)outVideo 
               andDelegate:(id<NaradaAVMixerDelegate>)delegate;

- (void)mixAudioVideo;

//Delegate for the callback to catch once the mix is done
@property (nonatomic, retain) id delegate;


//Path to the input audio
@property (nonatomic,copy) NSString* inAudioPath;

//Path to the input video only
@property (nonatomic,copy) NSString* inVideoPath;

//Path to the mixed audio+video
@property (nonatomic,copy) NSString* outVideoPath;

@end
