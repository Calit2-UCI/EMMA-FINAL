//
//  PathsAndConstants.h
//  BasicCharacter
//
//  Created by Enrique Oriol on 19/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef BasicCharacter_PathsAndConstants_h
#define BasicCharacter_PathsAndConstants_h

#define defaultFrameRate	25
#define recordingFrameRate	12 //-KaiK- mod. por pruebas. Previamente rarreaza 2012-02-20 modified to value=25 to correct buggy rotation while recording

//for moveModel Used also in ES1renderer.m
#define kMaxX 6
#define kMinX -6
#define kMaxY -6
#define kMinY -6

#define kMaxYaw 20.0f
#define kMinYaw -20.0f
#define kMaxPitch 12.0f
#define kMinPitch -12.0f

#define kInitYaw 0.0f
#define kInitPitch -4.0f

//rarreaza 2012-01-25 defines for differentiating wav and fba files for high pitch implementation
#define _DEFAULT_NORMALPITCH_AUDIO_FILENAME @"test.wav"
#define _DEFAULT_NORMALPITCH_ACTIONS_FILENAME @"test.fba"
#define _DEFAULT_HIGHPITCH_AUDIO_FILENAME @"test_high_pitch.wav"
#define _DEFAULT_HIGHPITCH_ACTIONS_FILENAME @"test_high_pitch.fba"
#define _MUTE_NORMALPITCH_ACTIONS_FILENAME @"test_Mute.fba"
#define _MUTE_HIGHPITCH_ACTIONS_FILENAME @"test_high_pitch_Mute.fba"


static NSString *const notificationModelCompletelyLoaded = @"modelHasBeenLoaded";

static NSString *const notificationGestureDetectedScratchX = @"scratchInXAxisDetected";
static NSString *const notificationGestureDetectedScratchY = @"scratchInYAxisDetected";
static NSString *const notificationGestureDetectedDoubleTap = @"doubleTapGestureDetected";
static NSString *const notificationGestureDetectedPinch = @"pinchGestureDetected";

static NSString *const notificationJustFinishedCoverProtectionTime = @"hideCover";
static NSString *const notificationModifiedLoadingBar = @"loadingBarModified";
static NSString *const notificationFinishedSpeaking = @"finishedSpeaking";
static NSString *const notificationDidFailSpeaking = @"didFailSpeaking";
static NSString *const notificationJustFinishedPlayingTrackedRotAndPos= @"finishedPlayingTracked";

static NSString *const notificationEAGLViewHasBeenResized=@"EAGLViewResized";

static NSString *const notificationSetRecordingAudioLabel1 = @"Recording.";
static NSString *const notificationSetRecordingAudioLabel2 = @"Recording..";
static NSString *const notificationSetRecordingAudioLabel3 = @"Recording...";
static NSString *const notificationEmptyRecordingAudioLabel = @"emptyRecordingLabel";
static NSString *const notificationAudioIsAlreadyRecorded = @"alreadyRecorded";
static NSString *const notificationDisplayRecordStatus = @"showRecord";
static NSString *const notificationRecordingHasExceedSize = @"stopRecording";

static NSString *const notificationJustFinishedTranslationAnimation = @"finishedTranslationAnim";
static NSString *const notificationJustFinishedRotationAnimation = @"finishedRotationAnim";


#endif
