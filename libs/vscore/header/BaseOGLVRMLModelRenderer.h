// BaseOGLVRMLModelRenderer.h: interface for the BaseOGLVRMLModelRenderer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BaseOGLVRMLModelRenderer_H__906584C0_4EF3_11D6_B175_009027A47E31__INCLUDED_)
#define AFX_BaseOGLVRMLModelRenderer_H__906584C0_4EF3_11D6_B175_009027A47E31__INCLUDED_

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

#ifdef WIN32
#include <gl\gl.h>
#elif defined(LINUX)
#include <GL/gl.h>
#elif defined(MAC)
#include <OpenGL/gl.h>
#endif


#include "Point3d.h"
#include "VRMLModelRenderer.h"
#include "VRMLRendererTraverser.h"
#include "VRMLModel.h"

#include <vector>
using namespace std;

#define MAX_N_MODELS 1000


namespace VisageSDK
{

class VRMLRendererTraverser;
class SurfaceData;


/**
* The basic OpenGL 3D model renderer.
* The basic OpenGL-based 3D model renderer. Performs morphing, skinning and basic drawing
* of one or more 3D model of type VRMLModel.
*
* The rendering can be integrated in an MFC application by implementing a class extending 
* the BaseOGLVRMLModelRenderer class and the MFC cView class. This is shown in the <a href="../Samples/MS_Visual_C++_6.0/Example1/doc/index.html">Example1</a> (and other examples)
* where the COpenGLWnd class does this. It is recommended to simply copy the openGLWnd.cpp and .h files from the example.
* In this case, it should not be necessary to go into details of the BaseOGLVRMLModelRenderer.
* In case additional rendering effects are needed (e.g. a background image, additional scene objects, etc.), COpenGLWnd  class
* is the place to do so and this is explained in the example.
*
* Implemented in visageBaseOGLVRMLModelRenderer.lib
*
* Demonstrated in <a href="../Samples/MS_Visual_C++_6.0/Example1/doc/index.html">Example1</a> and other examples.
*
*/
class VISAGE_DECLSPEC BaseOGLVRMLModelRenderer  : public VRMLModelRenderer
{
public:
	/**
	* Constructor.
	* Constructs a new BaseOGLVRMLModelRenderer.
	*/
	BaseOGLVRMLModelRenderer();

	/**
	* Destructor.
	*/
	virtual ~BaseOGLVRMLModelRenderer();


	/**
	* Initialise.
	* Initialise the renderer with a VRMLModel to render. Standard rendering mode is set.
	* The view is set such that the model is fully visible on screen.
	*
	* This function can be called several times with different
	* models in case more than one model is rendered. Up to 1000 models are supported.
	*
	* @param initModel the model to load in the renderer.
	*/
	void Init(VRMLModel *initModel);

	/**
	* Remove a VRMLModel from the list of models to be rendered.
	* When a model is removed from the scene, this should be called to remove it from the list of models to be rendered.
	*/
	void RemoveModel(VRMLModel *theModel);


	/**
	* Render the model to screen.
	* Render the loaded model to screen. If no model is loaded, clears the screen.
	*/
	void RenderToScreen();



	/**
	* Closes down and resets renderer. 
	*/
	void Close();

private:
	void RenderToScreen(VRMLModel *renderModel);
	VRMLModel *mLoadedModel[MAX_N_MODELS]; // loaded VRML models
	int mNLoadedModels; // number of loaded VRML models

	VRMLRendererTraverser *mTraversRenderer;
};

}

#endif // !defined(AFX_BaseOGLVRMLModelRenderer_H__906584C0_4EF3_11D6_B175_009027A47E31__INCLUDED_)
