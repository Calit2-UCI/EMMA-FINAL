
#include "FAPlayer.h"

#include "FBADecoder.h"

// C++
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



//////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////

extern void replaceExtension(char *fn, char *ext);
extern void DebugPrint(char *s);
//UINT FAPlayer::PlayThread(LPVOID pParam);	//CHANGED BY I.K. commented out

namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FAPlayer::FAPlayer()
{

	init();
}

FAPlayer::FAPlayer(VisageCharModel *model, FARenderer *_renderer)
{
	init();
	init(model);
	setRenderer(_renderer);
}

FAPlayer::FAPlayer(VisageCharModel *model)
{
	init();
	init(model);
}

void FAPlayer::init()
{

	//timeBeginPeriod(1);	//Why there's no timeEndPeriod(1) anywhere?
	timer = new FAPlayerTimer;
	nTracks = 0;
	/* IDLE TRACKS
	nIdleTracks = 0;
	*/
	for(int i =0;i<100;i++)
	{
		track[i]=0;
		track_dispensable[i]=0;
	/* IDLE TRACKS
		idleTrack[i]=0;
		*/
	}
	face = 0;
	playMode = PLAYMODE_REALTIME;
	readFiltersOnPlay = 0;
	audioDelay = 0;
	/* IDLE TRACKS
	initialTime = getCurrentTimeMs();
	lastPlayTime = -10000;
	globalTime = 0;
	*/
	playing = 0;
	currentTime = 0;
	renderer = 0;
	autoStopMode  = true;
	playRangeStart = 0;
	playRangeEnd = 0;
}

FAPlayer::~FAPlayer()
{
	if(timer)
		delete(timer);
}

bool FAPlayer::checkTrack(FbaAction *trk)
{
	if(!trk)
		return false;

	for(int i = 0;i < nTracks; i++)
	{
		if(track[i] == trk)
			return true;
	}
	return false;
}

void FAPlayer::removeTrack(FbaAction *trk)
{
	int found = 0;
	if(nTracks == 0)
		return;

	for(int i = 0;i < nTracks; i++)
	{
		if(!found && track[i] == trk)
		{
			found = 1;
//			if(track_dispensable[i] == 2)
//				delete trk;
		}

		// Changed by KStankovic
		if(found)
		{
			if (i == nTracks - 1)
			{
				track[i] = 0;
			}
			else if (i < nTracks - 1)
			{
				track_dispensable[i] = track_dispensable[i + 1];
				track[i] = track[i+1];
			}
		}
		//
	}
	if(found)
		nTracks--;
}

FbaFileAction *FAPlayer::addTrack(char *tname)
{
	return(addTrack(tname,0,0));
}

FbaFileAction *FAPlayer::addTrack(char *tname, int loop, long timeOffset)
{
    printf("%s\n", tname);
	FbaFileAction *trk = new FbaFileAction(tname);
	((FbaFileAction*)trk)->setLoopMode(loop); // main track does not loop
	((FbaFileAction*)trk)->setOffset(timeOffset); // no offset in main track
	((FbaFileAction*)trk)->setAudioDelay(audioDelay);
	addTrack(trk);

	return(trk);
}

void FAPlayer::addTrack(FbaAction *trk)
{
    //TRAPI para asegurarnos de que el tiempo está actualizado cuando añadimos la pista, en caso de que no haya ninguna otra
    if(playMode == PLAYMODE_REALTIME)
        timer->updateRT();
    currentTime = timer->getCurrentTime();
    //printf("----current time at add track: %ld\n",currentTime );
    
	track_dispensable[nTracks] = 0;
	if(playing)
		trk->start(currentTime);
	track[nTracks++] = trk;
}

FbaFileAction *FAPlayer::playTrack(char *tname)
{
	return(playTrack(tname,0));
}

FbaFileAction *FAPlayer::playTrack(char *tname, long timeOffset)
{
	FbaFileAction *trk = new FbaFileAction(tname);
	((FbaFileAction*)trk)->setLoopMode(0); // main track does not loop
	((FbaFileAction*)trk)->setOffset(timeOffset); // no offset in main track
	((FbaFileAction*)trk)->setAudioDelay(audioDelay);
	playTrack(trk);
	return(trk);
}

void FAPlayer::playTrack(FbaAction *trk)
{
	track_dispensable[nTracks] = 1;
	if(playing)
		trk->start(currentTime);
	track[nTracks++] = trk;
	if(!playing)
		play();
}

void FAPlayer::setPlayMode(int mode)
{
	playMode = mode;
}


void FAPlayer::clearAllTracks()
{
	for(int i=0;i<nTracks;i++)
	{
		if(track[i])
		{
			//delete track[i];
			// Added by KStankovic
			track[i] = 0;
			//
		}
	}
	nTracks = 0;
}

int FAPlayer::updateFaps()
{
	int end = 1;
	int currentFrame;
	
	if(!timer)
		return 0;


	if(playing) 
	{
		if(playMode == PLAYMODE_REALTIME)
			timer->updateRT();
		else
			timer->nextFrame();
		
		currentTime = timer->getCurrentTime();
		currentFrame = timer->getCurrentFrame();

		if(playRangeStart != 0 && playRangeEnd != 0)
		{
			if(currentFrame >= playRangeEnd)
				end = 0;
		}
		
		currentFBaps.reset();
	}
	for(int i=0;i<nTracks;i++)
	{
//        printf("current time is %ld\n", currentTime);
		FBAPs *fbaps = track[i]->getFBAPs(currentTime,&lastFBaps,face);
		if(!fbaps) 
		{
			if(track_dispensable[i])
			{
				removeTrack(track[i]);
				i--;
			}
			continue;
		}
		
		end = 0;

		if(fbaps->faps)
			// removed temporarily, until a suitable replacement is found -- TPejsa
			//face->interpolateInnerOuterLips(fbaps->faps);
		
		currentFBaps.merge(fbaps);
	}
	
	lastFBaps.set(&currentFBaps);

	if(!autoStopMode)
		end = 0;
	return(end);
}

void FAPlayer::updateFaps(int nfr)
{
	if(!timer)
		return;

	currentTime = timer->getTime(nfr);

	currentFBaps.reset();

	for(int i=0;i<nTracks;i++)
	{
//		if(!track[i]->getNFrames())
//			continue;
//		track[i]->setFrame(track[i]->getFrame((float)currentTime));
		FBAPs *fbaps = track[i]->getFBAPs(currentTime,&lastFBaps,face);
		if(!fbaps) continue;
		if(fbaps->faps)
			// removed temporarily, until a suitable replacement is found -- TPejsa
			//face->interpolateInnerOuterLips(fbaps->faps);
		
		currentFBaps.merge(fbaps);
	}
	lastFBaps.set(&currentFBaps);
}

void FAPlayer::rewind()
{
	//added by TPejsa
	if( !ready() )
		return;
	//

	for(int i=0;i<nTracks;i++)
	{
		track[i]->stop();
	}
	if(!face) return;
	face->clearFBAPs();
	face->updateGeometry();
	lastFBaps.reset();
}

int FAPlayer::update()
{
	//added by TPejsa
	if( !ready() )
		return 0;
	//

	int end=0;

	end = updateFaps();

	if(renderer)
		renderer->preRender(&currentFBaps, currentTime);

	face->clearFBAPs();
	face->applyFBAPs(&currentFBaps);
	face->updateGeometry();	

	return(end);
}

float FAPlayer::getFrameRate()
{
	if(timer)
		return(timer->getFrameRate());
	return(-1);
}

int FAPlayer::getFrame(long t)
{
	if(timer)
		return(timer->getFrame(t));
	return(-1);
}

void FAPlayer::updateAtTime(long t)
{
	update(getFrame(t));
}

void FAPlayer::update(int nfr)
{
	updateFaps(nfr);

	face->clearFBAPs();
	face->applyFBAPs(&currentFBaps);
	face->updateGeometry();	
}

VisageCharModel *FAPlayer::getFaceModel(int frame)
{
	//added by TPejsa
	if( !ready() )
		return NULL;
	//

	update(frame);
	return(getFaceModel());
}

VisageCharModel *FAPlayer::getFaceModel(long time)
{
	//added by TPejsa
	if( !ready() )
		return NULL;
	//

	return(getFaceModel(getFrame(time)));
}

int FAPlayer::ready()
{
	if(!face)
		return(0);
	if(!timer)
		return(0);
	if(nTracks==0)
		return(0);

	return(1);
}

int FAPlayer::getNFrames()
{
	int nfr = 0;
	
	for(int i=0;i<nTracks;i++)
	{
		if(!strcmp(track[i]->actionTypeName(),"FbaFileAction"))
		{
			int nfr1 = ((FbaFileAction*)track[i])->getNFrames();
			if(nfr1>nfr)
				nfr = nfr1;
		}
	}
	return nfr;
}

VisageCharModel* FAPlayer::getFaceModel()
{
	if(!face)
		return(0);
	else
		return face;
}

void FAPlayer::stop()
{
	stopPlaying = 1;
	playing = 0;

	for(int i=0;i<nTracks;i++)
	{
		if(track[i])
			track[i]->stop();
	}
}

void FAPlayer::init(VisageCharModel* model)
{
	/*
	if(face)
		delete (face);
	*/
	face = model;
}


void FAPlayer::play()
{
	if( !ready() )
		return;

	if(playing)
		return;

	// prepare everything for playback:

	if(!timer)
		return;

	if(!face)
		return;

	if(readFiltersOnPlay)
	{
		for(int i=0;i<nTracks;i++)
		{
			if(!strcmp(track[i]->actionTypeName(),"FbaFileAction"))
				((FbaFileAction*)track[i])->loadFilter();
		}
	}
	
	stopPlaying = 0;
	playing = 1;

	rewind();

	if(playRangeStart != 0 && playRangeEnd != 0)
	{
		timer->start((long)(1000.0f * (float)playRangeStart / getFrameRate()));
	}
	else
	{
		timer->start();
	}
	currentTime = timer->getCurrentTime();
	for(int i=0;i<nTracks;i++)
	{
		if(track[i])
			track[i]->start(0L);
	}
}

	/* IDLE TRACKS
void FAPlayer::updateGlobalTime()
{
	globalTime = getCurrentTimeMs() - initialTime;
}
*/

void FAPlayer::mergeAllTracks()
{
	if(!timer)
		return;

	// find the longest track
	int nfr = 0;
	int longestTrackInd = -1;
	
	for(int i=0;i<nTracks;i++)
	{
		if(!strcmp(track[i]->actionTypeName(),"FbaFileAction"))
		{
			int nfr1 = ((FbaFileAction*)track[i])->getNFrames();
			if(nfr1>nfr)
			{
				nfr = nfr1;
				longestTrackInd = i;
			}
		}
	}

	if(longestTrackInd == -1)
		return;


	FbaFileAction* mergeTrack = (FbaFileAction*)track[longestTrackInd];
	removeTrack(mergeTrack);
	mergeTrack->merge(track,nTracks);
/*	for(i=0;i<nTracks;i++)
	{
		if(track[i])
			delete track[i];
	}*/
	nTracks = 1;
	track[0] = mergeTrack;

	// filter is now discarded, otherwise it would filter a second time (the
	// track was filtered and saved with filtering during the merging
	// operation)
	mergeTrack->discardFilter();

}

void FAPlayer::save(char *fn)
{
	if(!timer)
		return;

	// find the longest track
	int nfr = 0;
	int longestTrackInd = -1;
	long t;
	
	for(int i=0;i<nTracks;i++)
	{
		if(!strcmp(track[i]->actionTypeName(),"FbaFileAction"))
		{
			int nfr1 = ((FbaFileAction*)track[i])->getNFrames();
			if(nfr1>nfr)
			{
				nfr = nfr1;
				longestTrackInd = i;
			}
		}
	}

	if(longestTrackInd == -1)
		t = 10000;          // if no file FbaFileAction, use default length of 10 seconds
	else t = 1000*(long)(nfr/((FbaFileAction*)track[longestTrackInd])->getFrameRate());

	save(fn,t);
}

void FAPlayer::save(char *fn, long t)
{
	if(!timer)
		return;
	if(!nTracks)
		return;

	int nfr = (int)(t * getFrameRate() / 1000.0f);

	FbaFileAction* mainTrack = new FbaFileAction(nfr,(int)getFrameRate());
	mainTrack->merge(track,nTracks);
	mainTrack->write(fn);
	delete(mainTrack);
}

FARenderer::FARenderer()
{
}

FARenderer::~FARenderer()
{
}


int FAPlayer::new_updateFaps()
{	
	int end=0;
/*	
	// assume - always  playMode == PLAYMODE_REALTIME 
	// when this new_updateFaps() method is called
	end = mainTrack->updateRT();
	
	currentTime = mainTrack->getCurrentTime();
		
	FBAPs *fbaps = mainTrack->getFBAPs(currentTime);

	if(!fbaps)
		return 0;

	if(fbaps->faps)
		face->interpolateInnerOuterLips(fbaps->faps);
	currentFBaps.set(fbaps);
	
	for(int i=0;i<nTracks;i++)
	{
//		track[i]->setFrame(track[i]->getFrame((float)currentTime));

		fbaps = track[i]->getFBAPs(currentTime);

		if(!fbaps) continue;
		
		if(fbaps->faps)
			face->interpolateInnerOuterLips(fbaps->faps);
		
		currentFBaps.merge(fbaps);
	}
	if(followGaze)
	{
		if(currentFBaps.faps)
			face->doGazeFollow(currentFBaps.faps);
	}
	

	if(renderer)
		renderer->preRender(&currentFBaps,currentTime);

	face->clearFAPs();
	face->applyFBAPs(&currentFBaps);
	face->updateGeometry();	
*/
	return(end);
}

void FAPlayer::new_play(FARenderer *_renderer, int iStartFrm)
{
//	AfxBeginThread(PlayThread,0);
/*
	renderer = _renderer;

	int end;
	if(!mainTrack)
		return;
	if(!mainTrackIsFile)
		return;
	if(!face)
		return;
	
	//if(readFiltersOnPlay)
	//{
	//	mainTrack->loadFilter();
	//	for(int i=0;i<nTracks;i++)
	//		track[i]->loadFilter();
	//}
	
	stopPlaying = 0;
	//rewind();

	((FbaFileAction*)mainTrack)->setFrame(iStartFrm);
	
	if (((FbaFileAction*)mainTrack)->getCurrentFrame() == 0 ) 
		((FbaFileAction*)mainTrack)->setStarted(0);
	else ((FbaFileAction*)mainTrack)->setStarted(1);
	
	if (((FbaFileAction*)mainTrack)->getStarted()) {

		((FbaFileAction*)mainTrack)->setStartT( getCurrentTimeMs() - mainTrack->getCurrentTime() );
	}
	do
	{
		end = new_updateFaps();
		if(renderer)
			renderer->render(getFaceModel(), mainTrack->getCurrentTime());
	}
	while(!end && !stopPlaying);
	
	lastStartT = ((FbaFileAction*)mainTrack)->getStartT();
*/	
}
int FAPlayer::new_pause()
{
	return 0;
/*	if(!mainTrackIsFile)
		return 0;

	stopPlaying = 1;
	mainTrack->stop();
	return ((FbaTimeReferenceAction*)mainTrack)->getCurrentFrame();
	*/
}

int FAPlayer::getNTracks()
{
	return nTracks;
}

bool FAPlayer::isPlaying()
{
	if(playing)
		return true;
	else
		return false;
}

FbaAction* FAPlayer::getTrack(int n)
{
	if(n>=0 && n<getNTracks())
		return(track[n]);
	else
		return(0);
}

void FAPlayer::removeTrack(int n)
{
	FbaAction *t = getTrack(n);

	if(t)
		removeTrack(t);
}

void FAPlayer::setPlayFramesRange(int start, int end)
{
	int start1 = start;
	int  end1 = end;
	if(start1 < 0)
		start1 = 0;
	if(end1 > getNFrames())
		end1 = getNFrames();

	playRangeStart = start1;
	playRangeEnd = end1;
}

void FAPlayer::getPlayFramesRange(int *start, int *end)
{
	(*start) = playRangeStart;
	(*end) = playRangeEnd;
}

void FAPlayer::setPlayTimeRange(long start, long end)
{
	int starti,endi;
	float frr = getFrameRate();

	starti = (int)(frr * (float)start / 1000.0f);
	endi = (int)(frr * (float)end / 1000.0f);

	setPlayFramesRange(starti,endi);
}

void FAPlayer::getPlayTimeRange(long *start, long *end)
{
	float frr = getFrameRate();
	if(frr == 0.0)
	{
		(*start) = 0;
		(*end) = 0;
	}
	else
	{
		(*start) = (long)(1000.0f * (float)playRangeStart / frr);
		(*end) = (long)(1000.0f * (float)playRangeEnd / frr);
	}
}

}
