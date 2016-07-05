// CubemapWriteTraverser.h: interface for the CubemapWriteTraverser class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "VRMLModel.h"


namespace VisageSDK
{

/**
* A VRML model traverser that writes the Cubemap nodes at the end of a file. 
* This is a sub-class of VRMLTraverser. The write() functions of the BaseFileIO
* use this class for writing out the Cubemap nodes at the end of a file, so it is recommended to use
* those functions rather than using this class directly. The callback function traverseMesh is used
* to obtain data from the model and print it out inside the Cubemap nodes at the end of a file using the
* VRML syntax. Other
* callback functions traverseRoot, traverseRootEnd, traverseTransform, traverseTransformEnd, traverseGroup,
* traverseGroupEnd and traverseOther are empty.
*
* The source code of this class is available in the src folder.
*
* Implemented in visagesdkAFM.lib
* @see VRMLTraverser
* @see VRMLModel
*/
class CubemapWriteTraverser : public VRMLTraverser  
{
public:
	CubemapWriteTraverser();
	CubemapWriteTraverser(FILE *fp);
	virtual ~CubemapWriteTraverser();

	void traverseRoot(VRMLModel *model,VRMLNode *thisNode,int index);
	void traverseRootEnd(VRMLModel *model,VRMLNode *thisNode,int index);
	int traverseTransform(VRMLModel *model,    // pointer to the traversed model
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

	void traverseTransformEnd(VRMLModel *model,VRMLNode *thisNode,int index);
	void traverseGroup(VRMLModel *model,VRMLNode *thisNode,int index);
	void traverseGroupEnd(VRMLModel *model,VRMLNode *thisNode,int index);
	void traverseMesh(VRMLModel *model,    // pointer to the traversed model
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
	void traverseOther(VRMLModel *model,VRMLNode *thisNode,int index);

private:
	FILE *outFile;
	bool protoWritten;
};

}
