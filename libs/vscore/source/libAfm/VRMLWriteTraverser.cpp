// VRMLWriteTraverser.cpp: implementation of the VRMLWriteTraverser class.
//
//////////////////////////////////////////////////////////////////////

#include "VRMLWriteTraverser.h"
#include "Node.h"


namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VRMLWriteTraverser::VRMLWriteTraverser()
{
	outFile = 0;
	indentLevel = 0;
}

VRMLWriteTraverser::VRMLWriteTraverser(FILE *fp)
{
	outFile = fp;
	indentLevel = 0;
}


VRMLWriteTraverser::~VRMLWriteTraverser()
{

}

void VRMLWriteTraverser::indent()
{
	for(int i =0;i<indentLevel;i++)
		fprintf(outFile," ");
}

void VRMLWriteTraverser::traverseRoot(VRMLModel *model,VRMLNode *thisNode,int index)
{
		fprintf(outFile,"#VRML V2.0 utf8\n");
}


void VRMLWriteTraverser::traverseRootEnd(VRMLModel *model,VRMLNode *thisNode,int index)
{
}

int VRMLWriteTraverser::traverseTransform(VRMLModel *model,    // pointer to the traversed model
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
	indent();fprintf(outFile,"DEF %s Transform\n",thisNode->getName());
	indent();fprintf(outFile,"{\n");
	indentLevel++;

	indent();fprintf(outFile,"center %f %f %f\n",c[0],c[1],c[2]);
	indent();fprintf(outFile,"rotation %f %f %f %f\n",r[0],r[1],r[2],r[3]);
	indent();fprintf(outFile,"scale %f %f %f\n",s[0],s[1],s[2]);
	indent();fprintf(outFile,"scaleOrientation %f %f %f %f\n",so[0],so[1],so[2],so[3]);
	indent();fprintf(outFile,"translation %f %f %f\n",t[0],t[1],t[2]);

	indent();fprintf(outFile,"children\n");
	indent();fprintf(outFile,"[\n");
	indentLevel++;

	return(0);
}

void VRMLWriteTraverser::traverseTransformEnd(VRMLModel *model,VRMLNode *thisNode,int index)
{
	indentLevel--;
	indent();fprintf(outFile,"]\n");
	indentLevel--;
	indent();fprintf(outFile,"}\n");
}


void VRMLWriteTraverser::traverseGroup(VRMLModel *model,VRMLNode *thisNode,int index)
{
	indent();fprintf(outFile,"Group\n");
	indent();fprintf(outFile,"{\n");
	indentLevel++;
	indent();fprintf(outFile,"children\n");
	indent();fprintf(outFile,"[\n");
	indentLevel++;
}


void VRMLWriteTraverser::traverseGroupEnd(VRMLModel *model,VRMLNode *thisNode,int index)
{
	indentLevel--;
	indent();fprintf(outFile,"]\n");
	indentLevel--;
	indent();fprintf(outFile,"}\n");
}


void VRMLWriteTraverser::traverseMesh(VRMLModel *model,    // pointer to the traversed model
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

	indent();fprintf(outFile,"Shape\n");
	indent();fprintf(outFile,"{\n");
	indentLevel++;
	indent();fprintf(outFile,"appearance Appearance\n");
	indent();fprintf(outFile,"{\n");
	indentLevel++;
	indent();fprintf(outFile,"material Material\n");
	indent();fprintf(outFile,"{\n");
	indentLevel++;
	indent();fprintf(outFile,"ambientIntensity %f\n",mat->ambientIntensity);
	indent();fprintf(outFile,"diffuseColor %f %f %f\n",mat->diffuseColor[0],mat->diffuseColor[1],mat->diffuseColor[2]);
	indent();fprintf(outFile,"shininess %f\n",mat->shininess);
	indent();fprintf(outFile,"specularColor %f %f %f\n",mat->specularColor[0],mat->specularColor[1],mat->specularColor[2]);
	indent();fprintf(outFile,"emissiveColor %f %f %f\n",mat->emissiveColor[0],mat->emissiveColor[1],mat->emissiveColor[2]);
	indent();fprintf(outFile,"transparency %f\n",mat->transparency);
	indentLevel--;
	indent();fprintf(outFile,"}\n");
	if(mat->textureName[0])
	{
		indent();fprintf(outFile,"texture ImageTexture\n");
		indent();fprintf(outFile,"{\n");
		indentLevel++;
		indent();fprintf(outFile,"url\n");
		indent();fprintf(outFile,"[\n");
		indentLevel++;
		indent();fprintf(outFile,"\"%s\"\n",mat->textureName);
		indentLevel--;
		indent();fprintf(outFile,"]\n");
		indentLevel--;
		indent();fprintf(outFile,"}\n");
	}
	indentLevel--;
	indent();fprintf(outFile,"}\n");
	indent();fprintf(outFile,"geometry DEF %s IndexedFaceSet\n",thisNode->getName());
	indent();fprintf(outFile,"{\n");
	indentLevel++;

	if(creaseAngle != 0.0f)
	{
		indent();fprintf(outFile,"creaseAngle %f\n",creaseAngle);
	}
	if(!solid)
	{
		indent();fprintf(outFile,"solid FALSE\n");
	}
	if(!ccw)
	{
		indent();fprintf(outFile,"ccw FALSE\n");
	}
	if(!convex)
	{
		indent();fprintf(outFile,"convex FALSE\n");
	}

	indent();fprintf(outFile,"coord Coordinate\n");
	indent();fprintf(outFile,"{\n");
	indentLevel++;
	indent();fprintf(outFile,"point\n");
	indent();fprintf(outFile,"[\n");
	indentLevel++;
	for(i=0;i<coordLength;i+=3)
	{
		indent();fprintf(outFile,"%f %f %f\n",coord[i],coord[i+1],coord[i+2]);
	}
	indentLevel--;
	indent();fprintf(outFile,"]\n");
	indentLevel--;
	indent();fprintf(outFile,"}\n");
	
	if(!normalsCalculated)
	{
		indent();fprintf(outFile,"normal Normal\n");
		indent();fprintf(outFile,"{\n");
		indentLevel++;
		indent();fprintf(outFile,"vector\n");
		indent();fprintf(outFile,"[\n");
		indentLevel++;
		for(i=0;i<normLength;i+=3)
		{
			indent();fprintf(outFile,"%f %f %f\n",norm[i],norm[i+1],norm[i+2]);
		}
		indentLevel--;
		indent();fprintf(outFile,"]\n");
		indentLevel--;
		indent();fprintf(outFile,"}\n");
	}

	if(mat->textureName[0])
	{
		indent();fprintf(outFile,"texCoord TextureCoordinate\n");
		indent();fprintf(outFile,"{\n");
		indentLevel++;
		indent();fprintf(outFile,"point\n");
		indent();fprintf(outFile,"[\n");
		indentLevel++;
		for(i=0;i<tcoordLength;i+=2)
		{
			indent();fprintf(outFile,"%f %f\n",tcoord[i],tcoord[i+1]);
		}
		indentLevel--;
		indent();fprintf(outFile,"]\n");
		indentLevel--;
		indent();fprintf(outFile,"}\n");
	}
	
	indent();fprintf(outFile,"coordIndex\n");
	indent();fprintf(outFile,"[\n");
	indentLevel++;
	for(i=0;i<coordIndLength;i+=3)
	{
		indent();fprintf(outFile,"%d %d %d -1\n",coordInd[i],coordInd[i+1],coordInd[i+2]);
	}
	indentLevel--;
	indent();fprintf(outFile,"]\n");
	
	if(!normalsCalculated)
	{
		if(!NPV)
		{
			indent();fprintf(outFile,"normalPerVertex FALSE\n");
		}
		indent();fprintf(outFile,"normalIndex\n");
		indent();fprintf(outFile,"[\n");
		indentLevel++;
		if(NPV)
		{
			for(i=0;i<normIndLength;i+=3)
			{
				indent();fprintf(outFile,"%d %d %d -1\n",normInd[i],normInd[i+1],normInd[i+2]);
			}
		}
		else
		{
			for(i=0;i<normIndLength;i++)
			{
				indent();fprintf(outFile,"%d\n",normInd[i]);
			}
		}
		indentLevel--;
		indent();fprintf(outFile,"]\n");
	}

	// Changed by KStankovic
	if(mat->textureName[0] || mat->cubemapNames[0])
	//
	{
		indent();fprintf(outFile,"texCoordIndex\n");
		indent();fprintf(outFile,"[\n");
		indentLevel++;
		for(i=0;i<tcoordIndLength;i+=3)
		{
			indent();fprintf(outFile,"%d %d %d -1\n",tcoordInd[i],tcoordInd[i+1],tcoordInd[i+2]);
		}
		indentLevel--;
		indent();fprintf(outFile,"]\n");
	}
	
	if(cpv)
	{
		indent();fprintf(outFile,"color Color\n");
		indent();fprintf(outFile,"{\n");
		indentLevel++;
		indent();fprintf(outFile,"color\n");
		indent();fprintf(outFile,"[\n");
		indentLevel++;
		for(i=0;i<cpvLength;i+=3)
		{
			indent();fprintf(outFile,"%f %f %f\n",cpv[i],cpv[i+1],cpv[i+2]);
		}
		indentLevel--;
		indent();fprintf(outFile,"]\n");
		indentLevel--;
		indent();fprintf(outFile,"}\n");
		
		indent();fprintf(outFile,"colorIndex\n");
		indent();fprintf(outFile,"[\n");
		indentLevel++;
		if(CPV)
		{
			for(i=0;i<cpvIndLength;i+=3)
			{
				indent();fprintf(outFile,"%d %d %d -1\n",cpvInd[i],cpvInd[i+1],cpvInd[i+2]);
			}
			indentLevel--;
			indent();fprintf(outFile,"]\n");
			indent();fprintf(outFile,"colorPerVertex TRUE\n");
		}
		else
		{
			for(i=0;i<cpvIndLength;i++)
			{
				indent();fprintf(outFile,"%d\n",cpvInd[i]);
			}
			indentLevel--;
			indent();fprintf(outFile,"]\n");
			indent();fprintf(outFile,"colorPerVertex FALSE\n");
		}
	}

	indentLevel--;
	indent();fprintf(outFile,"}\n");
	indentLevel--;
	indent();fprintf(outFile,"}\n");
}


void VRMLWriteTraverser::traverseOther(VRMLModel *model,VRMLNode *thisNode,int index)
{
	if(thisNode->parserNode)
		thisNode->parserNode->write(outFile, 0);
}

}
