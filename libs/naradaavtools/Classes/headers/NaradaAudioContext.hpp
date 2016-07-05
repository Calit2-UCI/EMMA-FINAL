//
//  NaradaAudioContext.hpp
//  NaradaAVTools
//
//  Created by Enrique Oriol on 05/11/10.
//  Copyright 2010 Narada Robotics. All rights reserved.
//


#ifndef __HPP_HEADER__NARADAAUDIOCONTEXT_HPP
#define __HPP_HEADER__NARADAAUDIOCONTEXT_HPP


#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <iostream>
#include "NaradaAudioResource.hpp"





	class NaradaAudioResource;



	class NaradaAudioContext{


	public:

		NaradaAudioContext(bool stream);

		~NaradaAudioContext();

		void initNaradaAudioContext(bool stream);

		ALboolean loadWAVonContext(NaradaAudioResource *audio);

		ALboolean loadCaptureOnContext(NaradaAudioResource *audio);

		ALboolean bindAudioToBuffer(NaradaAudioResource *audio);

		void deleteContext();



	private:


		ALCdevice *audioDevice;
		ALCcontext *alContext;
		ALuint alBuffer;

	};


#endif

