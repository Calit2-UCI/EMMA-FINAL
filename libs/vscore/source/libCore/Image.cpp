//!
//!
//! File: Image.cpp
//! Desc: Read/write images implementations
//! Author: Enrique Oriol
//! E-mail: enriqueoriol@naradarobotics.com
//!
//! Owner: Narada Robotics
//!
//! Data: 06/09/2010
//!
//!

//#ifndef IPHONE_DEV

#if defined(IPHONE)
 	#include <OpenGLES/EAGL.h>
 	#include <OpenGLES/ES2/gl.h>
    #import <UIKit/UIImage.h>

#endif

#include "Image.hpp"


int get_bitmap_dimensions(const char * file_name, long int * width, long int * height) {

	/* open file */
	FILE * bitmap_file = fopen(file_name, "rb");

	if(!bitmap_file) {

		printf("Could not find file.\n");
		fclose(bitmap_file);
		return 0;

	}

	printf("File opened.\n");

	/* grab width */
	if(fseek(bitmap_file, 18, SEEK_SET)) {

		printf("Error finding data.\n");
		fclose(bitmap_file);
		return 0;

	}

	fread(width, 4, 1, bitmap_file);
	printf("Bitmap with: %d\n", *width);

	/* grab height */
	if(fseek(bitmap_file, 22, SEEK_SET)) {

		printf("Error finding data.\n");
		fclose(bitmap_file);
		return 0;

	}

	fread(height, 4, 1, bitmap_file);
	printf("Bitmap height: %d\n", *height);

	/* check it worked */
	if((width == NULL) || (height == NULL)) {

		printf("Error reading data.\n");
		fclose(bitmap_file);
		return 0;

	}

	/* close file */
	fclose(bitmap_file);

	/* return success */
	return 1;

}

int load_bitmap_texture(const char * file_name, unsigned char * texture_buffer, long int bmp_width, long int bmp_height) {

	/* open file */
	FILE * bitmap_file = fopen(file_name, "rb");

	if(!bitmap_file) {

		printf("Could not find file.\n");
		fclose(bitmap_file);
		return 0;

	}

	printf("File opened.\n");

	/* declare some locals */
	short int i;

	/* read back to front */
	fseek(bitmap_file, 3, SEEK_END);

	for(i = 0; i < bmp_width*bmp_height; i=i+3) {

		fread(&texture_buffer[i+2], 1, 1, bitmap_file);
		fread(&texture_buffer[i+1], 1, 1, bitmap_file);
		fread(&texture_buffer[i], 1, 1, bitmap_file);
		fseek(bitmap_file, -6, SEEK_CUR);

	}

	/* close file */
	fclose(bitmap_file);

	/* return success */
    return 1;

}
AUX_RGBImageRec *auxDIBImageLoad(char *filename){

	AUX_RGBImageRec *auxImage;
	long int width, height;
	unsigned char * bmp_texture;

	// Get dimensions
	get_bitmap_dimensions(filename, &width, &height);

	// Allocate space for data
	bmp_texture = (unsigned char *)malloc(3 * width * height * sizeof(unsigned char));


	// Load bitmap
	load_bitmap_texture(filename, bmp_texture, width, height);


	auxImage->data=bmp_texture;
	auxImage->sizeX=width;
	auxImage->sizeY=height;


	return auxImage;
}



namespace VisageSDK
{
	
Image::Image()
{

}

Image::Image(int x, int y)
{
	size_x = x;
	size_y = y;
}


void Image::set_size_x(int x)
{
	size_x = x;
}

void Image::set_size_y(int y)
{
	size_y = y;
}


int Image::get_size_x()
{
	return size_x;
}


int Image::get_size_y()
{
	return size_y;
}


int Image::get_n_channels()
{
	return n_channels;
}


unsigned char * Image::get_data()
{
	return data;
}


void Image::read_image(char* path)
{
#ifndef IPHONE
	cout << "reading image" << endl;

	if(!strcmp(path+(strlen(path) - 4),".jpg"))
	{
		cout << "tipe jpeg" << endl;
		read_jpeg_image(path);
	}
	else if(!strcmp(path+(strlen(path) - 4),".png"))
	{
		cout << "tipe png" << endl;
		read_png_image(path);
	}
	else if(!strcmp(path+(strlen(path) - 4),".bmp"))
	{
		cout << "tipe bmp" << endl;
		read_bmp_image(path);
	}
	else
	{
		cout << "Unknown image format, image: " << (path+(strlen(path) - 4)) << endl;
	}
    
#else
    
    NSString *fileString =	[NSString stringWithUTF8String:path];
    NSString *pathToImg = [ [ [NSBundle mainBundle] resourcePath ] stringByAppendingPathComponent:fileString ];
    UIImage *uiImage = [UIImage imageWithContentsOfFile:pathToImg];
    
    CGImageRef textureImage;
    CGContextRef textureContext;	
 	if( uiImage ) {
 		textureImage = uiImage.CGImage;
 		
 		int width = CGImageGetWidth(textureImage);
 		int height = CGImageGetHeight(textureImage);
 		
 		GLubyte *textureData;
		if(textureImage) {
 			/*			if(width%256!=0)
 			 {
 			 
 			 textureData = (GLubyte *) malloc(512 * 512 * 4);
 			 textureContext = CGBitmapContextCreate(textureData, 512, 512, 8, width * 4, CGImageGetColorSpace(textureImage), kCGImageAlphaPremultipliedLast);
 			 CGContextTranslateCTM (textureContext, 0, 512);
			 CGContextScaleCTM (textureContext, 1.0, -1.0);
			 CGContextScaleCTM(textureContext, 512/width, 512/height);
			 CGContextDrawImage(textureContext, CGRectMake(0.0, 0.0, (float)512, (float)512), textureImage);
			 width = 512;
			 height = 512;
			 }
			 else
			 {
			 */				
			textureData = (GLubyte *) malloc(width * height * 4);
			textureContext = CGBitmapContextCreate(textureData, width, height, 8, width * 4, CGImageGetColorSpace(textureImage), kCGImageAlphaPremultipliedLast);
			CGContextTranslateCTM (textureContext, 0, height);
 			CGContextScaleCTM (textureContext, 1.0, -1.0);
 			CGContextDrawImage(textureContext, CGRectMake(0.0, 0.0, (float)width, (float)height), textureImage);
            
            data = textureData;
            
            size_x = width;
            size_y = height;
			
 			//			}
 		}
        else
        {
            std::cout << "Error loading texture, no textureImage" << std::endl;
            data = NULL;
        }
    }
    else
    {
        std::cout << "Error loading texture, no uiImage" << std::endl;
        data = NULL;
    }
        
    CGContextRelease(textureContext);
#endif
	

}

    
#ifndef IPHONE


void Image::read_png_image(char* path)
{
  png_structp png_ptr;
  png_infop info_ptr;

  FILE *fp = fopen(path, "rb");
  if(fp == NULL)
    return NULL;

  unsigned char buf[PNG_BYTES_TO_CHECK];
  if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK)
    return 0;
  if(png_sig_cmp(&buf[0], (png_size_t)0, PNG_BYTES_TO_CHECK))
	  return 0;
/*  if(check_if_png(fp) == 0)
    return NULL;*/

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fclose(fp);
    return NULL;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    fclose(fp);
    png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
    return NULL;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
    fclose(fp);
    return NULL;
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

  // high-level read
  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, png_voidp_NULL);
  size_x = (int)png_get_image_width(png_ptr, info_ptr);
  size_y = (int)png_get_image_height(png_ptr, info_ptr);
  n_channels = png_get_channels(png_ptr, info_ptr);
  //color_type = png_get_color_type(png_ptr, info_ptr);


  fclose(fp);


	//TODO: maybe I'll need to use that padBytes var.
	//int padBytes = IJL_DIB_PAD_BYTES( width, n_channels );
	//GLubyte* pngimg = new GLubyte[height * ((width/*+padBytes*/)* n_channels)];
	data = new unsigned char[size_y * (size_x/*+padBytes*/)*n_channels ];


	int cnt = 0;
	for(int i=size_y-1;i>=0;i--)
	{
	for(int j=0;j<size_x;j++)
		{
			for(int k=0;k<n_channels;k++)
				data[cnt++] = png_get_rows(png_ptr, info_ptr)[i][j*n_channels + k];
		}
		//cnt+=padBytes;
	}

}




void Image::read_jpeg_image(char* path)
{

	#ifdef WIN32

		JPEG_CORE_PROPERTIES *p = new JPEG_CORE_PROPERTIES;

		if( ijlInit( p ) != IJL_OK ) return -1;

		p->JPGFile = path;

			// Read in params first so we get the right DIB.
		if( ijlRead( p, IJL_JFILE_READPARAMS ) != IJL_OK )
		{
			std::cout << "ijlRead READPARAMS with path: " << path << std::endl;
			std::cout << "returns: " << ijlRead( p, IJL_JFILE_READPARAMS ) << std::endl;
			delete p;
			return -1;
		}


		switch( p->JPGChannels )
		{
		  case 1:  p->JPGColor    = IJL_G;
				   p->DIBChannels = 3;
				   p->DIBColor    = IJL_RGB;//changed from IJL_BGR to IJL_RGB by -KaiK-
				   break;
		  case 3:  p->JPGColor    = IJL_YCBCR;
				   p->DIBChannels = 3;
				   p->DIBColor    = IJL_RGB;//changed from IJL_BGR to IJL_RGB by -KaiK-
				   break;
		  case 4:  p->JPGColor    = IJL_YCBCRA_FPX;
				   p->DIBChannels = 4;
				   p->DIBColor    = IJL_RGBA_FPX;
				   break;
		  default: p->DIBColor = (IJL_COLOR)IJL_OTHER;
				   p->JPGColor = (IJL_COLOR) IJL_OTHER;
				   p->DIBChannels = p->JPGChannels;
				   break;
		}

		p->DIBWidth    = p->JPGWidth;
		p->DIBHeight   = p->JPGHeight;
		p->DIBPadBytes = IJL_DIB_PAD_BYTES( p->DIBWidth, p->DIBChannels );

		int imageSize = p->DIBWidth * p->DIBChannels + p->DIBPadBytes;
		imageSize *= p->DIBHeight;
		p->DIBBytes = new BYTE[ imageSize ];

		if ( ijlRead( p, IJL_JFILE_READWHOLEIMAGE ) != IJL_OK )
		{
			std::cout << "ijlRead READWHOLE returns: " << ijlRead( p, IJL_JFILE_READWHOLEIMAGE ) << std::endl;
			return -1;
		}

			if ( p->DIBColor == IJL_RGBA_FPX )
			{
		  if ( p->DIBBytes )
			  delete[] p->DIBBytes;
				delete p;
				return -1;
			}


			// Flip the image - since library gives us upside down images.
			int jMax = p->DIBWidth * p->DIBChannels + p->DIBPadBytes;

			size_x = p->DIBWidth;
			size_y = p->DIBHeight;
			n_channels = p->DIBChannels;

			data = new unsigned char[jMax*size_y];

			for(int i=0; i < size_y; i++)
				for(int j=0; j < jMax; j++)
					data[i * jMax + j] = p->DIBBytes[((size_y - 1) - i) * jMax + j];


			// Clean up
			delete [] p->DIBBytes;
			delete p;
			//

	#else

			struct jpeg_decompress_struct cinfo;
			struct jpeg_error_mgr jerr;
			/* More stuff */
			FILE * infile;		/* source file */
			JSAMPROW buffer[1];		/* Output buffer 1 row sized */
			int row_stride;		/* physical row width in output buffer */

			if ((infile = fopen(path, "rb")) == NULL) {
				fprintf(stderr, "can't open jpeg file: %s\n", path);
				return -1;
			}

			/* Step 1: allocate and initialize JPEG decompression object */

			/* We set up the normal JPEG error routines*/
			cinfo.err = jpeg_std_error(&jerr);
			/* Now we can initialize the JPEG decompression object. */
			jpeg_create_decompress(&cinfo);

			/* Step 2: specify data source (eg, a file) */
			jpeg_stdio_src(&cinfo, infile);

			/* Step 3: read file parameters with jpeg_read_header() */
			jpeg_read_header(&cinfo, TRUE);

			/* Step 4: Start decompressor */
			jpeg_start_decompress(&cinfo);

			/* JSAMPLEs per row in output buffer */
			row_stride = cinfo.output_width * cinfo.output_components;
			buffer[0] = (unsigned char*)malloc(row_stride);

			size_x = cinfo.output_width;
			size_y = cinfo.output_height;
			n_channels = cinfo.output_components;


			data = new unsigned char[row_stride*size_y];

	
			/* Step 6: while (scan lines remain to be read) */
			/*           jpeg_read_scanlines(...); */

			int i=0;
			while (cinfo.output_scanline < cinfo.output_height)
			{
				i++;
				//write line into buffer line, taking into account a reverse order reading
				jpeg_read_scanlines(&cinfo, buffer, 1);
				int k=0;
				for(int j=0; j < row_stride; j++, k++)
				{
					data[(size_y-i)*row_stride + k] = buffer[0][j];
				}

			}

			free(buffer[0]);

			/* Step 8: Release JPEG decompression object */
			(void) jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);


#endif

}



void Image::read_bmp_image(char * path)
{
	AUX_RGBImageRec *texPointer[1];
	memset(texPointer,0,sizeof(void *)*1);

	texPointer[0] = auxDIBImageLoad(path);


	size_x = texPointer[0]->sizeX;
	size_y = texPointer[0]->sizeY;
	n_channels = 3;

	data = new unsigned char[size_x * size_y * n_channels];

	for(int i=0; i<size_x * size_y * n_channels; i++)
	{
		data[i] = texPointer[0]->data[i];
	}

	if (texPointer[0])							// If Texture Exists
	{
		if (texPointer[0]->data)					// If Texture Image Exists
		{
			free(texPointer[0]->data);				// Free The Texture Image Memory
		}
		free(texPointer[0]);						// Free The Image Structure
	}
}
    
#endif

}

//#endif

