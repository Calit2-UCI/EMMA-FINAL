/*
 * RGBTextureArray.h
 *
 *  Created on: 01/02/2012
 *      Author: narada
 */

#ifndef RGBTEXTUREARRAY_H_
#define RGBTEXTUREARRAY_H_

#include <cstdio>


/**
* @class RGBTextureArray
* @brief Interface class that declares methods to store and read a stream of images in real time
*
* @see NaradaVideoDecoderFFMPEG
*
*/
class RGBTextureArray
{
public:
	RGBTextureArray(){};
	virtual ~RGBTextureArray(){};

	virtual void setSize(int w, int h){};
	virtual void setLineWidth(int lineWidth){};

	virtual bool setNextFrame(unsigned char* buffer){};
	void resizeWindow(int w, int h){};
	virtual unsigned char* getNextFrame(){};
	virtual bool dropNextFrame(){};
	virtual void setLastFrame(){};
	virtual bool isLastFrame(){};

	virtual int getWidth(){};
	virtual int getLineWidth(){};
	virtual int getHeight(){};
	virtual int getBufferedFramesNum(){};

};


#endif /* RGBTEXTUREARRAY_H_ */
