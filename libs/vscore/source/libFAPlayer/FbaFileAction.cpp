
#include "FbaFileAction.h"

// C++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <iostream>

// Win32
#ifdef WIN32
	#define S_IRWXG 1
	#include <windows.h> //necessary for messagebox in windows
#else
	#include <unistd.h>
#endif


#include <fcntl.h>
#include <sys/stat.h>

namespace VisageSDK
{

CFBAEncoder *fba_action_fba_encoder;
#define OUT_BUFFER_SIZE 5000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FbaFileAction::~FbaFileAction()
{
	// Changed by KStankovic
	if (fbapsList != NULL)
	{
		int i;
		for (i = 0; i < maxFrames; i++)
			if (fbapsList[i] != NULL) delete fbapsList[i];
		free(fbapsList);
	}

	// Added by KStankovic
	if (timeList != NULL)
	//
		free(timeList);

	if (fbadec) {
		delete fbadec;
		fbadec = NULL;
	}
	if(audioBuffer)
		free(audioBuffer);

	//OpenAL
	if(audioResource)
		delete audioResource;

	// Added by KStankovic
	if(filter != NULL)
		delete filter;
	//
}

FbaFileAction::FbaFileAction()
{
	actionFinished = false;
	audioResource = NULL;
	fbadec = NULL;
	maxFrames = 100;
	init();
}

FbaFileAction::FbaFileAction(int nFrames, int frameRate)
{
	audioResource = NULL;
	fbadec = NULL;
	maxFrames = nFrames;
	init();
	this->nFrames = nFrames;

	float t = 0;
	float ts = 1000.0f/frameRate;
	FBAPs zeroFbaps;
	for(int i = 0;i<nFrames;i++)
	{
		fbapsList[i]->set(&zeroFbaps);
		timeList[i]=(long)t;
		t+=ts;
	}
	loaded = 1;
	streamLoaded = 1;
}

void FbaFileAction::init()
{
	velocity = 1.0;
	loaded = 0;
	globalStartTime = 0L;
	currentFaps = 0;
	started = 0;
	streamLoaded = 0;
	nFrames = 0;
	fbapsList = (FBAPs**) malloc(maxFrames * sizeof(FBAPs*));
	for(int i=0;i<maxFrames;i++)
		fbapsList[i] = new FBAPs();
	timeList = (long*) malloc(maxFrames * sizeof(long));
	isAudio = 0;
	mute = 0;
	audioBuffer = 0;
	loopMode = 0;
	timeOffset = 0;
	speed = 1.0;
	filter = 0;
	audioDelay = 0;
}





void replaceExtension(char *fn, char *ext)
{
	if(strlen(fn) < 5)
		return;

	fn[strlen(fn) - 4] = 0;
	strcat(fn,ext);
}


FbaFileAction::FbaFileAction(char *_fbaFile)
{
	FILE *fp;
	
	if(_fbaFile)
	{
		strcpy(audioFile,_fbaFile);
		replaceExtension(audioFile,".wav");
		// does audio file exist?
		if((fp = fopen(audioFile,"r")))
		{
			fclose(fp);
			loadFiles(_fbaFile, audioFile, 1); // if yes, load face animation and sound
		} 
		else 
		{
			audioFile[0] = 0;
			audioResource = NULL;
			loadFiles(_fbaFile, 0, 1); // else, load animation only
			//loadFiles(_fbaFile, NULL, 1); // else, load animation only
		}
	}
}

FbaFileAction::FbaFileAction(char *_fbaFile, char *_audioFile)
{
			loadFiles(_fbaFile, _audioFile, 1); 
}

FbaFileAction::FbaFileAction(char *_fbaFile, char *_audioFile, bool _download)
{
	loadFiles(_fbaFile,_audioFile, _download);
}

void FbaFileAction::loadFiles(char *_fbaFile, char *_audioFile, bool _download)
{
	fbadec = NULL;
	maxFrames = 100;
	init();

	if(_fbaFile)
	{
		strcpy(fbaFile,_fbaFile);
		strcpy(filterFile,_fbaFile);
		replaceExtension(filterFile,".faf");
	}
	else
	{
		fbaFile[0] = 0;
		filterFile[0] = 0;
	}
	fbadec = new CFBADecoder(_fbaFile);
	for (int j = 30; j-- > 0;)
		decodeFrame();

	if(_audioFile)
	{
		strcpy(audioFile,_audioFile);
		setAudioTrack(_audioFile);
	}
	else
		audioFile[0] = 0;

	loadFilter();

	if(_download)
		download();

	if (fbadec) {
		delete fbadec;
		fbadec = NULL;
	}

}

void FbaFileAction::discardFilter()
{

	if(filter)
		delete filter;
	filter = 0;
}

// load the information from the filter file
void FbaFileAction::loadFilter()
{
	FILE *f;
	char buf[200];
	char *s;

	if(filter)
	{
		delete filter;
		filter = 0;
	}

	if(!(f = fopen(filterFile,"r")))
	{
		filter = 0; 
		return;
	}
	filter = new FAPs();
	while(fgets(buf,500,f))
	{
		if(buf[0] == '#' || strlen(buf) < 2)
			continue;

		if(!strncmp(buf,"TIME OFSET",10))
		{
			s = buf;
			while(s[0] && s[0]!=':')
				s++;
			if(!s[0])
			{
				char msg[300];
				sprintf(msg,"Filter file %s: wrong format (TIME OFSET).",filterFile);
				#ifdef WIN32
				MessageBox(0,msg,"Filter file format error",MB_ICONERROR);
				#endif
				exit(0);
			}
			s++;
			timeOffset = atoi(s);
			continue;
		}

		if(!strncmp(buf,"SPEED",5))
		{
			s = buf;
			while(s[0] && s[0]!=':')
				s++;
			if(!s[0])
			{
				char msg[300];
				sprintf(msg,"Filter file %s: wrong format (SPEED).",filterFile);
				#ifdef WIN32
				MessageBox(0,msg,"Filter file format error",MB_ICONERROR);
				#endif
				exit(0);
			}
			s++;
			speed = (float)atof(s) / 1000.0f;
			continue;
		}

		if(!strncmp(buf,"LOOP",4))
		{
			s = buf;
			while(s[0] && s[0]!=':')
				s++;
			if(!s[0])
			{
				char msg[300];
				sprintf(msg,"Filter file %s: wrong format (LOOP).",filterFile);
				#ifdef WIN32
				MessageBox(0,msg,"Filter file format error",MB_ICONERROR);
				#endif
				exit(0);
			}
			s++;
			loopMode = atoi(s);
			continue;
		}

		// it is not a comment, an empty line, or any of other parameters, so it must be a filter value
		int fapno = atoi(buf); // fap number
		s = buf;
		while(s[0] && isalnum(s[0]))
			s++;
		if(!s[0])
		{
			char msg[300];
			sprintf(msg,"Filter file %s: wrong format (filter fap number).",filterFile);
			#ifdef WIN32
			MessageBox(0,msg,"Filter file format error",MB_ICONERROR);
			#endif
			exit(0);
		}
		while(s[0] && !isalnum(s[0]))
			s++;
		if(!s[0])
		{
			char msg[300];
			sprintf(msg,"Filter file %s: wrong format (filter value).",filterFile);
			#ifdef WIN32
			MessageBox(0,msg,"Filter file format error",MB_ICONERROR);
			#endif
			exit(0);
		}
		if(fapno == 1)
			filter->fap1.blend = atoi(s);
		if(fapno == 2)
			filter->fap2.intensity1 = atoi(s);
		if(fapno >= 3 && fapno <= 68)
			filter->llf.value[fapno-1] = atoi(s);
	}
	fclose(f);
}

void FbaFileAction::setAudioTrack(char *_audioFile)
{
	if(_audioFile)
	{

#ifdef MSVC
		if(_audioFile[0])
		{
			#ifdef MSVC
				int fh = _open(_audioFile, _O_BINARY | _O_RDONLY);
			#else
				int fh = open(_audioFile, O_RDONLY);
			#endif

			if(fh < 0)
			{
				char tmps[399];
				sprintf(tmps,"Can not open audio track %s!",_audioFile);
				#ifdef WIN32
				MessageBox(0,tmps,"Error",MB_ICONERROR);
				#endif
				return;
			}
			isAudio = 1;
			if(audioBuffer)
				free(audioBuffer);
			
			int bc = 0;
			int loadLength = 1000;
			int bs = loadLength;
			int l;
			audioBuffer = (unsigned char *)malloc(bs);
			do
			{
				if(bc + loadLength > bs)
				{
					bs += loadLength;
					audioBuffer = (unsigned char *)realloc(audioBuffer, bs);
				}
#ifdef MSVC
				l = _read(fh,audioBuffer+bc,loadLength);
#else
				l = read(fh,audioBuffer+bc,loadLength);
#endif
				bc+=l;
			}
			while(l!=0);
			#ifdef WIN32
				_close(fh);
			#else
				close(fh);
			#endif
		}

#else

	audioResource = new AudioResource(_audioFile);
    //NaradaNotificationCenter::subscribeNotification(VirtualCharacter::naradaNotificationShutUp, (void*)&shutUp);

    isAudio = 1;
    mute = 0;
    printf("audio track AL set\n");


#endif
	}
}

void FbaFileAction::decodeFrame()
{
	if(!streamLoaded)
	{
		int unFinished = fbadec->decodeFrame();
		fbapsList[nFrames]->set(fbadec->getFBAPs());
		timeList[nFrames] = fbadec->getTimeStamp();
		nFrames++;
		if(nFrames==maxFrames)
		{
			int old_maxframes = maxFrames;
			maxFrames *= 2;
			fbapsList = (FBAPs**) realloc(fbapsList, maxFrames * sizeof(FBAPs*));
			for(int i=old_maxframes;i<maxFrames;i++)
				fbapsList[i] = new FBAPs();
			timeList = (long*) realloc(timeList, maxFrames * sizeof(long));
		}
		if(!unFinished)
			streamLoaded = true;
	}
}

void FbaFileAction::download()
{
	do
	{
		decodeFrame();
	}
	while (!streamLoaded);
}

int FbaFileAction::getNFrames()
{
	return(nFrames);
}

float FbaFileAction::getFrameRate()
{
	float td = (float)(timeList[nFrames-1] - timeList[0]) / 1000.0f;
	if(td==0.0) return(1);
	return((float)(nFrames-1) / td);
}


void FbaFileAction::setFBAPs(FBAPs *fbaps, int nfr)
{
	if(nfr<0 || nfr >= nFrames)
		return;

	fbapsList[nfr]->set(fbaps);
}

FBAPs *FbaFileAction::getFBAPs(long globalTime, FBAPs *lastFBAPs, VisageCharModel *model)
{
	if(!started)
		return NULL;

	localTime = globalTime - globalStartTime;
	localTime *= (long)velocity;

	if(localTime<0) // added by -KaiK-, to not load frame 0 of delayed actions
		return NULL;

	int fr = getFrame(localTime);
	
	return(getFBAPs(fr));
}
	

bool FbaFileAction::isFinished()
{
	return actionFinished;
}


bool FbaFileAction::isBeingPlayed()
{
	if(actionFinished)
		return false;

	return (localTime<0)?false:true;
}
	

FBAPs *FbaFileAction::getFBAPs(int fr)
{
	if(!started)
		return NULL;
	if(fr>=nFrames || fr<0)
	{
		actionFinished = true;
		return(NULL);
	}

	filteredFBaps.set(fbapsList[fr]);
	if(filter)
		filteredFBaps.faps->filter(filter);

	return(&filteredFBaps);
}



void FbaFileAction::setVelocity(double ve)
{
	velocity = ve;
}



void FbaFileAction::start(long globalTime)
{
	globalStartTime = globalTime;
	localTime = -1;

	if (isAudio && !mute)
	{
		printf("start audio is executed\n");
        printf("start audio yeah\n");
//		#if WIN32
//				sndPlaySound((char*)audioBuffer,SND_ASYNC | SND_MEMORY );
//		#else
				audioResource->play();
//		#endif
	}
	currentFaps = 0;
	actionFinished = false;
	started = true;
}



void FbaFileAction::stop()
{
	if (isAudio && !mute)
	{
//		#ifdef WIN32
//				sndPlaySound(NULL,SND_ASYNC);
//		#else
				audioResource->stop();
				audioResource->rewind();
//		#endif
	}
	started = false;
	currentFaps = 0;
}
    
    
void FbaFileAction::shutUp()
{
    if (isAudio && !mute)
    {
        //		#ifdef WIN32
        //				sndPlaySound(NULL,SND_ASYNC);
        //		#else
        audioResource->stop();
        audioResource->rewind();
        //		#endif
        
        started = false;
        currentFaps = 0;
    }

}



int FbaFileAction::getFrame(long t)
{
	int fr = 0;
	t-=timeOffset;
	t=(long)(t*speed);
	static int nRepetitions = 0;
	
	if(loopMode)
	{
		long tl = t;
		tl%=timeList[nFrames-1];

		if(t / timeList[nFrames-1] > nRepetitions)
		{
			nRepetitions++;
			if (isAudio && !mute)
			{
//				#if WIN32
//					sndPlaySound((char*)audioBuffer,SND_ASYNC | SND_MEMORY );
//				#else
					audioResource->play();
//				#endif
			}
		}

		t = tl;
	}
	
	while(fr < nFrames)
	{
		if(t <= timeList[fr])
		{
			if(fr==0)
				return(fr);
			if( t - timeList[fr-1] < timeList[fr] - t)
				return(fr-1);
			else
				return(fr);
		}
		fr++;
	}
	/*
	while(fr < nFrames-1)
	{
		if(t >= timeList[fr] && t <= timeList[fr+1])
		{
			if( t - timeList[fr] < timeList[fr+1] - t)
				return(fr);
			else
				return(fr+1);
		}
		fr++;
	}*/

	return(-1);
}


void _writeBits(int streamHandle, unsigned char *bits, int size)
{
	static unsigned char byte = 0;
	static int bitcnt = 0;
	int i,j;
	int nbytes = size / 8;
	int nrestbits = size % 8;
	int bit;

	if(!streamHandle)
	{
		byte = 0;
		bitcnt = 0;
		return;
	}

	if(nrestbits)
	{
		nbytes++; //take last incomplete byte into account
		bits[nbytes-1] <<= (8-nrestbits); // shift bits in last byte to the edge
	}

	for(i=0;i<nbytes;i++)
	{
		for(j=0;j<8;j++)
		{
			bit = 128 & bits[i];
			bits[i]<<=1;

			byte<<=1;
			if(bit) 
				byte |= 1;
			bitcnt++;

			if(bitcnt==8)
			{
				#ifdef WIN32
					_write(streamHandle,&byte,1);
				#else
					write(streamHandle,&byte,1);
				#endif
				bitcnt=0;
			}

			size--;
			if(size == 0) 
				break;
		}
		if(size == 0) 
			break;
	}
}
/*
void _writeBits(FILE *streamHandle, unsigned char *bits, int size)
{
	static unsigned char byte = 0;
	static int bitcnt = 0;
	int i,j;
	int nbytes = size / 8;
	int nrestbits = size % 8;
	int bit;

	if(!streamHandle)
	{
		byte = 0;
		bitcnt = 0;
		return;
	}

	if(nrestbits)
	{
		nbytes++; //take last incomplete byte into account
		bits[nbytes-1] <<= (8-nrestbits); // shift bits in last byte to the edge
	}

	for(i=0;i<nbytes;i++)
	{
		for(j=0;j<8;j++)
		{
			bit = 128 & bits[i];
			bits[i]<<=1;

			byte<<=1;
			if(bit) 
				byte |= 1;
			bitcnt++;

			if(bitcnt==8)
			{
				fwrite(&byte,1,1,streamHandle);
				bitcnt=0;
			}

			size--;
			if(size == 0) 
				break;
		}
		if(size == 0) 
			break;
	}


//	_commit(streamHandle);
 	
}
*/

void FbaFileAction::encode_frame(FBAPs *f,int fbaFileHandle)
{
#ifdef DO_DEBUG_PRINT
DebugPrint("FbaFileAction::encode_frame 1\n");
#endif
	unsigned char outBuffer[OUT_BUFFER_SIZE];
	long pts;
	int size = 1000;

	/*
	FBAPs fbaps;
	int i;
	for(i=0;i<N_FAP_GROUP;i++)
		fbaps.faps->maskType[i] = f->faps->maskType[i];
	for(i=0;i<NFAP;i++)
	{
		fbaps.faps->groupMask[i] = f->faps->groupMask[i];
		fbaps.faps->llf.value[i] = f->faps->llf.value[i];
	}
	fbaps.faps->fap1.viseme2= f->faps->fap1.viseme2;
	fbaps.faps->fap1.blend = f->faps->fap1.blend;
	fbaps.faps->fap1.defBit = 0;
	fbaps.faps->fap1.viseme1= f->faps->fap1.viseme1;

	fbaps.faps->fap2.expression1= f->faps->fap2.expression1;
	fbaps.faps->fap2.expression2= f->faps->fap2.expression2;
	fbaps.faps->fap2.intensity1= f->faps->fap2.intensity1;
	fbaps.faps->fap2.intensity2= f->faps->fap2.intensity2;
	fbaps.faps->fap2.initBit= f->faps->fap2.initBit;
	fbaps.faps->fap2.defBit = 0;

	for(i=0;i<BAP_NUM_GROUPS;i++)
		fbaps.baps->maskType[i] = f->baps->maskType[i];
	for(i=0;i<NBAP;i++)
	{
		fbaps.baps->groupMask[i] = f->baps->groupMask[i];
		fbaps.baps->value[i] = f->baps->value[i];
	}
	*/
	size = fba_action_fba_encoder->encodeFrame(f,outBuffer, OUT_BUFFER_SIZE, &pts);
	_writeBits(fbaFileHandle,outBuffer,size);
}

void FbaFileAction::smoothZeroValues()
{
	FBAPs *fbaps0 = new FBAPs(fbapsList[0]);
	int nf = getNFrames();

	// find out which FBAPs are animated by setting the mask
	fbaps0->setMask(false);
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i =1;i<nf;i++)
	{
		FBAPs *fbaps = fbapsList[i];
		fbaps0->setMaskIfDiff(fbaps);
	}
	/*for(i =0;i<nf;i++)
	{
		FBAPs *fbaps = fbapsList[i];
		for (int i = 2; i < NFAP; i++)
		{
			if(fbaps0->faps->groupMask[i])
				if(!fbaps->faps->llf.value[i])
					fbaps->faps->llf.value[i] = 1;
		}
		for (i = 0; i < NUM_BAPS; i++)
		{
			if(fbaps0->baps->groupMask[i])
				if(!fbaps->baps->value[i])
					fbaps->baps->value[i] = 1;
		}
	}*/
	//CHANGED BY I.K. replaced above loop with the one below,
	//due to very dodgy use of iterator regarding its scope
	for(i =0;i<nf;i++)
	{
		FBAPs *fbaps = fbapsList[i];
		int j;
		for (j = 2; j < NFAP; j++)
		{
			if(fbaps0->faps->groupMask[j])
				if(!fbaps->faps->llf.value[j])
					fbaps->faps->llf.value[j] = 1;
		}
		for (j = 0; j < NUM_BAPS; j++)
		{
			if(fbaps0->baps->groupMask[j])
				if(!fbaps->baps->value[j])
					fbaps->baps->value[j] = 1;
		}
	}

	// Added by KStankovic
	delete fbaps0;
	//
}

void FbaFileAction::optimizeForWriting()
{
	FBAPs *fbaps0 = fbapsList[0];
	int nf = getNFrames();

	fbaps0->setMask(false);
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i =1;i<nf;i++)
	{
		FBAPs *fbaps = fbapsList[i];
		fbaps0->setMaskIfDiff(fbaps);
	}
	for(i =1;i<nf;i++)
	{
		FBAPs *fbaps = fbapsList[i];
		fbaps->setMask(fbaps0);
	}
}

void FbaFileAction::write(char *fn)
{
	write(fn,true);
}

void FbaFileAction::write(char *fn, bool optimize)
{
	fba_action_fba_encoder = new CFBAEncoder();
	fba_action_fba_encoder->initializeEncoding();
	_writeBits(0,0,0); // initialize the bit writing utility

	#ifdef WIN32
		_unlink(fn); //just to make sure it does not exist
		int fbaFileHandle = _open( fn, _O_WRONLY | _O_CREAT | _O_BINARY, _S_IREAD | _S_IWRITE  );
	#else
		unlink(fn); //just to make sure it does not exist
		int fbaFileHandle = open( fn, O_WRONLY | O_CREAT, S_IRWXG );
	#endif
	if (fbaFileHandle == -1) 
	{
		#ifdef WIN32
		MessageBox (0,"Can't open the FBA file!","Error",MB_ICONERROR);
		#endif
		return;
	}
	if(optimize)
		optimizeForWriting();

	int tmpMute;

	tmpMute = mute;
	mute = 1;
	stop();
	start(timeList[0]);
	mute = tmpMute;

	for(int i =0;i<getNFrames();i++)
	{
		FBAPs *fbaps = getFBAPs(i);
		encode_frame(fbaps,fbaFileHandle);
	}

	unsigned char outBuffer[OUT_BUFFER_SIZE];
	long pts;
	int size;
	size = fba_action_fba_encoder->finalizeBitstream(outBuffer, OUT_BUFFER_SIZE, &pts);
	_writeBits(fbaFileHandle,outBuffer,size);
	#ifdef WIN32
		_close(fbaFileHandle);
	#else
		close(fbaFileHandle);
	#endif
//	fclose(fbaFileHandle);
	delete fba_action_fba_encoder;
}

void FbaFileAction::merge(FbaAction **fa, int nfa)
{
	int i,j;
	long currentTime;
	FBAPs lastFBaps;
	int tmpMute;

	tmpMute = mute;
	mute = 1;
	stop();
	start(timeList[0]);
	mute = tmpMute;

	for(i=0;i<nfa;i++)
	{
		if(!strcmp(fa[i]->actionTypeName(),"FbaFileAction"))
		{
			tmpMute = ((FbaFileAction*)fa[i])->mute;
			((FbaFileAction*)fa[i])->mute = 1;
		}
		fa[i]->stop();
		fa[i]->start(timeList[0]);
		if(!strcmp(fa[i]->actionTypeName(),"FbaFileAction"))
		{
			((FbaFileAction*)fa[i])->mute = tmpMute;
		}
	}

	for(i = 0;i < nFrames; i++)
	{
		FBAPs *fbaps = getFBAPs(i);
		if(!fbaps)
			continue;
		currentTime = timeList[i];
		for(j=0;j<nfa;j++)
		{
			FBAPs *fbaps1 = fa[j]->getFBAPs(currentTime,&lastFBaps,NULL);
			if(fbaps1) fbaps->merge(fbaps1);
		}
		setFBAPs(fbaps,i);
		lastFBaps.set(fbaps);
	}
}

}
