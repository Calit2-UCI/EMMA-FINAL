// VRMLModelRenderer.h: interface for the VRMLModelRenderer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VRMLModelRenderer_H__906584C0_4EF3_11D6_B175_009027A47E31__INCLUDED_)
#define AFX_VRMLModelRenderer_H__906584C0_4EF3_11D6_B175_009027A47E31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


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


#include "Point3d.h"

#include <vector>
using namespace std;


#define MAX_N_MODELS 1000

namespace VisageSDK
{

class VRMLModel;
class VRMLModelRendererTraverser;
class SurfaceData;


/**
* The VRML renderer.
* Abstract interface for the basic rendering of the VRMLModel. This is the minimal set of functions
* that a renderer should have.
*
* Note that rendering process must include morphing and skinning, which is not done in
* the animation/deformation stage. The purpose of leaving the morphing and skinning to the
* renderer is to leave the possibility of hardware implementations. The default renderer
* simply calls the VRMLModel::applySkinning() and VRMLModel::applyMorphing() functions and thus
* uses the default software morphing and skinning implementation (note that these functions must be called
* in this order, first skinning then morphing).
*
* Implemented in visageVRMLRenderer.lib
*
* Demonstrated in <a href="../Samples/MS_Visual_C++_6.0/Example1/doc/index.html">Example1</a> and other examples.
*
*/
class VRMLModelRenderer  
{
public:

	/**
	* Constructor.
	*/
	VRMLModelRenderer(){};

	/**
	* Destructor.
	*/
	virtual ~VRMLModelRenderer(){};


	/**
	* Initialise and add a 3D model to the list of models to render.
	* Initialise the renderer with a VRMLModel to render. The implementation should add the model
	* to its list of models to render, and perform any other necessary initialization steps.
	* The implementation must support the rendering of multiple models, achieved
	* by calling this function several times with different
	* models.
	*
	* @param initModel the model to load into the renderer.
	*/
	virtual void Init(VRMLModel *initModel)=0;

	/**
	* Remove a VRMLModel from the list of models to be rendered.
	* The implementation must remove the model from its list of models to be rendered.
	*
	* @param theModel the model to be removed from the renderer.
	*/
	virtual void RemoveModel(VRMLModel *theModel)=0;

	/**
	* Render the models to screen.
	* The implementation should render the loaded models, i.e. perform the drawing. If no model is loaded, clears the screen.
	*/
	virtual void RenderToScreen()=0;

	/**
	* Closes down and resets renderer. 
	*/
	virtual void Close()=0;
};

}

#endif // !defined(AFX_VRMLModelRenderer_H__906584C0_4EF3_11D6_B175_009027A47E31__INCLUDED_)
