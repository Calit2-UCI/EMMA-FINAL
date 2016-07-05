// BaseFileIO.cpp: implementation of the BaseFileIO class.
//
//////////////////////////////////////////////////////////////////////

///GET CWD/////
#include <unistd.h>
#include <sys/param.h>
///////////////

#include "BaseFileIO.h"
#include "BaseAFMDeformer.h"

#include "VRMLModel.h"
#include "VRMLWriteTraverser.h"


#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <iostream>
#include <errno.h>

#include "Scene.h"
#include "Node.h"
#include "Field.h"
#include "SFString.h"
#include "MFString.h"
#include "SFFloat.h"
#include "SFColor.h"
#include "MFColor.h"
#include "MFFloat.h"
#include "MFVec3f.h"
#include "MFVec2f.h"
#include "SFInt32.h"
#include "MFInt32.h"
#include "MFString.h"
#include "SFVec3f.h"
#include "SFNode.h"
#include "SFBool.h"
#include "MFNode.h"
#include "NodeCoordinateInterpolator.h"
#include "NodeIndexedFaceSet.h"
#include "NodeTransform.h"
#include "NodeMaterial.h"
#include "NodeImageTexture.h"
#include "NodeGroup.h"
#include "FieldValue.h"
#include "ExposedField.h"
#include "SFRotation.h"
#include "MFRotation.h"

#include "myMatrix.h"
#include "FBAPs.h"

// Added by KStankovic
#include "CubemapWriteTraverser.h"
//

namespace VisageSDK
{

#define TT_READ_FILE 0
#define TT_READ_ANIM_DATA 0
#define TT_INIT 1
#define TT_UPDATE_GEOM 2

#define NB_SD 88

#define AFM_VERSION_NUMBER 1.6f
//added by TPejsa
#define AFM_VERSION_SUPPORTED(x) ( ( (x) == 1.5f ) || ( (x) == 1.6f ) )
#define AFM_VERSION_STR "1.6"
//

#ifdef DO_DEBUG_PRINT
extern void DebugPrint(char *s);
extern void DebugPrintNum(char *s,float num);
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BaseFileIO::BaseFileIO()
{
	nVisemes = 15;
	nExp = 7;
	nFaps = 68;
	AFMVersionNumber = 0.0f;

	// Added by KStankovic
	BAPModifiers = NULL;
	coord = NULL;
	coordLength = NULL;
	ExpressionModifiers = NULL;
	fap1Coord = NULL;
	fap1Orientation = NULL;
	fap1Position = NULL;
	fap1Scale = NULL;
	fap1Value0 = NULL;
	fap2Coord = NULL;
	fap2Orientation = NULL;
	fap2Position = NULL;
	fap2Scale = NULL;
	fap2Value0 = NULL;
	fapCoord = NULL;
	FapDisplacement = NULL;
	FAPModifiers = NULL;
	fapOrientation = NULL;
	FapOrientationDisp = NULL;
	fapPosition = NULL;
	FapPositionDisp = NULL;
	fapScale = NULL;
	FapScaleDisp = NULL;
	fapValue0 = NULL;
	FapWeight = NULL;
	orientation = NULL;
	origCoord = NULL;
	origOrientation = NULL;
	origPosition = NULL;
	origScale = NULL;
	position = NULL;
	root = NULL;
	scale = NULL;
	VisemeModifiers = NULL;
	//
}

BaseFileIO::~BaseFileIO()
{
	// Added by KStankovic
	if (coord != NULL) freeDoubleFloatPointer(coord, nSurfaces);
	if (coordLength != NULL) free(coordLength);

	if (fap1Coord != NULL)
	{
		for(int i = 0; i < nVisemes; i++)
		{
			free(fap1Coord[i]);
		}
		free(fap1Coord);
	}

	if (fap1Orientation != NULL)
	{
		for(int i = 0; i < nVisemes; i++)
		{
			free(fap1Orientation[i]);
		}
		free(fap1Orientation);
	}

	if (fap1Position != NULL)
	{
		for(int i = 0; i < nVisemes; i++)
		{
			free(fap1Position[i]);
		}
		free(fap1Position);
	}

	if (fap1Scale != NULL)
	{
		for(int i = 0; i < nVisemes; i++)
		{
			free(fap1Scale[i]);
		}
		free(fap1Scale);
	}

	if (fap1Value0 != NULL) free(fap1Value0);

	if (fap2Coord != NULL)
	{
		for(int i = 0; i < nExp; i++)
		{
			free(fap2Coord[i]);
		}
		free(fap2Coord);
	}

	if (fap2Orientation != NULL)
	{
		for(int i = 0; i < nExp; i++)
		{
			free(fap2Orientation[i]);
		}
		free(fap2Orientation);
	}

	if (fap2Position != NULL)
	{
		for(int i = 0; i < nExp; i++)
		{
			free(fap2Position[i]);
		}
		free(fap2Position);
	}

	if (fap2Scale != NULL)
	{
		for(int i = 0; i < nExp; i++)
		{
			free(fap2Scale[i]);
		}
		free(fap2Scale);
	}

	if (fap2Value0 != NULL) free(fap2Value0);

	if (fapCoord != NULL)
	{
		for(int i = 0; i < nFaps; i++)
		{
			free(fapCoord[i]);
		}
		free(fapCoord);
	}

	if (FapDisplacement != NULL) free(FapDisplacement);

	if (fapOrientation != NULL)
	{
		for(int i = 0; i < nFaps; i++)
		{
			free(fapOrientation[i]);
		}
		free(fapOrientation);
	}

	if (FapOrientationDisp != NULL) free(FapOrientationDisp);

	if (fapPosition != NULL)
	{
		for(int i = 0; i < nFaps; i++)
		{
			free(fapPosition[i]);
		}
		free(fapPosition);
	}

	if (FapPositionDisp != NULL) free(FapPositionDisp);

	if (fapScale != NULL)
	{
		for(int i = 0; i < nFaps; i++)
		{
			free(fapScale[i]);
		}
		free(fapScale);
	}

	if (FapScaleDisp != NULL) free(FapScaleDisp);
	if (fapValue0 != NULL) free(fapValue0);
	if (FapWeight != NULL) free(FapWeight);

	if (orientation != NULL) freeDoubleFloatPointer(orientation, nTransforms);
	if (origCoord != NULL) freeDoubleFloatPointer(origCoord, nSurfaces);
	if (origOrientation != NULL) freeDoubleFloatPointer(origOrientation, nTransforms);
	if (origPosition != NULL) freeDoubleFloatPointer(origPosition, nTransforms);
	if (origScale != NULL) freeDoubleFloatPointer(origScale, nTransforms);
	if (position != NULL) freeDoubleFloatPointer(position, nTransforms);
	if (scale != NULL) freeDoubleFloatPointer(scale, nTransforms);

	if (root != NULL)
	{
		Scene * scene = root->getScene();
		delete scene;
	}
	//
}

// 0 if file could not be read, 1 if succesful, -1 if wrong type
int BaseFileIO::read(char *fileName, VRMLModel *model)
{
	// basic validity checks
	if(!fileName)
		return(0);
	if(!model)
		return(0);

	// check file type (extension)
	char * ext = fileName + strlen(fileName) - 3;
	bool typeOK = false;

	if(!strcmp(ext,"wrl"))
		typeOK = true;
	if(!strcmp(ext,"WRL"))
		typeOK = true;
	if(!strcmp(ext,"afm"))
		typeOK = true;
	if(!strcmp(ext,"AFM"))
		typeOK = true;

	if(!typeOK)
		return(-1);

	FILE *f;
	std::cout << "trying to read " << fileName << std::endl;

	char path [MAXPATHLEN];
	getcwd(path, MAXPATHLEN);
	std::cout << "path to WD -> " << path << std::endl;

	if(!(f = fopen(fileName,"r")))
	{
		std::cout << "CANNOT READ MODEL FILE: "<< strerror(errno)<< std::endl;
		char msg[300];
		sprintf(msg,"Error 163\nCould not open file: >%s< (%s)\n",fileName,strerror(NULL) );
//		MessageBox(0,msg,"Error",MB_ICONERROR);
		fprintf(stderr,"%s\n",msg);
		return(0);
	}
	std::cout << "file properly opened" << std::endl;

	
	Scene *thisScene;
	
	//init reading 
	currentMaterial = 0;
	currentModel = model;

	thisScene = new Scene();
	thisScene->parseVRML(f);
    root = thisScene->getRoot();
	myMatrix toWC1;
	identityMat(toWC1);
	currentModel->rootNode = new VRMLNode(NT_ROOT,0,0,NULL,NULL,(VRMLNode*)NULL);
	currentModel->nCoordLists = currentModel->nTransfLists = currentModel->nMaterials = 0;
#ifdef DO_DEBUG_PRINT
DebugPrint("BaseFileIO::read M6\n");
#endif
	parseNode(root,TT_READ_FILE,-1,0,toWC1,currentModel->rootNode);
#ifdef DO_DEBUG_PRINT
DebugPrint("BaseFileIO::read M7\n");
#endif

	fclose(f);
	
	currentModel->finalizeFileReading(fileName);

	return(1);
}

//added by TPejsa
void BaseFileIO::outputAFMVersion(FILE *of)
{
	fprintf( of, "\n\nPROTO AFMVersion [\n" );
	fprintf( of, "\texposedField\tSFFloat\tversionNumber\t0\n" );
	fprintf( of, "]{Group{}}\n\n" );
	fprintf( of, "AFMVersion {\n" );
	fprintf( of, "\tversionNumber\t%s", AFM_VERSION_STR );
	fprintf( of, "\n}\n\n" );
}
//

void BaseFileIO::outputNeutralPose(FILE *of)
{
	NeutralPoseData *np = &currentModel->neutralPoseData;
	int i;
	bool needKomma;
	float aa[4];

	if(!np->NPRotData && !np->initialFacialPose)
		return;

	fprintf(of,"PROTO NeutralPose [\n");
	fprintf(of,"    exposedField     MFString     FAPs                []\n");
	fprintf(of,"    exposedField     MFInt32      FAPValues           []\n");
	fprintf(of,"    exposedField     MFString     NonNPTransforms     []\n");
	fprintf(of,"    exposedField     MFRotation   NPRotations         []\n");
	fprintf(of,"]{ Group{}}\n");

	fprintf(of,"NeutralPose {\n");
	if(np->initialFacialPose)
	{
		LLFAPs *llf = &np->initialFacialPose->llf;

		fprintf(of,"FAPs [");
		needKomma = false;
		for(i=0;i<NFAP;i++)
		{
			if(!llf->value[i])
				continue;
			if(needKomma)
				fprintf(of,",");
			else
				needKomma = true;
			fprintf(of,"\"%s\"", FAPNameString[i]);
		}
		fprintf(of," ]\n");

		fprintf(of,"FAPValues [");
		needKomma = false;
		for(i=0;i<NFAP;i++)
		{
			if(!llf->value[i])
				continue;
			if(needKomma)
				fprintf(of,",");
			else
				needKomma = true;
			fprintf(of,"%d", llf->value[i]);
		}
		fprintf(of," ]\n");
	}
	
	if(np->NPRotData)
	{
		int transfInd;

		fprintf(of,"NonNPTransforms [");
		needKomma = false;
		for(transfInd=0;transfInd<currentModel->nTransfLists;transfInd++)
		{
			if(!np->NPRotData[transfInd]) 
				continue;
			VRMLNode *nd = currentModel->transfNode[transfInd];

			if(needKomma)
				fprintf(of,",");
			else
				needKomma = true;
			fprintf(of,"\"%s\"", nd->getName());
		}
		fprintf(of," ]\n");

		fprintf(of,"NPRotations [");
		needKomma = false;
		for(transfInd=0;transfInd<currentModel->nTransfLists;transfInd++)
		{
			if(!np->NPRotData[transfInd]) 
				continue;
			VRMLNode *nd = currentModel->transfNode[transfInd];

			if(needKomma)
				fprintf(of,",\n");
			else
				needKomma = true;
			
			aa[3] = quat2aa(np->NPRotData[transfInd],aa);
			fprintf(of,"%f, %f, %f, %f", aa[0],aa[1],aa[2],aa[3]);
		}
		fprintf(of," ]\n");
	}

	fprintf(of,"}\n");
}

int BaseFileIO::write(char *fileName, VRMLModel *model)
{
	// basic validity checks
	if(!fileName)
		return(0);
	if(!model)
		return(0);

	// check file type (extension)
	char * ext = fileName + strlen(fileName) - 3;
	bool typeOK = false;

	if(!strcmp(ext,"wrl"))
		typeOK = true;
	if(!strcmp(ext,"WRL"))
		typeOK = true;
	if(!strcmp(ext,"afm"))
		typeOK = true;
	if(!strcmp(ext,"AFM"))
		typeOK = true;

	if(!typeOK)
		return(-1);

	FILE *f;
	//	strcpy(sceneFileName,fn);
	if(!(f = fopen(fileName,"w")))
	{
		return(0);
	}
	
	VRMLWriteTraverser *vt= new VRMLWriteTraverser(f);
	model->traverse(model->rootNode,vt);

	//Added by KStankovic
	CubemapWriteTraverser *ct = new CubemapWriteTraverser(f);
	model->traverse(model->rootNode, ct);
	delete ct;
	//

	fclose(f);
	
	return(1);
}

void BaseFileIO::parseIndexedFaceSet(Node *node)
{
	MFVec3f *coords;
	int nc;
	MFInt32 *ind;
	MFInt32 *nind;
	int nn;
	MFVec2f *tcoords;
	int ntc;
	MFInt32 *tind;
	MFColor *colors;
	int ncolors;
	MFInt32 *cind;
	int totalPolygons=0;
	int totalCorners=0;
	int ni;
	int i,j;

	SFBool *colorPerVertex;
	NodeIndexedFaceSet *_nodeIndexedFaceSet   = (NodeIndexedFaceSet *) node; 
    FieldValue *_value1; 
	_value1 = _nodeIndexedFaceSet->getField(1);//FieldValue(SFNode):'coord' field	
	Node *coord = ((SFNode *) _value1)->getValue();//'coord' Node
	
	if(!coord)
		return;

	//Get the actual coord
	coords = (MFVec3f *) coord->getField(0);
	nc = coords->getSize();
	currentModel->coordListSize[currentModel->nCoordLists] = nc;
	currentModel->coordList[currentModel->nCoordLists] = (float*)malloc(nc*sizeof(float));
	currentModel->origCoordList[currentModel->nCoordLists] = (float*)malloc(nc*sizeof(float));
	for(i=0;i<nc/3;i++)
		for(j=0;j<3;j++)
		{
			currentModel->coordList[currentModel->nCoordLists][i*3+j] = (coords->getValue(i))[j];
			currentModel->origCoordList[currentModel->nCoordLists][i*3+j] = (coords->getValue(i))[j];
		}

		// get the crease angle
		SFFloat * ca = (SFFloat *)_nodeIndexedFaceSet->getField(9);
		if(ca)
			currentModel->creaseAngle[currentModel->nCoordLists] = ca->getValue();
		else
			currentModel->creaseAngle[currentModel->nCoordLists] = 0.0f;
		
		// get the ccw value
		SFBool * ccwf = (SFBool *)_nodeIndexedFaceSet->getField(4);
		if(ccwf)
			currentModel->ccw[currentModel->nCoordLists] = ccwf->getValue();
		else
			currentModel->ccw[currentModel->nCoordLists] = 1;
		
		// get the solid value
		SFBool * solidf = (SFBool *)_nodeIndexedFaceSet->getField(12);
		if(solidf)
			currentModel->solid[currentModel->nCoordLists] = solidf->getValue();
		else
			currentModel->solid[currentModel->nCoordLists] = 1;
		
		// get the convex value
		SFBool * convexf = (SFBool *)_nodeIndexedFaceSet->getField(7);
		if(convexf)
			currentModel->convex[currentModel->nCoordLists] = convexf->getValue();
		else
			currentModel->convex[currentModel->nCoordLists] = 1;
		
		// get the npv value
		SFBool * npvf = (SFBool *)_nodeIndexedFaceSet->getField(11);
		if(npvf)
			currentModel->NPV[currentModel->nCoordLists] = npvf->getValue();
		else
			currentModel->NPV[currentModel->nCoordLists] = 1;
		
		
		// get the index; turn polygons into triangles
		ind = (MFInt32 *) _nodeIndexedFaceSet->getField(8);
		ni = ind->getSize();
		totalPolygons = 0;
		totalCorners = 0;
		for(i=0;i<ni;i++)
		{
			int np = 0;
			while(ind->getValue(i+np) != -1)
				np++;
			totalCorners+=3*(np-2);
			totalPolygons+=(np-2);
			i+=np;
		}
		currentModel->triListSize[currentModel->nCoordLists] = totalPolygons*3;
		currentModel->triList[currentModel->nCoordLists] = (int*)malloc(totalPolygons*3*sizeof(int));
		int nt=0;
		for(i=0;i<ni;i++)
		{
			int np = 0;
			while(ind->getValue(i+np) != -1)
				np++;
			for(j=1;j<np-1;j++)
			{
				currentModel->triList[currentModel->nCoordLists][nt++] = ind->getValue(i);
				currentModel->triList[currentModel->nCoordLists][nt++] = ind->getValue(i+j);
				currentModel->triList[currentModel->nCoordLists][nt++] = ind->getValue(i+j+1);
			}
			i+=np;
		}
	
		// generate or get the normals
		FieldValue *_value3; 
		_value3 = _nodeIndexedFaceSet->getField(2);//FieldValue(SFNode):'normals' field	
		Node *nrms = ((SFNode *) _value3)->getValue();//'normals' Node
		if(nrms)
		{
			currentModel->normalsCalculated[currentModel->nCoordLists] = 0;
			MFVec3f *ncoords = (MFVec3f *) nrms->getField(0);
			nind = (MFInt32 *) _nodeIndexedFaceSet->getField(10);
			nn = ncoords->getSize();
			currentModel->normList[currentModel->nCoordLists] = (float *)malloc(sizeof(float)*nn);
			currentModel->normListSize[currentModel->nCoordLists] = nn;
			for(i =0;i<nn/3;i++)
			{
				for(j=0;j<3;j++)
					currentModel->normList[currentModel->nCoordLists][i*3+j] = (ncoords->getValue(i))[j];
			}

			// set the normals index
			nt=0;
			for(i=0;i<nind->getSize();i++)
			{
				int np = 0;
				while(nind->getValue(i+np) != -1)
					np++;
				nt+=3*(np-2);
				i+=np;
			}
			currentModel->normIndListSize[currentModel->nCoordLists] = nt;
			currentModel->normIndList[currentModel->nCoordLists] = (int*)malloc(nt*sizeof(int));
			nt=0;
			for(i=0;i<nind->getSize();i++)
			{
				int np = 0;
				while(nind->getValue(i+np) != -1)
					np++;
				for(j=1;j<np-1;j++)
				{
					currentModel->normIndList[currentModel->nCoordLists][nt++] = nind->getValue(i);
					currentModel->normIndList[currentModel->nCoordLists][nt++] = nind->getValue(i+j);
					currentModel->normIndList[currentModel->nCoordLists][nt++] = nind->getValue(i+j+1);
				}
				i+=np;
			}
		}
		else
		{
			currentModel->normalsCalculated[currentModel->nCoordLists] = 1;
		}
		

		// Changed by KStankovic
		// get the texture coords, if any
		if(currentMaterial)
		{
			if(currentMaterial->textureName[0])
			{
				FieldValue *_value2; 
				_value2 = _nodeIndexedFaceSet->getField(3);//FieldValue(SFNode):'texCoord' field	
				Node *tcoord = ((SFNode *) _value2)->getValue();//'texCoord' Node
				tcoords = (MFVec2f *) tcoord->getField(0);
				ntc = tcoords->getSize();
				currentModel->tCoordListSize[currentModel->nCoordLists] =ntc;
				currentModel->tCoordList[currentModel->nCoordLists] = (float*)malloc(ntc*sizeof(float));
				for(i =0;i<ntc/2;i++)
				{
					for(j=0;j<2;j++)
						currentModel->tCoordList[currentModel->nCoordLists][i*2+j] = tcoords->getValue(i*2 + j);
				}
				
			}

			tind = (MFInt32 *) _nodeIndexedFaceSet->getField(13);

			// set the texture index
			nt=0;
			if(tind->getSize() > 0)
			{
				for(i=0;i<tind->getSize();i++)					
				{
					int np = 0;
					while(tind->getValue(i+np) != -1)
						np++;
					nt+=3*(np-2);
					i+=np;
				}
				currentModel->texIndListSize[currentModel->nCoordLists] = nt;
				currentModel->texIndList[currentModel->nCoordLists] = (int*)malloc(nt*sizeof(int));
				nt=0;
				for(i=0;i<tind->getSize();i++)
				{
					int np = 0;
					while(tind->getValue(i+np) != -1)
						np++;
					for(j=1;j<np-1;j++)
					{
						currentModel->texIndList[currentModel->nCoordLists][nt++] = tind->getValue(i);
						currentModel->texIndList[currentModel->nCoordLists][nt++] = tind->getValue(i+j);
						currentModel->texIndList[currentModel->nCoordLists][nt++] = tind->getValue(i+j+1);
					}
					i+=np;
				}
			}
			else // if texture coord index is not given, take coordinate index
			{
				currentModel->texIndListSize[currentModel->nCoordLists] = currentModel->triListSize[currentModel->nCoordLists];
				currentModel->texIndList[currentModel->nCoordLists] = (int*)malloc(currentModel->triListSize[currentModel->nCoordLists]*sizeof(int));
				for(i=0;i<currentModel->triListSize[currentModel->nCoordLists];i++)
				{
					currentModel->texIndList[currentModel->nCoordLists][i] = currentModel->triList[currentModel->nCoordLists][i];
				}
			}
		}
		//
		
		// get the vertex colors, if any
		colorPerVertex = (SFBool *) _nodeIndexedFaceSet->getField(6);
		if(colorPerVertex)
			currentModel->CPV[currentModel->nCoordLists] = colorPerVertex->getValue();
		else
			currentModel->CPV[currentModel->nCoordLists] = 1;

		_value1 = _nodeIndexedFaceSet->getField(0);//FieldValue(SFNode):'color' field
		Node *color = ((SFNode *) _value1)->getValue();//'color' Node
		if(!color)
		{
			currentModel->CPV[currentModel->nCoordLists] = 0;
			currentModel->cpvList[currentModel->nCoordLists] = 0;
		}
		else
		{
			colors = (MFColor *) color->getField(0);
			cind = (MFInt32 *) _nodeIndexedFaceSet->getField(5);
			ncolors = colors->getSize();
			currentModel->cpvListSize[currentModel->nCoordLists] =ncolors;
			currentModel->cpvList[currentModel->nCoordLists] = (float*)malloc(ncolors*sizeof(float));
			for(i =0;i<ncolors/3;i++)
			{
				for(j=0;j<3;j++)
					currentModel->cpvList[currentModel->nCoordLists][i*3+j] = (colors->getValue(i))[j];
			}
			// set the color index
			int nci = cind->getSize();
			if(currentModel->CPV[currentModel->nCoordLists])
			{
				nt=0;
				for(i=0;i<nci;i++)
				{
					int np = 0;
					while(cind->getValue(i+np) != -1)
						np++;
					nt+=3*(np-2);
					i+=np;
				}
				currentModel->cpvIndList[currentModel->nCoordLists] = (int*)malloc(nt*sizeof(int));
				currentModel->cpvIndListSize[currentModel->nCoordLists] = nt;
				nt=0;
				for(i=0;i<nci;i++)
				{
					int np = 0;
					while(nind->getValue(i+np) != -1)
						np++;
					for(j=1;j<np-1;j++)
					{
						currentModel->cpvIndList[currentModel->nCoordLists][nt++] = cind->getValue(i);
						currentModel->cpvIndList[currentModel->nCoordLists][nt++] = cind->getValue(i+j);
						currentModel->cpvIndList[currentModel->nCoordLists][nt++] = cind->getValue(i+j+1);
					}
					i+=np;
				}
			}
			else
			{
				currentModel->cpvIndList[currentModel->nCoordLists] = (int*)malloc(nci*sizeof(int));
				currentModel->cpvIndListSize[currentModel->nCoordLists] = nci;
				for(i=0;i<nci;i++)
					currentModel->cpvIndList[currentModel->nCoordLists][i] = cind->getValue(i);
			}
		}
}

void BaseFileIO::parseTransform(Node *node)
{
	int nc = 20;
	float *out = (float*) malloc(nc*sizeof(float));
    FieldValue *fieldValue; 
    Field *field; 
	char fieldName[200];
	SFRotation *rot;
	SFVec3f *coords;
	int j;
	int numFields = node->getProto()->getNumFields();
 
	

	// process all fields and find rotation, transation, scale, scale orientation and center
	for(int i=0; i<numFields;i++)
	{
		fieldValue = node->getField(i);
		field = node->getProto()->getField(i);
		strcpy(fieldName,field->getName());
		
		if(!strcmp(fieldName,"translation"))
		{
			//Get the translation
			coords = (SFVec3f *) fieldValue;
			for(j=0;j<3;j++)
				out[j] = (coords->getValue())[j];
		}
		
		if(!strcmp(fieldName,"rotation"))
		{
			//Get the rotation
			rot = (SFRotation *) fieldValue;
			out[3] = rot->getQuat().x;
			out[4] = rot->getQuat().y;
			out[5] = rot->getQuat().z;
			out[6] = rot->getQuat().w;
		}
		
		if(!strcmp(fieldName,"scale"))
		{
			//Get the scale
			coords = (SFVec3f *) fieldValue;
			for(j=0;j<3;j++)
				out[j+7] = (coords->getValue())[j];
		}
		
		if(!strcmp(fieldName,"scaleOrientation"))
		{
			//Get the scale orientation
			rot = (SFRotation *) fieldValue;
			out[10] = rot->getQuat().x;
			out[11] = rot->getQuat().y;
			out[12] = rot->getQuat().z;
			out[13] = rot->getQuat().w;
		}
		
		if(!strcmp(fieldName,"center"))
		{
			//Get the center
			coords = (SFVec3f *) fieldValue;
			for(j=0;j<3;j++)
				out[j+14] = (coords->getValue())[j];
		}
		
	}

	out[17] = 1.0f; // temporary vector used for adjusting transforms when removing NUS
	out[18] = 1.0f;
	out[19] = 1.0f;

	currentModel->transfList[currentModel->nTransfLists] = out;
}


void BaseFileIO::parseMaterial(Node *_currentRootNode,VRMLMaterial *m)
{
	NodeMaterial *_nodeMaterial   = (NodeMaterial *) _currentRootNode; 
	SFFloat *flt;
	SFColor *clr;
	
	flt = (SFFloat *) _nodeMaterial->getField(0);
	m->ambientIntensity = flt->getValue();
	
	clr = (SFColor *) _nodeMaterial->getField(1);
	m->diffuseColor[0] = clr->getValue(0);
	m->diffuseColor[1] = clr->getValue(1);
	m->diffuseColor[2] = clr->getValue(2);
	
	clr = (SFColor *) _nodeMaterial->getField(2);
	m->emissiveColor[0] = clr->getValue(0);
	m->emissiveColor[1] = clr->getValue(1);
	m->emissiveColor[2] = clr->getValue(2);
	
	flt = (SFFloat *) _nodeMaterial->getField(3);
	m->shininess = flt->getValue();
	
	clr = (SFColor *) _nodeMaterial->getField(4);
	m->specularColor[0] = clr->getValue(0);
	m->specularColor[1] = clr->getValue(1);
	m->specularColor[2] = clr->getValue(2);
	
	flt = (SFFloat *) _nodeMaterial->getField(5);
	m->transparency = flt->getValue();
	strcpy(m->textureName,"");
	
	currentMaterial = m;
}

void BaseFileIO::parseNode(Node *_currentRootNode, int traverseType, int TI, int TL, myMatrix toWC1,VRMLNode * parentNode)
{
    NodeList *_allRootNodes;
    FieldValue *_value; 
	char tmp[400];
	char tmp1[400];
	VRMLNode *myNode = 0;
	char defName[400];
	
	myMatrix localToWC;
	copyMat(localToWC,toWC1);
	
	strcpy(tmp,_currentRootNode->getProto()->getName());
	int l = _currentRootNode->getName().length();
	if(l!=400)
		strncpy(defName,_currentRootNode->getName(),400);
	else
		strcpy(defName,_currentRootNode->getName());
	if(!strcmp(tmp,"Shape") || !strcmp(tmp,"Transform")) // these nodes must get a name
	{
		sprintf(tmp1,"Node%s",tmp);
		if(strlen(defName) == 0 || !strcmp(tmp1,defName)) // detect empty and non-unique names
			sprintf(defName,"%s%d",tmp,(int)_currentRootNode);
	}
	
	
#ifdef DO_DEBUG_PRINT
DebugPrint("BaseFileIO::parseNode M1\n");
DebugPrint(tmp);
#endif
	if(!strcmp(tmp,"Transform"))
	{
		switch(traverseType)
		{
		case TT_READ_FILE:
			parseTransform(_currentRootNode);
			updateToWCMatrix(localToWC);
			copyMat(currentModel->toWC[currentModel->nTransfLists],localToWC);
			currentModel->transfLevel[currentModel->nTransfLists] = TL;
			TI = currentModel->nTransfLists;
			myNode = new VRMLNode(NT_TRANSFORM,currentModel->nTransfLists,0,NULL,defName,parentNode);
			currentModel->nTransfLists++;
			break;
		};
		
		
		// process children
		_value = _currentRootNode->getField(1);
		_allRootNodes = ( (MFNode *)_value )->getValue();
		for (int k=0; k<_allRootNodes->size(); k++)
		{
			Node *n = _allRootNodes->get(k);
			parseNode(n, traverseType, TI, TL+1, localToWC,myNode);
		}
	}
	else if(!strcmp(tmp,"IndexedFaceSet"))
	{
		switch(traverseType)
		{
		case TT_READ_FILE:
			parentNode->ind = currentModel->nCoordLists;
			if(strlen(defName)!=0)
				parentNode->setName(defName);
			parseIndexedFaceSet(_currentRootNode);
			currentModel->transfInd[currentModel->nCoordLists] = TI;
			currentModel->nCoordLists++;
			break;
		};
	}
	else if(!strcmp(tmp,"Material"))
	{
		switch(traverseType)
		{
		case TT_READ_FILE:
			currentModel->materials[currentModel->nMaterials] = new VRMLMaterial();
			parseMaterial(_currentRootNode,currentModel->materials[currentModel->nMaterials]);
			parentNode->matInd = currentModel->nMaterials;
			currentModel->nMaterials++;
			break;
		};
	}
	else if(!strcmp(tmp,"ImageTexture"))
	{
		NodeImageTexture *_imageTexture   = (NodeImageTexture *) _currentRootNode; 
		switch(traverseType)
		{
		case TT_READ_FILE:
			MFString *url;
			url = (MFString *) _imageTexture->getField(0);
			if(currentMaterial)
				strcpy(currentMaterial->textureName,url->getValue(0));
			break;
		};
	}
	else if(!strcmp(tmp,"Group"))
	{
		switch(traverseType)
		{
		case TT_READ_FILE:
			myNode = new VRMLNode(NT_GROUP,0,0,NULL,defName,parentNode);
			break;
		};
		for(int i=0; i<_currentRootNode->getProto()->getNumFields();i++)
		{
			_value = _currentRootNode->getField(i);
			strcpy(tmp,_value->getTypeName());
			
			if(!strcmp(tmp,"MFNode"))
			{
				// process children
				_allRootNodes = ( (MFNode *)_value )->getValue();
				for (int k=0; k<_allRootNodes->size(); k++)
				{
					Node *n = _allRootNodes->get(k);
					parseNode(n, traverseType, TI, TL+1, localToWC,myNode);
				}
			}
		}
	}
	else if(!strcmp(tmp,"Shape"))
	{
		switch(traverseType)
		{
		case TT_READ_FILE:
			myNode = new VRMLNode(NT_SHAPE,0,0,NULL,defName,parentNode);
			break;
		};
		for(int i=0; i<_currentRootNode->getProto()->getNumFields();i++)
		{
			_value = _currentRootNode->getField(i);
			strcpy(tmp,_value->getTypeName());
			
			if(!strcmp(tmp,"SFNode"))
			{
				Node *n = ((SFNode *)_value )->getValue();
				if(n) parseNode(n, traverseType, TI, TL, localToWC,myNode);
			}
		}
	}
	else if(!strcmp(tmp,"Appearance"))
	{
		for(int i=0; i<_currentRootNode->getProto()->getNumFields();i++)
		{
			_value = _currentRootNode->getField(i);
			strcpy(tmp,_value->getTypeName());
			
			if(!strcmp(tmp,"SFNode"))
			{
				Node *n = ((SFNode *)_value )->getValue();
				if(n) parseNode(n, traverseType, TI, TL, localToWC,parentNode);
			}
		}
	}
	else
	{
		int skip = 0;
		if(!strcmp(tmp,"AFMVersion"))
		{
				skip = 1;
		}
		if(!strcmp(tmp,"AnimationParameter"))
		{
				skip = 1;
		}
		if(!strcmp(tmp,"IFSBoneWeights"))
		{
				skip = 1;
		}
		if(!strcmp(tmp,"NeutralPose"))
		{
				skip = 1;
		}
		// Added by KStankovic
		if (!strcmp(tmp, "Cubemap"))
		{
				skip = 1;
		}
		//
		if(!skip)
		{
			switch(traverseType)
			{
			case TT_READ_FILE:
				myNode = new VRMLNode(NT_OTHER,0,0,_currentRootNode,defName,parentNode);
				break;
			};
			for(int i=0; i<_currentRootNode->getProto()->getNumFields();i++)
			{
				_value = _currentRootNode->getField(i);
				strcpy(tmp,_value->getTypeName());
				
				if(!strcmp(tmp,"SFNode"))
				{
					Node *n = ((SFNode *)_value )->getValue();
					if(n) parseNode(n, traverseType, TI, TL, localToWC,myNode);
				}
			}
		}
	}
	if(parentNode && myNode)
		parentNode->addChild(myNode);
}


void BaseFileIO::updateToWCMatrix(myMatrix toWC)
{
	myMatrix t;
	identityMat(t);
	
	currentModel->getTransfMatrix(t,currentModel->transfList[currentModel->nTransfLists]);
	multMat(toWC,t,toWC);
}


// convert rotation from quaternion to axis and angle
void BaseFileIO::setRotation(float *r)
{
	if(!r)
		return;
	float axis[3];
	float angle = quat2aa(r,axis);
	r[0] = axis[0];
	r[1] = axis[1];
	r[2] = axis[2];
	r[3] = angle;
}



// write the internal data into the modifier structures
void BaseFileIO::setModifiers()
{
	int i,j;

	FAPModifiers = new VRMLModelModifier[NFAP];
	ExpressionModifiers = new VRMLModelModifier[7];
	VisemeModifiers = new VRMLModelModifier[15];
	BAPModifiers = new VRMLModelModifier[NUM_BAPS];

	for(i=2;i<nFaps;i++)
	{	
		FAPModifiers[i].MTData = 0;
		FAPModifiers[i].TTranData = 0;
		FAPModifiers[i].TRotData = 0;
		FAPModifiers[i].TScalData = 0;

		if(fapCoord[i])
		{
			FAPModifiers[i].MTData = (float**)malloc(currentModel->nCoordLists * sizeof(float*));
			for(j=0;j<currentModel->nCoordLists ; j++)
			{
				if(!fapCoord[i][j])
					continue;
				FAPModifiers[i].MTData[j] = 0;
				int k;	//CHANGED BY I.K. declared iterator outside of loop
				for(k=0;k<currentModel->coordListSize[j];k++)
				{
					if(fapCoord[i][j][k] != 0.0f)
						break;
				}
				if(k<currentModel->coordListSize[j])
					FAPModifiers[i].MTData[j] = fapCoord[i][j];
			}
		}
		if(fapPosition[i])
		{
			FAPModifiers[i].TTranData = (float**)malloc(currentModel->nTransfLists * sizeof(float*));
			for(j=0;j<currentModel->nTransfLists ; j++)
			{
				FAPModifiers[i].TTranData[j] = 0;
				if(fapPosition[i][j])
					if(fapPosition[i][j][0] != 0.0f || fapPosition[i][j][1] != 0.0f || fapPosition[i][j][2] != 0.0f)
						FAPModifiers[i].TTranData[j] = fapPosition[i][j];
			}
		}
		if(fapOrientation[i])
		{
			FAPModifiers[i].TRotData = (float**)malloc(currentModel->nTransfLists * sizeof(float*));
			for(j=0;j<currentModel->nTransfLists ; j++)
			{
				FAPModifiers[i].TRotData[j] = 0;
				if(fapOrientation[i][j])
					if(fapOrientation[i][j][0] != 0.0f || fapOrientation[i][j][1] != 0.0f || fapOrientation[i][j][2] != 0.0f || fapOrientation[i][j][3] != 0.0f)
					{
						FAPModifiers[i].TRotData[j] = fapOrientation[i][j];
						setRotation(FAPModifiers[i].TRotData[j]);
					}
			}
		}
		if(fapScale[i])
		{
			FAPModifiers[i].TScalData = (float**)malloc(currentModel->nTransfLists * sizeof(float*));
			for(j=0;j<currentModel->nTransfLists ; j++)
			{
				FAPModifiers[i].TScalData[j] =  0;
				if(fapScale[i][j])
					if(fapScale[i][j][0] != 0.0f || fapScale[i][j][1] != 0.0f || fapScale[i][j][2] != 0.0f)
						FAPModifiers[i].TScalData[j] = fapScale[i][j];
			}
		}
		FAPModifiers[i].refValue = (float)fapValue0[i];
	}
	for(i=0;i<nExp;i++)
	{
		ExpressionModifiers[i].MTData = 0;
		ExpressionModifiers[i].TTranData = 0;
		ExpressionModifiers[i].TRotData = 0;
		ExpressionModifiers[i].TScalData = 0;

		if(fap2Coord[i])
		{
			ExpressionModifiers[i].MTData = (float**)malloc(currentModel->nCoordLists * sizeof(float*));
			for(j=0;j<currentModel->nCoordLists ; j++)
				ExpressionModifiers[i].MTData[j] = fap2Coord[i][j];
		}
		if(fap2Position[i])
		{
			ExpressionModifiers[i].TTranData = (float**)malloc(currentModel->nTransfLists * sizeof(float*));
			for(j=0;j<currentModel->nTransfLists ; j++)
				ExpressionModifiers[i].TTranData[j] = fap2Position[i][j];
		}
		if(fap2Orientation[i])
		{
			ExpressionModifiers[i].TRotData = (float**)malloc(currentModel->nTransfLists * sizeof(float*));
			for(j=0;j<currentModel->nTransfLists ; j++)
			{
				ExpressionModifiers[i].TRotData[j] = fap2Orientation[i][j];
				setRotation(ExpressionModifiers[i].TRotData[j]);
			}
		}
		if(fap2Scale[i])
		{
			ExpressionModifiers[i].TScalData = (float**)malloc(currentModel->nTransfLists * sizeof(float*));
			for(j=0;j<currentModel->nTransfLists ; j++)
				ExpressionModifiers[i].TScalData[j] = fap2Scale[i][j];
		}
		ExpressionModifiers[i].refValue = (float)fap2Value0[i];
	}
	for(i=0;i<nVisemes;i++)
	{
		VisemeModifiers[i].MTData = 0;
		VisemeModifiers[i].TTranData = 0;
		VisemeModifiers[i].TRotData = 0;
		VisemeModifiers[i].TScalData = 0;

		if(fap1Coord[i])
		{
			VisemeModifiers[i].MTData = (float**)malloc(currentModel->nCoordLists * sizeof(float*));
			for(j=0;j<currentModel->nCoordLists ; j++)
				VisemeModifiers[i].MTData[j] = fap1Coord[i][j];
		}
		if(fap1Position[i])
		{
			VisemeModifiers[i].TTranData = (float**)malloc(currentModel->nTransfLists * sizeof(float*));
			for(j=0;j<currentModel->nTransfLists ; j++)
				VisemeModifiers[i].TTranData[j] = fap1Position[i][j];
		}
		if(fap1Orientation[i])
		{
			VisemeModifiers[i].TRotData = (float**)malloc(currentModel->nTransfLists * sizeof(float*));
			for(j=0;j<currentModel->nTransfLists ; j++)
			{
				VisemeModifiers[i].TRotData[j] = fap1Orientation[i][j];
				setRotation(VisemeModifiers[i].TRotData[j]);
			}
		}
		if(fap1Scale[i])
		{
			VisemeModifiers[i].TScalData = (float**)malloc(currentModel->nTransfLists * sizeof(float*));
			for(j=0;j<currentModel->nTransfLists ; j++)
				VisemeModifiers[i].TScalData[j] = fap1Scale[i][j];
		}
		VisemeModifiers[i].refValue = (float)fap1Value0[i];
	}

/*
	for(i=0;i<NBAP;i++)
	{
		BAPModifiers[i].MTData = 0;
		BAPModifiers[i].TTranData = 0;
		BAPModifiers[i].TRotData = 0;
		BAPModifiers[i].TScalData = 0;
		
		body->setBAPModifier(i,&BAPModifiers[i],currentModel);
		
	}
*/
}


// allocate the modifier structures and set to zero values
void BaseFileIO::initModifiers()
{
	int i;

	FAPModifiers = new VRMLModelModifier[NFAP];
	ExpressionModifiers = new VRMLModelModifier[7];
	VisemeModifiers = new VRMLModelModifier[15];
	BAPModifiers = new VRMLModelModifier[NUM_BAPS];

	for(i=2;i<nFaps;i++)
	{	
		FAPModifiers[i].MTData = 0;
		FAPModifiers[i].TTranData = 0;
		FAPModifiers[i].TRotData = 0;
		FAPModifiers[i].TScalData = 0;
		FAPModifiers[i].refValue = -1;
	}
	for(i=0;i<nExp;i++)
	{
		ExpressionModifiers[i].MTData = 0;
		ExpressionModifiers[i].TTranData = 0;
		ExpressionModifiers[i].TRotData = 0;
		ExpressionModifiers[i].TScalData = 0;
		ExpressionModifiers[i].refValue = -1;
	}
	for(i=0;i<nVisemes;i++)
	{
		VisemeModifiers[i].MTData = 0;
		VisemeModifiers[i].TTranData = 0;
		VisemeModifiers[i].TRotData = 0;
		VisemeModifiers[i].TScalData = 0;
		VisemeModifiers[i].refValue = -1;
	}
	for(i=0;i<NBAP;i++)
	{
		BAPModifiers[i].MTData = 0;
		BAPModifiers[i].TTranData = 0;
		BAPModifiers[i].TRotData = 0;
		BAPModifiers[i].TScalData = 0;
		BAPModifiers[i].refValue = -1;
	}
	
}


// compute bone weight such that it decreases from 1.0 at the center of rotation to 0.0 at the distance from center given by range. v is the vertex, center is the center of rotation, both expressed in the same coordinate system
float BaseFileIO::computeBoneWeight(float *v,float *center, float range)
{
	float r = v[1] - center[1];
	if(r > 0.5)
		r = 1.0f;
	else
		r = 0.0f;
	/*
	float r = vect3Dist(v,center);
	if(r > range)
		r = 0.0f;
	else
	{
		//		r = (1.0f - r/range)  / 2.0f ;
		if(v[1] > center[1])
			r = 0.0f;
		else
			r = 1.0f;
	}
	*/

	return(r);
}



int BaseFileIO::read(char *fileName, VRMLModel *model, AFMDeformer **deformers, int *nDeformers)
{
	// basic validity checks
	if(!fileName)
		return(0);
	if(!model)
		return(0);

//	body = 0;

	// check file type (extension)
	char * ext = fileName + strlen(fileName) - 3;
	bool typeOK = false;

	if(!strcmp(ext,"afm"))
		typeOK = true;
	if(!strcmp(ext,"AFM"))
		typeOK = true;
	//added by TPejsa
	if(!strcmp(ext,"wrl"))
		typeOK = true;
	if(!strcmp(ext,"WRL"))
		typeOK = true;
	//

	if(!typeOK)
		return(-1);

	read(fileName,model);
	currentModel = model;

	initAnimData();

	deformers[0] = new BaseAFMDeformer(model,FAPModifiers, ExpressionModifiers, VisemeModifiers, BAPModifiers);
	(*nDeformers) = 1;


	return(1);
}

void BaseFileIO::outputBoneData(VRMLMeshBones *bd, char *meshName, int surfaceIndex, FILE *of)
{
	int i,j;
	float w;
	bool needKomma;

	if(!bd->nBones)
		return;

	fprintf(of,"IFSBoneWeights {\n");
	fprintf(of,"meshName \"%s\"\n", meshName);

	fprintf(of,"bones [");
	needKomma = false;
	for(i=0;i<bd->nBones;i++)
	{
		if(needKomma)
			fprintf(of,",");
		else
			needKomma = true;
		fprintf(of,"\"%s\"", currentModel->transfNode[bd->boneIndex[i]]->getName());
	}
	fprintf(of," ]\n");
	
	fprintf(of,"vertices [");
	needKomma = false;
	for(i=0;i<bd->nBones;i++)
	{
		for(j=0;j<currentModel->coordListSize[surfaceIndex]/3;j++)
		{
			w = bd->vertexWeight[i][j];
			if(w==0.0f)
				continue;
			if(needKomma)
				//replaced by TPejsa
				//fprintf(of,",\n");
				fprintf(of,",");
				//
			else
				needKomma = true;
			fprintf(of,"%d", j);
		}
		if(needKomma)
			fprintf(of,",\n");
		else
			needKomma = true;
		fprintf(of,"-1");
	}
	fprintf(of," ]\n");
	
	fprintf(of,"weights [");
	needKomma = false;
	for(i=0;i<bd->nBones;i++)
	{
		for(j=0;j<currentModel->coordListSize[surfaceIndex]/3;j++)
		{
			w = bd->vertexWeight[i][j];
			if(w==0.0f)
				continue;
			if(needKomma)
				//replaced by TPejsa
				//fprintf(of,",\n");
				fprintf(of,",");
				//
			else
				needKomma = true;
			fprintf(of,"%f", w);
		}
	}
	fprintf(of," ]\n");
	
	fprintf(of,"}\n");
}


void BaseFileIO::outputBonesData(FILE *of)
{
	fprintf(of,"PROTO IFSBoneWeights [\n");
	fprintf(of,"    exposedField     SFString     meshName            \"\"\n");
	fprintf(of,"    exposedField     MFString     bones               []\n");
	fprintf(of,"    exposedField     MFInt32      vertices            []\n");
	fprintf(of,"    exposedField     MFFloat      weights             []\n");
	fprintf(of,"]{Group{}}\n");

	for(int i = 0;i < currentModel->nCoordLists;i++)
	{
		outputBoneData(&currentModel->boneData[i],currentModel->meshNode[i]->getName(),i,of);
	}
}


int BaseFileIO::write(char *fileName, VRMLModel *model, AFMDeformer **deformers, int nDeformers)
{
	// basic validity checks
	if(!fileName)
		return(0);
	if(!model)
		return(0);

	// check file type (extension)
	char * ext = fileName + strlen(fileName) - 3;
	bool typeOK = false;

	if(!strcmp(ext,"afm"))
		typeOK = true;
	if(!strcmp(ext,"AFM"))
		typeOK = true;

	if(!typeOK)
		return(-1);

	FILE *f;
	//	strcpy(sceneFileName,fn);
	if(!(f = fopen(fileName,"w")))
	{
		return(0);
	}

	//added by TPejsa
	outputAFMVersion(f);
	//
	
	currentModel = model;

	VRMLWriteTraverser *vt= new VRMLWriteTraverser(f);
	currentModel->traverse(model->rootNode,vt);
	delete vt;
	
	for(int i =0;i < nDeformers;i++)
		deformers[i]->write(f,"afm");

	outputBonesData(f);

	//Added by KStankovic
	CubemapWriteTraverser *ct = new CubemapWriteTraverser(f);
	model->traverse(model->rootNode, ct);
	delete ct;
	//

	fclose(f);
	return(1);
}

void BaseFileIO::allocAnimData()
{
	nSurfaces = currentModel->nCoordLists;
	nTransforms = currentModel->nTransfLists;
	nVisemes = 15;
	nExp = 7;
	nFaps = 68;
	nAllExp = nVisemes + nExp + nFaps;
	int len = 10;

	initDoubleFloatPointer(&coord,nSurfaces);
	initDoubleFloatPointer(&origCoord,nSurfaces);
	coordLength = (int*)malloc(nSurfaces*sizeof(int));	
	initTripleFloatPointer(&fap1Coord,nVisemes,nSurfaces);
	initTripleFloatPointer(&fap2Coord,nExp,nSurfaces);
	initTripleFloatPointer(&fapCoord,nFaps,nSurfaces);
	
	fapValue0 = (int*)malloc(nFaps*sizeof(int));
	fap1Value0 = (int*)malloc(nVisemes*sizeof(int));
	fap2Value0 = (int*)malloc(nExp*sizeof(int));
	
	initDoubleFloatPointer(&origPosition,nTransforms,3);
	initDoubleFloatPointer(&position,nTransforms,3);
	initTripleFloatPointer(&fap1Position,nVisemes,nTransforms,3);
	initTripleFloatPointer(&fap2Position,nExp,nTransforms,3);
	initTripleFloatPointer(&fapPosition,nFaps,nTransforms,3);
	
	initDoubleFloatPointer(&origOrientation,nTransforms,4);
	initDoubleFloatPointer(&orientation,nTransforms,4);
	initTripleFloatPointer(&fap1Orientation,nVisemes,nTransforms,4);
	initTripleFloatPointer(&fap2Orientation,nExp,nTransforms,4);
	initTripleFloatPointer(&fapOrientation,nFaps,nTransforms,4);
	
	initDoubleFloatPointer(&origScale,nTransforms,3);
	initDoubleFloatPointer(&scale,nTransforms,3);
	initTripleFloatPointer(&fap1Scale,nVisemes,nTransforms,3);
	initTripleFloatPointer(&fap2Scale,nExp,nTransforms,3);
	initTripleFloatPointer(&fapScale,nFaps,nTransforms,3);	

	FapDisplacement = (float***)malloc(150*sizeof(float**));
	FapPositionDisp = (float***)malloc(150*sizeof(float**));
	FapOrientationDisp = (float***)malloc(150*sizeof(float**));
	FapScaleDisp = (float***)malloc(150*sizeof(float**));
	FapWeight = (float*)malloc(150*sizeof(float));
	nCurrentFaps = 0;
}

void BaseFileIO::initAnimData()
{	
	initModifiers();
	currentModel->updateToWCMatrices();

	traverse(root, TT_READ_ANIM_DATA,"");

	setNeutralPoseAndAdjustModifiers();
}

// traverser functions
int BaseFileIO::traverseTransform(VRMLModel *model,    // pointer to the traversed model
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
	int i = 0;

	switch(traversalType)
	{
	case TT_INIT:
		for(i = 0; i < 3; i++)
			position[index][i] = origPosition[index][i] = t[i];	
		for(i = 0; i < 4; i++)
			orientation[index][i] = origOrientation[index][i] = rq[i];	
		for(i = 0; i < 3; i++)
			scale[index][i] = origScale[index][i] = s[i];
		return(0);
		break;
	case TT_UPDATE_GEOM:
		for(i = 0; i < 3; i++)
			t[i] = position[index][i];	
		for(i = 0; i < 4; i++)
			rq[i] = orientation[index][i];	
		for(i = 0; i < 3; i++)
			s[i] = scale[index][i];
		return(1);
		break;
		break;
	default:
		break;
	}
	return(0);
}


void BaseFileIO::traverseMesh(VRMLModel *model,    // pointer to the traversed model
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
							  VRMLMeshBones *bones)  // pointer to the list of bones for this mesh          
{
	int i;

	switch(traversalType)
	{
	case TT_INIT:
		this->coord[index] = (float*)malloc(coordLength * sizeof(float));
		this->origCoord[index] = (float*)malloc(coordLength * sizeof(float));
		this->coordLength[index] = coordLength;
		for(i = 0; i < coordLength; i++)
			this->origCoord[index][i] = this->coord[index][i] = coord[i];	
		break;
	case TT_UPDATE_GEOM:
		for(i = 0; i < coordLength; i++)
			coord[i] = this->coord[index][i];	
		break;
	default:
		break;
	}
}





/*

	int i,l,k,nBones;
	int childMesh=-1,parentMesh=-1;
	float range;
	myMatrix parentToWCMat; // parent transform local-to-world-coordinates transformation matrix 	
	myMatrix childToWCMat; // child transform world-to-local-coordinates transformation matrix 	
	myMatrix parentFromWCMat; // parent transform local-to-world-coordinates transformation matrix 	
	myMatrix childFromWCMat; // child transform world-to-local-coordinates transformation matrix 
	float * centerOfRotation; // center of rotation of the child transform

	copyMat(parentToWCMat,currentModel->toWC[parentTransform]); 
	copyMat(childToWCMat,currentModel->toWC[childTransform]); 
	copyMat(parentFromWCMat,currentModel->fromWC[parentTransform]); 
	copyMat(childFromWCMat,currentModel->fromWC[childTransform]); 
	centerOfRotation = &currentModel->transfList[childTransform][14];

	for(i =0 ; i< currentModel->nCoordLists;i++)
	{
		if(currentModel->transfInd[i] == childTransform)
			childMesh = i;
		if(currentModel->transfInd[i] == parentTransform)
			parentMesh = i;
	}

	if(parentMesh == -1)
		parentMesh=currentModel->findTransformMesh(parentTransform);
	if(childMesh == -1)
		childMesh=currentModel->findTransformMesh(childTransform);

	if(parentMesh == -1)
		return;
	if(childMesh == -1)
		return;


	// set parent mesh bones
	VRMLMeshBones *bones = &currentModel->boneData[parentMesh];
	nBones = bones->nBones;
	bones->nBones++;
	bones->changed = 1;
	bones->boneIndex[nBones] = childTransform;
	l = currentModel->coordListSize[parentMesh] /3;
	bones->vertexWeight[nBones] = (float*)malloc(l*sizeof(float));
	bones->bindCoords[nBones] = (float*)malloc(3*l*sizeof(float));
	range = 0.0f;
	for(k=0;k<l;k++)
	{
		// compute the binding coordinates
		float *f = &currentModel->origCoordList[parentMesh][k*3];
		float v[3];
		multVect3Mat(v,f,parentToWCMat); // take the original local coordinates and multiply them by the toWC matrix to get the global coordinates
		multVect3Mat(&bones->bindCoords[nBones][k*3],v,childFromWCMat); // multiply the global coords by the fromWC matrix of the bone; the result is the local coordinates in the bone's coordinate system
		
		// compute the range, i.e. max distance of the vertices from the center of rotation, in the bone's coord system; this is used for weoght calculation
		float d = vect3Dist(&bones->bindCoords[nBones][k*3],centerOfRotation);
		if(d > range)
			range = d;
	}
	for(k=0;k<l;k++)
	{
		float *f = &bones->bindCoords[nBones][k*3];
		bones->vertexWeight[nBones][k] = computeBoneWeight(f,centerOfRotation, range/2.0f); // first vector is the coordinate, second vector is the centre of rotation of the bone, the third is the influence range
	}


	// set child mesh bones
	bones = &currentModel->boneData[childMesh];
	nBones = bones->nBones;
	bones->nBones++;
	bones->changed = 1;
	bones->boneIndex[nBones] = parentTransform;
	l = currentModel->coordListSize[childMesh] /3;
	bones->vertexWeight[nBones] = (float*)malloc(l*sizeof(float));
	bones->bindCoords[nBones] = (float*)malloc(3*l*sizeof(float));
	range = 0.0f;
	for(k=0;k<l;k++)
	{
		// compute the binding coordinates
		float *f = &currentModel->origCoordList[childMesh][k*3];
		float v[3];
		multVect3Mat(v,f,childToWCMat); // take the original local coordinates and multiply them by the toWC matrix to get the global coordinates
		multVect3Mat(&bones->bindCoords[nBones][k*3],v,parentFromWCMat); // multiply the global coords by the fromWC matrix of the bone; the result is the local coordinates in the bone's coordinate system
		
		// compute the range, i.e. max distance of the vertices from the center of rotation; this is used for weight calculation
		float d = vect3Dist(f,centerOfRotation);
		if(d > range)
			range = d;
	}
	for(k=0;k<l;k++)
	{
		float *f = &currentModel->origCoordList[childMesh][k*3];
		bones->vertexWeight[nBones][k] = computeBoneWeight(f,centerOfRotation, range/2.0f); // first vector is the coordinate, second vector is the centre of rotation of the bone, the third is the influence range
	}





*/



#if 0

void BaseFileIO::outputAnimData(FILE *of)
{
	char fapName[100];
	int i,j,k;
	int zeroSeg = 0;
	int segLength[6000];
	int nSegs;
	float ***coordDiff;
	int fv;
	float **mtCoord;

	float sensitivityThreshold = 0.0001f;

	initTripleFloatPointer(&coordDiff,NB_SD,nSurfaces);
	for(i=0;i<NB_SD;i++)
	{
		for(j=0;j<nSurfaces;j++)
		{
			coordDiff[i][j] = (float*)malloc(coordLength[j]*sizeof(float));
			for(k=0;k<coordLength[j];k++)
				coordDiff[i][j][k] = 0.0f;
		}
	}

	for(i=0;i<NB_SD;i++)
	{
		if(i<15)
		{
			mtCoord = fap1Coord[i];
			fv = fap1Value0[i];
		}
		else
		{
			if(i<=21)
			{
				mtCoord = fap2Coord[i-15];
				fv = fap2Value0[i-15];
			}
			else
			{
				mtCoord = fapCoord[i-20];
				fv = fapValue0[i-20];
			}
		}

		if(fv == -1)
			continue;
		if(!mtCoord)
			continue;

		for(j=0;j<nSurfaces;j++)
		{
			for(k=0;k<coordLength[j];k++)
			{
				if(myfabs(mtCoord[j][k]) > sensitivityThreshold)
					coordDiff[i][j][k] = mtCoord[j][k];
				else
					coordDiff[i][j][k] = 0;
			}
		}
	}
	
	for(i=0;i<NB_SD;i++)
	{
		nSegs = 0;
		zeroSeg = 1;
		if(i<15)
		{
			fv = fap1Value0[i];
		}
		else
		{
			if(i<=21)
			{
				fv = fap2Value0[i-15];
			}
			else
			{
				fv = fapValue0[i-20];
			}
		}

		if(fv == -1)
			continue;
		
		segLength[nSegs] = 0;
		for(j=0;j<nSurfaces;j++)
		{
			for(k=0;k<coordLength[j];k+=3)
			{
				int diff = 1;
				float *f = &coordDiff[i][j][k];
				if(f[0] == 0.0f && f[1] == 0.0f && f[2] == 0.0f)
					diff = 0;
				
				if(zeroSeg == 1 && diff == 1)
				{
					zeroSeg = 0;
					nSegs++;
					segLength[nSegs] = 1;
				}
				else
					if(zeroSeg == 0 && diff == 0)
					{
						zeroSeg = 1;
						nSegs++;
						segLength[nSegs] = 1;
					}
					else
					{
						segLength[nSegs]++;
					}
			}
		}
		
		if(!nSegs)
			continue;
		
		if(i<15)
		{
			sprintf(fapName,"CIG1-%d",i);
		}
		else
		{
			if(i<=21)
			{
				sprintf(fapName,"CIG2-%d",i-15);
			}
			else
			{
				sprintf(fapName,"CIG%d",i-19);
			}
		}
		fprintf(of,"  DEF %s CoordinateInterpolator {\n",fapName);
		fprintf(of,"         key [ ");
		
		// print FAP value for llfaps
		fprintf(of,"%d, ",fv);
		
		nSegs++;
		fprintf(of,"%d, ",nSegs);
		for(j=0;j<nSegs;j++)
		{
			fprintf(of,"%d",segLength[j]);
			if(j < nSegs - 1) fprintf(of,", ");
		}
		fprintf(of," ] \n");
		fprintf(of,"         keyValue [ \n");
		for(j=0;j<nSurfaces;j++)
		{
			for(k=0;k<coordLength[j];k+=3)
			{
				int diff = 1;
				float *f = &coordDiff[i][j][k];
				if(f[0] == 0.0f && f[1] == 0.0f && f[2] == 0.0f)
					diff = 0;
				if(diff)
				{
					fprintf(of,"            %f %f %f",f[0],f[1],f[2]);
					if (k+3 < coordLength[j])
						fprintf(of,",\n");
					else
						if (j+1 < nSurfaces)
							fprintf(of,",\n");
				}
			}
		}
		fprintf(of," ] \n");
		fprintf(of,"      } \n");			
	}
	freeTripleFloatPointer(coordDiff,NB_SD,nSurfaces);
}

void BaseFileIO::outputTransfData(FILE *of)
{
	char fapName[100];
	int i,j,k;
	int numTransf[6000];
	int nActiveTransf;
	int nAnimTransf = 0;
	int diff = 0;
	float ***transfDiff;
	float transf[17];
	float *r,*s,*t;
	int fv;

	int cnt = 0;
			
	initTripleFloatPointer(&transfDiff,NB_SD,nTransforms,17);
	for(i=0;i<NB_SD;i++)
	{
		for(j=0;j<nTransforms;j++)
		{
			for(k=0;k<17;k++)
				transfDiff[i][j][k] = 0.0f;
		}
	}
	for(i=0;i<NB_SD;i++)
	{
		if(i<15)
		{
			fv = fap1Value0[i];
			if(!fap1Position[i])
				continue;
		}
		else
		{
			if(i<=21)
			{
				fv = fap2Value0[i-15];
				if(!fap2Position[i-15])
					continue;
			}
			else
			{
				fv = fapValue0[i-20];
				if(!fapPosition[i-20])
					continue;
			}
		}
		if(fv == -1)
			continue;
		for(j=0;j<nTransforms;j++)
		{
			if(i<15)
			{
				t = fap1Position[i][j];
				r = fap1Orientation[i][j];
				s = fap1Scale[i][j];
			}
			else
			{
				if(i<=21)
				{
					t = fap2Position[i-15][j];
					r = fap2Orientation[i-15][j];
					s = fap2Scale[i-15][j];
				}
				else
				{
					t = fapPosition[i-20][j];
					r = fapOrientation[i-20][j];
					s = fapScale[i-20][j];
				}
			}
			cnt = 0;
			for(k=0;k<3;k++)
				transfDiff[i][j][cnt++] = t[k];
			for(k=0;k<4;k++)
				transfDiff[i][j][cnt++] = r[k];
			for(k=0;k<3;k++)
				transfDiff[i][j][cnt++] = s[k];
			for(k=0;k<4;k++)
				transfDiff[i][j][cnt++] = 0.0f; // scale orientation ignored
			for(k=0;k<3;k++)
				transfDiff[i][j][cnt++] = 0.0f; // center ignored

			cnt = 0;
			for(k=0;k<3;k++)
				transf[cnt++] = origPosition[j][k];
			for(k=0;k<4;k++)
				transf[cnt++] = origOrientation[j][k];
			for(k=0;k<3;k++)
				transf[cnt++] = origScale[j][k];
			for(k=0;k<4;k++)
				transf[cnt++] = 0.0f;
			for(k=0;k<3;k++)
				transf[cnt++] = 0.0f;
			
			for(k=0;k<17;k++)
			{
				float f = transfDiff[i][j][k];
				transfDiff[i][j][k] = f;
			}
		}
	}
	
	for(i=0;i<NB_SD;i++)
	{
		nActiveTransf = 0;
		if(i<15)
		{
			fv = fap1Value0[i];
		}
		else
		{
			if(i<=21)
			{
				fv = fap2Value0[i-15];
			}
			else
			{
				fv = fapValue0[i-20];
			}
		}
		if(fv == -1)
			continue;
		
		
		for(j=0;j<nTransforms;j++)
		{
			diff = 0;
			for(k=0;k<17;k++)
			{
				float f = transfDiff[i][j][k];
				if(f!=0.0f)
					diff = 1;
			}
			if(diff)
			{
				numTransf[nActiveTransf++] = j;
				nAnimTransf++;
			}
		}
		if(!nActiveTransf)
			continue;
		
		if(i<15)
		{
			sprintf(fapName,"CIT1-%d",i);
		}
		else
		{
			if(i<=21)
			{
				sprintf(fapName,"CIT2-%d",i-15);
			}
			else
			{
				sprintf(fapName,"CIT%d",i-19);
			}
		}
		fprintf(of,"  DEF %s CoordinateInterpolator {\n",fapName);
		fprintf(of,"         key [ ");
		
		// print FAP value for llfaps
		fprintf(of,"%d, ",fv);
		
		for(j=0;j<nActiveTransf;j++)
		{
			fprintf(of,"%d",numTransf[j] + 1);
			if(j < nActiveTransf - 1) fprintf(of,", ");
		}
		fprintf(of," ] \n");
		fprintf(of,"         keyValue [ \n");
		for(j=0;j<nActiveTransf;j++)
		{
			for(k=0;k<17;k++)
			{
				fprintf(of,"%f",transfDiff[i][numTransf[j]][k]);
				if(k<16) fprintf(of,", ");
			}
			fprintf(of,", 0.0"); // pad to make a valid coodrinterpolatornode
			if(j < nActiveTransf - 1) fprintf(of,"\n");
		}
		fprintf(of," ] \n");
		fprintf(of,"      } \n");			
	}
	freeTripleFloatPointer(transfDiff,NB_SD,nTransforms);
}

#endif

void BaseFileIO::readNeutralPose(Node *node)
{
    MFString     *f_FAPs;
    MFInt32      *f_FAPValues;
    MFString     *f_NonNPTransforms;
    MFRotation    *f_NPRotations;

    char **faps;
	int nFaps;
    int *FAPValues;
    char **NonNPTransforms;
    int nNonNPTransforms;
    float *NPRotations;
	int i,j;
	char tmpStr[200];

    f_FAPs = (MFString*) node->getField(0);
	nFaps = f_FAPs->getSize();
	faps = (char**) malloc(nFaps*sizeof(char*));
	for(i=0;i<nFaps;i++)
	{
		strcpy(tmpStr,f_FAPs->getValue(i));
		faps[i] = (char*)malloc((strlen(tmpStr)+1)*sizeof(char));
		strcpy(faps[i],tmpStr);
	}

    f_FAPValues = (MFInt32*) node->getField(1);
	FAPValues = (int*) malloc(nFaps * sizeof(int));
	for(i=0;i<nFaps;i++)
		FAPValues[i] = f_FAPValues->getValue(i);


	if(nFaps)
	{
		FBAPs *tmpFBAPs = new FBAPs();

		for(i=0;i<nFaps;i++)
		{
			tmpFBAPs->setFBAP(faps[i],FAPValues[i]);
		}

		currentModel->neutralPoseData.initialFacialPose = tmpFBAPs->faps;
	}


    f_NonNPTransforms = (MFString*) node->getField(2);
	nNonNPTransforms = f_NonNPTransforms->getSize();
	NonNPTransforms = (char**) malloc(nNonNPTransforms*sizeof(char*));
	for(i=0;i<nNonNPTransforms;i++)
	{
		strcpy(tmpStr,f_NonNPTransforms->getValue(i));
		NonNPTransforms[i] = (char*)malloc((strlen(tmpStr)+1)*sizeof(char));
		strcpy(NonNPTransforms[i],tmpStr);
	}

    f_NPRotations = (MFRotation*) node->getField(3);
	NPRotations = (float*) malloc(nNonNPTransforms * sizeof(float) * 4);
	for(i=0;i<nNonNPTransforms;i++)
	{
		SFRotation *rot = (SFRotation *) f_NPRotations->getSFValue(i);
		NPRotations[(i*4)+0] = rot->getQuat().x;
		NPRotations[(i*4)+1] = rot->getQuat().y;
		NPRotations[(i*4)+2] = rot->getQuat().z;
		NPRotations[(i*4)+3] = rot->getQuat().w;
	}
//		for(j = 0;j<4;j++)
//			NPRotations[(i*4)+j] = f_NPRotations->getValue(i)[j];


	if(nNonNPTransforms)
	{
		int nt = currentModel->nTransfLists;
		currentModel->neutralPoseData.NPRotData = (float**) malloc(nt * sizeof(float*));
		for(i=0;i<nt;i++)
			currentModel->neutralPoseData.NPRotData[i] = 0;
		for(i=0;i<nNonNPTransforms;i++)
		{
			int transfInd = currentModel->findNamedTransform(NonNPTransforms[i]);
			if(transfInd==-1)
				continue;

			currentModel->neutralPoseData.NPRotData[transfInd] = (float*)malloc(4*sizeof(float));

			for(j=0;j<4;j++)
				currentModel->neutralPoseData.NPRotData[transfInd][j] = NPRotations[i*4+j];
		}
	}


	for(i=0;i<nFaps;i++)
	{
		free(faps[i]);
	}
	free(faps);
	free(FAPValues);

	for(i=0;i<nNonNPTransforms;i++)
	{
		free(NonNPTransforms[i]);
	}
	free(NonNPTransforms);
	free(NPRotations);
}

void BaseFileIO::setBoneData(char *meshName,
    char **bones,
	int nBones,
    int *vertices,
	int nVertices,
    float *weights,
	int nWeights)
{
	int i,j,vcnt,wcnt;
	myMatrix toWC,fromWCtoBoneRef;
	int boneInd;
	int meshInd = currentModel->findNamedMesh(meshName);


	if(meshInd == -1)
		return;

	VRMLMeshBones *bd = &currentModel->boneData[meshInd];
	int coordLength = currentModel->coordListSize[meshInd];
	float *origCoord = currentModel->origCoordList[meshInd];
	copyMat(toWC,currentModel->toWC[currentModel->transfInd[meshInd]]); 
	bd->nBones = nBones;
	bd->changed = true;

	vcnt = wcnt =0;
	for(i=0;i<nBones;i++)
	{
		boneInd = currentModel->findNamedTransform(bones[i]);
		bd->boneIndex[i] = boneInd;
		if(boneInd==-1)
			continue;

		bd->vertexWeight[i] = (float*)malloc((coordLength/3)*sizeof(float));
		bd->bindCoords[i] = (float*)malloc(coordLength*sizeof(float));
		for(j=0;j<coordLength/3;j++)
			bd->vertexWeight[i][j] = 0.0f;

		while(vertices[vcnt] != -1)
		{
			bd->vertexWeight[i][vertices[vcnt]] = weights[wcnt];
			vcnt++;
			wcnt++;
		}
		vcnt++;

		copyMat(fromWCtoBoneRef,currentModel->fromWC[boneInd]); 
		for(j=0;j<coordLength/3;j++)
		{
			// compute the binding coordinates
			float *f = &origCoord[j*3];
			float fw[3];
			multVect3Mat(fw,f,toWC); // take the original local coordinates and multiply them by the toWC matrix to get the global coordinates
			multVect3Mat(&bd->bindCoords[i][j*3],fw,fromWCtoBoneRef); // multiply the global coords by the fromWC matrix of the bone; the result is the local coordinates in the bone's coordinate system
		}
	}

	// debug - test if sum of weights is 1
	for(j=0;j<coordLength/3;j++)
	{
		float w = 0.0f;
		if(bd->boneIndex[i]==-1)
			continue;
		for(i=0;i<nBones;i++)
		{
			w += bd->vertexWeight[i][j];
		}
		if(w < 0.99 || w > 1.001)
		{
//			MessageBox(0,"Weights not 1","Error",MB_ICONERROR);
			fprintf(stderr,"Weights not 1\n");
		}
	}
	
}

void BaseFileIO::readIFSBoneWeights(Node *node)
{
    SFString     *f_meshName;
    MFString     *f_bones;
    MFInt32      *f_vertices;
    MFFloat      *f_weights;

    char meshName[200];
    char **bones;
	int nBones;
    int *vertices;
	int nVertices;
    float *weights;
	int nWeights;

	int i;
	char tmpStr[200];

    f_meshName = (SFString*) node->getField(0);
	strcpy(meshName,f_meshName->getValue());

    f_bones = (MFString*) node->getField(1);
	nBones = f_bones->getSize();
	bones = (char**) malloc(nBones*sizeof(char*));
	for(i=0;i<nBones;i++)
	{
		strcpy(tmpStr,f_bones->getValue(i));
		bones[i] = (char*)malloc((strlen(tmpStr)+1)*sizeof(char));
		strcpy(bones[i],tmpStr);
	}

    f_vertices = (MFInt32*) node->getField(2);
	nVertices = f_vertices->getSize();
	vertices = (int*) malloc(nVertices * sizeof(int));
	for(i=0;i<nVertices;i++)
		vertices[i] = f_vertices->getValue(i);

    f_weights = (MFFloat*) node->getField(3);
	nWeights = f_weights->getSize();
	weights = (float*) malloc(nVertices * sizeof(float));
	for(i=0;i<nWeights;i++)
		weights[i] = f_weights->getValue(i);


	setBoneData(meshName,
		bones,
		nBones,
		vertices,
		nVertices,
		weights,
		nWeights);




	for(i=0;i<nBones;i++)
	{
		free(bones[i]);
	}
	free(bones);
	free(vertices);
	free(weights);
}

// find a modifier corresponding to the name of the animation parameter
VRMLModelModifier *BaseFileIO::findModifier(char *name)
{
	int i;

	// search visemes
	for(i=0;i<nVisemes;i++)
	{
		if(!strcmp(name,VisemeNameString[i]))
			return(&VisemeModifiers[i]);
	}

	// search expressions
	for(i=0;i<nExp;i++)
	{
		if(!strcmp(name,ExpressionNameString[i]))
			return(&ExpressionModifiers[i]);
	}

	// search FAPs
	for(i=0;i<NFAP;i++)
	{
		if(!strcmp(name,FAPNameString[i]))
			return(&FAPModifiers[i]);
	}

	// search BAPs
	for(i=0;i<NBAP;i++)
	{
		if(!strcmp(name,BAPNameString[i]))
			return(&BAPModifiers[i]);
	}

	return (0);
}

// finish initializing the rotation modifiers and set the neutral pose
void BaseFileIO::setNeutralPoseMod(VRMLModelModifier *mod,int initPoseValue)
{

	for(int transfInd=0;transfInd<currentModel->nTransfLists;transfInd++)
	{
		if(!mod->TRotData[transfInd]) continue;
		float r0[4];
		int j;	//CHANGED BY I.K. declared iterator outside of loop
		for(j=0;j<4;j++)
			r0[j] = mod->TRotData[transfInd][j];
		aa2quat(mod->TRotData[transfInd],r0,r0[3]);
		for(j=0;j<4;j++)
			r0[j] = currentModel->transfList[transfInd][3+j];
		quatDiff(mod->TRotData[transfInd],r0);
		setRotation(mod->TRotData[transfInd]);
		
		if(!currentModel->neutralPoseData.initialFacialPose) continue;
		
		if(initPoseValue)
		{
			// rotate the transform to put it into initial pose
			int flag = 0;
			for(int j1 = 0; j1 < 3; j1++)
			{
				if(mod->TRotData[transfInd][j1] != 0.0)
					flag = 1;
			}
			if(flag)
			{
				float axis[3];
				float q[4];
				float * orient = &(currentModel->transfList[transfInd][3]);
				
				axis[0] = mod->TRotData[transfInd][0];
				axis[1] = mod->TRotData[transfInd][1];
				axis[2] = mod->TRotData[transfInd][2];
				float angle = mod->TRotData[transfInd][3];
				
				float weight = (float)(- initPoseValue) / (float)(mod->refValue - initPoseValue);
				
				angle *= weight;
				
				aa2quat(q,axis,angle);
				multQuat(orient,q,orient);
			}
		}
	}
}

void BaseFileIO::setNeutralPoseAndAdjustModifiers()
{
	int i;


	if(currentModel->neutralPoseData.NPRotData)
	{
		for(int transfInd=0;transfInd<currentModel->nTransfLists;transfInd++)
		{
			if(!currentModel->neutralPoseData.NPRotData[transfInd]) continue;
			float * orient = &(currentModel->transfList[transfInd][3]);

			for(int j=0;j<4;j++)
				orient[j] = currentModel->neutralPoseData.NPRotData[transfInd][j];
		}
	}

	for(i=0;i<NFAP;i++)
	{
		if(!FAPModifiers[i].TRotData) continue;

		VRMLModelModifier *mod = &FAPModifiers[i];
		int initPoseValue = 0;
		
		if(currentModel->neutralPoseData.initialFacialPose)
			initPoseValue = currentModel->neutralPoseData.initialFacialPose->llf.value[i];

		setNeutralPoseMod(mod,initPoseValue);
	}

	for(i=0;i<NBAP;i++)
	{
		if(!BAPModifiers[i].TRotData) continue;

		VRMLModelModifier *mod = &BAPModifiers[i];
		int initPoseValue = 0;

		setNeutralPoseMod(mod,initPoseValue);
	}

	for(i=0;i<nExp;i++)
	{
		if(!ExpressionModifiers[i].TRotData) continue;

		VRMLModelModifier *mod = &ExpressionModifiers[i];
		int initPoseValue = 0;

		setNeutralPoseMod(mod,initPoseValue);
	}
	for(i=0;i<nVisemes;i++)
	{
		if(!VisemeModifiers[i].TRotData) continue;

		VRMLModelModifier *mod = &VisemeModifiers[i];
		int initPoseValue = 0;

		setNeutralPoseMod(mod,initPoseValue);
	}

}


// correct the translation data of a modifier to compensate for removal of NUS
void BaseFileIO::correctTranslationDataNUSMod(VRMLModelModifier *mod)
{
	for(int transfInd=0;transfInd<currentModel->nTransfLists;transfInd++)
	{
		if(!mod->TTranData[transfInd]) continue;
		float s[3];
		int j;	//CHANGED BY I.K. declared iterator outside of loop
		for(j=0;j<3;j++)
			s[j] = currentModel->transfList[transfInd][17+j];
		for(j=0;j<3;j++)
			mod->TTranData[transfInd][j] = s[j];
	}
}


void BaseFileIO::correctTranslationDataNUS()
{
	int i;


	for(i=0;i<NBAP;i++)
	{
		VRMLModelModifier *mod = &BAPModifiers[i];
		if(!mod->TTranData) continue;		
		correctTranslationDataNUSMod(mod);
	}

	for(i=0;i<NFAP;i++)
	{
		VRMLModelModifier *mod = &FAPModifiers[i];
		if(!mod->TTranData) continue;
		correctTranslationDataNUSMod(mod);
	}

	for(i=0;i<nVisemes;i++)
	{
		VRMLModelModifier *mod = &VisemeModifiers[i];
		if(!mod->TTranData) continue;
		correctTranslationDataNUSMod(mod);
	}

	for(i=0;i<nExp;i++)
	{
		VRMLModelModifier *mod = &ExpressionModifiers[i];
		if(!mod->TTranData) continue;
		correctTranslationDataNUSMod(mod);
	}
}



// set mtVertices to values from val, using seg as indication of alternating segments of
// zero values and values taken from val; update indices into both seg and val arrays
void BaseFileIO::setMTData(float *mtVertices,int *seg,int *segInd,float *val,int *valInd, float *origCoords)
{
	bool zeroSeg = false;
	int vind = 0;
	int cnt;

	while(seg[(*segInd)] != -1)
	{
		cnt = seg[(*segInd)] * 3;

		while(cnt!=0)
		{
			if(zeroSeg)
			{
				mtVertices[vind++] = 0.0f;
			}
			else
			{
				mtVertices[vind] = val[(*valInd)] - origCoords[vind];
				(*valInd)++;
				vind++;
			}
			cnt--;
		}
		if(zeroSeg)
			zeroSeg = false;
		else
			zeroSeg = true;;
		(*segInd)++;
	}
}

void BaseFileIO::setModifier(VRMLModelModifier *mod,
							 int refValue,
							 char **morphedIFSs,
							 int nMorphedIFSs,
							 int *morphedVertices,
							 int nMorphedVertices,
							 float *morphValues,
							 int nMorphValues,
							 char **tTransforms,
							 int nTTransforms,
							 float *translations,
							 char **rTransforms,
							 int nRTransforms,
							 float *rotations,
							 char **sTransforms,
							 int nSTransforms,
							 float *scales)
{
	int i,j;
	int *morphedIFSIndices;

	mod->refValue = (float)refValue;
	
	if(nMorphedIFSs)
	{
		morphedIFSIndices = (int*)malloc(nMorphedIFSs*sizeof(int));
		for(i=0;i<nMorphedIFSs;i++)
			morphedIFSIndices[i] = currentModel->findNamedMesh(morphedIFSs[i]);
		mod->MTData = (float**) malloc(currentModel->nCoordLists * sizeof(float*));
		for(i=0;i<currentModel->nCoordLists;i++)
			mod->MTData[i] = 0;
		int segInd = 0;
		int valInd = 0;
		for(i=0;i<nMorphedIFSs;i++)
		{
			int meshInd = morphedIFSIndices[i];
			if(meshInd==-1)
				continue;
			int meshSize = currentModel->coordListSize[meshInd];
			mod->MTData[meshInd] = (float*)malloc(meshSize*sizeof(float));
			setMTData(mod->MTData[meshInd],morphedVertices,&segInd,morphValues,&valInd,currentModel->origCoordList[meshInd]);
			segInd++;
		}
		free(morphedIFSIndices);
	}
	
	if(nTTransforms)
	{
		mod->TTranData = (float**) malloc(currentModel->nTransfLists * sizeof(float*));
		for(i=0;i<currentModel->nTransfLists;i++)
			mod->TTranData[i] = 0;
		for(i=0;i<nTTransforms;i++)
		{
			int transfInd = currentModel->findNamedTransform(tTransforms[i]);
			if(transfInd==-1)
				continue;
			mod->TTranData[transfInd] = (float*)malloc(3*sizeof(float));
			for(j=0;j<3;j++)
				mod->TTranData[transfInd][j] = (translations[(i*3)+j] * currentModel->transfList[transfInd][17+j]) - currentModel->transfList[transfInd][j];
		}
	}
	
	if(nRTransforms)
	{
		mod->TRotData = (float**) malloc(currentModel->nTransfLists * sizeof(float*));
		for(i=0;i<currentModel->nTransfLists;i++)
			mod->TRotData[i] = 0;
		for(i=0;i<nRTransforms;i++)
		{
			int transfInd = currentModel->findNamedTransform(rTransforms[i]);
			if(transfInd==-1)
				continue;
			mod->TRotData[transfInd] = (float*)malloc(4*sizeof(float));

			for(j=0;j<4;j++)
				mod->TRotData[transfInd][j] = rotations[i*4+j];
		}
	}

	if(nSTransforms)
	{
		mod->TScalData = (float**) malloc(currentModel->nTransfLists * sizeof(float*));
		for(i=0;i<currentModel->nTransfLists;i++)
			mod->TScalData[i] = 0;
		for(i=0;i<nSTransforms;i++)
		{
			int transfInd = currentModel->findNamedTransform(sTransforms[i]);
			if(transfInd==-1)
				continue;
			mod->TScalData[transfInd] = (float*)malloc(3*sizeof(float));
			for(j=0;j<3;j++)
				mod->TScalData[transfInd][j] = scales[(i*3)+j] - currentModel->transfList[transfInd][7 + j];
		}
	}
}

void BaseFileIO::readAFMVersion(Node *node)
{
    SFFloat      *f_versionNumber;

	f_versionNumber = (SFFloat*) node->getField(0);
	AFMVersionNumber = f_versionNumber->getValue();

	//replaced by TPejsa
	//if(AFMVersionNumber != AFM_VERSION_NUMBER)
	if( !AFM_VERSION_SUPPORTED(AFMVersionNumber) )
	//
	{
		fprintf(stderr,"\n\nWARNING: Wrong AFM file format version!\nCurrent version is %1.1f. \nYou are trying to read a file in AFM format version %1.1f. \n Please use AFM file format version %1.1f.",AFM_VERSION_NUMBER,AFMVersionNumber,AFM_VERSION_NUMBER);
		exit(199);
	}
}

void BaseFileIO::readAnimationParameter(Node *node)
{
    SFString     *f_name;
 //   SFInt32      *f_refValue;
    SFFloat      *f_refValue;
    MFString     *f_morphedIFSs;
    MFInt32      *f_morphedVertices;
    MFVec3f      *f_morphValues;
    MFString     *f_tTransforms;
    MFVec3f      *f_translations;
    MFString     *f_rTransforms;
    MFRotation   *f_rotations;
    MFString     *f_sTransforms;
    MFVec3f      *f_scales;

	char name[100];
    int refValue;
    char **morphedIFSs;
	int nMorphedIFSs;
    int *morphedVertices;
    int nMorphedVertices;
    float *morphValues;
    int nMorphValues;
    char **tTransforms;
    int nTTransforms;
    float *translations;
    char **rTransforms;
    int nRTransforms;
    float *rotations;
    char **sTransforms;
    int nSTransforms;
    float *scales;

	int i,j;
	char tmpStr[500];
	VRMLModelModifier *mod;

    f_name = (SFString*) node->getField(0);
	strcpy(name,f_name->getValue());

	f_refValue = (SFFloat*) node->getField(1);
	refValue = (int)f_refValue->getValue();

    f_morphedIFSs = (MFString*) node->getField(2);
	nMorphedIFSs = f_morphedIFSs->getSize();
	morphedIFSs = (char**) malloc(nMorphedIFSs*sizeof(char*));
	for(i=0;i<nMorphedIFSs;i++)
	{
		strcpy(tmpStr,f_morphedIFSs->getValue(i));
		morphedIFSs[i] = (char*)malloc((strlen(tmpStr)+1)*sizeof(char));
		strcpy(morphedIFSs[i],tmpStr);
	}

    f_morphedVertices = (MFInt32*) node->getField(3);
	nMorphedVertices = f_morphedVertices->getSize();
	morphedVertices = (int*) malloc(nMorphedVertices * sizeof(int));
	for(i=0;i<nMorphedVertices;i++)
		morphedVertices[i] = f_morphedVertices->getValue(i);

    f_morphValues = (MFVec3f*) node->getField(4);
	nMorphValues = f_morphValues->getSize();
	morphValues = (float*) malloc(nMorphValues * sizeof(float));
	for(i=0;i<nMorphValues;i+=3)
		for(j = 0;j<3;j++)
			morphValues[i+j] = f_morphValues->getValue(i/3)[j];

    f_tTransforms = (MFString*) node->getField(5);
	nTTransforms = f_tTransforms->getSize();
	tTransforms = (char**) malloc(nTTransforms*sizeof(char*));
	for(i=0;i<nTTransforms;i++)
	{
		strcpy(tmpStr,f_tTransforms->getValue(i));
		tTransforms[i] = (char*)malloc((strlen(tmpStr)+1)*sizeof(char));
		strcpy(tTransforms[i],tmpStr);
	}

    f_translations = (MFVec3f*) node->getField(6);
	translations = (float*) malloc(nTTransforms * sizeof(float) * 3);
	for(i=0;i<nTTransforms;i++)
		for(j = 0;j<3;j++)
			translations[(i*3)+j] = f_translations->getValue(i)[j];

    f_rTransforms = (MFString*) node->getField(7);
	nRTransforms = f_rTransforms->getSize();
	rTransforms = (char**) malloc(nRTransforms*sizeof(char*));
	for(i=0;i<nRTransforms;i++)
	{
		strcpy(tmpStr,f_rTransforms->getValue(i));
		rTransforms[i] = (char*)malloc((strlen(tmpStr)+1)*sizeof(char));
		strcpy(rTransforms[i],tmpStr);
	}

    f_rotations = (MFRotation*) node->getField(8);
	rotations = (float*) malloc(nRTransforms * sizeof(float) * 4);
	for(i=0;i<nRTransforms;i++)
		for(j = 0;j<4;j++)
			rotations[(i*4)+j] = f_rotations->getValue(i)[j];

    f_sTransforms = (MFString*) node->getField(9);
	nSTransforms = f_sTransforms->getSize();
	sTransforms = (char**) malloc(nSTransforms*sizeof(char*));
	for(i=0;i<nSTransforms;i++)
	{
		strcpy(tmpStr,f_sTransforms->getValue(i));
		sTransforms[i] = (char*)malloc((strlen(tmpStr)+1)*sizeof(char));
		strcpy(sTransforms[i],tmpStr);
	}

    f_scales = (MFVec3f*) node->getField(10);
	scales = (float*) malloc(nSTransforms * sizeof(float) * 3);
	for(i=0;i<nSTransforms;i++)
		for(j = 0;j<3;j++)
			scales[(i*3)+j] = f_scales->getValue(i)[j];

	mod = findModifier(name);
/*
	if(!strcmp(name,"raise_l_m_eyebrow"))
	{
		j = j;
	}
	*/
	if(mod)
		setModifier(mod,
		refValue,
		morphedIFSs,
		nMorphedIFSs,
		morphedVertices,
		nMorphedVertices,
		morphValues,
		nMorphValues,
		tTransforms,
		nTTransforms,
		translations,
		rTransforms,
		nRTransforms,
		rotations,
		sTransforms,
		nSTransforms,
		scales);
	
	for(i=0;i<nMorphedIFSs;i++)
	{
		free(morphedIFSs[i]);
	}
	free(morphedIFSs);
	free(morphedVertices);
	free(morphValues);
	for(i=0;i<nTTransforms;i++)
	{
		free(tTransforms[i]);
	}
	free(tTransforms);
	free(translations);
	for(i=0;i<nRTransforms;i++)
	{
		free(rTransforms[i]);
	}
	free(rTransforms);
	free(rotations);
	for(i=0;i<nSTransforms;i++)
	{
		free(sTransforms[i]);
	}
	free(sTransforms);
	free(scales);
}

void BaseFileIO::traverse(Node *_currentRootNode, int traverseType,char *iid)
{
    NodeList *_allRootNodes;
    FieldValue *_value; 
	char tmp[200];
	
	strcpy(tmp,_currentRootNode->getProto()->getName());
	
	if(!strcmp(tmp,"AFMVersion"))
	{
		switch(traverseType)
		{
		case TT_READ_ANIM_DATA:
			readAFMVersion(_currentRootNode);
			break;
		};
	}
	if(!strcmp(tmp,"AnimationParameter"))
	{
		switch(traverseType)
		{
		case TT_READ_ANIM_DATA:
			readAnimationParameter(_currentRootNode);
			break;
		};
	}
	else
		if(!strcmp(tmp,"IFSBoneWeights"))
		{
			switch(traverseType)
			{
			case TT_READ_ANIM_DATA:
				readIFSBoneWeights(_currentRootNode);
				break;
			};
		}
		else
			if(!strcmp(tmp,"NeutralPose"))
			{
				switch(traverseType)
				{
				case TT_READ_ANIM_DATA:
					readNeutralPose(_currentRootNode);
					break;
				};
			}
			// Added by KStankovic
			else
				if (!strcmp(tmp, "Cubemap"))
				{
					switch (traverseType)
					{
						case TT_READ_ANIM_DATA:
							readCubemap(_currentRootNode);
						break;
					}
				}
			//
			else
			{
				for(int i=0; i<_currentRootNode->getProto()->getNumFields();i++)
				{
					_value = _currentRootNode->getField(i);
					strcpy(tmp,_value->getTypeName());
					
					if(!strcmp(tmp,"MFNode"))
					{
						// process children
						_allRootNodes = ( (MFNode *)_value )->getValue();
						for (int k=0; k<_allRootNodes->size(); k++)
						{
							Node *n = _allRootNodes->get(k);
							traverse(n,traverseType,iid);
						}
					}
					else
						if(!strcmp(tmp,"SFNode"))
						{
							Node *n = ((SFNode *)_value )->getValue();
							if(n) traverse(n,traverseType,iid);
						}
				}
			}
}

void BaseFileIO::initTripleFloatPointer(float ****p,int l1,int l2)
{
	(*p) = (float***)malloc(l1 * sizeof(float**));
	for(int i = 0;i<l1;i++)
	{
		(*p)[i] = (float**)malloc(l2*sizeof(float*));
		for(int j=0;j<l2;j++)
			(*p)[i][j] = 0;
	}
}

void BaseFileIO::initTripleFloatPointer(float ****p,int l1,int l2,int l3)
{
	initTripleFloatPointer(p,l1,l2);
	for(int i = 0;i<l1;i++)
		for(int j = 0;j<l2;j++)
		{
			(*p)[i][j] = (float*)malloc(l3*sizeof(float));
			for(int k = 0;k<l3;k++)
				(*p)[i][j][k] = 0.0;
		}
}

void BaseFileIO::initDoubleFloatPointer(float ***p,int l1)
{
	(*p) = (float**)malloc(l1 * sizeof(float*));
	for(int i = 0;i<l1;i++)
		(*p)[i] = 0;
}

void BaseFileIO::initDoubleFloatPointer(float ***p,int l1,int l2)
{
	(*p) = (float**)malloc(l1 * sizeof(float*));
	for(int i = 0;i<l1;i++)
	{
		(*p)[i] = (float*)malloc(l2*sizeof(float));
		for(int j=0;j<l2;j++)
			(*p)[i][j] = 0.0;
	}
}

void BaseFileIO::freeTripleFloatPointer(float ***p,int l1,int l2)
{
	if(p)
	{
		for(int i = 0;i<l1;i++)
			freeDoubleFloatPointer(p[i],l2);
	}
	free(p);
}

void BaseFileIO::freeDoubleFloatPointer(float **p,int l1)
{
	if(p)
	{
		for(int i = 0;i<l1;i++)
		{
			if(p[i])
				free(p[i]);
		}
		free(p);
	}
}
/*
int BaseFileIO::readFapCoords(float **af)
{
	if(nCigKey == 0)
		return(-1);

	int *ai = (int*)malloc(nCigKey * sizeof(int));
	for(int j = 0; j < nCigKey; j++)
		ai[j] = cigKey[j];
	
	for(int k = 0; k < nSurfaces; k++)
	{
		af[k] = (float*)malloc(coordLength[k] * sizeof(float));
		for(int i=0;i<coordLength[k];i++)
			af[k][i]=0.0f;
	}
	
	int l = ai[0];
	int i1 = 0;
	int j1 = 0;
	int k1 = 0;
	int flag = 1;
	for(int l1 = 2; l1 < nCigKey; l1++)
	{
		int i2 = ai[l1];
		if(flag)
		{
			flag = 0;
			for(int j2 = 0; j2 < i2; j2++)
			{
				if(j1 == coordLength[i1])
				{
					i1++;
					j1 = 0;
				}
				for(int l2 = 0; l2 < 3; l2++)
					af[i1][j1++] = 0.0;
				
			}
			
		}
		else
		{
			flag = 1;
			for(int k2 = 0; k2 < i2; k2++)
			{
				if(j1 == coordLength[i1])
				{
					i1++;
					j1 = 0;
				}
				for(int i3 = 0; i3 < 3; i3++)
					af[i1][j1++] = cigValue[k1++];
				
			}
			
		}
	}
	free(ai);
	return l;
}


int BaseFileIO::readFapTransform(float **af, float **af1, float **af2)
{
	if(nCitKey == 0)
		return(-1);

	int *ai = (int*)malloc(nCitKey * sizeof(int));
	for(int j = 0; j < nCitKey; j++)
		ai[j] = citKey[j];
	
	int k = 0;
	for(int l = 1; l < nCitKey; l++)
	{
		ai[l]--;
		for(int i1 = 0; i1 < 3; i1++)
			af[ai[l]][i1] = citValue[k++];
		
		for(int j1 = 0; j1 < 4; j1++)
			af1[ai[l]][j1] = citValue[k++];

		
		for(int k1 = 0; k1 < 3; k1++)
			af2[ai[l]][k1] = citValue[k++];
		k+=8;
		
	}
	int df = ai[0];
	free(ai);
	return (df);
}

void BaseFileIO::readInterpolator(Node *node,int **key,int *nKey,float **value,int *nValue)
{
	NodeCoordinateInterpolator *ciNode   = (NodeCoordinateInterpolator *) node; 
    MFFloat *k;
	MFVec3f *v;
	int i,j;
	k = (MFFloat *) ciNode->getField(0);	
	v = (MFVec3f *) ciNode->getField(1);	
	
	(*nKey) = k->getSize();
	(*nValue) = v->getSize();
	
	if((*nValue) == 0)
	{
//		MessageBox(0,"Corrupt CoordinateInterpolator Node!","Error",MB_ICONERROR);
		fprintf(stderr,"Corrupt CoordinateInterpolator Node!\n");
		exit(0);
	}

	(*key) = (int*)malloc((*nKey) * sizeof(int));
	(*value) = (float*)malloc((*nValue) * sizeof(float));
	
	for(i = 0;i<(*nKey);i++)
		(*key)[i] = (int)k->getValue(i);
	for(i = 0;i<(*nValue);i+=3)
		for(j = 0;j<3;j++)
			(*value)[i+j] = v->getValue(i/3)[j];
}



// BODY RELATED FUNCTIONS

hAnimBody::hAnimBody(char *bodyName)
{
	strcpy(name,bodyName);
	nJoints = 0;
//	afm = _afm;
	for(int i=0;i<150;i++)
		joint[i]=0;
	for(i=0;i<NBAP;i++)
		BAPJoint[i]=0;
}

hAnimBody::~hAnimBody()
{
	for(int i=0;i<nJoints;i++)
		if(joint[i]) delete joint[i];
}

// find a joint by name and return its index
int hAnimBody::getJointIndex(char *jointName)
{
	int i;

	for(i=0;i<nJoints;i++)
	{
		if(!strcmp(joint[i]->getName(),jointName))
			break;
	}
	if (i==nJoints) // joint not found
		return -1;

	return i;
}

// find a joint by name and return the index of its transform node in the VRMLModel
int hAnimBody::getJointTransfIndex(char *jointName)
{
	int i = getJointIndex(jointName);
	
	if(i == -1)
		return -1;
	
	return joint[i]->getTransfIndex();
}


// add a joint to the body with name jointName and the index to the transform in the VRMLModel jointTransfInd
void hAnimBody::addJoint(char *jointName, int jointTransfInd)
{
		int nj = nJoints;
		hAnimBodyJoint *jnt= new hAnimBodyJoint(jointName,jointTransfInd,0.0f);
		joint[nj] = jnt;


		nJoints++;
}

// hAnimBodyJoint constructor
hAnimBodyJoint::hAnimBodyJoint(char *_name, int jointTransfInd, float _value)
{
	strcpy(name,_name);
	ind = jointTransfInd;
	reset();
	changed=1;
}

// hAnimBodyJoint destructor
hAnimBodyJoint::~hAnimBodyJoint()
{
//	delete rq;
}

void hAnimBodyJoint::reset()
{
	r[0]=0.0f;
	r[1]=0.0f;
	r[2]=0.0f;
	r[3]=1.0f;
	_hasTranslation = 0;
	t[0]=0.0f;
	t[1]=0.0f;
	t[2]=0.0f;
}

// add rotation around axis (0-x, 1-y, 2-z) by angle in radians
void hAnimBodyJoint::addRotation(int axis, float angle)
{
	changed = 1;
	
	float a[3];

	if(axis<4) // rotations
	{
		switch(axis)
		{
		case 0:
			break;
		case 1:
			a[0] = 1.0;
			a[1] = 0.0;
			a[2] = 0.0;
			break;
		case 2:
			a[0] = 0.0;
			a[1] = 1.0;
			a[2] = 0.0;
			break;
		case 3:
			a[0] = 0.0;
			a[1] = 0.0;
			a[2] = 1.0;
			break;
		case -1:
			a[0] = -1.0;
			a[1] = 0.0;
			a[2] = 0.0;
			break;
		case -2:
			a[0] = 0.0;
			a[1] = -1.0;
			a[2] = 0.0;
			break;
		case -3:
			a[0] = 0.0;
			a[1] = 0.0;
			a[2] = -1.0;
			break;
		}
		

		r[0]=a[0];
		r[1]=a[1];
		r[2]=a[2];
		r[3]=angle/(float)1e5;
	}
	else // translation (this is only for the humanoid_root BAPs)
	{
		switch(axis)
		{
		case 4:
			t[0] += (float)(angle / 1000.0);
			_hasTranslation = 1;
			break;
		case 5:
			t[1] += (float)(angle / 1000.0);
			_hasTranslation = 1;
			break;
		case 6:
			t[2] += (float)(angle / 1000.0);
			_hasTranslation = 1;
			break;
		}
	}
}



void hAnimBody::initBAPJoints()
{
	char s[50];
	for(int i=0;i<NBAP;i++)
	{
		strcpy(s,"hanim_");
		strcat(s,BAPJointName[i]);
		int ji = getJointIndex(s);
		if(ji>=0)
			BAPJoint[i] = getJoint(ji);
	}
}


// modify q so that q == newq * q0
// newq =  q * inv(q0)
// i.e. new quaternion is the difference between old one and q0
void hAnimBody::quatDiff(float *q, float *q0)
{
	for(int i = 0;i<4;i++)
		if(q[i] != 0.0f)
			break;
		if(i==4)
			return;
		q0[0] = -q0[0];
		q0[1] = -q0[1];
		q0[2] = -q0[2];
		multQuat(q,q,q0);
}


void hAnimBody::arrangeQuat(float *q, float *q0)
{
	for(int i = 0;i<4;i++)
		if(q[i] != 0.0f)
			break;
		if(i==4)
			return;
		quatDiff(q,q0);		
}


#define CONST_PI 3.1415926f

void hAnimBody::setBAPModifier(int n,VRMLModelModifier *mod,VRMLModel *model)
{
	int i;
	hAnimBodyJoint *jnt = BAPJoint[n];
	if(!jnt)
		return;

			


	if(BAPRotationAxis[n] < 4)  // is this a rotation BAP
	{
		float refValue = CONST_PI * 10000.0f / 2.0f;
		jnt->reset();
		jnt->addRotation(BAPRotationAxis[n],refValue);
		int ti = jnt->getTransfIndex();
		mod->refValue = refValue;
		mod->TRotData = (float**)malloc(model->nTransfLists * sizeof(float*));
		for(i=0;i<model->nTransfLists;i++)
			mod->TRotData[i] = 0;
		mod->TRotData[ti] = (float*)malloc(4*sizeof(float));
		float *r = jnt->getRotation();
		for(i=0;i<4;i++)
			mod->TRotData[ti][i] = r[i];
	}
	else //otherwise this BAP is a translation BAP
	{
		float refValue = 1000.0f;
		jnt->reset();
		jnt->addRotation(BAPRotationAxis[n],refValue);
		int ti = jnt->getTransfIndex();
		mod->refValue = refValue;
		mod->TTranData = (float**)malloc(model->nTransfLists * sizeof(float*));
		for(i=0;i<model->nTransfLists;i++)
			mod->TTranData[i] = 0;
		mod->TTranData[ti] = (float*)malloc(3*sizeof(float));
		float *t = jnt->getTranslation();
		for(i=0;i<3;i++)
			mod->TTranData[ti][i] = t[i];
	}
}

void hAnimBody::applyBAPs(BAPs *baps)
{
	for(int i=0;i<NBAP;i++)
	{
		if(baps->value[i] == 0) // if rotation is 0 no need to apply it
			continue;
		hAnimBodyJoint *jnt = BAPJoint[i];
		if(!jnt)
			continue;
		jnt->addRotation(BAPRotationAxis[i],(float)baps->value[i]);
	}

}

void hAnimBody::clearBAPs()
{
	for(int i=0;i<nJoints;i++)
		joint[i]->reset();
}

*/
// helper function for AFM; produces all modifier data from a set of VRMLModels which are considered as morph targets
void BaseFileIO::initAnimData(VRMLModel *model0,VRMLModel **models, int *fv,AFMDeformer **deformers, int *nDeformers, float _coordSensitivity)
{
	currentModel = model0;
	coordSensitivity = _coordSensitivity;

	allocAnimData();
	int cnt = 0;
	int k1;
	int transfChanged, coordChanged;

	traversalType = TT_INIT;
	currentModel->traverse(currentModel->rootNode,this);

	for( k1 = 0; k1 < nVisemes; k1++)
	{
		fap1Value0[k1] = fv[cnt];
		transfChanged = 0;
		coordChanged = 0;
		if(fv[cnt] != -1)
		{
			coordChanged = readFapCoords(fap1Coord[k1],models[cnt]);
			transfChanged = readFapTransform(fap1Position[k1], fap1Orientation[k1], fap1Scale[k1],models[cnt]);
		}
		if(!coordChanged)
		{
			freeDoubleFloatPointer(fap1Coord[k1],nSurfaces);
			fap1Coord[k1] = 0;
		}
		if(!transfChanged)
		{
			freeDoubleFloatPointer(fap1Position[k1],nTransforms);
			freeDoubleFloatPointer(fap1Orientation[k1],nTransforms);
			freeDoubleFloatPointer(fap1Scale[k1],nTransforms);
			fap1Position[k1] = 0;
			fap1Orientation[k1] = 0;
			fap1Scale[k1] = 0;
		}
		cnt++;
	}
	
	for( k1 = 0; k1 < nExp; k1++)
	{
		fap2Value0[k1] = fv[cnt];
		transfChanged = 0;
		coordChanged = 0;
		if(fv[cnt] != -1)
		{
			coordChanged = readFapCoords(fap2Coord[k1],models[cnt]);
			transfChanged = readFapTransform(fap2Position[k1], fap2Orientation[k1], fap2Scale[k1],models[cnt]);
		}
		if(!coordChanged)
		{
			freeDoubleFloatPointer(fap2Coord[k1],nSurfaces);
			fap2Coord[k1] = 0;
		}
		if(!transfChanged)
		{
			freeDoubleFloatPointer(fap2Position[k1],nTransforms);
			freeDoubleFloatPointer(fap2Orientation[k1],nTransforms);
			freeDoubleFloatPointer(fap2Scale[k1],nTransforms);
			fap2Position[k1] = 0;
			fap2Orientation[k1] = 0;
			fap2Scale[k1] = 0;
		}
		cnt++;
	}
	
	for(k1 = 2; k1 < nFaps; k1++)
	{
		fapValue0[k1] = fv[cnt];
		transfChanged = 0;
		coordChanged = 0;
		if(fv[cnt] != -1)
		{
			coordChanged = readFapCoords(fapCoord[k1],models[cnt]);
			transfChanged = readFapTransform(fapPosition[k1], fapOrientation[k1], fapScale[k1],models[cnt]);
		}
		if(!coordChanged)
		{
			freeDoubleFloatPointer(fapCoord[k1],nSurfaces);
			fapCoord[k1] = 0;
		}
		if(!transfChanged)
		{
			freeDoubleFloatPointer(fapPosition[k1],nTransforms);
			freeDoubleFloatPointer(fapOrientation[k1],nTransforms);
			freeDoubleFloatPointer(fapScale[k1],nTransforms);
			fapPosition[k1] = 0;
			fapOrientation[k1] = 0;
			fapScale[k1] = 0;
		}
		cnt++;
	}

	setModifiers();
	deformers[0] = new BaseAFMDeformer(model0,FAPModifiers, ExpressionModifiers, VisemeModifiers, BAPModifiers);
	(*nDeformers) = 1;
}

int BaseFileIO::readFapCoords(float **af, VRMLModel *m)
{
	int i,j;
	int diff = 0;

	if(!m)
		return(0);

	if(!currentModel->initialized)
		return(0);

	for(i = 0;i<nSurfaces;i++)
	{
		af[i] = (float*)malloc(coordLength[i] * sizeof(float));
		for(j=0;j<coordLength[i];j++)
		{
			af[i][j] = m->coordList[i][j] - currentModel->coordList[i][j];
			if(testDiff(af[i][j],m->coordList[i][j],currentModel->coordList[i][j]))
				diff = 1;
		}
	}
	return(diff);
}

int BaseFileIO::readFapTransform(float **af, float **af1, float **af2,VRMLModel *m)
{
	int i,j;
	int diff = 0;


	if(!m)
		return(0);

	if(!currentModel->initialized)
		return(0);

	for(i = 0;i<nTransforms;i++)
	{
		int k = 0;
		for(j=0;j<3;j++)
		{
			af[i][j] = m->transfList[i][k] - currentModel->transfList[i][k];
			if(testDiff(af[i][j],m->transfList[i][k],currentModel->transfList[i][k]))
				diff = 1;
			k++;
		}
		int diffr = 0;
		for(j=0;j<4;j++)
		{
			af1[i][j] = m->transfList[i][k] - currentModel->transfList[i][k];
			if(testDiff(af1[i][j],m->transfList[i][k],currentModel->transfList[i][k]))
				diffr = 1;
			k++;
		}
		if(diffr)
		{
			diff = 1;
			arrangeQuat(af1[i],&currentModel->transfList[i][3]);
		}

		for(j=0;j<3;j++)
		{
			af2[i][j] = m->transfList[i][k] - currentModel->transfList[i][k];
			if(testDiff(af2[i][j],m->transfList[i][k],currentModel->transfList[i][k]))
				diff = 1;
			k++;
		}
	}
	return(diff);
}

// modify q so that q == newq * q0
// newq =  q * inv(q0)
// i.e. new quaternion is the difference between old one and q0
void BaseFileIO::quatDiff(float *q, float *q0)
{
	float q1[4];
	float q2[4];
	float angle1, axis1[4];
	
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i = 0;i<4;i++)
	{
		if(q[i] != 0.0f)
			break;
	}
	if(i==4)
		return;

	for(i = 0;i<4;i++)
	{
		q1[i] = q[i];
		q2[i] = -q[i];
	}

	q0[0] = -q0[0];
	q0[1] = -q0[1];
	q0[2] = -q0[2];
	
	
	multQuat(q1,q1,q0);
	angle1 = quat2aa(q1,axis1);
	
	if(angle1 > 3.141592f)
	{
		angle1 -= 6.2831853f;
		aa2quat(q,axis1,angle1);
	}
	else if (angle1 < -3.141592f)
	{
		angle1 += 6.2831853f;
		aa2quat(q,axis1,angle1);
	}
	else
	{
		for(int i = 0;i<4;i++)
		{
			q[i] = q1[i];
		}
	}
	/*
	multQuat(q2,q2,q0);
	angle2 = quat2aa(q2,axis2);

	
	// choose the shorter interpolation path
	if(angle1 < angle2) 
	{
		for(int i = 0;i<4;i++)
		{
			q[i] = q1[i];
		}
	}
	else
	{
		for(int i = 0;i<4;i++)
		{
			q[i] = q2[i];
		}
	}
	*/
}


void BaseFileIO::arrangeQuat(float *q, float *q0)
{
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i = 0;i<4;i++)
		if(q[i] != 0.0f)
			break;
		if(i==4)
			return;
		
		q[0] += q0[0];
		q[1] += q0[1];
		q[2] += q0[2];
		q[3] += q0[3];
	
		quatDiff(q,q0);		
}


int BaseFileIO::testDiff(float d,float f1,float f2)
{
	float f0;
	
	if(f1 != 0.0f)
	{
		f0 = f1;
	}
	else
		if(f2 != 0.0f)
		{
			f0 = f2;
		}
		else
			return(0);

	if( (myfabs(d)*100.0) / myfabs(f0) > coordSensitivity)
		return(1);
	else
		return(0);
}

// Added by KStankovic
void BaseFileIO::readCubemap(Node *node)
{
	FieldValue *field = node->getField(0);
	SFString *ifs = (SFString *) field;

	int index = currentModel->findNamedMesh((char *) ifs->getValue());
	VRMLNode *mesh = currentModel->meshNode[index];

	VRMLMaterial *material = currentModel->materials[mesh->matInd];
	int ind = mesh->ind;

	if (material->textureName[0]) strcpy(material->textureName, "");
	
	int i, j;
	field = node->getField(1);
	MFString *urls = (MFString *) field;

	for (i = 0; i < 6; i++)
	{
		if (material->cubemapNames[i] != NULL)
			delete [] material->cubemapNames[i];
		String url = urls->getValue(i);
		material->cubemapNames[i] = new char[url.length() + 1];
		strcpy(material->cubemapNames[i], url);
	}

	field = node->getField(2);
	MFVec3f *texCoords = (MFVec3f *) field;
	int size = texCoords->getSize();

	currentModel->tCoordListSize[ind] = size;
	if (currentModel->tCoordList[ind] != NULL)
		free(currentModel->tCoordList[ind]);
	currentModel->tCoordList[ind] = (float *) malloc(size * sizeof(float));
	for(i = 0; i < (size / 3); i++)
	{
		for(j = 0; j < 3; j++)
			currentModel->tCoordList[ind][i * 3 + j] = (texCoords->getValue(i))[j];
	}
}
//

}
