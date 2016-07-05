// VRMLFitToScreenTraverser.h: interface for the VRMLFitToScreenTraverser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VRMLFITTOSCREENTRAVERSER_H__9D406800_4FBF_11D6_B175_009027A47E31__INCLUDED_)
#define AFX_VRMLFITTOSCREENTRAVERSER_H__9D406800_4FBF_11D6_B175_009027A47E31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <vector>
using namespace std;

#include "VRMLModel.h"
#include "Point3d.h"

namespace VisageSDK
{

/**
* A traverser for measuring the VRML model.
* VRMLFitToScreenTraverser measures the size of the VRMLModel it traverses and obtains 
* the necessary measurements that can be used to automatically fit the model to screen when rendering it.
* The traversal is initiated by passing an instance of VRMLFitToScreenTraverser to the VRMLModel::traverse()
* function. The traverser will traverse the model and fetch the necessary data. After that, the
* GetCoords() and GetNumberShapes() functions are used to get the data.
*
* Implemented in visageVRMLrenderer.lib
* @see GetCoords()
* @see GetNumberShapes()
*/
class VISAGE_DECLSPEC VRMLFitToScreenTraverser : public VRMLTraverser 
{
public:
	/**
	* Constructor. Constructs a VRMLFitToScreenTraverser.
	*/
	VRMLFitToScreenTraverser();
	virtual ~VRMLFitToScreenTraverser();

	#include "VRMLModel.h"


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
	
	virtual void traverseTransformEnd(VRMLModel *model, VRMLNode *thisNode, int index);

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
	
	
	/**
	* Get the min and max coordinates.
	* Returns the minimum and maximum coordinates of the traversed model, effectively its axis-aligned
	* bounding box.
	*
	* @param sceneMinX returns the smallest X coordinate of the model
	* @param sceneMinY returns the smallest Y coordinate of the model
	* @param sceneMinZ returns the smallest Z coordinate of the model
	* @param sceneMaxX returns the biggest X coordinate of the model
	* @param sceneMaxY returns the biggest Y coordinate of the model
	* @param sceneMaxZ returns the biggest Z coordinate of the model
	* @param maxRadius returns the radius of the model, i.e. the radius of a sphere that can fit the whole model
	* @param storedVerts 
	*/
	void GetCoords(GLfloat &sceneMinX, GLfloat &sceneMaxX, GLfloat &sceneMinY, GLfloat &sceneMaxY,
								 GLfloat &sceneMinZ, GLfloat &sceneMaxZ, GLfloat &maxRadius, vector<Point3d> &storedVerts);

	/**
	* Gets the number of shapes which the model contains.
	* A shape is an Indexed face set, i.e. an independent mesh within a model.
	*
	* @param noShap returns the number of shapes which the model contains
	*/
	void GetNumberShapes(int& noShap);

	/**
	* Reset.
	* This must be called every time the class is reused, i.e. before a new traversal.
	*/
	void Reset();

private:
	
	GLfloat mSceneMinX, mSceneMaxX, mSceneMinY, mSceneMaxY, mSceneMinZ, mSceneMaxZ;

	vector<Point3d> mStoredVerts;

	// How many shapes does the model consist of?
	int mNumberShapes;
};

}
#endif // !defined(AFX_VRMLFITTOSCREENTRAVERSER_H__9D406800_4FBF_11D6_B175_009027A47E31__INCLUDED_)
