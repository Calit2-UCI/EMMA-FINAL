/*
 * AudioStreamLipSync.hpp
 *
 *  Created on: 16 Nov 2010
 *      Author: narada
 */

#ifndef NARADA_SILENTSTREAMLIPSYNC_HPP_
#define NARADA_SILENTSTREAMLIPSYNC_HPP_

#include "AudioStreamSilentResource.hpp"
#include "CVisemes.h"
#include "LipSync.h"



namespace VisageSDK
{


	class SilentStreamLipSync : public AudioStreamSilentResource
	{

	public:

		~SilentStreamLipSync();

		SilentStreamLipSync(char* file, VisageLipSync* vsLipSync, ALsizei samplesPerBuffer);

		bool doLipSync();


	private:

        void handleProcessBuffer();
        
        bool handleEndLipSync(bool finalReached);

		VisageLipSync* lipSync;

		CVisemes* WaveCV;


	};


}

#endif /* NARADA_AUDIOSTREAMLIPSYNC_HPP_ */
