/*
 * SilentStreamLipSync.cpp
 *
 *  Created on: 16 Nov 2010
 *      Author: narada
 */

#include "SilentStreamLipSync.hpp"

#include <iostream>

namespace VisageSDK

{
	
	SilentStreamLipSync::SilentStreamLipSync(char* file, VisageLipSync* vslipSync, ALsizei samplesPerBuffer):AudioStreamSilentResource(file)
	{
		lipSync = vslipSync;
		bufferSize = samplesPerBuffer;// * getBytesPerFrame();
		
	}
	

	SilentStreamLipSync::~SilentStreamLipSync()
	{
		std::cout << "deleting stream lip sync" << std::endl;

	}


//	ALboolean SilentStreamLipSync::loadNextStreamInBuffer(ALuint buffID)
//	{
//		int err = 0;
//		
//		ALsizei auxBuffSize = bufferSize;
//
//		int totalChunks = audioSize / STREAMING_BUFFER_SIZE;
//
//		if(bufferIndex > totalChunks)
//			return AL_FALSE;
//
//		long startOffset = bufferIndex*STREAMING_BUFFER_SIZE;
//
//		if(bufferIndex == totalChunks) //the audioFile does not have to be an exact multiple of bufferIndex
//			auxBuffSize = audioSize - (bufferSize*totalChunks);
//
//		//unsigned char *outData = (unsigned char*)malloc(bufferSize*sizeof(unsigned char*));
//		short *outData = (short*)malloc(auxBuffSize/2*sizeof(short*));
//
//
//		if(!readAudioStream((ALCvoid*)outData, auxBuffSize, startOffset))
//		{
//			std::cout << "cannot read audio stream" << std::endl;
//		   return AL_FALSE;
//		}
//
//
//		///CALCULAR LOS VISEMES AQUI!!!!!!
//		WaveCV = lipSync->cvis;
//		bool finished = true;
//
//
//		//para el último trozo no hago el análisis xq su tamaño me puede dar problemas
//		if (bufferIndex>4 && (bufferIndex < totalChunks))
//			WaveCV->HandleFrames(auxBuffSize, (short*)outData, &finished, freq);
//
//
//		//////////////////////////////////
//
//		if(!mute)
//		{
//			alGetError();
//			alBufferData(buffID, format, outData, auxBuffSize, freq);
//
//			if ((err =alGetError()) != AL_NO_ERROR)
//			{
//				std::cout << "cannot buffer data stream, error: " << err << std::endl;
//			   return AL_FALSE;
//			}
//
//		}
//		
//		//printFormat();
//
//		if(outData)
//		{
//			free(outData);
//			outData = NULL;
//		}
//
//		bufferIndex++;
//		
//		if(mute)
//		{
//			muteBufferID++;
//			if(muteBufferID == N_BUFFERS)
//				muteBufferID = 0;
//		}
//
//		if(bufferIndex > totalChunks || lipSync->stop_flag)
//		{
//			WaveCV->LastFrameRender();
//			lipSync->stop_flag = 1;
//			return AL_FALSE;
//			//there are no more data to enqueue. IF we want to loop, here check loop var and do bufferIndex = 0 again, insted of return false.
//		}
//		return AL_TRUE;
//
//	}

    
    
	void SilentStreamLipSync::handleProcessBuffer()
	{
		
		WaveCV = lipSync->cvis;
		bool finished = true;


		//necesito al menos 4 muestras para el analisis.
        //para el último trozo no hago el análisis xq su tamaño me puede dar problemas
        
		if (bufferIndex>4 && (bufferIndex < audioSize / SILENT_STREAMING_BUFFER_SIZE ))
			WaveCV->HandleFrames(bufferSize, (short*)bufferData, &finished, freq);
        else
            std::cout << "not handling frames, bufferIndex: " << bufferIndex << std::endl;

	}
    
    
    
    bool SilentStreamLipSync::handleEndLipSync(bool finalReached)
    {
        if(finalReached || lipSync->stop_flag)
		{
			WaveCV->LastFrameRender();
			lipSync->stop_flag = 1;
			return false;
		}
		return true;
    }




	bool SilentStreamLipSync::doLipSync()
	{
		readWAV();

		if ((lipSync->bFbaFile) &(lipSync->h_heMode == LS_HE_MODE_VISEME_BASED))
		{
			//save animation in fba file
			WaveCV->FinalizeStoreAnimation(); //added by G.
		}
		
		lipSync->cvis->FreeAllGeneratedNNetworks();

		return AL_TRUE;
	}



}
