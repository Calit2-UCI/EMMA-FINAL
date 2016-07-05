

#include "LipSync.h"
#include "CVisemes.h"
#include "wavePlay.h"



//#ifdef VISAGE_SDK_LICENSING_ON
//#include "sdkversion.h"
//#include "visagelicensemanager.h"
//#include "licensemanagerexports.h"
//#endif

#ifndef VISAGE_STATIC
#ifdef WIN3255
	BOOL APIENTRY DllMain( HANDLE hModule, 
						   DWORD  ul_reason_for_call, 
						   LPVOID lpReserved
						 )
	{
		switch (ul_reason_for_call)
		{
			case DLL_PROCESS_ATTACH:
			case DLL_THREAD_ATTACH:
			case DLL_THREAD_DETACH:
			case DLL_PROCESS_DETACH:
				break;
		}
		return TRUE;
	}

	#elif defined(MAC) || defined(LINUX) //Other O.S., standar C++ usage
		int main(int argc, char* argv[])
		{
			return 1;
		}
	#else
	int __stdcall main(int argc, char* argv[])
	{
		return 1;
	}

	#endif

#endif

#ifndef MSVC
	#include <unistd.h>
#endif



namespace VisageSDK
{

extern VisageLipSync*	lipSync;


LipSyncEventHandler::LipSyncEventHandler()
{ }

LipSyncEventHandler::~LipSyncEventHandler()
{ }

VisageLipSync::VisageLipSync()
{ 
	constructorInit();
}
	
	
VisageLipSync::VisageLipSync(bool isSequential)
{
	constructorInit();
	
	sequential = isSequential;
	
}
	

VisageLipSync::~VisageLipSync()
{
	if (Visemes)
	{
		delete [] Visemes;
		Visemes=NULL;
	}
	if (strFileName)
	{
		delete [] strFileName;
		strFileName=NULL;
	}
	if (fbaFileName)
	{
		delete [] fbaFileName;
		fbaFileName=NULL;
	}
	if (fbaps)
	{
		delete fbaps;
		//delete [] fbaps;
		fbaps=NULL;
	}
	/*if (eventH)
	{
		//delete [] eventH;
		//eventH=NULL;
	}*/
	if (cvis)
	{
		// Changed by KStankovic
		delete cvis;
		//
		cvis=NULL;
	}
}


void VisageLipSync::constructorInit()
{
	//#ifdef VISAGE_SDK_LICENSING_ON
	//	// check license for SDK runtime
	//	printf("vsLipSync constructor, before checking Register\n");
	//	int licenseType = checkRegistration(VLTID_SDK_LIPSYNC,SDK_VERSION_STRING);
	//	printf("vsLipSync constructor, after checking Register\n");
	//		if(licenseType < 0)
	//		{
	//			printf("vsLipSync constructor, inside licenseType if\n");
	//			//exit(1);
	//		}
	//		printf("vsLipSync constructor, before endif\n");
	//#endif
	printf("lipsync created\n");


	bNumberOfFrames = 0;

	fileTime = 0;
	fileSamples = 0;
	nVisemes = 0;
	bRecordOver = 0;
	bPlayOver = 0;
	currentViseme = 0;
	actionFinished = true;

	//eventH=NULL;
	Visemes=NULL;
	fbaps = NULL;
	strFileName=NULL;
	fbaFileName=NULL;
	cvis=NULL;

	active = 0;

	sequential = false;
}


void VisageLipSync::init()
{
	bAudioFile = 0;
	bFbaFile = 0;
	bEventHandler = 0;
	bRealTime = 0;
	bPreProcess = 0;
	stop_flag = 0;

	h_syncMode = LS_MODE_REAL_TIME;

	if (Visemes)
	{
		//delete Visemes;	 
		Visemes=NULL;
	}
	if (cvis)
	{
		//delete [] cvis;
		cvis=NULL;
	}	
}

void VisageLipSync::sync(char* audioFile, char *FBAFile, LipSyncEventHandler *eventHandler, bool mute)
{
	//stopSync();
	init();
	actionFinished = false;

	cvis = new CVisemes(this);
	lipSync = this;
		
	fbaps = new FBAPs();

	eventH[bEventHandler++] = eventHandler; //added by A.C., add eventHandler 
	active = 1;

	// initializations needed for the interpolateVisemes function
	currentViseme = 0;
	lastTimeMs = -1;
	lastViseme = 0;
	interpolationDir = 1.0;

	if (audioFile != NULL )
	{
		//creating animation from audio file
		strFileName = new char[strlen(audioFile)+1];
		strcpy(strFileName,audioFile);
		bAudioFile = 1;
	}

	if ((FBAFile != NULL) & (h_heMode == LS_HE_MODE_VISEME_BASED))
	{
		//animation is stored in the given fba file	
		fbaFileName = new char[strlen(FBAFile)+1];
		strcpy(fbaFileName,FBAFile);
		bFbaFile = 1;
		cvis->InitStoreAnimation(); //added by G.
	}

	if(!(stop_flag))
	{
#ifdef WIN32_OLD
		if(!(bAudioFile))
		{
#ifdef MSVC
			HANDLE	recordThread;
			DWORD	err;
#else
			void * recordThread;
			unsigned long err;
#endif
			// microphone is used !!!!
			bRealTime = 1;
			bNumberOfFrames = 4;

			recordThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RecordWav, 0, 0, &err);
			if (!recordThread)
			{
				printf("Can't create WAVE recording thread!");
				return;
			}
			CloseHandle(recordThread);
		}
		else 
		{


			//if( h_syncMode == LS_MODE_OFFLINE )
			//{
			//	HANDLE	playOffThread;
			//	DWORD	err;

			//	// process given audio file offline (offline part)!!!
			//	bPreProcess = 1;
			//	bNumberOfFrames = 4;

			//	Visemes = cvis->PreprocessWavFile(audioFile);
			//	//PlayWavFile(audioFile);

			//	playOffThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)PlayWavFile, 0, 0, &err);
			//	if (!playOffThread)
			//	{
			//		printf("Can't create WAVE recording thread!");
			//		return;
			//	}
			//	CloseHandle(playOffThread);
			//}
			//else if( h_syncMode == LS_MODE_REAL_TIME )
			//{
				HANDLE	playOnThread;
				DWORD	err;
				
				// process given audio file offline (real-time part)!!!
				bRealTime = 1;
				bNumberOfFrames = 4;
				//PlayWavFile(audioFile);



				
				playOnThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)PlayWavFile, 0, 0, NULL);
				if (!playOnThread)
				{
					printf("Can't create WAVE recording thread!");
					return;
				}
				CloseHandle(playOnThread);

#else //directly considering there is an audioFile

				bRealTime = 1;
				bNumberOfFrames = 4;

				//bool *isMute;			
				//isMute = (bool *) malloc(sizeof(bool));
				//This allocated memory is then freed in the playWavFile function
				//*isMute = mute;
				bool *args= (bool *) malloc(2*sizeof(bool));
				args[0] = mute;
				args[1] = sequential;

#ifdef MSVC
				HANDLE	playOnThread;
				DWORD	err;

				playOnThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)playWavFile, (void*)args, 0, NULL);
				if (!playOnThread)
				{
					printf("Can't create WAVE recording thread!");
					return;
				}
				CloseHandle(playOnThread);
#else

				pthread_t playOnThread;
            
            int threadCreated = -1;
            if(!mute)
            {
                threadCreated = pthread_create(&playOnThread, NULL, playWavFile, (void*)args);
                printf("launched playWavFile thread\n");
            }
            else
            {
            
                threadCreated = pthread_create(&playOnThread, NULL, readWavFile, (void*)args);
                printf("launched readWavFile thread\n");
            }
        
            if(threadCreated)
            {
                printf("Can't create WAVE recording pthread!\n");
                return;
            }
			

			
				if(sequential)
					pthread_join(playOnThread, NULL);
				else
					pthread_detach(playOnThread);
#endif
				

#endif
			//}
#ifdef MSVC
		}
#endif
	}
}





//void VisageLipSync::syncTTS(std::string text, char* audioFile, LipSyncEventHandler *eventHandler, bool mute)
//{
//
//	init();
//	actionFinished = false;
//
//	cvis = new CVisemes(this);
//	lipSync = this;
//
//	fbaps = new FBAPs();
//
//	eventH[bEventHandler++] = eventHandler; //added by A.C., add eventHandler
//	active = 1;
//
//	// initializations needed for the interpolateVisemes function
//	currentViseme = 0;
//	lastTimeMs = -1;
//	lastViseme = 0;
//	interpolationDir = 1.0;
//
//	if (audioFile != NULL )
//	{
//		//creating animation from audio file
//		strFileName = new char[strlen(audioFile)+1];
//		strcpy(strFileName,audioFile);
//		bAudioFile = 1;
//	}
//
////	if ((FBAFile != NULL) & (h_heMode == LS_HE_MODE_VISEME_BASED))
////	{
////		//animation is stored in the given fba file
////		fbaFileName = new char[strlen(FBAFile)+1];
////		strcpy(fbaFileName,FBAFile);
////		bFbaFile = 1;
////		cvis->InitStoreAnimation(); //added by G.
////	}
//
//	if(!(stop_flag))
//	{
//				bRealTime = 1;
//				bNumberOfFrames = 4;
//
//				std::string *textToSay = new std::string();
//				textToSay->assign(text);
//
//				pthread_t playOnThread;
//				if(pthread_create(&playOnThread, NULL, playText, (void*)textToSay))
//				{printf("Can't create WAVE recording pthread!\n");return;}
//
//				printf("launched playWavFile thread\n");
//
//				if(sequential)
//					pthread_join(playOnThread, NULL);
//				else
//					pthread_detach(playOnThread);
//
//	}
//}




void VisageLipSync::stopSync()
{
	//stopLipSynching
	if (isSpeaking())
	{
		printf("stopSync:: is speaking\n");
		stop_flag = 1;
		
		if(!(bAudioFile))
		{
			printf("stopSync:: !bAudioFile\n");

			while(!bRecordOver)
			{
				printf("stopSync:: !bRecordOver\n");
				bRecordOver = true;
#ifdef WIN32
				Sleep(100);
#else
				usleep(100000);
#endif
			}
			active = 0;
		}
		else
		{
			while(!bPlayOver)
			{
				printf("stopSync:: !bPlayOver\n");
#ifdef WIN32
				Sleep(100);
#else
				usleep(100000);
#endif
			}
			active = 0;
			
		}
	}
	
	actionFinished = true;
}
	
	
	bool VisageLipSync::isFinished()
	{
		return actionFinished;
	}
	

//void VisageLipSync::setSyncMode(int mode)
//{
//	//needed when microphone is not used
//	if (mode) { 
//		h_syncMode = LS_MODE_OFFLINE;
//	}
//	else {
//		h_syncMode = LS_MODE_REAL_TIME;
//	}
//}
//added by G.
void VisageLipSync::setHandleEventMode(int mode)
{
	if (mode==10) { 
		
		h_heMode = LS_HE_MODE_VISEME_BASED;
	}
	else {
		h_heMode = LS_HE_MODE_FRAME_BASED;
	}
}

/* Interpolate the visemes.
* This is an implementation of a very simple coarticulation scheme. When a new viseme is encountered
* the method interpolates towards it. The currentViseme is set by the Notify() function. Here we check
* whteher it has changed.
*
* @param timeMs current time in the animation, in milliseconds.
*/
void VisageLipSync::interpolateVisemes(long timeMs)
{
	long time_step = 40;
	double blend_step;
	double onset_time = 100.0; // how long does it take for the mouth to shape the viseme

	if(lastTimeMs != -1)
		time_step = timeMs - lastTimeMs;
	lastTimeMs = timeMs;
	blend_step = 64.0 * ((double)time_step / onset_time);

	if(lastViseme != currentViseme)
	{
		// there is a new viseme. set the interpolation parameters so that we start interpolating towards it.
		lastViseme = currentViseme;
		if(fbaps->faps->fap1.blend<32)
		{
			interpolationDir = 1.0;
			fbaps->faps->fap1.viseme1 = currentViseme;
		}
		else
		{
			interpolationDir = -1.0;
			fbaps->faps->fap1.viseme2 = currentViseme;
		}
	}
	fbaps->faps->fap1.blend += (int)(blend_step * interpolationDir);
	if(fbaps->faps->fap1.blend>63.0) fbaps->faps->fap1.blend = 63;
	if(fbaps->faps->fap1.blend<0.0) fbaps->faps->fap1.blend = 0;
}


FBAPs *VisageLipSync::getFBAPs(long globalTime, FBAPs *lastFBAPs, VisageCharModel *model) 
{
	if(!active)
		return(NULL);

	localTime = globalTime - globalStartTime;

	// interpolate the visemes to achieve coarticulation and put the result into speechFaps
	interpolateVisemes(localTime);
	
	return fbaps;
}

//added by G.
void VisageLipSync::getCurrentData(FBAPs *tempFBAPs, long *currentTime, int *lViseme, short *audioBuf)
{
	if (tempFBAPs != NULL)
		*tempFBAPs = *fbaps;

	if (currentTime != NULL)
		*currentTime = localTime;

	if (lViseme != NULL)
		*lViseme = lastViseme;

	if (audioBuf != NULL)
		*audioBuf = *cvis->get_audioBuf();
}


void VisageLipSync::start(long globalTime)
{
	globalStartTime = globalTime;
}

void VisageLipSync::stop()
{
	stopSync();
	// something should be done here to stop speaking
}

int VisageLipSync::isSpeaking()
{
	if(active)
		return 1;
	else
		return 0;
}

}

