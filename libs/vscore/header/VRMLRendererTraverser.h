// VRMLRenderer.h: interface for the VRMLRenderer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VRMLRENDERER_H__D6CA9C47_4E24_11D6_B175_009027A47E31__INCLUDED_)
#define AFX_VRMLRENDERER_H__D6CA9C47_4E24_11D6_B175_009027A47E31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

// CString
#ifdef WIN32
#include <windows.h>  //necessary for windows gl.h library
#endif

#include <map>

using namespace std;

#ifdef WIN32
	#include <gl\gl.h>
#elif defined(LINUX)
	#include <GL/gl.h>
#elif defined(MAC)
	#include <OpenGL/gl.h>
#elif defined(IPHONE)
	#include <OpenGLES/ES1/gl.h>
#endif
 
#include "VRMLModel.h"

#ifndef IPHONE_DEV
	#include "Image.hpp"
#endif

#define MAX_TEXTURES 500
#define MAX_MESHES 20


namespace VisageSDK
{

class SurfaceData;


/**
* A traverser for rendering the VRML model.
* An instance of this traverser is passed to VRMLModel::traverse() function. The traverser
* issues the OpenGL commands rendering the model to screen.
*
* Implemented in visageVRMLrenderer.lib
*/
class VRMLRendererTraverser : public VRMLTraverser
{
public:
	VRMLRendererTraverser();
	virtual ~VRMLRendererTraverser();



	virtual void traverseRootEnd(VRMLModel *model,VRMLNode *thisNode, int index) { mRenderPart = false; };

	virtual int traverseTransform(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
							 int index,      // index of the transform within the face model
							 float *c,       // Center of rotation expressed as translation (x,y,z)
							 float *negc,    // Negative translation of the center of rotation
							 float *r,       // Rotation expressed as axis and angle in radians (x,y,z,angle)
							 float *rq,      // Rotation expressed as quaternion (x,y,z,w)
							 float *s,       // Scale (x,y,z)
							 float *so,      // Scale orientation (acis and angle)
							 float *soq,     // Scale orientation (quaternion)
							 float *negso,   // Inverse of scale orientation (axis and angle)
							 float *negsoq,  // Inverse of scale orientation (quaternion)
							 float *t,       // Translation (x,y,z)
							 myMatrix m);  // Complete transformation matrix

	void traverseTransformEnd(VRMLModel *model, VRMLNode *thisNode,int index);

	virtual void traverseMesh(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
							 int index,           // index of the polygon mesh within the face model
							 VRMLMaterial *mat,   // material for this shape
							 float *coord,        // vertex coordinates
							 float *origCoord,    // original vertex coordinates
							 int coordLength,     // length of the vertex coordinates array (number of vertices * 3)
							 int *coordInd,       // vertex indices for triangles
							 int coordIndLength,  // length of coordInd array
							 float *norm,         // normals
							 int normLength,      // length of norm array
							 int *normInd,        // normal indices, per vertex
							 int normIndLength,   // length of normInd array
							 int NPV,              // Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
							 int normalsCalculated,// If 1, normals are calculated by VRMLModel; if 0, normals were read from the file
							 float creaseAngle,   // VRML creaseAngle (default 0.0; 1.2 is good value to smooth)
							 int solid,           // VRML solid (default 1)
							 int ccw,             // VRML counter-clockwise (default 1)
							 int convex,          // VRML convex (default 1)
							 float *tcoord,       // texture coordinates, if any (else zero)
							 int tcoordLength, 
							 int *tcoordInd, 
							 int tcoordIndLength, 
							 float *cpv,          // colors per vertex, if any (else zero)
							 int cpvLength, 
							 int *cpvInd, 
							 int cpvIndLength,
							 int CPV,           // color per vertex (1) or per face (1)
							 VRMLMeshMT *mt,		// pointer to the list of morph targets for this mesh
							 VRMLMeshBones *bones);            // color per vertex (1) or per face (1)


	// Set this if a separate piece of the VRML file is to be rendered.
	// If value is out of bound, nothing will be shown onscreen.
	// This will apply to the next rendering only. After that value needs to be reset.

	/**
	* Enter part-rendering mode and set the index of the part to be rendered.
	* The part-rendering is the mode where only one part (surface, indexed face set) of the model is rendered. Other parts are not rendered.
	* This will apply to the next rendering only, after which it is automatically reset. Therefore
	* this function should be called before each rendering while the part-rendering mode needs to be
	* turned on.
	*
	* @param index index of the surface (part) of the model to be rendered (corresponds to VRMLModel::coordList). 
	*/
	void SetRenderingIndex(int index);

	// As defined in surfaceData. Call this with NULL to turn off this feature.
	void EnterLipEditingMode(SurfaceData *lipSurface);

	void SetFlashingPointSize(float pointSize) { mFlashingPointSize = pointSize; };

private:

	// Changed by KStankovic
	// Loads textures (for now .jpg and .bmp) using Intels JPEG library and opengl's
	// aux lib. Scales images to nearest lower power of 2 for speed. Returns
	// texture index on success or -1 on failure. folder path is the path to the folder
	// where the model is, and full path can be either just the file name or the full path and name.
	// target is the type of the target texture, for example GL_TEXTURE_2D,
	// GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT, GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT,
	// GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT, ... It is used to detect what kind of texture is
	// being loaded. If the texture is Cubemap then the return value will be texture index only
	// in case of GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT texture. It relates to the whole Cubemap
	// texture. For all other Cubemap textures (GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT,
	// GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT, ...) it will return a default value.
	int LoadTextures(char *fullPath, char *folderPath, GLenum target);
	//

	SurfaceData *mLipsSurface;

	// renders a mesh node 
	void RenderMesh(
		VRMLMaterial *mat,   // material for this shape
		float *coord,        // vertex coordinates
		float *origCoord,    // original vertex coordinates
		int coordLength,     // length of the vertex coordinates array (number of vertices * 3)
		int *coordInd,       // vertex indices for triangles
		int coordIndLength,  // length of coordInd array
		float *norm,         // normals
		int normLength,      // length of norm array
		int *normInd,        // normal indices, per vertex
		int normIndLength,   // length of normInd array
		int NPV,              // Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
		int normalsCalculated,// If 1, normals are calculated by VRMLModel; if 0, normals were read from the file
		float creaseAngle,   // VRML creaseAngle (default 0.0; 1.2 is good value to smooth)
		int solid,           // VRML solid (default 1)
		int ccw,             // VRML counter-clockwise (default 1)
		int convex,          // VRML convex (default 1)
		float *tcoord,       // texture coordinates, if any (else zero)
		int tcoordLength, 
		int *tcoordInd, 
		int tcoordIndLength, 
		float *cpv,          // colors per vertex, if any (else zero)
		int cpvLength, 
		int *cpvInd, 
		int cpvIndLength,
		int CPV);           // color per vertex (1) or per face (1)

	// Index to loaded textures, saved with path and index to the loaded texture.
	char *loadedTextureName[MAX_TEXTURES];
	GLuint loadedTexture[MAX_TEXTURES];
	int nLoadedTextures;
//	map<my_string, GLuint> mLoadedTextures;

	bool mRenderPart;
	int mRenderShape;

	// Size of vertex that is flashing.
	GLfloat mFlashingPointSize;

	int nBuffersCreated;
	int nWorkingBuffer;
	GLuint meshBuffer[MAX_MESHES];
	bool isStaticDataInBuffer;

};

}


#endif // !defined(AFX_VRMLRENDERER_H__D6CA9C47_4E24_11D6_B175_009027A47E31__INCLUDED_)
