
#ifndef IL_IMAGE
#define IL_IMAGE

#include <il/il.h>
#include <il/ilu.h>
#include <il/ilut.h>


namespace VisageSDK
{

////////////////////////////////////////////////////////////
/**
*  Image class. This class is used to load and save images.
*  it needs devIl
*/
class Image  
{
public:
	/**
	* Constructor.
	*/
	Image()
	{
		data=NULL;
		width=0;
		height=0;
	}

	/**
	* Destructor.
	*/
	~Image()
	{
		if (data != NULL)
			delete [] data;
	}

	/**
	* Check if an image is valid.
	* @return true if image is valid, false otherwise.
	*/
	bool ValidImage() { return ((data == NULL) ? false : true); }

	/**
	* Create an image with the given dimensions.
	* @param w : width of the image.
	* @param h : height of the image.
	*/
	void Create(int w, int h)
	{
		if (data!=NULL)
		{
			delete []  data;
			data=NULL;
		}

		if ((w > 0) && (h > 0))
		{
			width = w;
			height = h;
			data = new unsigned char[width * height * 3];
			memset(data, 40, sizeof(char) * width * height * 3);

		}
	}

	/**
	* Load image.
	* @param filename : file to load.
	*/
	int Load(const char * filename)
	{
		if(data!=NULL)
		{
			delete []  data;
			data=NULL;
		}

		ilInit();
		ILuint ili;
		ilGenImages(1,&ili);
		ilBindImage(ili);
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
		ilEnable(IL_ORIGIN_SET);
		ilLoadImage((ILstring)filename);
		ILenum Error = ilGetError(); 
		if (Error) 
			return 0; 

		width=ilGetInteger(IL_IMAGE_WIDTH );
		height=ilGetInteger(IL_IMAGE_HEIGHT );
		int size=width*height*3;
		data=new unsigned char[size];

		ilCopyPixels(0, 0, 0, width, height, 1, IL_RGB, IL_UNSIGNED_BYTE, data);
		ilDeleteImages(1, &ili);
		ilShutDown();
		return 1;
	}
	/**
	* Save image.
	* @param filename : file to save.
	*/
	void Save(const char* filename)
	{
		ilInit();

		ILuint id;
		ilGenImages(1, &id);
		ilBindImage(id);

		ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, data);
		ilEnable(IL_FILE_OVERWRITE);
		ilSaveImage((ILstring)filename);

		ilDeleteImages(1, &id);
		ilShutDown();
	}


	/**
	* Return width of the image.
	*/
	int GetWidth() { return width; }
	/**
	* Return height of the image.
	*/
	int GetHeight() { return height; }

	/**
	* Return image data.
	*/
	unsigned char* GetData() { return data; }

protected:
	/** 
	* width of the image.
	*/
	int width;
	/** 
	* height of the image.
	*/
	int height;
	/** 
	image data.
	*/
	unsigned char* data;

};

}

#endif