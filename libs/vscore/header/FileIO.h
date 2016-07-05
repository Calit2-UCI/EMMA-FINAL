// FileIO.h: interface for the FileIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEIO_H__6EEE8DC1_D8DA_4BDC_AED9_6F9F1F2075CF__INCLUDED_)
#define AFX_FILEIO_H__6EEE8DC1_D8DA_4BDC_AED9_6F9F1F2075CF__INCLUDED_



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




#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace VisageSDK
{

class VRMLModel;
class VRMLModelModifier;
class AFMDeformer;

/**
* File input/output interface for VRMLModel and AFM instances.
* Abstract interface for reading VRMLModel and AFM instances from files, and writing them into files. 
* One or more FileIOs may be attached to an AFM or a VRMLModel. By default, a BaseFileIO is 
* attached to any AFM and VRMLModel. A FileIO implementation can read/write one or more file types, 
* defined by file extension. When an AFM or VRMLModel class must read or write a file, it asks each 
* attached FileIO instance whether it can handle the requested file type. The first FileIO that 
* can handle the requested file type is used.
*/
class VISAGE_DECLSPEC FileIO  
{
public:
	FileIO(){ refs = 1; };
	virtual ~FileIO(){};

	/**
	* Read the VRMLModel from a file.
	*
	* The implementation of this function must read the file designated by fileName and
	* fill in the data structures of VRMLModel based on the data from the file.
	*
	* The function must first check the extension of the fileName. If the extension does not
	* correspond to a file type that this implementation knows how to read, the read function
	* must return -1.
	* If it is a supported file type, but reading was not succesful (file not found, corrupt, wrong version or similar
	* reason), it returns 0. On succesful reading, return value is 1.
	*
	* The read function must allocate (when necessary) and fill the data attributes of the VRMLModel
	* that describe the model scene graph, geometry and materials.The detailed description of each attribute
	* is given in the @ref VRMLModel documentation under "Public Attributes". There it is also indicated
	* which attributes must be provided by read(), and which may be omitted, and how each attribute should be treated.
	*
	* IMPORTANT: after initializing all data of the VRMLModel, the VRMLModel::finalizeFileReading() must be called
	* in order to let the VRMLModel perform final initializations after file reading.
	*
	* @param fileName the name of the file to be read (with full path if necessary)
	* @param model the model to be read
	*
	* @return 1 if succesful; 0 if file could not be read (file not found, file corrupt, wrong version etc.); -1 if the extension of the fileName does not correspond to any supported file type.
	*/
	virtual int read(char *fileName, VRMLModel *model) = 0;

	/**
	* Write the VRMLModel into a file.
	*
	* The implementation of this function must write the given VRMLModel into a file designated by fileName.
	*
	* The function must first check the extension of the fileName. If the extension does not
	* correspond to a file type that this implementation knows how to write, the write function
	* must return -1.
	* If it is a supported file type, but writing was not succesful (file could not be oppen, corrupt data or similar
	* reason), it returns 0. On succesful writing, return value is 1.
	*
	* @param fileName the name of the file to be write (with full path if necessary)
	* @param model the model to be written into file
	*
	* @return 1 if succesful; 0 if file could not be read (file not found, file corrupt etc.); -1 if the extension of the fileName does not correspond to any supported file type.
	*/
	virtual int write(char *fileName, VRMLModel *model) = 0;

	/**
	* Read the AFM from a file.
	*
	* The implementation of this function must read the file designated by fileName,
	* fill in the data structures of VRMLModel based on the data from the file, as well
	* as return one or more AFMDeformer objects based on the animation parameter data read from the file.
	*
	* The function must first check the extension of the fileName. If the extension does not
	* correspond to a file type that this implementation knows how to read, the read function
	* must return -1.
	* If it is a supported file type, but reading was not succesful (file not found, corrupt, wrong version or similar
	* reason), it returns 0. On succesful raeding, return value is 1.
	*
	* The read function must allocate (when necessary) and fill the data attributes of the VRMLModel
	* that describe the model scene graph, geometry and materials.The detailed description of each attribute
	* is given in the @ref VRMLModel documentation under "Public Attributes". There it is also indicated
	* which attributes must be provided by read(), and which may be omitted, and how each attribute should be treated.
	*
	* IMPORTANT: after initializing all data of the VRMLModel, the VRMLModel::finalizeFileReading() must be called
	* in order to let the VRMLModel perform final initializations after file reading.
	*
	* Besides the VRMLModel, the reader function should create one or more AFMDeformer objects that will
	* be attached to the AFM and used to implement animation. The implementation may use the default
	* deformer class @ref BaseAFMDeformer, and/or implement custom ones.
	*
	* @param fileName the name of the file to be read (with full path if necessary)
	* @param model the model to be read
	* @param deformers a list of deformer objects created by the read function; these deformers will be attached the AFM; the list of pointer is allocated to hold up to 100 pointers, and the read function should assign them, e.g. _deformers[0] = new BaseAFMDeformer(...).
	* @param nDeformers returns the number of deformers returned in the deformers list
	*
	* @return 1 if succesful; 0 if file could not be read (file not found, file corrupt etc.); -1 if the extension of the fileName does not correspond to any supported file type.
	*
	* @see VRMLModel
	* @see VRMLModelModifier
	*/
	virtual int read(char *fileName, VRMLModel *model, AFMDeformer **deformers, int *nDeformers) = 0;

	/**
	* Write the AFM into a file.
	*
	* The implementation of this function must write out an %AFM file based on the VRMLModel data
	* and the list of AFMDeformer objects that implement the animation (the AFMDeformer objects
	* implement a function to write their data into a file).
	* The %AFM file to be created is designated by fileName.
	*
	* The function must first check the extension of the fileName. If the extension does not
	* correspond to a file type that this implementation knows how to write, the write function
	* must return -1.
	* If it is a supported file type, but writing was not succesful (file could not be oppen, corrupt data or similar
	* reason), it returns 0. On succesful writing, return value is 1.
	*
	*
	* @param fileName the name of the file to be write (with full path if necessary)
	* @param model the model to be written into file
	* @param deformers a list of deformer objects attached to the AFM
	* @param nDeformers the number of deformers in the deformer list
	*
	* @return 1 if succesful; 0 if file could not be read (file not found, file corrupt etc.); -1 if the extension of the fileName does not correspond to any supported file type.
	*
	* @see VRMLModel
	* @see VRMLModelModifier
	*/
	virtual int write(char *fileName, VRMLModel *model, AFMDeformer **deformers, int nDeformers) = 0;

	// Increments the number of references to this FileIO object. Initial number of
	// references is 1 (after the construction of the object).
	void ref() { refs++; };

	// Decrements the number of references to this FileIO object. When the number of
	// references becomes equal to 0 object is being destroyed.
	void unref() { refs--; if (refs == 0) delete this; };

private:
	// Counter of references to this FileIO object.
	int refs;

};

}

#endif // !defined(AFX_FILEIO_H__6EEE8DC1_D8DA_4BDC_AED9_6F9F1F2075CF__INCLUDED_)
