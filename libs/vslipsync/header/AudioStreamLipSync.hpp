/*
 * AudioStreamLipSync.hpp
 *
 *  Created on: 16 Nov 2010
 *      Author: narada
 */

#ifndef NARADA_AUDIOSTREAMLIPSYNC_HPP_
#define NARADA_AUDIOSTREAMLIPSYNC_HPP_

#include "AudioStreamResource.hpp"
#include "CVisemes.h"
#include "LipSync.h"



namespace VisageSDK
{


	class AudioStreamLipSync : public AudioStreamResource
	{

	public:

		~AudioStreamLipSync();
		
		AudioStreamLipSync();

		AudioStreamLipSync(char* file, VisageLipSync* vsLipSync, ALsizei samplesPerBuffer, bool isMute, bool isSequential);

		ALboolean playWave();


	private:

		ALboolean loadNextStreamInBuffer(ALuint buffID);

		VisageLipSync* lipSync;

		CVisemes* WaveCV;


	};


}

#endif /* NARADA_AUDIOSTREAMLIPSYNC_HPP_ */
