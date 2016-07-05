/*
 * ImageToDisc.h
 *
 *  Created on: 01/02/2012
 *      Author: narada
 */

#ifndef IMAGETODISC_H_
#define IMAGETODISC_H_

#include "RGBTextureArray.h"
#include <cstdio>
#include <cstring>
#include <vector>


class ImageToDisc : public RGBTextureArray {
public:
	ImageToDisc();
	virtual ~ImageToDisc();


private:
	void setSize(int w, int h);
	bool setNextFrame(unsigned char* buffer);
	unsigned char* getNextFrame();
	bool dropNextFrame();
	int getWidth();
	int getHeight();
	void setLineWidth(int lineWidth);
	int getLineWidth();
	void setLastFrame();
	bool isLastFrame();
	int getBufferedFramesNum(){return frames.size();};

	std::vector<unsigned char*> frames;
	int width;
	int height;
	int lineWidth;
	bool lastFrameLoad;

};

#endif /* IMAGETODISC_H_ */
