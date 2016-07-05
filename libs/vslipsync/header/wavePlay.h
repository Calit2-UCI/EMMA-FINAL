
#ifndef __wavePlay_h__
#define __wavePlay_h__

// C++
#include <math.h>
#include <vector>

#ifdef WIN32
// Windows
#include <windows.h>//necessary for audio usage (think about OpenAL)
#include <mmsystem.h>//necessary for audio usage (think about OpenAL)??
#else
typedef unsigned long DWORD;
#endif

using namespace std;

namespace VisageSDK
{

#ifdef MSVC
typedef vector<BYTE> BUFFERBYTES;
#else
typedef vector<unsigned char> BUFFERBYTES;
#endif

typedef vector<BUFFERBYTES> RECORDEDBYTES;


//-----------------------------------------------------------------------------
// Function-prototypes
//-----------------------------------------------------------------------------

//int PlayWavFile(char* audiofile);
#ifdef MSVC
DWORD WINAPI PlayWavFile(LPVOID arg);
long QueueWaveData(WAVEHDR* waveHeader);
void CALLBACK WaveOutProc(HWAVEOUT waveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
void PlayWave();
//void HandleFramesInThread(void *dummy);

void PrintWaveErrorMsg(DWORD err, TCHAR * str);
DWORD WINAPI waveInProc(LPVOID arg);
DWORD WINAPI RecordWav(LPVOID arg);

#else
void * playWavFile(void * args);
void * readWavFile(void * args);
//void * playText(void* args);

#endif

}

#endif // __wavePlay_h__


