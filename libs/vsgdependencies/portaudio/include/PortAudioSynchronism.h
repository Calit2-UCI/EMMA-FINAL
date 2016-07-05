/*
 * PortAudioSynchronism.h
 *
 *  Created on: 3 Oct 2011
 *      Author: narada
 */

#ifndef PORTAUDIOSYNCHRONISM_H_
#define PORTAUDIOSYNCHRONISM_H_

class PortAudioSynchronism {
public:
	PortAudioSynchronism();
	virtual ~PortAudioSynchronism();

	static void initPortAudioSyncronism()
	{
		PortAudioSynchronism::isPortAudioBeingUsed = false;
		PortAudioSynchronism::nTimesLocked = 0;
	}


	static bool ifPortAudioAvailableLock()
	{
		bool ret;
		ret = __sync_bool_compare_and_swap(&isPortAudioBeingUsed, false, true);
		if(ret)
		{
			std::cerr << "locking Port Audio, value:" << isPortAudioBeingUsed << std::endl;
			nTimesLocked = 0;
		}
		return ret;
	}

	static void unlockPortAudio()
	{
		__sync_bool_compare_and_swap(&isPortAudioBeingUsed, true, false);
		std::cout << "unlocking Port Audio" << std::endl;
	}


	static void waitUntilPortAudioAvailable()
	{
    	while(!PortAudioSynchronism::ifPortAudioAvailableLock())
    	{
    		std::cout << "portAudio resource unavailable, sleeping a little bit" << std::endl;
    		usleep(100000);
    		if(PortAudioSynchronism::nTimesLocked == 10)
    		{
    			std::cout << "lets force unlock" << std::endl;
    			unlockPortAudio();
    		}
    		else
    			PortAudioSynchronism::nTimesLocked++;

    	}
	}

private:
	static bool isPortAudioBeingUsed;
	static int nTimesLocked;
};

//This has to be instantiated in a C++ source file, but only once
//bool PortAudioSynchronism::isPortAudioBeingUsed = false;
//int PortAudioSynchronism::nTimesLocked = 0;

#endif /* PORTAUDIOSYNCHRONISM_H_ */
