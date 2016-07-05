// CubemapWriteTraverser.cpp: implementation of the CubemapWriteTraverser class.
//
//////////////////////////////////////////////////////////////////////

#include "CubemapWriteTraverser.h"
#include "Node.h"


namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CubemapWriteTraverser::CubemapWriteTraverser()
{
	outFile = 0;
	protoWritten = false;
}

CubemapWriteTraverser::CubemapWriteTraverser(FILE *fp)
{
	outFile = fp;
	protoWritten = false;
}


CubemapWriteTraverser::~CubemapWriteTraverser()
{
}


void CubemapWriteTraverser::traverseRoot(VRMLModel *model,VRMLNode *thisNode,int index)
{
}


void CubemapWriteTraverser::traverseRootEnd(VRMLModel *model,VRMLNode *thisNode,int index)
{
}

int CubemapWriteTraverser::traverseTransform(VRMLModel *model,    // pointer to the traversed model
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
										  myMatrix m)
{
	return(0);
}

void CubemapWriteTraverser::traverseTransformEnd(VRMLModel *model,VRMLNode *thisNode,int index)
{
}


void CubemapWriteTraverser::traverseGroup(VRMLModel *model,VRMLNode *thisNode,int index)
{
}


void CubemapWriteTraverser::traverseGroupEnd(VRMLModel *model,VRMLNode *thisNode,int index)
{
}


void CubemapWriteTraverser::traverseMesh(VRMLModel *model,    // pointer to the traversed model
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
	int i;

	if (mat->cubemapNames[0] != NULL)
	{
		if (protoWritten == false)
		{
			fprintf(outFile, "\n\nPROTO Cubemap [\n");
			fprintf(outFile, "    exposedField     SFString     IFS                 \"\"\n");
			fprintf(outFile, "    exposedField     MFString     urls                []\n");
			fprintf(outFile, "    exposedField     MFVec3f      texCoord            []\n");
			fprintf(outFile, "]{Group{}}\n");

			protoWritten = true;
		}

		fprintf(outFile, "\n\nCubemap {\n");
		fprintf(outFile, "    IFS \"%s\"\n", thisNode->getName());
		fprintf(outFile, "    urls [\n");
		for (i = 0; i < 6; i++)
		{
			if (i != 5) fprintf(outFile, "           \"%s\",\n", mat->cubemapNames[i]);
			else fprintf(outFile, "           \"%s\"\n", mat->cubemapNames[i]);
		}
		fprintf(outFile, "         ]\n");

		fprintf(outFile, "    texCoord [\n");
		for(i = 0; i < tcoordLength; i += 3)
		{
			if (i != (tcoordLength - 3))
				fprintf(outFile, "               %g %g %g,\n", tcoord[i], tcoord[i + 1], tcoord[i + 2]);
			else
				fprintf(outFile, "               %g %g %g\n", tcoord[i], tcoord[i + 1], tcoord[i + 2]);

		}
		fprintf(outFile, "             ]\n");
		fprintf(outFile, "}\n");
	}
}


void CubemapWriteTraverser::traverseOther(VRMLModel *model,VRMLNode *thisNode,int index)
{
}

}
