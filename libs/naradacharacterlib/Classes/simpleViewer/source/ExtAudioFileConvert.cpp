/*
        File: ExtAudioFileConvert.cpp
    Abstract: Demonstrates converting audio using ExtAudioFile.
     Version: 1.0
    
    Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
    Inc. ("Apple") in consideration of your agreement to the following
    terms, and your use, installation, modification or redistribution of
    this Apple software constitutes acceptance of these terms.  If you do
    not agree with these terms, please do not use, install, modify or
    redistribute this Apple software.
    
    In consideration of your agreement to abide by the following terms, and
    subject to these terms, Apple grants you a personal, non-exclusive
    license, under Apple's copyrights in this original Apple software (the
    "Apple Software"), to use, reproduce, modify and redistribute the Apple
    Software, with or without modifications, in source and/or binary forms;
    provided that if you redistribute the Apple Software in its entirety and
    without modifications, you must retain this notice and the following
    text and disclaimers in all such redistributions of the Apple Software.
    Neither the name, trademarks, service marks or logos of Apple Inc. may
    be used to endorse or promote products derived from the Apple Software
    without specific prior written permission from Apple.  Except as
    expressly stated in this notice, no other rights or licenses, express or
    implied, are granted by Apple herein, including but not limited to any
    patent rights that may be infringed by your derivative works or by other
    works in which the Apple Software may be incorporated.
    
    The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
    MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
    THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
    OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
    
    IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
    OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
    MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
    AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
    STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
    
    Copyright (C) 2010 Apple Inc. All Rights Reserved.
    
*/

// standard includes
#include <AudioToolbox/AudioToolbox.h>

// helpers
#include "CAStreamBasicDescription.h"


/* 

For more information on the importance of interruption handling and Audio Session setup when performing offline
encoding please see the Audio Session Programming Guide.

Offline format conversion requires interruption handling. Specifically, you must handle interruptions at the audio data buffer level.

By way of background, you can use a hardware assisted-codec—on certain devices—to encode linear PCM audio to AAC format.
The codec is available on the iPhone 3GS and on the iPod touch (2nd generation), but not on older models. You use the codec as part
of an audio converter object (of type AudioConverterRef), which in turn is part of an extended audio file object (of type ExtAudioFileRef).
For information on these opaque types, refer to Audio Converter Services Reference and Extended Audio File Services Reference.

To handle an interruption during hardware-assisted encoding, take two things into account:

1. The codec may or may not be able to resume encoding after the interruption ends.
2. The last buffer that you sent to the codec, before the interruption, may or may not have been successfully written to disk.

Encoding takes place as you repeatedly call the ExtAudioFileWrite function with new buffers of audio data. To handle an interruption,
you respond to the function’s result code, as described here:

• kExtAudioFileError_CodecUnavailableInputConsumed — This result code indicates that the last buffer you provided, prior to interruption,
was successfully written to disk. On receiving this result code, stop calling the ExtAudioFileWrite function. If you can resume conversion,
wait for an interruption-ended call from the audio session. In your interruption-end handler, reactivate the session and then resume writing the file.
Because the last buffer (before interruption) was successfully written to disk, proceed by writing the next buffer.

• kExtAudioFileError_CodecUnavailableInputNotConsumed — This result code indicates that the last buffer you provided, prior to interruption, was not
successfully written to disk. Exactly as for the other result code, on receiving this error, stop calling the ExtAudioFileWrite function. If you can
resume conversion, wait for an interruption-ended call from the audio session. In your interruption-end handler, reactivate the session and then resume
writing the file. Here is where your interruption handling differs from the other result code: Because the last buffer (before interruption) was not
successfully written to disk, begin by writing that buffer again.

To check if the AAC codec can resume, obtain the value of the associated converter’s kAudioConverterPropertyCanResumeFromInterruption property.
The value is 1 (can resume) or 0 (cannot resume). You can obtain this value any time after instantiating the converter—immediately after instantiation,
upon interruption, or after interruption ends.

If the converter cannot resume, then on interruption you must abandon the conversion. After the interruption ends, or after the user relaunches your application
and indicates they want to resume conversion, re-instantiate the extended audio file object and perform the conversion again.

*/

#import <iostream.h>

using namespace std;


// ***********************
#pragma mark- Convert
/* The main Audio Conversion function using ExtAudioFile APIs */

// our own error code when we cannot continue from an interruption
enum {
    kMyAudioConverterErr_CannotResumeFromInterruptionError = 'CANT'
};

OSStatus DoConvertFile(CFURLRef sourceURL, CFURLRef destinationURL, OSType outputFormat, Float64 outputSampleRate) 
{
    ExtAudioFileRef sourceFile = 0;
    ExtAudioFileRef destinationFile = 0;
    Boolean         canResumeFromInterruption = true; // we can continue unless told otherwise
    OSStatus        error = noErr;
	OSStatus		err2 = noErr;
    
	cout << "Inside DoConvertFile:" << endl;
//	cout << "Source: " << [[(NSURL *)sourceURL absoluteString] cStringUsingEncoding:1] << endl;
//	cout << "Destination: " << [[(NSURL *)destinationURL absoluteString] cStringUsingEncoding:1] << endl;
	cout << "Output Format: " << outputFormat << endl;
	cout << "Output Sample Rate: " << outputSampleRate << endl;
	
	
    // in this sample we should never be on the main thread here
	//    assert(![NSThread isMainThread]);
    
    // transition thread state to kStateRunning before continuing
	//    ThreadStateSetRunning();
    
    printf("DoConvertFile\n");
    
	//	try {
	CAStreamBasicDescription srcFormat, dstFormat;
	
	// open the source file
	//        XThrowIfError(ExtAudioFileOpenURL(sourceURL, &sourceFile), "ExtAudioFileOpenURL failed");
	err2 = ExtAudioFileOpenURL(sourceURL, &sourceFile);
	
	// get the source data format
	UInt32 size = sizeof(srcFormat);
	//		XThrowIfError(ExtAudioFileGetProperty(sourceFile, kExtAudioFileProperty_FileDataFormat, &size, &srcFormat), "couldn't get source data format");
	err2 = ExtAudioFileGetProperty(sourceFile, kExtAudioFileProperty_FileDataFormat, &size, &srcFormat);
	cout << "Err2 ExtAuxioFileGetProperty: " << err2 << endl;
	
	printf("Source file format: "); srcFormat.Print();
	
	// setup the output file format
	dstFormat.mSampleRate = (outputSampleRate == 0 ? srcFormat.mSampleRate : outputSampleRate); // set sample rate
	if (outputFormat == kAudioFormatLinearPCM) {
		// if PCM was selected as the destination format, create a 16-bit int PCM file format description
		dstFormat.mFormatID = outputFormat; 
		dstFormat.mChannelsPerFrame = srcFormat.NumberChannels();
		dstFormat.mBitsPerChannel = 16;
		dstFormat.mBytesPerPacket = dstFormat.mBytesPerFrame = 2 * dstFormat.mChannelsPerFrame;
		dstFormat.mFramesPerPacket = 1;
		dstFormat.mFormatFlags = kLinearPCMFormatFlagIsPacked | kLinearPCMFormatFlagIsSignedInteger; // little-endian
	} else {
		// compressed format - need to set at least format, sample rate and channel fields for kAudioFormatProperty_FormatInfo
		dstFormat.mFormatID = outputFormat;
		dstFormat.mChannelsPerFrame =  (outputFormat == kAudioFormatiLBC ? 1 : srcFormat.NumberChannels()); // for iLBC num channels must be 1
		
		// use AudioFormat API to fill out the rest of the description
		size = sizeof(dstFormat);
		//            XThrowIfError(AudioFormatGetProperty(kAudioFormatProperty_FormatInfo, 0, NULL, &size, &dstFormat), "couldn't create destination data format");
		err2 = AudioFormatGetProperty(kAudioFormatProperty_FormatInfo, 0, NULL, &size, &dstFormat);
		cout << "Err2 AudioFormatGetProperty: " << err2 << endl;
	}
	
	cout << "Source Format:" << endl;
	cout << "Sample Rate " << srcFormat.mSampleRate << endl;
	cout << "formatID " << srcFormat.mFormatID << endl;
	cout << "channelsPerframe " << srcFormat.mChannelsPerFrame << endl;
	cout << "bitsPerChannel " << srcFormat.mBitsPerChannel << endl;
	cout << "bytesPerPacket " << srcFormat.mBytesPerPacket << endl;
	cout << "bytesPerFrame " << srcFormat.mBytesPerFrame << endl;
	cout << "framesPerPacket " << srcFormat.mFramesPerPacket << endl;
	cout << "formatFlags "	<< srcFormat.mFormatFlags << endl;
	
	cout << endl;
	
	cout << "Destination Format:" << endl;
	cout << "SampleRate " << dstFormat.mSampleRate << endl;
	cout << "formatID " << dstFormat.mFormatID << endl;
	cout << "channelsPerframe " << dstFormat.mChannelsPerFrame << endl;
	cout << "bitsPerChannel " << dstFormat.mBitsPerChannel << endl;
	cout << "bytesPerPacket " << dstFormat.mBytesPerPacket << endl;
	cout << "bytesPerFrame " << dstFormat.mBytesPerFrame << endl;
	cout << "framesPerPacket " << dstFormat.mFramesPerPacket << endl;
	cout << "formatFlags "	<< dstFormat.mFormatFlags << endl;
	
	printf("Destination file format: "); dstFormat.Print();
	
	// create the destination file 
	//      XThrowIfError(ExtAudioFileCreateWithURL(destinationURL, kAudioFileCAFType, &dstFormat, NULL, kAudioFileFlags_EraseFile, &destinationFile), "ExtAudioFileCreateWithURL failed!");
	err2 = ExtAudioFileCreateWithURL(destinationURL, kAudioFileCAFType, &dstFormat, NULL, kAudioFileFlags_EraseFile, &destinationFile);
	cout << "Err2 ExtAuxioFileCreateWithURL: " << err2 << endl;
	
	// set the client format - The format must be linear PCM (kAudioFormatLinearPCM)
	// You must set this in order to encode or decode a non-PCM file data format
	// You may set this on PCM files to specify the data format used in your calls to read/write
	CAStreamBasicDescription clientFormat;
	if (outputFormat == kAudioFormatLinearPCM) {
		clientFormat = dstFormat;
	} else {
		clientFormat.SetCanonical(srcFormat.NumberChannels(), true);
		clientFormat.mSampleRate = srcFormat.mSampleRate;
	}
	
	size = sizeof(clientFormat);
	//      XThrowIfError(ExtAudioFileSetProperty(sourceFile, kExtAudioFileProperty_ClientDataFormat, size, &clientFormat), "couldn't set source client format");
	err2 = ExtAudioFileSetProperty(sourceFile, kExtAudioFileProperty_ClientDataFormat, size, &clientFormat);
	cout << "Err2 ExtAudioFileSetProperty: " << err2 << endl;
	
	size = sizeof(clientFormat);
	//      XThrowIfError(error = ExtAudioFileSetProperty(destinationFile, kExtAudioFileProperty_ClientDataFormat, size, &clientFormat), "couldn't set destination client format");
	err2 = ExtAudioFileSetProperty(destinationFile, kExtAudioFileProperty_ClientDataFormat, size, &clientFormat);
	cout << "Err2 ExtAuxioFileSetProperty: " << err2 << endl;
	
	cout << "Client Format:" << endl;
	cout << "formatID " << clientFormat.mFormatID << endl;
	cout << "channelsPerframe " << clientFormat.mChannelsPerFrame << endl;
	cout << "bitsPerChannel " << clientFormat.mBitsPerChannel << endl;
	cout << "bytesPerPacket " << clientFormat.mBytesPerPacket << endl;
	cout << "bytesPerFrame " << clientFormat.mBytesPerFrame << endl;
	cout << "framesPerPacket " << clientFormat.mFramesPerPacket << endl;
	cout << "formatFlags "	<< clientFormat.mFormatFlags << endl;
	
	printf("Error is :%d\n", error);
	// can the audio converter (which in this case is owned by an ExtAudioFile object) resume conversion after an interruption?
	AudioConverterRef audioConverter;
	
	size = sizeof(audioConverter);
	//      XThrowIfError(ExtAudioFileGetProperty(destinationFile, kExtAudioFileProperty_AudioConverter, &size, &audioConverter), "Couldn't get Audio Converter!");
	err2 = ExtAudioFileGetProperty(destinationFile, kExtAudioFileProperty_AudioConverter, &size, &audioConverter);
	cout << "Err2 ExtAudioFileGetProperty: " << err2 << endl;
	
	// this property may be queried at any time after construction of the audio converter (which in this case is owned by an ExtAudioFile object)
	// after setting the output format -- there's no clear reason to prefer construction time, interruption time, or potential resumption time but we prefer
	// construction time since it means less code to execute during or after interruption time
	UInt32 canResume = 0;
	size = sizeof(canResume);
	error = AudioConverterGetProperty(audioConverter, kAudioConverterPropertyCanResumeFromInterruption, &size, &canResume);
	cout << "Error AudioConverterGetProperty: " << error << endl;
	if (noErr == error) {
		// we recieved a valid return value from the GetProperty call
		// if the property's value is 1, then the codec CAN resume work following an interruption
		// if the property's value is 0, then interruptions destroy the codec's state and we're done
		
		if (0 == canResume) canResumeFromInterruption = false;
		
		printf("Audio Converter %s continue after interruption!\n", (canResumeFromInterruption == 0 ? "CANNOT" : "CAN"));
	} else {
		// if the property is unimplemented (kAudioConverterErr_PropertyNotSupported, or paramErr returned in the case of PCM),
		// then the codec being used is not a hardware codec so we're not concerned about codec state
		// we are always going to be able to resume conversion after an interruption
		
		if (kAudioConverterErr_PropertyNotSupported == error) {
			printf("kAudioConverterPropertyCanResumeFromInterruption property not supported!\n");
		} else {
			printf("AudioConverterGetProperty kAudioConverterPropertyCanResumeFromInterruption result %ld\n", error);
		}
		
		error = noErr;
	}
	
	// set up buffers
	UInt32 bufferByteSize = 32768;
	char srcBuffer[bufferByteSize];
	
	// keep track of the source file offset so we know where to reset the source for
	// reading if interrupted and input was not consumed by the audio converter
	SInt64 sourceFrameOffset = 0;
	
	//***** do the read and write - the conversion is done on and by the write call *****//
	printf("Converting...\n");
	while (1) {
        
		AudioBufferList fillBufList;
		fillBufList.mNumberBuffers = 1;
		fillBufList.mBuffers[0].mNumberChannels = clientFormat.NumberChannels();
		fillBufList.mBuffers[0].mDataByteSize = bufferByteSize;
		fillBufList.mBuffers[0].mData = srcBuffer;
		
		// client format is always linear PCM - so here we determine how many frames of lpcm
		// we can read/write given our buffer size
		UInt32 numFrames = clientFormat.BytesToFrames(bufferByteSize); // (bufferByteSize / clientFormat.mBytesPerFrame);
		
		//            XThrowIfError(ExtAudioFileRead(sourceFile, &numFrames, &fillBufList), "ExtAudioFileRead failed!");	
		err2 = ExtAudioFileRead(sourceFile, &numFrames, &fillBufList);	
		cout << "Err2 ExtAudioFileRead: " << err2 << endl;
		
		if (!numFrames) {
			// this is our termination condition
			error = noErr;
			std::cout << "Terminating conversion. SourceFrameOffset " << sourceFrameOffset << " numFrames: " << numFrames << std::endl;
			break;
		}
		std::cout << "Doing conversion. SourceFrameOffset " << sourceFrameOffset << " numFrames: " << numFrames << std::endl;
		sourceFrameOffset += numFrames;
		
		// this will block if we're interrupted
		//            Boolean wasInterrupted = ThreadStatePausedCheck();
		
		//            if ((error || wasInterrupted) && (false == canResumeFromInterruption)) {
		// this is our interruption termination condition
		// an interruption has occured but the audio converter cannot continue
		//                error = kMyAudioConverterErr_CannotResumeFromInterruptionError;
		//                break;
		//            }
		
		error = ExtAudioFileWrite(destinationFile, numFrames, &fillBufList);
		// if interrupted in the process of the write call, we must handle the errors appropriately
		if (error) {
			if (kExtAudioFileError_CodecUnavailableInputConsumed == error) {
                
				printf("ExtAudioFileWrite kExtAudioFileError_CodecUnavailableInputConsumed error %ld\n", error);
				
				/*
				 Returned when ExtAudioFileWrite was interrupted. You must stop calling
				 ExtAudioFileWrite. If the underlying audio converter can resume after an
				 interruption (see kAudioConverterPropertyCanResumeFromInterruption), you must
				 wait for an EndInterruption notification from AudioSession, and call AudioSessionSetActive(true)
				 before resuming. In this situation, the buffer you provided to ExtAudioFileWrite was successfully
				 consumed and you may proceed to the next buffer
				 */
				
			} else if (kExtAudioFileError_CodecUnavailableInputNotConsumed == error) {
                
				printf("ExtAudioFileWrite kExtAudioFileError_CodecUnavailableInputNotConsumed error %ld\n", error);
				
				/*
				 Returned when ExtAudioFileWrite was interrupted. You must stop calling
				 ExtAudioFileWrite. If the underlying audio converter can resume after an
				 interruption (see kAudioConverterPropertyCanResumeFromInterruption), you must
				 wait for an EndInterruption notification from AudioSession, and call AudioSessionSetActive(true)
				 before resuming. In this situation, the buffer you provided to ExtAudioFileWrite was not
				 successfully consumed and you must try to write it again
				 */
				
				// seek back to last offset before last read so we can try again after the interruption
				sourceFrameOffset -= numFrames;
				//                    XThrowIfError(ExtAudioFileSeek(sourceFile, sourceFrameOffset), "ExtAudioFileSeek failed!");
				err2 = ExtAudioFileSeek(sourceFile, sourceFrameOffset);
				cout << "Err2 ExtAudioFileSeek: " << err2 << endl;
				
			} else {
				cout << "Ha entrat aquí, amb error " << error << endl;
				//                    XThrowIfError(error, "ExtAudioFileWrite error!");
			}
		} // if
	} // while
	//	}
	//    catch (CAXException e) {
	//		char buf[256];
	//		fprintf(stderr, "Error: %s (%s)\n", e.mOperation, e.FormatError(buf));
	//      error = e.mError;
	//	}
    
    // close
    if (destinationFile) ExtAudioFileDispose(destinationFile);
    if (sourceFile) ExtAudioFileDispose(sourceFile);
	
    // transition thread state to kStateDone before continuing
	//    ThreadStateSetDone();
    
    return error;
}
