//!
//!
//! File: Image.hpp
//! Desc: Header of Image.cpp for read/write images
//! Author: Enrique Oriol
//! E-mail: enriqueoriol@naradarobotics.com
//!
//! Owner: Narada Robotics
//!
//!
//! Data: 06/09/2010
//!
//!



#ifndef IMAGE_CORE_HPP_FILE
#define IMAGE_CORE_HPP_FILE


//#ifndef IPHONE_DEV

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>

#ifndef IPHONE

#ifdef WIN32
	// Intels jpg library
	#include <windows.h>
	#include <gl\gl.h>
	#include <gl\glu.h>
	#include <gl\glaux.h>
	#include "ijl.h"
#else
	//libjpg free library
	#include "jpeglib.h"

#endif

	#include "png.h"
#endif

#ifdef VISAGE_STATIC
	#define VISAGE_DECLSPEC
#else
	#if defined(MAC) || defined(LINUX)
		#define VISAGE_DECLSPEC __attribute__((__visibility__("default")))

	#else
		#ifdef VISAGE_EXPORTS
			#define VISAGE_DECLSPEC __declspec(dllexport)
		#else
			#define VISAGE_DECLSPEC __declspec(dllimport)
		#endif
	#endif

#endif


using namespace std;


const int PNG_BYTES_TO_CHECK = 4;

//typedef unsigned char BYTE;

#ifndef WIN32
struct AUX_RGBImageRec
{
	public:
	unsigned char *data;
	long sizeX;
	long sizeY;
};
#endif




namespace VisageSDK
{


	class VISAGE_DECLSPEC Image
	{

	private:
		int n_channels;
		int size_x;
		int size_y;
		unsigned char * data;


	
	public:


		Image();

		~Image()
		{
			if(data)
				delete [] data;
		};
		//! \brief
		//!
		//! a new sized image is created
		//!
		//! \param[in] x: image size X
		//! \param[in] y: image size Y
		//!
		Image(int x, int y);


		//! \brief size_X setter
		//!
		//! \param[in] x: x size of the image
		//!
		void set_size_x(int x);
	

		//! \brief size_Y setter
		//!
		//! \param[in] y: y size of the image
		//!
		void set_size_y(int y);


		//! \brief reads an image from the specified path, and sets it into data array
		//!
		//! \param[in] path: string with the path to the image
		//!
		void read_image(char* path);


		//! \brief getter of size_X data member
		//!
		//! \return int with x size of the image
		//!
		int get_size_x();


		//! \brief getter of size_Y data member
		//!
		//! \return int with y size of the image
		//!
		int get_size_y();


		//! \brief getter of n_channels data member
		//!
		//! \return int with number of channels of the image
		//!
		int get_n_channels();


		//! \brief getter of image data
		//!
		//! \return unsigned char array with data of the image (ordered as RGB if 3 channels)
		//!
		unsigned char * get_data();


	private:

		//! \brief reads a JPEG image from the specified path, and sets it into data array
		//!
		//! \param[in] path: string with the path to the image
		//!
		void read_jpeg_image(char* path);

		//! \brief reads a PNG image from the specified path, and sets it into data array
		//!
		//! \param[in] path: string with the path to the image
		//!
		void read_png_image(char* path);

		//! \brief reads a BMP image from the specified path, and sets it into data array
		//!
		//! \param[in] path: string with the path to the image
		//!
		void read_bmp_image(char* path);



	};
	
}

#endif

//#endif

