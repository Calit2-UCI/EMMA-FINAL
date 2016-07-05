
// standard includes
#include <AudioToolbox/AudioToolbox.h>

// helpers
#include "CAStreamBasicDescription.h"
#import <iostream>

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
    
    printf("DoConvertFile\n");

	CAStreamBasicDescription srcFormat, dstFormat;
	
	// open the source file
	err2 = ExtAudioFileOpenURL(sourceURL, &sourceFile);
	if (err2 != noErr)
		cout << "ExtAuxioFileOpenURL Error"	<< err2 << endl;
	
	// get the source data format
	UInt32 size = sizeof(srcFormat);
	err2 = ExtAudioFileGetProperty(sourceFile, kExtAudioFileProperty_FileDataFormat, &size, &srcFormat);
	if (noErr != err2)
		cout << "ExtAudioFileGetProperty Error: " << err2 << endl;
	
//	printf("Source file format: "); srcFormat.Print();
	
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
		err2 = AudioFormatGetProperty(kAudioFormatProperty_FormatInfo, 0, NULL, &size, &dstFormat);
		if (err2 != noErr)
			cout << "Err2 AudioFormatGetProperty: " << err2 << endl;
	}
	
//	printf("Destination file format: "); dstFormat.Print();
	
	// create the destination file 
	err2 = ExtAudioFileCreateWithURL(destinationURL, kAudioFileCAFType, &dstFormat, NULL, kAudioFileFlags_EraseFile, &destinationFile);
	if (err2 != noErr) 
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
	err2 = ExtAudioFileSetProperty(sourceFile, kExtAudioFileProperty_ClientDataFormat, size, &clientFormat);
	if (err2 != noErr)
		cout << "Err2 ExtAudioFileSetProperty: " << err2 << endl;
	
	size = sizeof(clientFormat);
	err2 = ExtAudioFileSetProperty(destinationFile, kExtAudioFileProperty_ClientDataFormat, size, &clientFormat);
	if (err2 != noErr)
		cout << "Err2 ExtAudioFileSetProperty: " << err2 << endl;
	
	// can the audio converter (which in this case is owned by an ExtAudioFile object) resume conversion after an interruption?
	AudioConverterRef audioConverter;
	
	size = sizeof(audioConverter);
	err2 = ExtAudioFileGetProperty(destinationFile, kExtAudioFileProperty_AudioConverter, &size, &audioConverter);
	if (err2 != noErr)
		cout << "Err2 ExtAudioFileGetProperty: " << err2 << endl;
	
	// this property may be queried at any time after construction of the audio converter (which in this case is owned by an ExtAudioFile object)
	// after setting the output format -- there's no clear reason to prefer construction time, interruption time, or potential resumption time but we prefer
	// construction time since it means less code to execute during or after interruption time
	UInt32 canResume = 0;
	size = sizeof(canResume);
	error = AudioConverterGetProperty(audioConverter, kAudioConverterPropertyCanResumeFromInterruption, &size, &canResume);
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
		if (err2 != noErr)
			cout << "Err2 ExtAudioFileRead: " << err2 << endl;
		
		if (!numFrames) {
			// this is our termination condition
			error = noErr;
			cout << "Terminating conversion. SourceFrameOffset " << sourceFrameOffset << " numFrames: " << numFrames << endl;
			break;
		}
		cout << "Doing conversion. SourceFrameOffset " << sourceFrameOffset << " numFrames: " << numFrames << endl;
		sourceFrameOffset += numFrames;
		
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
				err2 = ExtAudioFileSeek(sourceFile, sourceFrameOffset);
				if (err2 != noErr)
					cout << "Err2 ExtAudioFileSeek: " << err2 << endl;
				
			} else {
				cout << "Ha entrat aquí, amb error " << error << endl;
			}
		} // if
	} // while
    
    // close
    if (destinationFile) ExtAudioFileDispose(destinationFile);
    if (sourceFile) ExtAudioFileDispose(sourceFile);
    
    return error;
}