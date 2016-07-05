

#include "CVisemes.h"
#include "LipSync.h"
#include "wavePlay.h"
#include "mfcc.h"
#ifdef MSVC
#include "wavio.h"
#endif
#include "NeuralNetwork.h"


// C++
#include <stdlib.h>

#ifdef NDEBUG
#define USE_SLEEP 1
#else 
#define USE_SLEEP 0
#endif

//#define OUT_BUFFER_SIZE 10000
//#define OUT_BUFFER_SIZE 200000


namespace VisageSDK
{


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVisemes::CVisemes(VisageLipSync *_lipSync)
{
	fa=NULL;
	af=NULL;
	encoder=NULL;
	realVisemeVector=NULL;
	frameBeggining = 0;

	constructStreamingSound();
	lipSync = _lipSync;
	last_tms = 0;
	frameCount = 0;
}

CVisemes::~CVisemes()
{
	if (realVisemeVector)
	{
		delete [] realVisemeVector;
		realVisemeVector=NULL;
	}
	if (af)
	{
		delete [] af;
		af=NULL;
	}

/*	if (fa)
	{
		delete [] fa;
		fa=NULL;
	}*/

	//delete [] Classes;
	//FreeAllGeneratedNNetworks();
}

//ne koristi se, doraditi
void CVisemes::ClearAll()
{
	if (realVisemeVector)
	{
		delete [] realVisemeVector;
		realVisemeVector=NULL;
	}
	if (af)
	{
		delete [] af;
		af=NULL;
	}
	if (encoder)
	{
		delete [] encoder;
		encoder = NULL;
	}
	return;
}

void CVisemes::FreeAllGeneratedNNetworks()
{
	FreeNetwork(&n0);
	FreeNetwork(&n1);
	FreeNetwork(&n2);
	FreeNetwork(&n4);
	FreeNetwork(&n5);
	FreeNetwork(&n6);
	FreeNetwork(&n7);
	FreeNetwork(&n8);
	FreeNetwork(&n9);
	FreeNetwork(&n10);
	FreeNetwork(&n11);
	FreeNetwork(&n12);
	FreeNetwork(&n13);
	FreeNetwork(&n14);

}

//-----------------------------------------------------------------------------
// Name: PreprocessWavFile()
// Desc: Calles CalculateVisemes() which performs Digital Signal Processing on 
// the .wav file and calculates the visemes in advanced. Used in "PreProcessning mode".
//-----------------------------------------------------------------------------
int* CVisemes::PreprocessWavFile(char* strFileName)
{
	int* Visemes = CalculateVisemes(strFileName);
	return Visemes;
}


//-----------------------------------------------------------------------------
// Name: CStreamingSound::CalculateVisemes()
// Desc: Calculates the visemes in advance. Used in "PreProcessing mode"
//-----------------------------------------------------------------------------

int* CVisemes::CalculateVisemes(char* strFileName)
{
#ifdef MSVC
	double PreProcClasses[15];
	double PreProcLargest;
	double OutputValues[15];
	int index;

	mfcc_SIGNAL PreProcSampleValues;
	mfcc_SIGNAL PreProcOverlapFrame;
	mfcc_VECTORLIST PreProcPreFisherCoeff;

	long PreProcFramecount=0;

	PreProcOverlapFrame.signal_data = new double[256];
	PreProcOverlapFrame.signal_data_lenght = 256;
	PreProcOverlapFrame.fs = 16000;

	for (int k=0; k< 15; k++)
		PreProcClasses[k]=0;


	GenerateNetwork(&n0,200);
	GetWeights(&n0,"NN/0Weight.txt");
    GenerateNetwork(&n1,100);
    GetWeights(&n1,"NN/1Weight.txt");
    GenerateNetwork(&n2,300);
    GetWeights(&n2,"NN/2Weight.txt");
    //GenerateNetwork(&n3,200);
    //GetWeights(&n3,"NN/3Weight.txt");
    GenerateNetwork(&n4,200);
    GetWeights(&n4,"NN/4Weight.txt");
    GenerateNetwork(&n5,150);
    GetWeights(&n5,"NN/5Weight.txt");
    GenerateNetwork(&n6,200);
    GetWeights(&n6,"NN/6Weight.txt");
    GenerateNetwork(&n7,200);
    GetWeights(&n7,"NN/7Weight.txt");
    GenerateNetwork(&n8,200);
    GetWeights(&n8,"NN/8Weight.txt ");
    GenerateNetwork(&n9,200);
    GetWeights(&n9,"NN/9Weight.txt");
    GenerateNetwork(&n10,200);
    GetWeights(&n10,"NN/10Weight.txt");
    GenerateNetwork(&n11,200);
    GetWeights(&n11,"NN/11Weight.txt");
    GenerateNetwork(&n12,100);
    GetWeights(&n12,"NN/12Weight.txt");
    GenerateNetwork(&n13,170);
    GetWeights(&n13,"NN/13Weight.txt ");
    GenerateNetwork(&n14,80);
    GetWeights(&n14,"NN/14Weight.txt");	

	LoadWav(strFileName,&PreProcSampleValues);
	
	int NrSamples= PreProcSampleValues.signal_data_lenght;
	int NumFrames=lipSync->bNumberOfFrames;
	int FirstViseme=((4*NumFrames)-3);
	int OtherVisemes=4*NumFrames;
	//int num=(int)(floor(NrSamples/(256*NumFrames))) + 1;	
	int num=(NrSamples/(256*NumFrames)) + 1;	//CHANGED BY I.K. floor() redundant

	
	int* PreProcVisemes;
	int m=0;

	//total number of visemes 
	numVisemes = num;

	PreProcVisemes = new int[num];
	PreProcVisemes[num-1]=0;

	//for(int i=0; i<(floor(NrSamples/64)-3); i++)
	for(int i=0; i<((NrSamples/64)-3); i++)	//CHANGED BY I.K. floor() redundant
	{
		for(int k=0; k<256; k++) {
			
			PreProcOverlapFrame.signal_data[k]=PreProcSampleValues.signal_data[64*i+k];
		}

		PreProcOverlapFrame.fs = PreProcSampleValues.fs; //frekvencija za ekstrakciju MFCC/ova, 16000 Hz
		Mfcc(PreProcOverlapFrame,12,256,&PreProcPreFisherCoeff); 

		SimulateNet(&n0,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[0]);
		SimulateNet(&n1,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[1]);
		SimulateNet(&n2,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[2]);
		SimulateNet(&n4,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[4]);
		SimulateNet(&n5,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[5]);
		SimulateNet(&n6,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[6]);
		SimulateNet(&n7,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[7]);
		SimulateNet(&n8,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[8]);
		SimulateNet(&n9,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[9]);
		SimulateNet(&n10,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[10]);
		SimulateNet(&n11,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[11]);
		SimulateNet(&n12,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[12]);
		SimulateNet(&n13,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[13]);
		SimulateNet(&n14,&PreProcPreFisherCoeff.vector->data[0],&OutputValues[14]);

		for(int i1 = 0; i1 < 15; i1++)
			PreProcClasses[i1] = PreProcClasses[i1] + OutputValues[i1];

		if((i+1) == FirstViseme)
		{
			PreProcLargest = PreProcClasses[0];
			index = 0;

			for (int t=1; t< 15; t++){
				if (PreProcClasses[t] > PreProcLargest){
					PreProcLargest = PreProcClasses[t];
					index = t;
				}
			}

			PreProcVisemes[PreProcFramecount]= index;
			
			if(index==3 || index < 0 || index > 14)
				PreProcVisemes[PreProcFramecount]=0;

			PreProcFramecount++;
			index = 0;
			for(int p = 0; p < 15; p++)
				PreProcClasses[p]=0;
		}

		m = i-(FirstViseme-1);

		if( ((m % OtherVisemes) == 0) & (m > 0))
		{
			PreProcLargest = PreProcClasses[0];
			index = 0;

			for (int t=1; t< 15; t++) {
				if (PreProcClasses[t]>PreProcLargest){
					PreProcLargest = PreProcClasses[t];
					index = t;
				}
			}

			PreProcVisemes[PreProcFramecount]= index;

			if(index==3 || index <0 || index>14)
				PreProcVisemes[PreProcFramecount]=0;

			PreProcFramecount++;
			index = 0;
			for(int p=0; p<15; p++)
				PreProcClasses[p]=0;
		}
	}
	CloseSignal(&PreProcSampleValues);
	return PreProcVisemes;
#else
	return 0;
#endif
}

//-----------------------------------------------------------------------------
// Name: StoreAnimation()
// Desc: Creates animation (fba) file from previously calculated visemes 
//-----------------------------------------------------------------------------
#ifdef MSVC
void CVisemes::StoreAnimation3()
{	
	//FBAPs *fbaps = new FBAPs();
	
	//retrives visemes calculated in real time mode
	if(lipSync->bRealTime)
	{
		int num = (int)allVisemes.size();	//CHANGED BY I.K. added explicit cast
		lipSync->nVisemes = num;
		realVisemeVector = new int[num];
		realVisemeVector[num-1]=0;
	
		//allVisemes vector containes visemes calculated in the real time 
		int count = 0;
		for (allVisemesIterator = allVisemes.begin(); allVisemesIterator != allVisemes.end(); allVisemesIterator++)
		{
			realVisemeVector[count] = *allVisemesIterator;
			count++; 
		}

		allVisemes.clear();
		lipSync->Visemes = realVisemeVector;
	}
	//lipSync->Visemes contains visemes that need to be stored in animation file
	int* VisemeVector = lipSync->Visemes;
	//number of visemes that need to be stored in fba file
	numVisemes = lipSync->nVisemes;


	//////encoding//////////////
	//opens new empty (fba) file
	_unlink(lipSync->fbaFileName); //just to make sure it does not exist
	fbaFileHandle = _open( lipSync->fbaFileName, _O_WRONLY | _O_CREAT | _O_BINARY|_O_TRUNC, _S_IREAD | _S_IWRITE );
	if (fbaFileHandle == -1) 
	{
		MessageBox (0,"visagesapi5tts: Can't open the FBA file!","Error",MB_ICONERROR);
	}

	//initialization
	writeBits(0,0,0);
	last_ts = -1;
	ev1 = ev2 = 0;
	eb = 0.0;
	int isLastFrame = 0;
	encoder = new CFBAEncoder();
	encoder->initializeEncoding();
	encoder->getCodingParameters()->paramset_mask= 1;
	encoder->getCodingParameters()->frame_rate = 50;

	encode_viseme(0, 0, 0);
	////////////

	int num = (int)allFBAPs.size();	
	static long pts=0;
	last_viseme = 0;
	tempFBAPs = new FBAPs();

	//allVisemes vector containes visemes calculated in the real time 
	int count = 0;
	for (allFBAPsIterator = allFBAPs.begin(); allFBAPsIterator != allFBAPs.end(); allFBAPsIterator++)
	{
		tempFBAPs = *allFBAPsIterator;
		
		//tempFBAPs = tFBAPs.fbaps;
		viseme = tempFBAPs->faps->fap1.viseme1;
		//ts = tFBAPs.fbaps_tms;
		ts = (long)(count*((lipSync->fileTime)/num)*1000);

		if (count == num-1)
			isLastFrame = 1;

		encode_viseme(viseme, ts, isLastFrame);

		if(count != 0) 
			last_viseme = viseme;
		count++;
	}
	allFBAPs.clear();
	

	size = encoder->finalizeBitstream(outBuffer, OUT_BUFFER_SIZE, &pts);
	//save animation in file
	writeBits(fbaFileHandle,outBuffer,size);
	_close(fbaFileHandle);
	if (outBuffer)
		outBuffer[OUT_BUFFER_SIZE]=NULL;

	return;
}
#endif



#ifdef MSVC
void CVisemes::StoreAnimation()
{	
	//FBAPs *fbaps = new FBAPs();
	
	//retrives visemes calculated in real time mode
	if(lipSync->bRealTime)
	{
		int num = (int)allVisemes.size();	//CHANGED BY I.K. added explicit cast
		lipSync->nVisemes = num;

		realVisemeVector = new int[num];
		realVisemeVector[num-1]=0;
	
		//allVisemes vector containes visemes calculated in the real time 
		int count = 0;
		for (allVisemesIterator = allVisemes.begin(); allVisemesIterator != allVisemes.end(); allVisemesIterator++)
		{
			realVisemeVector[count] = *allVisemesIterator;
			count++; 
		}

		allVisemes.clear();
		lipSync->Visemes = realVisemeVector;
	}

	//lipSync->Visemes contains visemes that need to be stored in animation file
	// static int* VisemeVector = lipSync->Visemes;
	int* VisemeVector = lipSync->Visemes;
	//number of visemes that need to be stored in fba file
	numVisemes = lipSync->nVisemes;
	

	//////encoding//////////////
	//opens new empty (fba) file
	_unlink(lipSync->fbaFileName); //just to make sure it does not exist
	fbaFileHandle = _open( lipSync->fbaFileName, _O_WRONLY | _O_CREAT | _O_BINARY|_O_TRUNC, _S_IREAD | _S_IWRITE );
	if (fbaFileHandle == -1) 
	{
		MessageBox (0,"visagesapi5tts: Can't open the FBA file!","Error",MB_ICONERROR);
	}

	//initialization
	writeBits(0,0,0);
	last_ts = -1;
	ev1 = ev2 = 0;
	eb = 0.0;
	int isLastFrame = 0;
	encoder = new CFBAEncoder();
	encoder->initializeEncoding();
	encoder->getCodingParameters()->paramset_mask= 1;

	encode_viseme(0, 0, 0);
	////////////
	last_viseme = 0;
	for (int vs = 0; vs < numVisemes; vs++)
	{ 		
		viseme = VisemeVector[vs];
		if(vs != 0) last_viseme = VisemeVector[vs-1];
		//fbaps->faps->fap1.viseme1 = viseme;

		//lipSync->fileTime ---- animation file should be this long
		//ts ---- moment in which comes certain viseme
		ts = (long)(vs*((lipSync->fileTime)/numVisemes)*1000);

		if (vs==numVisemes-1)
			isLastFrame = 1;
	

		encode_viseme(viseme, ts, isLastFrame);
	}

	long pts;

//	encoder->finalizeBitstream(outBuffer, &size, &pts);
	size = encoder->finalizeBitstream(outBuffer, OUT_BUFFER_SIZE, &pts);
	//save animation in file
	writeBits(fbaFileHandle,outBuffer,size);
	_close(fbaFileHandle);
	if (outBuffer)
		outBuffer[OUT_BUFFER_SIZE]=NULL;
	


	return;
}

#endif



void CVisemes::InitStoreAnimation()
{
    cout << "fba file name: " << lipSync->fbaFileName << endl;
	#ifdef WIN32
		_unlink(lipSync->fbaFileName); //just to make sure it does not exist
		fbaFileHandle = _open( lipSync->fbaFileName, _O_WRONLY | _O_CREAT | _O_BINARY|_O_TRUNC, _S_IREAD | _S_IWRITE );
	#else
		unlink(lipSync->fbaFileName); //just to make sure it does not exist
		fbaFileHandle = open(lipSync->fbaFileName, O_WRONLY | O_CREAT | O_TRUNC, 0755);
	#endif

		if (fbaFileHandle == -1) 
	{
#ifdef MSVC
		MessageBox (0,"visagesapi5tts: Can't open the FBA file!","Error",MB_ICONERROR);
#else
        std::cout << "CANNOT open the FBA file to Store Animation. Path: " << lipSync->fbaFileName << std::endl;
#endif
	}

	//initialization
	writeBits(0,0,0);
	last_ts = -1;
	ev1 = ev2 = 0;
	eb = 0.0;
	int isLastFrame = 0;
	encoder = new CFBAEncoder();
	encoder->initializeEncoding();
	encoder->getCodingParameters()->paramset_mask= 1;

	if (lipSync->h_heMode == LS_HE_MODE_FRAME_BASED)
		encoder->getCodingParameters()->frame_rate = 60;

	//set_audioBuf(0);
	encode_viseme(0, 0, 0);
	////////////
	last_viseme = 0;

	return;
}
void CVisemes::FinalizeStoreAnimation()
{	
	long pts;
//	encoder->finalizeBitstream(outBuffer, &size, &pts);
	size = encoder->finalizeBitstream(outBuffer, OUT_BUFFER_SIZE, &pts);
	//save animation in file
	writeBits(fbaFileHandle,outBuffer,size);
	#ifdef WIN32
		_close(fbaFileHandle);
	#else
		close(fbaFileHandle);
	#endif
	if (outBuffer)
		outBuffer[OUT_BUFFER_SIZE]=NULL;
	
	delete encoder;
	encoder = NULL;
	printf("encoder deleted\n");

	return;
}

//-----------------------------------------------------------------------------
// Name: CVisemes::constructStreamingSound()
// Desc:  
//-----------------------------------------------------------------------------
void CVisemes::constructStreamingSound()          
{
	framecount=10;
	framecount_mod=0;

	Classes[0]=0;
	Classes[1]=0;
	Classes[2]=0;
	Classes[3]=0;
	Classes[4]=0;
	Classes[5]=0;
	Classes[6]=0;
	Classes[7]=0;
	Classes[8]=0;
	Classes[9]=0;
	Classes[10]=0;
	Classes[11]=0;
	Classes[12]=0;
	Classes[13]=0;
	Classes[14]=0;

	GenerateNetwork(&n0,200);
	GetWeights(&n0,(char *)("NN/0Weight.txt"));
	GenerateNetwork(&n1,100);
    GetWeights(&n1,(char *)("NN/1Weight.txt"));
    GenerateNetwork(&n2,300);
    GetWeights(&n2,(char *)("NN/2Weight.txt"));
//    GenerateNetwork(&n3,200);
//    GetWeights(&n3,"NN/3Weight.txt");
    GenerateNetwork(&n4,200);
    GetWeights(&n4,(char *)("NN/4Weight.txt"));
    GenerateNetwork(&n5,150);
    GetWeights(&n5,(char *)("NN/5Weight.txt"));
    GenerateNetwork(&n6,200);
    GetWeights(&n6,(char *)("NN/6Weight.txt"));
    GenerateNetwork(&n7,200);
    GetWeights(&n7,(char *)("NN/7Weight.txt"));
    GenerateNetwork(&n8,200);
    GetWeights(&n8,(char *)("NN/8Weight.txt"));
    GenerateNetwork(&n9,200);
    GetWeights(&n9,(char *)("NN/9Weight.txt"));
    GenerateNetwork(&n10,200);
    GetWeights(&n10,(char *)("NN/10Weight.txt"));
    GenerateNetwork(&n11,200);
    GetWeights(&n11,(char *)("NN/11Weight.txt"));
    GenerateNetwork(&n12,100);
    GetWeights(&n12,(char *)("NN/12Weight.txt"));
    GenerateNetwork(&n13,170);
    GetWeights(&n13,(char *)("NN/13Weight.txt"));
    GenerateNetwork(&n14,80);
    GetWeights(&n14,(char *)("NN/14Weight.txt"));
 
}


//-----------------------------------------------------------------------------
// Name: CVisemes::HandleFrames()
// Desc: Handle the notification that tells us to put more wav data in the 
//       circular buffer
//----------------------------------------------------------------------------
//void CVisemes::HandleFrames( DWORD wBufSize, short* wBuf, bool* finished )
void CVisemes::HandleFrames( unsigned long wBufSize, short* wBuf, bool* finished, int fs )
{
	*finished = false;

	//FBAPs *fbaps = new FBAPs();	
	//FILE *testFP = fopen("saveFBA.txt", "a+");

	//if audio signal is processed in real time (audio file & microphone)
	//if((lipSync->bRealTime) & !(lipSync->bPreProcess))
	if(lipSync->bRealTime)
	{
		double OutputValues[15];
		mfcc_VECTORLIST FisherCoeff;

		int i;	//CHANGED BY I.K. declared iterator outside of loop
		for (i=0; i< 15; i++)
			OutputValues[i]= 0;

		int index = 0;
		double LargestValue = 0;
		
		mfcc_SIGNAL SampleValues;
		SampleValues.signal_data = new double[wBufSize/2];
		SampleValues.signal_data_lenght = wBufSize/2;
		//SampleValues.fs = 16000;
		SampleValues.fs = fs;

		for(i=0; i<(int)wBufSize/2; i++)	//CHANGED BY I.K. added explicit cast
		{	
			short data=wBuf[i];
			float value = ((float)data)/32768; //signed short => 2 bytes = 16 bits: -2¹⁶ <= data <= 2¹⁶ = 32768

			//SampleValues(i+1)=value;
			SampleValues.signal_data[i]=value;
		}

		framecount++;
		//fprintf(testFP,"framecount = %d\n", framecount);

		Mfcc(SampleValues,12,256,&FisherCoeff);
		
		SimulateNet(&n0,&FisherCoeff.vector->data[0],&OutputValues[0]);
		SimulateNet(&n1,&FisherCoeff.vector->data[0],&OutputValues[1]);
		SimulateNet(&n2,&FisherCoeff.vector->data[0],&OutputValues[2]);
		//SimulateNet(&n3,&FisherCoeff.vector->data[0],&OutputValues[3]);
		OutputValues[3] = 0;
		SimulateNet(&n4,&FisherCoeff.vector->data[0],&OutputValues[4]);
		SimulateNet(&n5,&FisherCoeff.vector->data[0],&OutputValues[5]);
		SimulateNet(&n6,&FisherCoeff.vector->data[0],&OutputValues[6]);
		SimulateNet(&n7,&FisherCoeff.vector->data[0],&OutputValues[7]);
		SimulateNet(&n8,&FisherCoeff.vector->data[0],&OutputValues[8]);
		SimulateNet(&n9,&FisherCoeff.vector->data[0],&OutputValues[9]);
		SimulateNet(&n10,&FisherCoeff.vector->data[0],&OutputValues[10]);
		SimulateNet(&n11,&FisherCoeff.vector->data[0],&OutputValues[11]);
		SimulateNet(&n12,&FisherCoeff.vector->data[0],&OutputValues[12]);
		SimulateNet(&n13,&FisherCoeff.vector->data[0],&OutputValues[13]);
		SimulateNet(&n14,&FisherCoeff.vector->data[0],&OutputValues[14]);
		

		for(int i1 = 0; i1 < 15; i1++)
		{
			Classes[i1] = Classes[i1] + OutputValues[i1];
		}
//		Classes[14] = 0;

		int timems = framecount*17;
		int isLastFrame = 0;

		//if(framecount % 5 == 0)
		if(framecount % lipSync->bNumberOfFrames == 0)
		{
			LargestValue = Classes[0];
			index = 0;
			
			for (int t=1; t< 15; t++)
			{
				if (Classes[t]>LargestValue)
				{
					LargestValue = Classes[t];
					index = t;
				}

				#ifdef WIN32
				if(USE_SLEEP) 	Sleep((unsigned long)0.006);	//CHANGED BY I.K. added explicit cast
#else
				if(USE_SLEEP)	usleep((unsigned long)6000);
#endif
			}

			if(index==3 || index <0 || index>14) {
				LargestValue=0;
				index=0;
			}
			
			for(i=0; i<15; i++)
				Classes[i]=0;
			
			//new calculated viseme
			viseme = index;
			//lipSync->fbaps->faps->fap1.viseme1 = viseme;

			//Spremanje u file u tijeku izvodjenja
			if (lipSync->currentViseme != viseme)
			{
				lipSync->currentViseme = viseme;
				//fprintf(testFP,"lipSync->currentViseme = %d\n", viseme);
				

				if (lipSync->h_heMode == LS_HE_MODE_VISEME_BASED)
				{
					if (!lipSync->bFbaFile) 
					{
						for(int k =0;k<lipSync->bEventHandler;k++)
							if(lipSync->eventH[k]) 
							{
								//comment by G. - ???[W] UMR: Uninitialized memory read in CVisemes::HandleFrames(DWORD,short *) 
								lipSync->eventH[k]->handleEvent(this->lipSync,lipSync->fbaps,(CodingParameters*)NULL,timems,viseme, wBuf);
							}
					}
					else
					{
						if (lipSync->stop_flag)
						{
							isLastFrame = 1;
							//encode_viseme(lipSync->currentViseme,lipSync->lsGlobalTime,isLastFrame);
							encode_viseme(lipSync->currentViseme,timems,isLastFrame);
							//fprintf(testFP,"save: viseme = %d, time = %d\n", lipSync->currentViseme,timems);
						}
						else
						{
							isLastFrame = 0;
							//encode_viseme(lipSync->currentViseme,lipSync->lsGlobalTime,isLastFrame);
							encode_viseme(lipSync->currentViseme,timems,isLastFrame);
							//fprintf(testFP,"save: viseme = %d, time = %d\n", lipSync->currentViseme,timems);
						}

					}
				}
			}
			/*
			//Spremanje u file na kraju file-a
			if (lipSync->currentViseme != viseme)
			{
				lipSync->currentViseme = viseme;
				if (lipSync->h_heMode == LS_HE_MODE_VISEME_BASED)
				{
					for(int k =0;k<lipSync->bEventHandler;k++)
						if(lipSync->eventH[k]) 
						{
							//comment by G. - ???[W] UMR: Uninitialized memory read in CVisemes::HandleFrames(DWORD,short *) 
							lipSync->eventH[k]->handleEvent(this->lipSync,lipSync->fbaps,(CodingParameters*)NULL,timems,viseme, wBuf, lipSync->lsGlobalTime);
						}
				}
			}
			//CHANGED BY G.
			//if animation is stored in FBA file, we need these vesemes later!!!
			if(lipSync->bFbaFile) 
				allVisemes.push_back((int)viseme);
			*/

			framecount_mod++;
			
			// Changed by KStankovic
			//delete [] SampleValues.signal_data; //MLK A.C. added
		}

		//TEST
		//if(framecount % 1 == 0)
		//{

		//	lipSync->fbaps->faps->fap1.viseme1 = lipSync->currentViseme;

		//ADDED by G. - call handleEvent in every frame if frame_based mode
		if (lipSync->h_heMode == LS_HE_MODE_FRAME_BASED)
		{
			set_audioBuf(wBuf);
			//if (!lipSync->bFbaFile) 
			//{
				for(int k =0;k<lipSync->bEventHandler;k++)
				{
					if(lipSync->eventH[k]) 
					{
						//comment by G. - ???[W] UMR: Uninitialized memory read in CVisemes::HandleFrames(DWORD,short *) 
						lipSync->eventH[k]->handleEvent(this->lipSync,lipSync->fbaps,(CodingParameters*)NULL,timems,viseme, wBuf);
					}
				}
			//}
		//}
//save fba not needed/available for frame based mode
/*			else
			{
				if (lipSync->stop_flag)
				{
					isLastFrame = 1;
					encode_viseme(lipSync->currentViseme,timems,isLastFrame);
					//fprintf(testFP,"save: viseme = %d, time = %d\n", lipSync->currentViseme,timems);
				}
				else
				{
					isLastFrame = 0;
					encode_viseme(lipSync->currentViseme,timems,isLastFrame);
					//fprintf(testFP,"save: viseme = %d, time = %d\n", lipSync->currentViseme,timems);
				}
			}
*/		}
	//	}

		delete [] SampleValues.signal_data; //MLK A.C. added
		VectorListFree(&FisherCoeff); //MLK A.C. added	
		//
	}

	//if audio signal is processed offline but ....
	//if((lipSync->bPreProcess) & !(lipSync->bRealTime))
	if(lipSync->bPreProcess)
	{
		framecount++;
		//static int* VisemeVector = lipSync->Visemes;
		int* VisemeVector = lipSync->Visemes;

		if(framecount % 2 == 0)
		{	
			viseme = VisemeVector[framecount_mod];
			//fbaps->faps->fap1.viseme1 = viseme;
			lipSync->currentViseme = viseme;
			framecount_mod++;
		}
	}

	// Changed by G.
	/*
	if (mfbaps)
	{
		delete mfbaps;
		mfbaps=NULL;
	}
	*/
	//
	//delete(fbaps); //MLK A.C. added

//	fprintf(testFP,"handleframe endtime = %d\n", lipSync->lsGlobalTime);

	//fclose(testFP);

	*finished = true;
    return;
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CVisemes::HandleFrames( unsigned long wBufSize, unsigned char* wBuf, bool* finished,  int fs )
{
	*finished = false;

	//FBAPs *fbaps = new FBAPs();
	//FILE *testFP = fopen("saveFBA.txt", "a+");

	//if audio signal is processed in real time (audio file & microphone)
	//if((lipSync->bRealTime) & !(lipSync->bPreProcess))
	if(lipSync->bRealTime)
	{
		double OutputValues[15];
		mfcc_VECTORLIST FisherCoeff;

		int i;	//CHANGED BY I.K. declared iterator outside of loop
		for (i=0; i< 15; i++)
			OutputValues[i]= 0;

		int index = 0;
		double LargestValue = 0;

		mfcc_SIGNAL SampleValues;
		SampleValues.signal_data = new double[wBufSize/2];
		SampleValues.signal_data_lenght = wBufSize/2;
		//SampleValues.fs = 16000;
		SampleValues.fs = fs;

		for(i=0; i<(int)wBufSize/2; i++)	//CHANGED BY I.K. added explicit cast
		{
			unsigned char data=wBuf[i];
			float value = ((float)data)/256; //signed short => 2 bytes = 16 bits: -2¹⁶ <= data <= 2¹⁶ = 32768

			//SampleValues(i+1)=value;
			SampleValues.signal_data[i]=value;
		}

		framecount++;
		//fprintf(testFP,"framecount = %d\n", framecount);

		Mfcc(SampleValues,12,256,&FisherCoeff);

		SimulateNet(&n0,&FisherCoeff.vector->data[0],&OutputValues[0]);
		SimulateNet(&n1,&FisherCoeff.vector->data[0],&OutputValues[1]);		
		SimulateNet(&n2,&FisherCoeff.vector->data[0],&OutputValues[2]);
		//SimulateNet(&n3,&FisherCoeff.vector->data[0],&OutputValues[3]);
		OutputValues[3] = 0;
		SimulateNet(&n4,&FisherCoeff.vector->data[0],&OutputValues[4]);
		SimulateNet(&n5,&FisherCoeff.vector->data[0],&OutputValues[5]);
		SimulateNet(&n6,&FisherCoeff.vector->data[0],&OutputValues[6]);
		SimulateNet(&n7,&FisherCoeff.vector->data[0],&OutputValues[7]);
		SimulateNet(&n8,&FisherCoeff.vector->data[0],&OutputValues[8]);
		SimulateNet(&n9,&FisherCoeff.vector->data[0],&OutputValues[9]);
		SimulateNet(&n10,&FisherCoeff.vector->data[0],&OutputValues[10]);
		SimulateNet(&n11,&FisherCoeff.vector->data[0],&OutputValues[11]);
		SimulateNet(&n12,&FisherCoeff.vector->data[0],&OutputValues[12]);
		SimulateNet(&n13,&FisherCoeff.vector->data[0],&OutputValues[13]);
		SimulateNet(&n14,&FisherCoeff.vector->data[0],&OutputValues[14]);

		
		for(int i1 = 0; i1 < 15; i1++)
		{
			Classes[i1] = Classes[i1] + OutputValues[i1];
		}
//		Classes[14] = 0;

		int timems = framecount*16;
		int isLastFrame = 0;

		//if(framecount % 5 == 0)
		if(framecount % lipSync->bNumberOfFrames == 0)
		{
			LargestValue = Classes[0];
			index = 0;

			for (int t=1; t< 15; t++)
			{
				if (Classes[t]>LargestValue)
				{
					LargestValue = Classes[t];
					index = t;
				}

				#ifdef WIN32
				if(USE_SLEEP) 	Sleep((unsigned long)0.006);	//CHANGED BY I.K. added explicit cast
#else
				if(USE_SLEEP)	usleep((unsigned long)6000);
#endif
			}

			if(index==3 || index <0 || index>14) {
				LargestValue=0;
				index=0;
			}

			for(i=0; i<15; i++)
				Classes[i]=0;

			//new calculated viseme
			viseme = index;
			//lipSync->fbaps->faps->fap1.viseme1 = viseme;

			//Spremanje u file u tijeku izvodjenja
			if (lipSync->currentViseme != viseme)
			{
				lipSync->currentViseme = viseme;
				//fprintf(testFP,"lipSync->currentViseme = %d\n", viseme);


				if (lipSync->h_heMode == LS_HE_MODE_VISEME_BASED)
				{
				/*	if (!lipSync->bFbaFile)
					{
						for(int k =0;k<lipSync->bEventHandler;k++)
							if(lipSync->eventH[k])
							{
								//comment by G. - ???[W] UMR: Uninitialized memory read in CVisemes::HandleFrames(DWORD,short *)
								lipSync->eventH[k]->handleEvent(this->lipSync,lipSync->fbaps,(CodingParameters*)NULL,timems,viseme, wBuf);
							}
					}
					else
					{
				*/		if (lipSync->stop_flag)
						{
							isLastFrame = 1;
							//encode_viseme(lipSync->currentViseme,lipSync->lsGlobalTime,isLastFrame);
							encode_viseme(lipSync->currentViseme,timems,isLastFrame);
							//fprintf(testFP,"save: viseme = %d, time = %d\n", lipSync->currentViseme,timems);
						}
						else
						{
							isLastFrame = 0;
							//encode_viseme(lipSync->currentViseme,lipSync->lsGlobalTime,isLastFrame);
							encode_viseme(lipSync->currentViseme,timems,isLastFrame);
							//fprintf(testFP,"save: viseme = %d, time = %d\n", lipSync->currentViseme,timems);
						}

					//}
				}
			}
			/*
			//Spremanje u file na kraju file-a
			if (lipSync->currentViseme != viseme)
			{
				lipSync->currentViseme = viseme;
				if (lipSync->h_heMode == LS_HE_MODE_VISEME_BASED)
				{
					for(int k =0;k<lipSync->bEventHandler;k++)
						if(lipSync->eventH[k])
						{
							//comment by G. - ???[W] UMR: Uninitialized memory read in CVisemes::HandleFrames(DWORD,short *)
							lipSync->eventH[k]->handleEvent(this->lipSync,lipSync->fbaps,(CodingParameters*)NULL,timems,viseme, wBuf, lipSync->lsGlobalTime);
						}
				}
			}
			//CHANGED BY G.
			//if animation is stored in FBA file, we need these vesemes later!!!
			if(lipSync->bFbaFile)
				allVisemes.push_back((int)viseme);
			*/

			framecount_mod++;

			// Changed by KStankovic
			//delete [] SampleValues.signal_data; //MLK A.C. added
		}


		delete [] SampleValues.signal_data; //MLK A.C. added
		VectorListFree(&FisherCoeff); //MLK A.C. added
		//
	}

	//if audio signal is processed offline but ....
	//if((lipSync->bPreProcess) & !(lipSync->bRealTime))
	if(lipSync->bPreProcess)
	{
		framecount++;
		//static int* VisemeVector = lipSync->Visemes;
		int* VisemeVector = lipSync->Visemes;

		if(framecount % 2 == 0)
		{
			viseme = VisemeVector[framecount_mod];
			//fbaps->faps->fap1.viseme1 = viseme;
			lipSync->currentViseme = viseme;
			framecount_mod++;
		}
	}

	// Changed by G.
	/*
	if (mfbaps)
	{
		delete mfbaps;
		mfbaps=NULL;
	}
	*/
	//
	//delete(fbaps); //MLK A.C. added

//	fprintf(testFP,"handleframe endtime = %d\n", lipSync->lsGlobalTime);

	//fclose(testFP);

	*finished = true;
    return;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////










//-----------------------------------------------------------------------------
// Name: CVisemes::LastFrameRender()
// Desc: Sets last viseme on 0 (closed mouth) 
//-----------------------------------------------------------------------------
void CVisemes::LastFrameRender()
{
	FBAPs *fbaps = new FBAPs();

/*	if(lipSync->bEventHandler)
	{
		LipSyncEventHandler *lseh;
		lseh = lipSync->eventH;
		lseh->handleEvent(0, faps, 0);
	}
	*/
	fbaps->faps->fap1.viseme1 = 0;
	lipSync->currentViseme = 0;

	//Changed by G.
	if (fbaps)
	{
		delete fbaps;
		fbaps=NULL;
	}
}



//-----------------------------------------------------------------------------
// Name: CVisemes::writeBits()
// Desc: needed for encoding 
//-----------------------------------------------------------------------------
void CVisemes::writeBits(int streamHandle, unsigned char *bits, int size)
{
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
					write(streamHandle, &byte, 1);
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

//	_commit(streamHandle);
 	
}

long CVisemes::encode_frame(int v1,int v2,int blend)
{
	FBAPs fbaps;
	int i;
	unsigned char outBuffer[OUT_BUFFER_SIZE];
	static long pts=0;
	int size;

	for(i=0;i<BAP_NUM_GROUPS;i++)
		fbaps.baps->maskType[i] = 0;
	for(i=0;i<NUM_BAPS;i++)
		fbaps.baps->groupMask[i] = 0;
	
	for(i=0;i<N_FAP_GROUP;i++)
		fbaps.faps->maskType[i] = 0;
	for(i=0;i<NFAP;i++)
		fbaps.faps->groupMask[i] = 0;
	fbaps.faps->maskType[0] = 2;
	fbaps.faps->groupMask[0] = 1;

	fbaps.faps->fap1.viseme2= v2;
	fbaps.faps->fap1.blend = blend;
	fbaps.faps->fap1.defBit = 0;
	fbaps.faps->fap1.viseme1= v1;

	//ukoliko spremanje u file ide paralelno sa sviranjem
	for(i =0;i<lipSync->bEventHandler;i++) //A.C. added, notify observer, changed by G.
	{
		if(lipSync->eventH[i])
			lipSync->eventH[i]->handleEvent(this->lipSync,&fbaps,encoder->getCodingParameters(),frameBeggining, viseme, aBuf);
	}		 
	//end of A.C.

	size = encoder->encodeFrame(&fbaps,outBuffer, OUT_BUFFER_SIZE, &pts);
	writeBits(fbaFileHandle,outBuffer,size);

	//delete [] outBuffer;

	return pts;
}

// encode interval between ts1 and ts2 with viseme v2, assuming that v1 was
// active in interval before ts1, and v3 after ts2.
void CVisemes::encode_interval(int v2, long ts1, long ts2)
{
	long ts;
	long pts;
	long time_step = 25;
	float blend_step, dir;
	float onset_time = 62.0;
	blend_step = (float)64.0 * ((float)time_step / onset_time);
	ts = ts1;
	
	if(eb<32)
	{
		dir = 1.0;
		ev1 = v2;
	}
	else
	{
		dir = -1.0;
		ev2 = v2;
	}

	while(ts < ts2)
	{				
		pts = encode_frame(ev1,ev2,(int)eb);
		eb += blend_step * dir;
		if(eb>63.0) eb = 63.0;
		if(eb<0.0) eb = 0.0;
		if(pts<0) return;
		if(pts>ts)
		{
			if(pts - ts != time_step)
			{
				time_step = pts - ts;
				blend_step = (float)64.0 * ((float)time_step / onset_time);
			}
			ts = pts;
			frameBeggining = pts;
		}
	}
}

//-----------------------------------------------------------------------------
// Name: CVisemes::encode_viseme()
// Desc: needed for encoding 
//-----------------------------------------------------------------------------
void CVisemes::encode_viseme(int v, long ts, int isLastFrame)
{
	if(isLastFrame)
	{
                encode_interval(v,last_ts,ts);
                encode_interval(0,ts,ts+(long)62);
		
		last_ts = -1;
	}
	else
	{
		if(last_ts != -1)
		{
			encode_interval(v,last_ts,ts);
		}
		last_ts = ts;
	}
} 

}

