/*
 * ImageToDisc.cpp
 *
 *  Created on: 01/02/2012
 *      Author: narada
 */

#include "ImageToDisc.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>

ImageToDisc::ImageToDisc() {
	// TODO Auto-generated constructor stub
	lastFrameLoad = false;

}

ImageToDisc::~ImageToDisc() {
	// TODO Auto-generated destructor stub
	frames.clear();
}


void ImageToDisc::setSize(int w, int h)
{
	width = w;
	height = h;
	std::cout << "width is " << w << std::endl;
	std::cout << "height is " << h << std::endl;
}


int ImageToDisc::getWidth()
{
	return width;
}


int ImageToDisc::getHeight()
{
	return height;
}

void ImageToDisc::setLineWidth(int lw)
{
	lineWidth = lw;
	std::cout << "line Width is " << lw << std::endl;
}

int ImageToDisc::getLineWidth()
{
	return lineWidth;
}


bool ImageToDisc::setNextFrame(unsigned char* buffer)
{
	std::cout << ">>>>>>setting next frame" << std::endl;
	//unsigned char* image = new unsigned char[width*height*3];
	unsigned char* image = (unsigned char *)malloc(width*height*3);
	memcpy(image, buffer, width*height*3);
	frames.push_back(image);
	//free(image);//ESTO DEBERÍA ESTAR MAL Y CAUSAR UN SEGFAULT AL ACCEDER A IMAGE DESDE FRAME

	return true;
}

unsigned char* ImageToDisc::getNextFrame()
{
	if(frames.size())
	{
		std::cout << "getting next frame<<<<<<<<<<<<<<" << std::endl;

		unsigned char* img = frames.front();
		frames.erase(frames.begin());

		return img;
	}
	return NULL;
}
bool ImageToDisc::dropNextFrame()
{
	if(frames.size())
	{
		std::cout << "dropping next frame" << std::endl;
		unsigned char* img = frames.front();
		free(img);
		frames.erase(frames.begin());
		return true;
	}
	return false;
}

void ImageToDisc::setLastFrame()
{
	lastFrameLoad = true;
}


bool ImageToDisc::isLastFrame()
{
	if(lastFrameLoad)
	{
		if(frames.size()==0)
			return true;
	}
	return false;
}

