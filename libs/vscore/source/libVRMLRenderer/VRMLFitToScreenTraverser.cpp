// VRMLFitToScreenTraverser.cpp: implementation of the VRMLFitToScreenTraverser class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include "VRMLFitToScreenTraverser.h"
#include  <math.h>
#include  <stdlib.h>
#include  <float.h>

namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VRMLFitToScreenTraverser::VRMLFitToScreenTraverser()
{
	Reset();
	
}

VRMLFitToScreenTraverser::~VRMLFitToScreenTraverser()
{

}

void VRMLFitToScreenTraverser::Reset()
{
	mSceneMinX = mSceneMinY = mSceneMinZ = FLT_MAX;
	mSceneMaxX = mSceneMaxY = mSceneMaxZ = FLT_MIN;

	mStoredVerts.clear();

	mNumberShapes = 0;
}

int VRMLFitToScreenTraverser::traverseTransform(VRMLModel *model,    // pointer to the traversed model
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
												myMatrix m)  // Complete transformation matrix
{
	// Save off old matrix
	glPushMatrix();
	glMultMatrixf((GLfloat *) m);

	return 0;
}

void VRMLFitToScreenTraverser::traverseTransformEnd(VRMLModel *model, VRMLNode *thisNode, int index)
{
	// Get back old GL_MODELVIEW matrix.

	glPopMatrix();
}
	
void VRMLFitToScreenTraverser::traverseMesh(VRMLModel *model,    // pointer to the traversed model
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
											VRMLMeshBones *bones)            // color per vertex (1) or per face (1)
{

	mNumberShapes++;
	
	GLfloat currModelviewMatrix[16];

	// Get current modelmatrix.
	glGetFloatv(GL_MODELVIEW_MATRIX, currModelviewMatrix);
	GLfloat x,y,z;

	for(int i=0; i<coordLength; i += 3)
	{
		// Since we're only dealing with the modelview matrix the w (1.0 parameter)
		// will be unchanged, since only perspective transformation affects this param.

		x = currModelviewMatrix[0]*coord[i] + currModelviewMatrix[4]*coord[i+1] 
				+ currModelviewMatrix[8]*coord[i+2] + currModelviewMatrix[12]*1.0f;

		y = currModelviewMatrix[1]*coord[i] + currModelviewMatrix[5]*coord[i+1] 
				+ currModelviewMatrix[9]*coord[i+2] + currModelviewMatrix[13]*1.0f;

		z = currModelviewMatrix[2]*coord[i] + currModelviewMatrix[6]*coord[i+1] 
				+ currModelviewMatrix[10]*coord[i+2] + currModelviewMatrix[14]*1.0f;

		// Store vertice.
		Point3d tempPoint(x,y,z, index, i / 3);

		mStoredVerts.push_back(tempPoint);

		// Scan points for extreme-values on all sides. This will be used to set up
		// perspective matrix.
		if(mSceneMaxX < x) mSceneMaxX = x;
		if(mSceneMinX > x) mSceneMinX = x;

		if(mSceneMaxY < y) mSceneMaxY = y;
		if(mSceneMinY > y) mSceneMinY = y;

		if(mSceneMaxZ < z) mSceneMaxZ = z;
		if(mSceneMinZ > z) mSceneMinZ = z;
	}
}

void VRMLFitToScreenTraverser::GetCoords(GLfloat &sceneMinX, 
																				 GLfloat &sceneMaxX, 
																				 GLfloat &sceneMinY, 
																				 GLfloat &sceneMaxY,
																				 GLfloat &sceneMinZ, 
																				 GLfloat &sceneMaxZ,
																				 GLfloat &maxRadius,
																				 vector<Point3d> &storedVerts)
{
	sceneMinX = mSceneMinX;
	sceneMaxX = mSceneMaxX;

	sceneMinY = mSceneMinY;
	sceneMaxY = mSceneMaxY;

	sceneMinZ = mSceneMinZ;
	sceneMaxZ = mSceneMaxZ;

	maxRadius = fabs(mSceneMinX - mSceneMaxX);
	if(fabs(mSceneMinY - mSceneMaxY) > maxRadius) maxRadius = fabs(mSceneMinY - mSceneMaxY);
	if(fabs(mSceneMinZ - mSceneMaxZ) > maxRadius) maxRadius = fabs(mSceneMinZ - mSceneMaxZ);

	storedVerts = mStoredVerts;
}

void VRMLFitToScreenTraverser::GetNumberShapes(int& noShap)
{
	noShap = mNumberShapes;
}

}
