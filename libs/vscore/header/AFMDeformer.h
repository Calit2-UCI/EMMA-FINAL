// AFMDeformer.h: interface for the AFMDeformer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AFMDEFORMER_H__EF60A536_6203_4639_8200_8308D56AA74D__INCLUDED_)
#define AFX_AFMDEFORMER_H__EF60A536_6203_4639_8200_8308D56AA74D__INCLUDED_

#include <stdlib.h>
#include <stdio.h>
#include <float.h>


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

class FBAPs;
class VRMLModel;

/**
* Abstract MPEG-4 compatible deformer class.
*
* Provides a general interface for mapping MPEG-4 FBAPs to the specific modification of 
* the 3d model (VRMLModel) geometry. An AFMDeformer must implement functions that modify (deform) the model geometry based 
* on MPEG-4 FBAPs. 
*
* Provides a mechanism to selectively enable/disable animation of (groups of)
* parameters in order to save CPU time, selectively replace animation of some parameters by
* an implementation from another deformer, or other purposes.
*
* The default implementation, BaseAFMDeformer, implements the deformation of the model based on morphing and skinning.
* Other implementations may implement any method of deformation, e.g. a muscle based model or other.
*/
class VISAGE_DECLSPEC AFMDeformer  
{
public:

	AFMDeformer();
	virtual ~AFMDeformer();
	/**
	* Add a set of Face and Body Animation Parameters (FBAPs) to the list of FBAPs to be applied to the 3D model (VRMLModel).
	* The FBAPs are *not* immediately applied to the model 
	* geometry - to do that, the updateGeometry() function must be called. The reason for this
	* is to allow several sets of FBAPs to be applied together by several subsequent calls
	* to applyFBAPs().
	* 
	* In a simple case, you may want to apply a set of FBAPs to the face and see the result immediately.
	* In this case, you call applyFBAPs() followed by updateGeometry().
	*
	* In a slightly more complex case, you may want to apply several sets of FBAPs (for example, when mixing
	* several FBAP streams). In this case, applyFBAPs() is called several times with all FBAPs to apply. The
	* FBAPs are merged together internally and applied to the actual model geometry
	* when the updateGeometry() function is called.
	* @param fbaps the FBAPs to apply.
	* @param model the VRMLModel on which FBAPs are applied.
	* @see updateGeometry
	*/	
	virtual void applyFBAPs(VRMLModel *model,const FBAPs *fbaps)=0;

	/**
	* Clear the list of FBAPs to be applied to the model.
	* Clears the current FBAPs. This effectively returns the model geometry to neutral position next time updateGeometry() is called. 
	* @see applyFBAPs
	* @see updateGeometry
	*/
	virtual void clearFBAPs()=0;

	/**
	* Updates the geometry of the VRMLModel using the current FBAPs.
	* The model is deformed corresponding to the current FBAPs. The "current FBAPs" are the internaly
	* stored FBAPs that have been set using applyFAPs() function. The list of current FBAPs is cleared
	* when clearFBAPs() is called, and all subsequent calls to applyFBAPs() add FBAPs to the current FBAPs list.
	* @see applyFBAPs
	*/
	virtual void updateGeometry(VRMLModel *model)=0;

	/**
	* Write the deformer data into file.
	* This method may write the deformer data into a file, in any format, if it is intended
	* that a FileIO object should read the data. For example, the default implementation of AFMDeformer - BaseAFMDeformer
	* writes out the part of the AFM file relating to the deformation, and the BaseFileIO
	* class reads this data.
	*
	* A custom deformer implementation may choose not to implement this function, i.e. simply
	* leave it empty, if it is not necessary to write deformation data. In general, writing out the
	* deformer data is useful for file format conversions and in most other situations can
	* be skipped.
	*
	* @param f the handle of the file into which the data should be written
	* @param format the file format in which the data should be written; may be used to support multiple formats; the format is indicated by extension, e.g. "afm"
	*/
	virtual void write(FILE *f, char *format)=0;

	/**
	* Get the deformation filter.
	* The deformation filter specifies which FBAPs are applied by the AFMDeformer. The information is stored
	* in the BAPs::groupMask and FAPs::groupMask arrays in the filter, each groupMask corresponding to one FAP or
	* BAP. If the groupMask for a parameter is set to 1, it means that the deformer applies this parameter;
	* value 0 means that the deformer does not apply the parameter.
	*
	* The default constructor AFMDeformer() sets all values to 1, which would mean that all parameters are
	* applied. Each implementation of AFMDeformer must set the values appropriately in order to indicate which
	* parameters it implements.
	*
	* Additionally, the implementation of AFMDeformer, when deforming the model, must ignore the parameters 
	* for which the filter is set to 0.
	*
	* The filter can be used in two ways:
	*
	* (1) To find out, for a particular AFMDeformer, which parameters it implements (e.g. some deformers may
	* only implement body animation and ignore face animation, or vice versa; the BaseAFMDeformer implements
	* any parameters it finds in the AFM file so it may contain any combination of implemented parameters).
	*
	* (2) to force a particular AFMDeformer to ignore some of the parameters. This may be useful in various 
	* situations, for example to reduce the CPU load by ignorind some less important parameters, or to force
	* the default AFMDeformer to ignore a set of parameters, and replace the implementation of those
	* parameters by another AFMDeformer - in this way several deformers may be combined, each implementing a subset
	* of animation parameters.
	*/
	FBAPs *getFilter(){return filter;};

	
protected:
	FBAPs *filter;

	/**
	* Filter the FBAPs - set to 0 value all FBAPs for which the AFMDeformer's filter is 0.
	* This is a helper function for the implementations of custom AFMDeformers. The recommended usage
	* is to pass incoming FBAPs to this function, then apply the resulting filtered FBAPs
	* when doing the deformation.
	* 
	* All FBAPs that must be ignored according to AFMDeformer::filter are set to 0. The new FBAPs
	* object is returned. Note that the returned value is a pointer to a newly allocated object, and
	* it should be freed after use to avoid memory leaks.
	*
	* For more information about filtering, see @ref AFMDeformer::getFilter()
	*
	* @param fbaps FBAPs to be filtered
	* @return filtered FBAPs; this is a newly allocated object and should be freed after use to avoid memory leaks.
	*/
	FBAPs *filterFBAPs(const FBAPs *fbaps);
};

}

#endif // !defined(AFX_AFMDEFORMER_H__EF60A536_6203_4639_8200_8308D56AA74D__INCLUDED_)
