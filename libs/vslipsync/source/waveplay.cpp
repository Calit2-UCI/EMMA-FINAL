

//----------------------------------------------------------------------------
// File: wavePlay.cpp
//
// You must link with winmm.lib. If using Visual C++, go to Build->Settings. Flip to the Link page,
//and add winmm.lib to the library/object modules.
//
//This app shows how to use the Low Level Wave API to play the WAVE file.
//It also uses the MMIO (RIFF) File Parsing functions.
//
//-----------------------------------------------------------------------------

#include "wavePlay.h"
#include "LipSync.h"
#include "CVisemes.h"
#include "AudioStreamLipSync.hpp"
#include "SilentStreamLipSync.hpp"


//#ifdef WIN32
//#include "mWave.h"
//#endif

// C++
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <time.h>
#include <iostream>

// Windows
#ifdef WIN32
	#include <windows.h>
	#include <conio.h>
	//#include <mmsystem.h>
	#include <malloc.h>
	#include <process.h>
#endif


namespace VisageSDK
{


//-----------------------------------------------------------------------------
// Defines, constants, and global variables - P L A Y
//-----------------------------------------------------------------------------

#ifdef WIN32
/* Name of the WAVE file */
LPSTR		WaveName;

/* WAVEFORMATEX structure for reading in the WAVE fmt chunk */
WAVEFORMATEX WaveFormat;

/* file handle for open file */
HMMIO		HMmio;

/* subchunk information structure for reading WAVE file */
MMCKINFO    MmckinfoSubchunk;

/* Count of waveform data still to play */
DWORD		WaveDataPlayed;

/* Handle of opened WAVE Out device */
HWAVEOUT	HWaveOut;

/* The size of each WAVEHDR's wave buffer */
DWORD		WaveBufSize;

/* 2 WAVEHDR structures (for double-buffered playback of WAVE) */
WAVEHDR		WaveHeader[8];

/* Ptr to last WAVEHDR played */
WAVEHDR *	LastHdr;

/* Event used by callback to signal main thread */
HANDLE		Event;

#endif


//samples->visemes calculation...
CVisemes*	WaveCV;
VisageLipSync *lipSync;

//length of audio file 
double		totalTime = 0;
//total number of samples in audio file
int			totalSamples = 0;
//total number of visemes that can be calculated from audio file
int			NumFrames = 0;
int			NumVisemes = 0;
int			stopPlaying = 0;

//-----------------------------------------------------------------------------
// Defines, constants, and global variables - R E C O R D
//-----------------------------------------------------------------------------

#ifdef WIN32
/* Handle to the WAVE In Device */
HWAVEIN		WaveInHandle;

/* Handle to Mixer for WAVE In Device */
HMIXER		MixerHandle;

/* Handle to the disk file where we permanently store the recorded audio data */
HANDLE		WaveFileHandle = INVALID_HANDLE_VALUE;

/* We use two WAVEHDR's for recording (ie, double-buffering) in this example */
WAVEHDR		WaveHeaderRecord[4];

/* Variable used to indicate whether we are in record */
BOOL		InRecord = FALSE;

#endif


/* Variable used by recording thread to indicate whether we are in record */
unsigned char	DoneAll;

//total time of recording 
time_t   startRecord, finishRecord;
double   recordingTime;

//size in bytes
long	 recordedSize = 0;
//total number of samples recorded
int		 nSamplesRecorded = 0;
int		 bytesPerSample = 1;

int		playWaveCount;

//here we store recorded samples, bytes so that we can store them later in audio file
RECORDEDBYTES  recordedBytes;
RECORDEDBYTES::iterator recordedBytesIterator;

// here we store pointer to header data from audio file (used in function HandleFramesInThread)
//WAVEHDR * waveHeaderPtr;

// flag used for checking whether thread that handles frames needs to end 
//bool killThread;

// flag that indicates whether the waveHeaderPtr is updated
//bool isUpdated = true;
// flag used for checking whether thread that handles frames has ended 
//bool endOfHThread;

bool finished;

// here we store pointer to header data from microphone file (used in function HandleFramesInThread)
//WAVEHDR * recordHeaderPtr;

int stopRecording = 0;


/* ******************************** **** ******************************** */



//#ifndef WIN32_OLD
//
//void * playText(void * args)
//		{
//			std::string *textToSay = (std::string*)args;
//			std::cout << *textToSay << std::endl;
//			AudioStreamVerbioTTS* my_audio = new AudioStreamVerbioTTS(lipSync, 512, *textToSay, lipSync->strFileName);
//
//
//			my_audio->playWave();
//
//			lipSync->bPlayOver = 1;
//
//			delete my_audio;
//			delete textToSay;
//
//			std::cout << "AudioStreamLipSync deleted" << std::endl;
//
//
//
//			lipSync->stopSync();
//
//#ifdef WIN32
//
//#else
//			pthread_exit(NULL);
//#endif
//
//		}
//
//#endif



    
    
    
    
#ifndef WIN32_OLD
    
    void * readWavFile(void * args)
    {
        bool *arguments = (bool*)args;
        bool mute = arguments[0];
        bool sequential = arguments[1];
        
        SilentStreamLipSync* my_audio = new SilentStreamLipSync(lipSync->strFileName, lipSync, 512);
        free(arguments);
        
        std::cout << "freq: " << my_audio->getFrequency() << std::endl;
        std::cout << "bytes per frame: " << my_audio->getBytesPerFrame() << std::endl;
        std::cout << "size: " << my_audio->getSize() << std::endl;			
        std::cout << "num samples: " << my_audio->getSize() / my_audio->getBytesPerFrame() << std::endl;	
        
        
        totalTime = (float) my_audio->getSize() / ( my_audio->getFrequency() * my_audio->getBytesPerFrame());
        totalSamples = (int) (floor((double)my_audio->getSize() / my_audio->getBytesPerFrame()));
        NumFrames=lipSync->bNumberOfFrames; //MIRAR DE DONDE SALE
        NumVisemes = (totalSamples/(256*NumFrames))+1;
        
        lipSync->fileTime = (float)totalTime;
        lipSync->fileSamples = totalSamples;
        lipSync->nVisemes = NumVisemes;
        
        my_audio->doLipSync();
        
        delete my_audio;
        
        std::cout << "AudioStreamLipSync deleted" << std::endl;
        
        lipSync->bPlayOver = 1;
        
        lipSync->stopSync();
        
#ifdef WIN32
        
#else
        pthread_exit(NULL);
#endif
        
}

#endif

    
    
    
    

#ifndef WIN32_OLD

void * playWavFile(void * args)
		{
			bool *arguments = (bool*)args;
			bool mute = arguments[0];
			bool sequential = arguments[1];
			
			AudioStreamLipSync* my_audio = new AudioStreamLipSync(lipSync->strFileName, lipSync, 512/*256*/, mute, sequential);
            std::cout << "filename: " << lipSync->strFileName << std::endl;
			free(arguments);
						
			std::cout << "freq: " << my_audio->getFrequency() << std::endl;
			std::cout << "bytes per frame: " << my_audio->getBytesPerFrame() << std::endl;
			std::cout << "size: " << my_audio->getSize() << std::endl;			
			std::cout << "num samples: " << my_audio->getSize() / my_audio->getBytesPerFrame() << std::endl;	


			totalTime = (float) my_audio->getSize() / ( my_audio->getFrequency() * my_audio->getBytesPerFrame());
			totalSamples = (int) (floor((double)my_audio->getSize() / my_audio->getBytesPerFrame()));
			NumFrames=lipSync->bNumberOfFrames; //MIRAR DE DONDE SALE
			NumVisemes = (totalSamples/(256*NumFrames))+1;

			lipSync->fileTime = (float)totalTime;
			lipSync->fileSamples = totalSamples;
			lipSync->nVisemes = NumVisemes;

			my_audio->playWave();

			delete my_audio;
			
			std::cout << "AudioStreamLipSync deleted" << std::endl;

			lipSync->bPlayOver = 1;
			
			lipSync->stopSync();

#ifdef WIN32

#else
			pthread_exit(NULL);
#endif

		}




#else
//int PlayWavFile(char* audiofile)
DWORD WINAPI PlayWavFile(LPVOID arg)
{
	MMCKINFO	mmckinfoParent;		/* parent chunk information structure */
//	WaveName = audiofile;
	WaveName = lipSync->strFileName;


	/* Allocate an Event signal */
	if ((Event = CreateEvent(0, FALSE, FALSE, 0)))//pthread_cond_init
	{
		/* Open the file for reading */
		if ((HMmio = mmioOpen(&WaveName[0], 0, MMIO_READ)))
		{
			/*	Tell Windows to locate a WAVE FileType chunk header somewhere in the file.
				This marks the start of any embedded WAVE format within the file */
			mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E'); 
			if (mmioDescend(HMmio, (LPMMCKINFO)&mmckinfoParent, 0, MMIO_FINDRIFF)) 
			{
				/* Oops! No embedded WAVE format within this file */
				printf("ERROR: This file doesn't contain a WAVE!\n");

				/* Close the file and exit with error */
out:			mmioClose(HMmio, 0);

				goto out2;
			} 
 
			/* Tell Windows to locate the WAVE's "fmt " chunk, and read in its size field */
			MmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' '); 
			if (mmioDescend(HMmio, &MmckinfoSubchunk, &mmckinfoParent, MMIO_FINDCHUNK)) 
			{
				/* Oops! The required fmt chunk was not found! */
				printf("ERROR: Required fmt chunk was not found!\n");
				goto out;
			}
 
			/* Tell Windows to read in the "fmt " chunk into our WAVEFORMATEX structure */
			if (mmioRead(HMmio, (HPSTR)&WaveFormat, MmckinfoSubchunk.cksize) != (LRESULT)MmckinfoSubchunk.cksize)
			{
				/* Oops! */
				printf("ERROR: reading the fmt chunk!\n");
				goto out;
			}
 
			/*	Ascend out of the "fmt " subchunk. If you plan to parse any other chunks in the file, you need to
				"ascend" out of any chunk that you've mmioDescend()'ed into */
			mmioAscend(HMmio, &MmckinfoSubchunk, 0); 

			/*	Tell Windows to locate the data chunk. Upon return, the file
				pointer will be ready to read in the actual waveform data within
				the data chunk */
			MmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a'); 
			if (mmioDescend(HMmio, &MmckinfoSubchunk, &mmckinfoParent, MMIO_FINDCHUNK)) 
			{
				/* Oops! */
				printf("ERROR: reading the data chunk!\n");
				goto out;
			}
 
			/* Set how many total bytes to play */
			WaveDataPlayed = MmckinfoSubchunk.cksize;


			//--G--
			/////////////////////////
			//total time in seconds
			totalTime = (float) WaveDataPlayed / WaveFormat.nAvgBytesPerSec;
			totalSamples = (int) (floor((double)WaveDataPlayed / WaveFormat.nBlockAlign));
			NumFrames=lipSync->bNumberOfFrames;
			//NumVisemes = (int)floor(totalSamples/(256*NumFrames))+1;
			NumVisemes = (totalSamples/(256*NumFrames))+1;	//CHANGED BY I.K. floor() redundant

			lipSync->fileTime = (float)totalTime;	//CHANGED BY I.K. added explicit cast
			lipSync->fileSamples = totalSamples;
			lipSync->nVisemes = NumVisemes;
			//////////////////////////////////////////////////
			playWaveCount = 0;
		
			/* Play the wave */
			finished = true;
 			PlayWave();

			//if ((lipSync->bFbaFile)& (stopPlaying) &(lipSync->h_heMode == LS_HE_MODE_VISEME_BASED))
			if ((lipSync->bFbaFile) &(lipSync->h_heMode == LS_HE_MODE_VISEME_BASED))
			{
				//save animation in fba file
				WaveCV->FinalizeStoreAnimation(); //added by G.
			}
			
	//		WaveCV->ClearAll();

			//lipSync->active = 0;
			//lipSync->stop_flag = 0;
				
			/* Close the file */
			mmioClose(HMmio, 0);

			/* Free the Event */
			CloseHandle(Event);//pthread_cond_destroy

			lipSync->bPlayOver = 1;
			WaveCV->FreeAllGeneratedNNetworks();

			/* Success */
			return(0);
		}

		/* Oops! */
		printf("ERROR: Can't find the WAVE file!\n");

		/* Free the Event */
out2:	CloseHandle(Event);//pthread_cond_destroy

		return(-2);
	}
	/* Oops! */
	printf("ERROR: Can't allocate Event Semaphore!\n");
	return(-1);
}




/* ************************** QueueWaveData() *****************************
 * Queues a buffer worth of data for the passed WAVEHDR. This is called
 * by my callback WaveOutProc().
 *
 * Returns a 0 for playback to continue. A -1 if an error. A 1 if the
 * wave has finished playing back and no more buffers need to be queued.
 */

long QueueWaveData(WAVEHDR * waveHeader)
{
	/* More WAVE data to be queued? */
	

	if (WaveDataPlayed)
	{
		/* Only a partial block left? */
		if (WaveDataPlayed < WaveBufSize)
		{
			/* Tell Windows to read the remaining waveform data into our allocated memory */
			if(mmioRead(HMmio, (HPSTR)waveHeader->lpData, WaveDataPlayed) != (long)WaveDataPlayed) goto bad;

			/* Set the length field to remaining amount */
			waveHeader->dwBufferLength = WaveDataPlayed;

			/* Store last WAVEHDR */
			LastHdr = waveHeader;

			/* Indicate done */
			WaveDataPlayed = 0;

			goto good;
		}
		
		/* Tell Windows to read another full block of waveform data into our allocated memory */
		if(mmioRead(HMmio, (HPSTR)waveHeader->lpData, WaveBufSize) != (long)WaveBufSize)
		{
bad:		/* Stop playback */
			waveOutPause(HWaveOut);

			/* Oops! */
			printf("ERROR: reading WAVE data!\n");

			return(-1);
		}

		//ADDAD BY G. to stop audio from file
		stopPlaying = lipSync->stop_flag;
		if (stopPlaying)
		{
			// Store last WAVEHDR 
			LastHdr = waveHeader;
			WaveDataPlayed = 0;
			
		} else
		{
			playWaveCount++;
			//FILE *testFP = fopen("saveFBA.txt", "a+");
			//fprintf(testFP,"WaveDataPlayed = %d, playWaveCount = %d\n", WaveDataPlayed, playWaveCount);
			//fclose(testFP);
			
			// Decrease # of bytes yet to play 
			WaveDataPlayed -= WaveBufSize;
		}

		//WaveDataPlayed -= WaveBufSize;

good:	/* Clear the WHDR_DONE bit (which the driver set last time that
		   this WAVEHDR was sent via waveOutWrite and was played). Some
		   drivers need this to be cleared */
		waveHeader->dwFlags &= ~WHDR_DONE;
		
		//--G--
		//calculating visemes or just storing them or playing...
		WaveCV = lipSync->cvis;

		//FILE *testFP = fopen("saveFBA.txt", "a+");
		//fprintf(testFP,"playWaveCount = %d, finished = %d\n", playWaveCount, finished);
		//fclose(testFP);

		//addeg by G.
		if ((playWaveCount>4) & (finished))//s ovom linijom osisguramo da u frame based modu rada sustav radi ok - jel to kriticni dio, pocetak animacije?
			WaveCV->HandleFrames(WaveBufSize, (short*)waveHeader->lpData, &finished);
	
		
		/* Queue the WAVEHDR */
		waveOutWrite(HWaveOut, waveHeader, sizeof(WAVEHDR));


		/* Allow playback to continue */
		return(0);
	}

	/* Did the last WAVEHDR get a chance to play? */
	if (waveHeader == LastHdr)
	{
		/* Finished playing all WAVE data */
		WaveCV->LastFrameRender();
		return(1);
	}

	/* Allow playback to continue */
	return(0);
}
/* ************************** WaveOutProc() *****************************
 * My WAVE callback. This is called by Windows every time that some event
 * needs to be handled by my program.
 */

void CALLBACK WaveOutProc(HWAVEOUT waveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	/* Has a buffer finished playing? */
	if (uMsg == MM_WOM_DONE)
	{
		/* NOTE: This works upon Windows 95/98/ME, but Windows NT/2000/XP do
		 * not seem to like certain operating system functions called
		 * from within a callback. One such function is waveOutWrite().
		 * If designing your app for Win NT/2000 then you should either
		 * use a different method than a callback handler (such as the
		 * Window Message method), or have your callback signal another
		 * thread to do the below queuing of the next buffer. I recommend
		 * the Windows Message method.
		 */

		/* Queue another buffer */
		if (QueueWaveData((WAVEHDR *)dwParam1))
		{
			/* If no more buffers, notify PlayWave() that the WAVE is done playing */
			SetEvent(Event);//pthread_cond_signal
		}
		
	}
}

/* ************************** PlayWave() *****************************
 * Plays the WAVE. It is presumed that HMmio is an open MultiMedia File
 * handle, currently pointing to the start of the waveform data.
 * WaveDataPlayed is set to the total number of bytes in the waveform.
 * The WAVEFORMATEX structure (ie, WaveFormat) has already been filled
 * in.
 */

void PlayWave() 
{
	//DWORD	err;
	unsigned long err;

	/* Open the default WAVE Out Device, specifying my callback */
	if ((err = waveOutOpen(&HWaveOut, WAVE_MAPPER, &WaveFormat, (DWORD)WaveOutProc, 0, CALLBACK_FUNCTION)))
	{
		printf("ERROR: Can't open WAVE Out Device! -- %08X\n", err);
		return;
	}

	/* I'll allocate 2 buffers large enough to hold 2 seconds worth of waveform data. NOTE: Just to make it
	   easy, I'll use 1 call to VirtualAlloc to allocate both buffers, but you can use 2 separate calls since
	   the buffers do NOT need to be contiguous */

	//stavljeno na 4 buffera jer je s 2 "usporen" - zvuk se rasteze???
	//G. vista update - ponovo usporeno, rasteze, probati sa 8

	//--G--
	//sound - 256 samples of data
	WaveBufSize = 256*WaveFormat.nBlockAlign; //in PCM --> snBlockAlign = nChannels*(bitsPerSample/8)
	//WaveBufSize = WaveFormat.nAvgBytesPerSec << 1;

	if (!(WaveHeader[0].lpData = (char *)VirtualAlloc(0, WaveBufSize*8, MEM_COMMIT, PAGE_READWRITE)))
	{
		printf("ERROR: Can't allocate memory for WAVE buffer!\n");
		goto bad1;
	}
	/* Fill in WAVEHDR fields for buffer starting address and size */
	WaveHeader[1].lpData = WaveHeader[0].lpData + WaveBufSize;
	WaveHeader[1].dwBufferLength = WaveHeader[0].dwBufferLength = WaveBufSize;
	/* Fill in WAVEHDR fields for buffer starting address and size */
	WaveHeader[2].lpData = WaveHeader[1].lpData + WaveBufSize;
	WaveHeader[2].dwBufferLength = WaveHeader[1].dwBufferLength = WaveBufSize;
	/* Fill in WAVEHDR fields for buffer starting address and size */
	WaveHeader[3].lpData = WaveHeader[2].lpData + WaveBufSize;
	WaveHeader[3].dwBufferLength = WaveHeader[2].dwBufferLength = WaveBufSize;
	/* Fill in WAVEHDR fields for buffer starting address and size */
	WaveHeader[4].lpData = WaveHeader[3].lpData + WaveBufSize;
	WaveHeader[4].dwBufferLength = WaveHeader[3].dwBufferLength = WaveBufSize;
	/* Fill in WAVEHDR fields for buffer starting address and size */
	WaveHeader[5].lpData = WaveHeader[4].lpData + WaveBufSize;
	WaveHeader[5].dwBufferLength = WaveHeader[4].dwBufferLength = WaveBufSize;
	/* Fill in WAVEHDR fields for buffer starting address and size */
	WaveHeader[6].lpData = WaveHeader[5].lpData + WaveBufSize;
	WaveHeader[6].dwBufferLength = WaveHeader[5].dwBufferLength = WaveBufSize;
	/* Fill in WAVEHDR fields for buffer starting address and size */
	WaveHeader[7].lpData = WaveHeader[6].lpData + WaveBufSize;
	WaveHeader[7].dwBufferLength = WaveHeader[6].dwBufferLength = WaveBufSize;

	/* Leave other WAVEHDR fields at 0 */

	/* Prepare the 2 WAVEHDR's */
	if ((err = waveOutPrepareHeader(HWaveOut, &WaveHeader[0], sizeof(WAVEHDR))))
	{
		printf("ERROR: preparing WAVEHDR 1! -- %08X\n", err);
		goto bad2;
	}

	if ((err = waveOutPrepareHeader(HWaveOut, &WaveHeader[1], sizeof(WAVEHDR))))
	{		
		printf("ERROR: preparing WAVEHDR 2! -- %08X\n", err);
		goto bad3;
	}

	if ((err = waveOutPrepareHeader(HWaveOut, &WaveHeader[2], sizeof(WAVEHDR))))
	{		
		printf("ERROR: preparing WAVEHDR 3! -- %08X\n", err);
		goto bad4;
	}

	if ((err = waveOutPrepareHeader(HWaveOut, &WaveHeader[3], sizeof(WAVEHDR))))
	{		
		printf("ERROR: preparing WAVEHDR 4! -- %08X\n", err);
		goto bad5;
	}

	if ((err = waveOutPrepareHeader(HWaveOut, &WaveHeader[4], sizeof(WAVEHDR))))
	{		
		printf("ERROR: preparing WAVEHDR 4! -- %08X\n", err);
		goto bad6;
	}

	if ((err = waveOutPrepareHeader(HWaveOut, &WaveHeader[5], sizeof(WAVEHDR))))
	{		
		printf("ERROR: preparing WAVEHDR 4! -- %08X\n", err);
		goto bad7;
	}
	if ((err = waveOutPrepareHeader(HWaveOut, &WaveHeader[6], sizeof(WAVEHDR))))
	{		
		printf("ERROR: preparing WAVEHDR 4! -- %08X\n", err);
		goto bad8;
	}
	if ((err = waveOutPrepareHeader(HWaveOut, &WaveHeader[7], sizeof(WAVEHDR))))
	{		
		printf("ERROR: preparing WAVEHDR 4! -- %08X\n", err);
		goto bad9;
	}

	/* Queue first buffer (it starts playing because device isn't paused) */
	if (!QueueWaveData(&WaveHeader[0]))
	{
		/* Queue second buffer */
		if (!QueueWaveData(&WaveHeader[1]))
		{
			/* Queue third buffer */
			if (!QueueWaveData(&WaveHeader[2]))
			{
				/* Queue third buffer */
				if (!QueueWaveData(&WaveHeader[3]))
				{
					/* Queue third buffer */
					if (!QueueWaveData(&WaveHeader[4]))
					{
						if (!QueueWaveData(&WaveHeader[5]))
						{
							if (!QueueWaveData(&WaveHeader[6]))
							{
								if (QueueWaveData(&WaveHeader[7]) >= 0)
								{
									// Wait for playback to finish. My callback notifies me when all wave data has been played */
									WaitForSingleObject(Event, INFINITE); //pthread_cond_wait
								}
							}
						}
					}
				}
			}
		}
	}

	//killThread = true;

	lipSync->stop_flag = 1;
	
	waveOutReset(HWaveOut);

	// Free WAVE buffers
	VirtualFree(WaveHeader[0].lpData, WaveBufSize*8, MEM_FREE);

	// Unprepare WAVE buffers
bad9:
	waveOutUnprepareHeader(HWaveOut, &WaveHeader[7], sizeof(WAVEHDR));
bad8:
	waveOutUnprepareHeader(HWaveOut, &WaveHeader[6], sizeof(WAVEHDR));
bad7:
	waveOutUnprepareHeader(HWaveOut, &WaveHeader[5], sizeof(WAVEHDR));
bad6:
	waveOutUnprepareHeader(HWaveOut, &WaveHeader[4], sizeof(WAVEHDR));
bad5:
	waveOutUnprepareHeader(HWaveOut, &WaveHeader[3], sizeof(WAVEHDR));
bad4:
	waveOutUnprepareHeader(HWaveOut, &WaveHeader[2], sizeof(WAVEHDR));
bad3:
	waveOutUnprepareHeader(HWaveOut, &WaveHeader[1], sizeof(WAVEHDR));
bad2:
	waveOutUnprepareHeader(HWaveOut, &WaveHeader[0], sizeof(WAVEHDR));
bad1:
	/* Close WAVE Out device */
	waveOutClose(HWaveOut);
	
}



/////////////////////////////////////
//////////
/////////////////////////////////////
DWORD WINAPI RecordWav(LPVOID arg)
{
	DWORD							err;
	WAVEFORMATEX					waveFormat;
//	MIXERLINE						mixerLine;
	HANDLE							waveInThread;
//	unsigned long					n, numSrc;
	
	string							wavFile;
	char*							micWavFile = NULL;

	finished = true;

	/* Create our thread that we use to store our incoming "blocks" of digital audio data (sent to us
	 * from the driver. The main procedure of this thread is waveInProc(). We need to get the threadID
	 * and pass that to waveInOpen(). CreateThread() will give it to us
	 */
	waveInThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)waveInProc, 0, 0, &err);
	if (!waveInThread)
	{
		printf("Can't create WAVE recording thread! -- %08X\n", GetLastError());
		return(-2);
	}
	CloseHandle(waveInThread);


	/* Clear out both of our WAVEHDRs. At the very least, waveInPrepareHeader() expects the dwFlags field to
	 * be cleared
	 */
	ZeroMemory(&WaveHeaderRecord[0], sizeof(WAVEHDR) * 4);

	//format that is suitable for later analysis
	/* Initialize the WAVEFORMATEX for 16-bit, 44KHz, stereo. That's what I want to record */
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = 1;
	waveFormat.nSamplesPerSec = 16000;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample/8);
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	bytesPerSample = waveFormat.nBlockAlign;

	/* Open the default WAVE In Device, specifying my callback. Note that if this device doesn't
	 * support 16-bit, 44KHz, stereo recording, then Windows will attempt to open another device
	 * that does. So don't make any assumptions about the name of the device that opens. After
	 * waveInOpen returns the handle, use waveInGetID to fetch its ID, and then waveInGetDevCaps
	 * to retrieve the actual name
	 */
	err = waveInOpen(&WaveInHandle, WAVE_MAPPER, &waveFormat, (DWORD)err, 0, CALLBACK_THREAD);
	if (err)
	{
		PrintWaveErrorMsg(err, "Can't open WAVE In Device!");
		return(-1);
	}
	else
	{
	    /* Open the mixer associated with the WAVE In device opened above. Note that
		 * I pass the handle obtained via waveInOpen() above and specify MIXER_OBJECTF_HWAVEIN
		 */
		err = mixerOpen(&MixerHandle, (DWORD)WaveInHandle, 0, 0, MIXER_OBJECTF_HWAVEIN);
		if (err)
		{
			/* This card's driver may not support the Mixer API, so this error check is
			 * very important. NOTE: Hopefully, the card ships with some sort of proprietary
			 * software to set the recording level, or the card has a hardware knob that the
			 * user can adjust. In either case, setting the recording level is not doable here.
			 * You may want to inform the user of that.
			 */
			printf("Device does not have mixer support! -- %08X\n", err);
		}
/*
		else
		{
			//This device should have a WAVEIN destination line. Let's get its ID so
			//that we can determine what source lines are available to record from
			
			mixerLine.cbStruct = sizeof(MIXERLINE);
			mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
			err = mixerGetLineInfo((HMIXEROBJ)MixerHandle, &mixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE);
			if (err)
			{
				printf("Device does not have a WAVE recording control! -- %08X\n", err);
				goto record;
			}

			//Get how many source lines are available from which to record. For example, there could
			//be a Mic In (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE), Line In (MIXERLINE_COMPONENTTYPE_SRC_LINE),
			//and/or SPDIF In (MIXERLINE_COMPONENTTYPE_SRC_DIGITAL)
			
			numSrc = mixerLine.cConnections;

			//If the WAVEIN destination line has a mute switch, then make sure that it is not muted. So too, you should
			//make sure that the volume is turned up. This would be the master volume and master mute for all input
			//source lines. But you may wish to save the current settings (by fetching them with mixerGetControlDetails)
			//and subsequently restore them (when your program exits) so that you don't permanently alter his settings.
			//Here we only unmute the line, but don't set its volume. Take a look at set_mute() to see how we adjust the
			//value of any mute control. You can modify this easily to create a set_volume() function that sets the value
			//of any MIXERCONTROL_CONTROLTYPE_VOLUME type of control for the line. The only thing that you may need to do
			//is make sure the value is within the MIXERCONTROLDETAILS' dwMinimum and dwMaximum range. (And you need to
			//fetch the current value to get a filled in MIXERCONTROLDETAILS)
			
			mixerLine.dwSource = mixerLine.dwDestination;
			set_mute(&mixerLine, 1, ERRMSG_NONE);

			//Make sure that there is at least one WAVEIN source line available. If not, then just go with the
			//WAVEIN master volume and mute above
			
			if (!numSrc)
			{
				printf("ERROR: There are no WAVE inputs to adjust!\n");
				goto record;
			}

			// If there is more than one source line, let's choose which ones to record from (ie, which to leave
			// unmuted). NOTE: The audio card may allow all the inputs to simultaneously record (and the card will
			// digitally mix all wave data into our buffer that we waveInAddBuffer()). The user may actually want
			// this. So what you may wish to do here is present a list of the available source lines, and allow him
			// to mute inputs that he doesn't want to record from. That will perhaps reduce any noise. If a particular
			// source line has no mute switch, then if it has a volume, you can set its volume to 0. But you may wish
			// to save the current settings and subsequently restore them (when your program exits) so that you don't
			// permanently alter his settings.
			//

			// Now let's enumerate the names of source lines that are available for WAVE recording. For example, there
			// could be a Mic In (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE), Line In (MIXERLINE_COMPONENTTYPE_SRC_LINE),
			// and/or SPDIF In (MIXERLINE_COMPONENTTYPE_SRC_DIGITAL). NOTE: mixerLine.dwDestination already has the
			// index of our WAVEIN destination line.
			
			// NOTE: Some sound cards will list source lines such as MIDI input, CD audio (analog) input, and other
			// types of inputs that aren't really connected to audio recording (ie, aren't capable of being digitized
			// by the ADC into a WAVE file), under the MIXERLINE_COMPONENTTYPE_DST_WAVEIN destination line. In other
			// words, some manufacturers list every input source line under MIXERLINE_COMPONENTTYPE_DST_WAVEIN,
			// regardless of whether it can be recorded (into a WAVE or MIDI file) by software. Other card manufacturers
			// may take a more sensible software approach and list only those sources that can be recorded (but still
			// you may have a MIDI input listed under MIXERLINE_COMPONENTTYPE_DST_WAVEIN, which is a bit of misnomer
			// since MIDI is not digital audio, and is not recorded via the waveXXX API). Ideally, any sources that
			// aren't recordable by a card's ADC shouldn't be listed under MIXERLINE_COMPONENTTYPE_DST_WAVEIN. For
			// example, a CD audio input jack (that can't be digitized) should be a MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC
			// attached to MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, and a MIDI IN should be a
			// MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER attached to MIXERLINE_COMPONENTTYPE_DST_UNDEFINED (since
			// unfortunately Microsoft never came up with some sort of MIXERLINE_COMPONENTTYPE_DST_MIDIIN). But we live
			// in an imperfect world. At the least, you should filter out any MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER
			// from your listing.
			
			// If you're writing a windowed app, what you should probably do is create a dialog that contains all of
			 //the controls for each source line below, plus the WAVEIN destination line (ie, a small mixer that contains
			// the individual volume sliders, mute switches, etc, for each source line, plus the master volume for the
			// WAVEIN destination line). That way, he can more easily mute desired inputs, as well as adjust the recording
			// levels. In this simple example, we haven't provided a way to adjust the input levels.
			//
remute:
			printf("The device has the following WAVE inputs. Choose which one to mute,\nor press ENTER to start recording.\n\n");

			// Print the names of the source lines for the MIXERLINE_COMPONENTTYPE_DST_WAVEIN destination. But,
			// omit any MIDI In
			/
			for (n = 0; n < numSrc; n++)
			{
				mixerLine.cbStruct = sizeof(MIXERLINE);
				mixerLine.dwSource = n;

				if (!(err = mixerGetLineInfo((HMIXEROBJ)MixerHandle, &mixerLine, MIXER_GETLINEINFOF_SOURCE)))
				{
					if (mixerLine.dwComponentType != MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER)
						printf("\t#%lu: %s\n", n, mixerLine.szName);
				}
			}

			// Get his choice of source line to mute, or ENTER to continue with recording 
			{
			TCHAR	buffer[3];

			fgets(&buffer[0], 3, stdin);

			// Did he choose a source line to mute (ie, he typed a number)? 
			if (buffer[0] >= '1' && buffer[0] <= '9')
			{
				// Let's use the mixer API to mute this line. This is an example of how you can adjust the
				// setting of some line
				//
				buffer[0] -= '1';
				if ((unsigned long)buffer[0] < numSrc)
				{
					// set_mute() wants us to fill in a MIXERLINE struct with information about
					// the desired source line
					/
					mixerLine.cbStruct = sizeof(MIXERLINE);
					mixerLine.dwSource = buffer[0];
					mixerGetLineInfo((HMIXEROBJ)MixerHandle, &mixerLine, MIXER_GETLINEINFOF_SOURCE);

					// Check if there is a mute control for this line, and if so, set it's value to
					//  0 (ie, mute it)
					 //
					set_mute(&mixerLine, 0, ERRMSG_PRINT);
				}

				// Go back to check for more muted sources 
				goto remute;
			}
			}

			// Close the mixer 
record:		mixerClose(MixerHandle);
		}
*/
		/* Now that the inputs are all set up, we can record an audio file using waveInPrepareHeader(),
		 * waveInAddBuffer(), and waveInStart().
		 */

		/* First, let's create a file on disk where we'll store the audio data we record. We'll name it
		 * test.snd in the current directory. We'll save the data in raw format, with no headers on it,
		 * just for simplicity. Normally, you would instead save it in WAVE file format so any program
		 * can read it and know the sample rate and number of channels, etc.
		 */

		//WaveFileHandle = CreateFile("testic.snd", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0); //A.C. put comment
		//if (WaveFileHandle == INVALID_HANDLE_VALUE) //A.C. put comment
		Sleep(100); //A.C. added
		if(false) //A.C. added
		{
			printf("Can't create test.snd disk file!\n");
		}
		
		else
		{
//			WriteFile(WaveFileHandle, ((WAVEHDR *)msg.lParam)->lpData, ((WAVEHDR *)msg.lParam)->dwBytesRecorded, &msg.time, 0)
			/* Allocate, prepare, and queue two buffers that the driver can use to record blocks of audio data.
			 * (ie, We're using double-buffering. You can use more buffers if you'd like, and you should do that
			 * if you suspect that you may lag the driver when you're writing a buffer to disk and are too slow
			 * to requeue it with waveInAddBuffer. With more buffers, you can take your time requeueing
			 * each).
			 *
			 * I'll allocate 2 buffers large enough to hold 2 seconds worth of waveform data at 44Khz. NOTE:
			 * Just to make it easy, I'll use 1 call to VirtualAlloc to allocate both buffers, but you can
			 * use 2 separate calls since the buffers do NOT need to be contiguous. You should do the latter if
			 * using many, large buffers
			 */

			//WaveHeaderRecord[1].dwBufferLength = WaveHeaderRecord[0].dwBufferLength = waveFormat.nAvgBytesPerSec << 1;

			//--G--
			//buffer can hold 256 samples 
			WaveHeaderRecord[3].dwBufferLength = WaveHeaderRecord[2].dwBufferLength = WaveHeaderRecord[1].dwBufferLength = WaveHeaderRecord[0].dwBufferLength = 256*waveFormat.nBlockAlign;

			if (!(WaveHeaderRecord[0].lpData = (char *)VirtualAlloc(0, WaveHeaderRecord[0].dwBufferLength * 4, MEM_COMMIT, PAGE_READWRITE)))
			{
				printf("ERROR: Can't allocate memory for WAVE buffer!\n");
			}
			else
			{
				/* Fill in WAVEHDR fields for buffer starting address. We've already filled in the size fields above */
				WaveHeaderRecord[1].lpData = WaveHeaderRecord[0].lpData + WaveHeaderRecord[0].dwBufferLength;
				WaveHeaderRecord[2].lpData = WaveHeaderRecord[0].lpData + 2*WaveHeaderRecord[0].dwBufferLength;
				WaveHeaderRecord[3].lpData = WaveHeaderRecord[0].lpData + 3*WaveHeaderRecord[0].dwBufferLength;

				/* Leave other WAVEHDR fields at 0 */

				/* Prepare the 2 WAVEHDR's */
				if ((err = waveInPrepareHeader(WaveInHandle, &WaveHeaderRecord[0], sizeof(WAVEHDR))))
				{
					printf("Error preparing WAVEHDR 1! -- %08X\n", err);
				}
				else
				{
					if ((err = waveInPrepareHeader(WaveInHandle, &WaveHeaderRecord[1], sizeof(WAVEHDR))))
					{
						printf("Error preparing WAVEHDR 2! -- %08X\n", err);
					}
					else
					{
						if ((err = waveInPrepareHeader(WaveInHandle, &WaveHeaderRecord[2], sizeof(WAVEHDR))))
						{
							printf("Error preparing WAVEHDR 3! -- %08X\n", err);
						}
						else
						{
							if ((err = waveInPrepareHeader(WaveInHandle, &WaveHeaderRecord[3], sizeof(WAVEHDR))))
							{
								printf("Error preparing WAVEHDR 4! -- %08X\n", err);
							}
							else
							{
							/* Queue first WAVEHDR (recording hasn't started yet) */
								if ((err = waveInAddBuffer(WaveInHandle, &WaveHeaderRecord[0], sizeof(WAVEHDR))))
								{
									printf("Error queueing WAVEHDR 1! -- %08X\n", err);
								}
								else
								{
								/* Queue second WAVEHDR */
									if ((err = waveInAddBuffer(WaveInHandle, &WaveHeaderRecord[1], sizeof(WAVEHDR))))
									{
										printf("Error queueing WAVEHDR 2! -- %08X\n", err);
									//	DoneAll = 1;
									//	goto abort;
									}
									else
									{
										if ((err = waveInAddBuffer(WaveInHandle, &WaveHeaderRecord[2], sizeof(WAVEHDR))))
										{
											printf("Error queueing WAVEHDR 1! -- %08X\n", err);
										}
										else
										{
											if ((err = waveInAddBuffer(WaveInHandle, &WaveHeaderRecord[3], sizeof(WAVEHDR))))
											{
												printf("Error queueing WAVEHDR 2! -- %08X\n", err);
												DoneAll = 1;
												goto abort;
											}
											else
											{


											/* Start recording. Our secondary thread will now be receiving
											* and storing audio data to disk
											*/
												InRecord = TRUE;
												if ((err = waveInStart(WaveInHandle)))
												{
													printf("Error starting record! -- %08X\n", err);
												}
												else
												{
													/* Wait for user to stop recording */
													//printf("Recording has started. Press ENTER key to stop recording...\n");
													//start recording
													time( &startRecord );	
													while (!stopRecording)
													{
														stopRecording = lipSync->stop_flag;
														Sleep(100);
													}
													//getchar();
													//stop recording
													time( &finishRecord );
													//stopRecording = 0;
												}

												/* Tell our recording thread not to queue any WAVEHDRs it gets back via MM_WIM_DONE */
abort:											InRecord = FALSE;
								
												/////////////////////////////
												//CHANGED BY G.
												//if we want to store new animation
												if (lipSync->bFbaFile)
												{
													recordingTime = difftime( finishRecord, startRecord );
													totalTime = recordingTime;
													lipSync->fileTime = (float)totalTime;
													//???
													//lipSync->stop_flag = stopRecording;

													unsigned char*	allBytes = NULL;
													allBytes = new byte[recordedSize];
								
													BUFFERBYTES::iterator bufferBytesIterator;
													long count = 0;
								
													for (recordedBytesIterator = recordedBytes.begin(); recordedBytesIterator != recordedBytes.end(); recordedBytesIterator++)
													{
														for (bufferBytesIterator = (*recordedBytesIterator).begin(); bufferBytesIterator != (*recordedBytesIterator).end(); bufferBytesIterator++)
														{
															allBytes[count] = *bufferBytesIterator;
															count++;
														}
													}
													recordedBytes.clear();
												
													if ((lipSync->bFbaFile) &(lipSync->h_heMode == LS_HE_MODE_VISEME_BASED))
													{
														//save animation in fba file
														//WaveCV->StoreAnimation3();
														WaveCV->FinalizeStoreAnimation(); //added by G.
													}
													//create name for audio file from animation file name 
													wavFile = lipSync->fbaFileName;
													wavFile.erase(wavFile.find_last_of(".")+1);
													wavFile.append("wav");
													micWavFile = const_cast<char *>(wavFile.c_str());
													//save samples in audio file
													WaveSaveFile_ls(micWavFile, recordedSize, nSamplesRecorded, &waveFormat, allBytes);
													}

													/*		if (allBytes)
														{
															delete [] allBytes;
															allBytes=NULL;
														}

													*/		
													/////////////////////////////

												/* Stop recording and tell the driver to unqueue/return all of our WAVEHDRs.
												* The driver will return any partially filled buffer that was currently
												* recording. Because we use waveInReset() instead of waveInStop(),
												* all of the other WAVEHDRs will also be returned via MM_WIM_DONE too
												*/
												waveInReset(WaveInHandle);

												/* Wait for the recording thread to receive the MM_WIM_DONE for each
												* of our queued WAVEHDRs. It will count them off, and after processing
												* them all, 'DoneAll' will be equal to how many WAVEHDRs we used. We
												* can then unprepare the WAVEHDRs, close our WAVE device and our disk
												* file, and free our buffers
												*/
												while (DoneAll < 4) Sleep(100);
											}
										}
									}
								}
								/* Unprepare second WAVEHDR. NOTE: It should be ok to unprepare a WAVEHDR
								* that was never successfully prepared, as long as you initially cleared
								* out the dwFlags field (unless the audio driver is badly written)
								*/
								if ((err = waveInPrepareHeader(WaveInHandle, &WaveHeaderRecord[3], sizeof(WAVEHDR))))
								{
									printf("Error unpreparing WAVEHDR 2! -- %08X\n", err);
								}
							}
							if ((err = waveInPrepareHeader(WaveInHandle, &WaveHeaderRecord[2], sizeof(WAVEHDR))))
							{
								printf("Error unpreparing WAVEHDR 2! -- %08X\n", err);
							}
						}
						if ((err = waveInPrepareHeader(WaveInHandle, &WaveHeaderRecord[1], sizeof(WAVEHDR))))
						{
							printf("Error unpreparing WAVEHDR 2! -- %08X\n", err);
						}
					}
					/* Unprepare first WAVEHDR */
					if ((err = waveInPrepareHeader(WaveInHandle, &WaveHeaderRecord[0], sizeof(WAVEHDR))))
					{
						printf("Error unpreparing WAVEHDR 1! -- %08X\n", err);
					}
				}
			}
		} //wave file without header
	}

	/* Close the WAVE In device */
	do
	{
		err = waveInClose(WaveInHandle);
		if (err) PrintWaveErrorMsg(err, "Can't close WAVE In Device!");
	} while (err);

	/* Close the disk file if it opened */
	if (WaveFileHandle != INVALID_HANDLE_VALUE) CloseHandle(WaveFileHandle);

	/* Free any memory allocated for our buffers */
	if (WaveHeaderRecord[0].lpData) VirtualFree(WaveHeaderRecord[0].lpData, 0, MEM_RELEASE);

	//
//	if (micWavFile)
//		delete [] micWavFile;

	lipSync->bRecordOver = 1;
	stopRecording = 0;

	return(0);
	
}

/**************************** waveInProc() *******************************
 * This is my thread to which the Windows Low Level WAVE API passes messages
 * regarding digital audio recording (such as MM_WIM_DATA, MM_WIM_OPEN, and
 * MM_WIM_CLOSE).
 *
 * Before opening the WAVE In Device, I use CreateThread() (from my main
 * thread) to start up waveInProc() as a background thread. waveInProc()
 * simply loops around a call to GetMessage() to process those messages
 * that the audio driver sends me during recording.
 *
 * When I subsequently open the WAVE In device (using waveInOpen() in my
 * main thread), I pass CALLBACK_THREAD and the ID of this thread.
 *
 * Why don't I use CALLBACK_FUNCTION instead, and use a real callback (instead
 * of this background thread)? That would actually be more efficient since a
 * callback function gets called directly by the driver at interrupt time,
 * right when the driver needs to inform us of something, and the driver
 * directly passes args on the stack (instead of needing to queue a MSG to
 * some thread queue).
 *
 * By contrast, this thread is passed a message usually after the driver's
 * interrupt routine is done, which can be some time later than it would be
 * with a callback. And args are retrieved via filling in a MSG with
 * GetMessage().
 *
 * I don't use CALLBACK_FUNCTION because it is restricted to calling only
 * a few particular Windows functions, namely some of the time functions,
 * and a few of the Low Level MIDI API. If you violate this rule, your app can
 * hang inside of the callback). One of the Windows API that a callback can't
 * call is waveInAddBuffer() which is what I need to use whenever I receive a
 * MM_WIM_DATA. My callback would need to defer that job to another thread
 * anyway, so I just use CALLBACK_THREAD here instead.
 *
 * NOTE: You have to link with the multi-threaded version of the C library
 * if you intend to call any C library functions in this secondary thread.
 * (I don't). And in that case, you should also use beginthread() to create
 * the thread instead of CreateThread().
 *************************************************************************/

DWORD WINAPI waveInProc(LPVOID arg)
{
	MSG		msg;
	BUFFERBYTES  bufferBytes;

	/* Wait for a message sent to me by the audio driver */
	while (GetMessage(&msg, 0, 0, 0) == 1)
	{
		/* Figure out which message was sent */
		switch (msg.message)
		{
			/* A buffer has been filled by the driver */
			case MM_WIM_DATA:
			{
				/* msg.lParam contains a pointer to the WAVEHDR structure for the filled buffer.
				 * NOTE: The WAVEHDR's dwBytesRecorded field specifies how many bytes of
				 * audio data are in the buffer. It may not be full, as would be the case
				 * if recording was stopped *while* this buffer was being filled. (ie, A
				 * final, partial block of audio data may be returned)
				 */
				if (((WAVEHDR *)msg.lParam)->dwBytesRecorded)
				{
					//--G--
					///////////////////

					if (!(stopRecording) & (finished))
					{

					WaveBufSize = ((WAVEHDR *)msg.lParam)->dwBytesRecorded;
					WaveCV = lipSync->cvis;
					
						WaveCV->HandleFrames(WaveBufSize, (short*)((WAVEHDR *)msg.lParam)->lpData, &finished);
					
					//recordHeaderPtr = ((WAVEHDR *)msg.lParam);
					//isUpdated = true;

					//recorded size in bytes
					recordedSize += ((WAVEHDR *)msg.lParam)->dwBytesRecorded;
					//number of samples recorded
					nSamplesRecorded += ((WAVEHDR *)msg.lParam)->dwBytesRecorded / bytesPerSample; 
					
					for (int i = 0; i < (int)((WAVEHDR *)msg.lParam)->dwBytesRecorded; i++)	//CHANGED BY I.K. added explicit cast
					{
						bufferBytes.push_back((int)(((WAVEHDR *)msg.lParam)->lpData[i]));
					}
					recordedBytes.push_back(bufferBytes);
					bufferBytes.clear();
					
					
					
					/////////////////////////////////////////////////////
				
					/* Let's write out this next block of data to our already open file. NOTE: The main
					 * thread opened this file prior to starting recording, and stored its handle in
					 * 'WaveFileHandle'
					 */
					if (!WriteFile(WaveFileHandle, ((WAVEHDR *)msg.lParam)->lpData, ((WAVEHDR *)msg.lParam)->dwBytesRecorded, &msg.time, 0) ||

						/* Check for an error */
						msg.time != ((WAVEHDR *)msg.lParam)->dwBytesRecorded)
					{
						/* Normally you'd do something here like signal the main thread to have it stop recording and
						 * display an error message about there being a problem writing the audio data to disk
						 */
					}
					}
				}

				/* Are we still recording? */
				if (InRecord)
				{
					/* Yes. Now we need to requeue this buffer so the driver can use it for another block of audio
					 * data. NOTE: We shouldn't need to waveInPrepareHeader() a WAVEHDR that has already been prepared once
					 */
					waveInAddBuffer(WaveInHandle, (WAVEHDR *)msg.lParam, sizeof(WAVEHDR));
				}

				/* We aren't recording, so another WAVEHDR has been returned to us after recording has stopped.
				 * When we get all of them back, DoneAll will be equal to how many WAVEHDRs we queued
				 */
				else
				{
					++DoneAll;
				}

				/* Keep waiting for more messages */
                continue;
			}

			/* Our main thread is opening the WAVE device */
			case MM_WIM_OPEN:
			{
				/* Re-initialize 'DoneAll' */
				DoneAll = 0;

				/* Keep waiting for more messages */
                continue;
			}

			/* Our main thread is closing the WAVE device */
			case MM_WIM_CLOSE:
			{
				WaveCV->LastFrameRender();
				
				/* Terminate this thread (by return'ing) */
				break;
			}
		}
	}

	return(0);
}


/*********************** PrintWaveErrorMsg() **************************
 * Retrieves and displays an error message for the passed Wave In error
 * number. It does this using mciGetErrorString().
 *************************************************************************/

void PrintWaveErrorMsg(DWORD err, TCHAR * str)
{
#define BUFFERSIZE 128
	char	buffer[BUFFERSIZE];
	
	printf("ERROR 0x%08X: %s\r\n", err, str);
	if (mciGetErrorString(err, &buffer[0], sizeof(buffer)))
	{
		printf("%s\r\n", &buffer[0]);
	}
	else
	{
		printf("0x%08X returned!\r\n", err);
	}
}

#endif

}

