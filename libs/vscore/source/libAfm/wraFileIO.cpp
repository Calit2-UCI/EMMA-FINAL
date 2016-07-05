// wraFileIO.cpp: implementation of the wraFileIO class.
//
//////////////////////////////////////////////////////////////////////

#include "wraFileIO.h"
#include "BaseAFMDeformer.h"

#include "VRMLModel.h"
#include "VRMLWriteTraverser.h"


#include <stdlib.h>
#include <stdio.h>
#include <float.h>

#include "Scene.h"
#include "Node.h"
#include "Field.h"
#include "SFFloat.h"
#include "SFColor.h"
#include "MFColor.h"
#include "MFFloat.h"
#include "MFVec3f.h"
#include "MFVec2f.h"
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

#include "myMatrix.h"

#define TT_READ_FILE 0
#define TT_READ_INTERPOLATORS 0
#define TT_INIT 1
#define TT_UPDATE_GEOM 2
#define TT_ASSIGN_BONE_WEIGHTS 3

#define NB_SD 88


namespace VisageSDK
{

int *wrafio_citKey;
int wrafio_nCitKey;
float *wrafio_citValue;
int wrafio_nCitValue;
int *wrafio_cigKey;
int wrafio_nCigKey;
float *wrafio_cigValue;
int wrafio_nCigValue;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

wraFileIO::wraFileIO()
{
	// Added by KStankovic
	BAPModifiers = NULL;
	body = NULL;
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
	scale = NULL;
	VisemeModifiers = NULL;
	//
}

wraFileIO::~wraFileIO()
{
	// Added by KStankovic
	if (body != NULL) delete body;
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
int wraFileIO::read(char *fileName, VRMLModel *model)
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
	if(!strcmp(ext,"wra"))
		typeOK = true;
	if(!strcmp(ext,"WRA"))
		typeOK = true;

	if(!typeOK)
		return(-1);

	FILE *f;
	if(!(f = fopen(fileName,"r")))
	{
		char msg[300];
		sprintf(msg,"Error 163\nCould not open file: >%s< (%s)\n",fileName,strerror(NULL) );
//		MessageBox(0,msg,"Error",MB_ICONERROR);
		fprintf(stderr,"%s\n",msg);
		return(0);
	}
	
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
	parseNode(root,TT_READ_FILE,-1,0,toWC1,currentModel->rootNode);
	
	fclose(f);
	
	currentModel->finalizeFileReading(fileName);

	return(1);
}

int wraFileIO::write(char *fileName, VRMLModel *model)
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
	if(!strcmp(ext,"wra"))
		typeOK = true;
	if(!strcmp(ext,"WRA"))
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
	fclose(f);
	
	return(1);
}

void wraFileIO::parseIndexedFaceSet(Node *node)
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
	for(i=0;i<nc/3;i++)
		for(j=0;j<3;j++)
			currentModel->coordList[currentModel->nCoordLists][i*3+j] = (coords->getValue(i))[j];
		
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
		
		
		// get the texture coords, if any
		if(currentMaterial)
		{
			if(currentMaterial->textureName[0])
			{
				FieldValue *_value2; 
				_value2 = _nodeIndexedFaceSet->getField(3);//FieldValue(SFNode):'texCoord' field	
				Node *tcoord = ((SFNode *) _value2)->getValue();//'texCoord' Node
				tcoords = (MFVec2f *) tcoord->getField(0);
				tind = (MFInt32 *) _nodeIndexedFaceSet->getField(13);
				ntc = tcoords->getSize();
				currentModel->tCoordListSize[currentModel->nCoordLists] =ntc;
				currentModel->tCoordList[currentModel->nCoordLists] = (float*)malloc(ntc*sizeof(float));
				for(i =0;i<ntc/2;i++)
				{
					for(j=0;j<2;j++)
						currentModel->tCoordList[currentModel->nCoordLists][i*2+j] = tcoords->getValue(i*2 + j);
				}
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
		}
		
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

void wraFileIO::parseTransform(Node *node)
{
	int nc = 17;
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

	currentModel->transfList[currentModel->nTransfLists] = out;
}


void wraFileIO::parseMaterial(Node *_currentRootNode,VRMLMaterial *m)
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

void wraFileIO::parseNode(Node *_currentRootNode, int traverseType, int TI, int TL, myMatrix toWC1,VRMLNode * parentNode)
{
    NodeList *_allRootNodes;
    FieldValue *_value; 
	char tmp[200];
	char tmp1[200];
	VRMLNode *myNode = 0;
	char defName[100];
	
	myMatrix localToWC;
	copyMat(localToWC,toWC1);
	
	strcpy(tmp,_currentRootNode->getProto()->getName());
	strcpy(defName,_currentRootNode->getName());
	if(!strcmp(tmp,"Shape") || !strcmp(tmp,"Transform")) // these nodes must get a name
	{
		sprintf(tmp1,"Node%s",tmp);
		if(strlen(defName) == 0 || !strcmp(tmp1,defName)) // detect empty and non-unique names
			sprintf(defName,"%s%d",tmp,(int)_currentRootNode);
	}	
	
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
	else
		if(!strcmp(tmp,"Joint"))
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
				char jointName[200];
				strcpy(jointName,_currentRootNode->getName());
				body->addJoint(jointName,currentModel->nTransfLists);  // body initialization
				currentModel->nTransfLists++;
				break;
			};
			
			
			// process children
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
		else
			if(!strcmp(tmp,"Humanoid"))
			{
			/*
			switch(traverseType)
			{
			case TT_READ_FILE:
			myNode = new VRMLNode(NT_GROUP,0,0,_currentRootNode);
			break;
			};
				*/
				switch(traverseType)
				{
				case TT_READ_FILE:
					parseTransform(_currentRootNode);
					updateToWCMatrix(localToWC);
					copyMat(currentModel->toWC[currentModel->nTransfLists],localToWC);
					currentModel->transfLevel[currentModel->nTransfLists] = TL;
					TI = currentModel->nTransfLists;
					myNode = new VRMLNode(NT_TRANSFORM,currentModel->nTransfLists,0,NULL,defName,parentNode);
					char humName[200];
					strcpy(humName,_currentRootNode->getName());
					body = new hAnimBody(humName); // body initialization
					currentModel->nTransfLists++;
					break;
				};
				
				for(int i=0; i<_currentRootNode->getProto()->getNumFields();i++)
				{
					Field *field = _currentRootNode->getProto()->getField(i);
					if(!strcmp(field->getName(),"joints")) continue; // otherwise the joint list would be copied as double geometry
					if(!strcmp(field->getName(),"segments")) continue; // otherwise the joint list would be copied as double geometry
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
			else
				if(!strcmp(tmp,"IndexedFaceSet"))
				{
					switch(traverseType)
					{
					case TT_READ_FILE:
						parentNode->ind = currentModel->nCoordLists;
						parseIndexedFaceSet(_currentRootNode);
						currentModel->transfInd[currentModel->nCoordLists] = TI;
						currentModel->nCoordLists++;
						break;
					};
				}
				else	
					if(!strcmp(tmp,"Material"))
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
					else
						if(!strcmp(tmp,"ImageTexture"))
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
						else
							if(!strcmp(tmp,"Group") || !strcmp(tmp,"Segment"))
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
							else
								if(!strcmp(tmp,"Shape"))
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
								else
									if(!strcmp(tmp,"Appearance"))
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
										if(!strcmp(tmp,"CoordinateInterpolator"))
										{
											//											char defName[100];
											//											strcpy(defName,_currentRootNode->getName());
											if(!strncmp(defName,"CIT",3))
												skip = 1;
											if(!strncmp(defName,"CIG",3))
												skip = 1;
										}
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


void wraFileIO::updateToWCMatrix(myMatrix toWC)
{
	myMatrix t;
	identityMat(t);
	
	currentModel->getTransfMatrix(t,currentModel->transfList[currentModel->nTransfLists]);
	multMat(toWC,t,toWC);
}


// convert rotation from quaternion to axis and angle
void wraFileIO::setRotation(float *r)
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
void wraFileIO::setModifiers()
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


	if(body)
	{
		for(i=0;i<NBAP;i++)
		{
			BAPModifiers[i].MTData = 0;
			BAPModifiers[i].TTranData = 0;
			BAPModifiers[i].TRotData = 0;
			BAPModifiers[i].TScalData = 0;

			body->setBAPModifier(i,&BAPModifiers[i],currentModel);

		}
	}

}

// compute bone weight such that it decreases from 1.0 at the center of rotation to 0.0 at the distance from center given by range. v is the vertex, center is the center of rotation, both expressed in the same coordinate system
float wraFileIO::computeBoneWeight(float *v,float *center, float range)
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

/**
* Automatically assign bone weights for a joint.
* A joint is defined by the child transform and the parent transform (given by indices).
* The polygon meshes attached to these two transforms are influenced by the joint.
* For example, the shoulder joint is defined by the shoulder transform (child), whose
* parent is the torso transform, The upper arm polygon mesh is attached to the
* shoulder transform, and the torso mesh is attached to the torso transform.
* In skinning, the movement of the shoulder transform affects the vertices in both the
* upper arm mesh and the torso mesh.
* 
* The torso mesh (parent mesh) has the torso transform as the native bone, and the shoulder transform as
* the additional bone. The weights of the shoulder bone on the torso mesh vertices are 
* high (close to 1) near the center of rotation of the shoulder transform, and rapidly drop
* off to 0 when we move away from the center.
*
* Similarly, the upper arm mesh (child mesh) hase the shoulder transform as the native bone, and the 
* torso transform as the additional bone. The weights of the torso bone are high in the vicinity of the centre of rotation
* of the shoulder transform, and drop off with the distance from it.
*
* In this way the affected region (vicinity of the child transform center of rotation) is deformed smoothly.
* 
*/
void wraFileIO::addBones(int childTransform, int parentTransform)
{
	float * centerOfRotation; // center of rotation of the child transform
	myMatrix childToWCMat; // child transform world-to-local-coordinates transformation matrix 	

	bwt_boneTransformIndex = childTransform;
	bwt_boneRefTransformIndex = childTransform;
	bwt_boneParentTransformIndex = parentTransform;
	bwt_traversingUnderBone= false;
	bwt_range = 15.0f;
	centerOfRotation = &currentModel->transfList[childTransform][14];
	copyMat(childToWCMat,currentModel->toWC[childTransform]); 
	multVect3Mat(bwt_boneCenter,centerOfRotation,childToWCMat);

	traversalType = TT_ASSIGN_BONE_WEIGHTS;
	currentModel->traverse(currentModel->rootNode,this);
}

int wraFileIO::read(char *fileName, VRMLModel *model, AFMDeformer **deformers, int *nDeformers)
{
	// basic validity checks
	if(!fileName)
		return(0);
	if(!model)
		return(0);

	body = 0;

	// check file type (extension)
	char * ext = fileName + strlen(fileName) - 3;
	bool typeOK = false;

	if(!strcmp(ext,"wrl"))
		typeOK = true;
	if(!strcmp(ext,"WRL"))
		typeOK = true;
	if(!strcmp(ext,"wra"))
		typeOK = true;
	if(!strcmp(ext,"WRA"))
		typeOK = true;

	if(!typeOK)
		return(-1);

	read(fileName,model);
	currentModel = model;

	initAnimData();

	setModifiers();
	deformers[0] = new BaseAFMDeformer(model,FAPModifiers, ExpressionModifiers, VisemeModifiers, BAPModifiers);
	(*nDeformers) = 1;
/*
	int j,m;
//	j = body->getJointTransfIndex("hanim_r_shoulder");
//	m = body->getJointTransfIndex("hanim_vl1");
//	addBones(j,m);

	// find the neck
	VRMLModelModifier *neckModifier = &FAPModifiers[head_yaw];
	for(j=0;j<currentModel->nTransfLists;j++)
		if(neckModifier->TRotData[j]) break; // the rotation modifier is non-zero for the neck joint, therefore j now contains the index to the neck joint

	int i;
	do
	{ // transform j controls the neck, but there may not be the native bone for the neck geometry; we may need to descend a bit
		for(i = 0;i<currentModel->nCoordLists;i++)
		{
			if(currentModel->transfInd[i]==j) break;
		}
		if(i==currentModel->nCoordLists)
		{
			VRMLNode *child = currentModel->transfNode[j]->children[0];
			if(child)
				j=child->ind;
			else
				j=-1;
		}
	}
	while(i==currentModel->nCoordLists && j!=-1);
	
	m = currentModel->transfNode[j]->parent->ind;
*/

	//this is specific for the king model, just for testing

//	addBones(j,m);
//	addBones(20,11);
	return(1);
}

int wraFileIO::write(char *fileName, VRMLModel *model, AFMDeformer **deformers, int nDeformers)
{
	// basic validity checks
	if(!fileName)
		return(0);
	if(!model)
		return(0);

	// check file type (extension)
	char * ext = fileName + strlen(fileName) - 3;
	bool typeOK = false;

	if(!strcmp(ext,"wra"))
		typeOK = true;
	if(!strcmp(ext,"WRA"))
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
	
//	outputAnimData(f);
//	outputTransfData(f);

	for(int i =0;i < nDeformers;i++)
		deformers[i]->write(f,"wra");

	fclose(f);
	return(1);
}


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
void wraFileIO::allocAnimData()
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

void wraFileIO::initAnimData()
{
	allocAnimData();

	traversalType = TT_INIT;
	currentModel->traverse(currentModel->rootNode,this);
	
	char iid[5];
	for(int k1 = 0; k1 < nVisemes; k1++)
	{
		iid[0]=0;
		sprintf(iid,"1-%d",k1);
		surfCnt = 0;
		transfCnt = 0;
		wrafio_nCitKey = wrafio_nCigKey = 0;
		traverse(root, TT_READ_INTERPOLATORS,iid);
		int i = readFapCoords(fap1Coord[k1]);
		int l = readFapTransform(fap1Position[k1], fap1Orientation[k1], fap1Scale[k1]);
		fap1Value0[k1] = -1;
		if(i != -1)
			fap1Value0[k1] = 63;
		if(l != -1)
			fap1Value0[k1] = 63;
		if(i == -1)
		{
			freeDoubleFloatPointer(fap1Coord[k1],nSurfaces);
			fap1Coord[k1] = 0;
		}
		if(l == -1)
		{
			freeDoubleFloatPointer(fap1Position[k1],nTransforms);
			freeDoubleFloatPointer(fap1Orientation[k1],nTransforms);
			freeDoubleFloatPointer(fap1Scale[k1],nTransforms);
			fap1Position[k1] = 0;
			fap1Orientation[k1] = 0;
			fap1Scale[k1] = 0;
		}
	}
	
	for(int l1 = 0; l1 < nExp; l1++)
	{
		iid[0]=0;
		sprintf(iid,"2-%d",l1);
		surfCnt = 0;
		transfCnt = 0;
		wrafio_nCitKey = wrafio_nCigKey = 0;
		traverse(root, TT_READ_INTERPOLATORS,iid);
		int j = readFapCoords(fap2Coord[l1]);
		int i1 = readFapTransform(fap2Position[l1], fap2Orientation[l1], fap2Scale[l1]);
		fap2Value0[l1] = -1;
		if(j != -1)
			fap2Value0[l1] = 63;
		if(i1 != -1)
			fap2Value0[l1] = 63;
		if(j == -1)
		{
			freeDoubleFloatPointer(fap2Coord[l1],nSurfaces);
			fap2Coord[l1] = 0;
		}
		if(i1 == -1)
		{
			freeDoubleFloatPointer(fap2Position[l1],nTransforms);
			freeDoubleFloatPointer(fap2Orientation[l1],nTransforms);
			freeDoubleFloatPointer(fap2Scale[l1],nTransforms);
			fap2Position[l1] = 0;
			fap2Orientation[l1] = 0;
			fap2Scale[l1] = 0;
		}
	}
	
	for(int i2 = 2; i2 < nFaps; i2++)
	{
		iid[0]=0;
		sprintf(iid,"%d",i2 + 1);
		surfCnt = 0;
		transfCnt = 0;
		wrafio_nCitKey = wrafio_nCigKey = 0;
		traverse(root, TT_READ_INTERPOLATORS,iid);
		int k = readFapCoords(fapCoord[i2]);
		int j1 = readFapTransform(fapPosition[i2], fapOrientation[i2], fapScale[i2]);
		fapValue0[i2] = -1;
		if(k != -1)
			fapValue0[i2] = k;
		if(j1 != -1)
			fapValue0[i2] = j1;
		if(k == -1)
		{
			freeDoubleFloatPointer(fapCoord[i2],nSurfaces);
			fapCoord[i2] = 0;
		}
		if(j1 == -1)
		{
			freeDoubleFloatPointer(fapPosition[i2],nTransforms);
			freeDoubleFloatPointer(fapOrientation[i2],nTransforms);
			freeDoubleFloatPointer(fapScale[i2],nTransforms);
			fapPosition[i2] = 0;
			fapOrientation[i2] = 0;
			fapScale[i2] = 0;
		}
	}

	// Body animation initialization
//	body = 0;
//	traverseInitBodyAnim(currentModel->rootNode);
	if(body)
		body->initBAPJoints();
}

// traverser functions
int wraFileIO::traverseTransform(VRMLModel *model,    // pointer to the traversed model
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
	case TT_ASSIGN_BONE_WEIGHTS:
		if(index == bwt_boneTransformIndex)
		{
			bwt_traversingUnderBone = true;
			bwt_boneRefTransformIndex = bwt_boneParentTransformIndex;
		}
		break;
	default:
		break;
	}
	return(0);
}

void wraFileIO::traverseTransformEnd(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
							 int index)
{
	int i = 0;

	switch(traversalType)
	{
	case TT_ASSIGN_BONE_WEIGHTS:
		if(index == bwt_boneTransformIndex)
		{
			bwt_traversingUnderBone = false;
			bwt_boneRefTransformIndex = bwt_boneTransformIndex;
		}
		break;
	default:
		break;
	}
}

void wraFileIO::traverseMesh(VRMLModel *model,    // pointer to the traversed model
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
	int i,j;
	float *v;
	float vw[3];
	float w;
	myMatrix toWC,fromWCtoBoneRef;
	float boneInitialized = false;
	int nBones;

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
	case TT_ASSIGN_BONE_WEIGHTS:
		for(i = 0; i < coordLength/3; i++)
		{
			v = &coord[i*3];
			copyMat(toWC,model->toWC[model->transfInd[index]]); 
			multVect3Mat(vw,v,toWC);
			w = computeBoneWeight(vw,bwt_boneCenter, bwt_range);
			if(w==0.0f) continue;
			
			if(!boneInitialized)
			{
				boneInitialized = true;
				copyMat(fromWCtoBoneRef,model->fromWC[bwt_boneRefTransformIndex]); 
				nBones = bones->nBones;
				bones->nBones++;
				bones->changed = 1;
				bones->vertexWeight[nBones] = (float*)malloc((coordLength/3)*sizeof(float));
				bones->bindCoords[nBones] = (float*)malloc(coordLength*sizeof(float));
				for(j=0;j<coordLength/3;j++)
					bones->vertexWeight[nBones][j] = 0.0f;
				for(j=0;j<coordLength/3;j++)
				{
					// compute the binding coordinates
					float *f = &origCoord[j*3];
					float fw[3];
					multVect3Mat(fw,f,toWC); // take the original local coordinates and multiply them by the toWC matrix to get the global coordinates
					multVect3Mat(&bones->bindCoords[nBones][j*3],fw,fromWCtoBoneRef); // multiply the global coords by the fromWC matrix of the bone; the result is the local coordinates in the bone's coordinate system
				}
				bones->boneIndex[nBones] = bwt_boneRefTransformIndex;
			}
			if(bwt_traversingUnderBone)
				bones->vertexWeight[nBones][i] = 1.0f - w;
			else
				bones->vertexWeight[nBones][i] = w;
		}
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




void wraFileIO::traverse(Node *_currentRootNode, int traverseType,char *iid)
{
    NodeList *_allRootNodes;
    FieldValue *_value; 
	char tmp[200];
	
	strcpy(tmp,_currentRootNode->getProto()->getName());
	
	if(!strcmp(tmp,"CoordinateInterpolator"))
	{
		switch(traverseType)
		{
		case TT_READ_INTERPOLATORS:
			char defName[100];
			strcpy(defName,_currentRootNode->getName());
			if(!strcmp(defName+3,iid))
			{
				switch(defName[2])
				{
				case 'T':
					readInterpolator(_currentRootNode,&wrafio_citKey,&wrafio_nCitKey,&wrafio_citValue,&wrafio_nCitValue);
					break;
				case 'G':
					readInterpolator(_currentRootNode,&wrafio_cigKey,&wrafio_nCigKey,&wrafio_cigValue,&wrafio_nCigValue);
					break;
				};
			}
			break;
		};
	}
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

void wraFileIO::initTripleFloatPointer(float ****p,int l1,int l2)
{
	(*p) = (float***)malloc(l1 * sizeof(float**));
	for(int i = 0;i<l1;i++)
	{
		(*p)[i] = (float**)malloc(l2*sizeof(float*));
		for(int j=0;j<l2;j++)
			(*p)[i][j] = 0;
	}
}

void wraFileIO::initTripleFloatPointer(float ****p,int l1,int l2,int l3)
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

void wraFileIO::initDoubleFloatPointer(float ***p,int l1)
{
	(*p) = (float**)malloc(l1 * sizeof(float*));
	for(int i = 0;i<l1;i++)
		(*p)[i] = 0;
}

void wraFileIO::initDoubleFloatPointer(float ***p,int l1,int l2)
{
	(*p) = (float**)malloc(l1 * sizeof(float*));
	for(int i = 0;i<l1;i++)
	{
		(*p)[i] = (float*)malloc(l2*sizeof(float));
		for(int j=0;j<l2;j++)
			(*p)[i][j] = 0.0;
	}
}

void wraFileIO::freeTripleFloatPointer(float ***p,int l1,int l2)
{
	if(p)
	{
		for(int i = 0;i<l1;i++)
			freeDoubleFloatPointer(p[i],l2);
	}
	free(p);
}

void wraFileIO::freeDoubleFloatPointer(float **p,int l1)
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

int wraFileIO::readFapCoords(float **af)
{
	if(wrafio_nCigKey == 0)
		return(-1);

	int *ai = (int*)malloc(wrafio_nCigKey * sizeof(int));
	for(int j = 0; j < wrafio_nCigKey; j++)
		ai[j] = wrafio_cigKey[j];
	
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
	for(int l1 = 2; l1 < wrafio_nCigKey; l1++)
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
					af[i1][j1++] = wrafio_cigValue[k1++];
				
			}
			
		}
	}
	free(ai);
	return l;
}

int wraFileIO::readFapTransform(float **af, float **af1, float **af2)
{
	if(wrafio_nCitKey == 0)
		return(-1);

	int *ai = (int*)malloc(wrafio_nCitKey * sizeof(int));
	for(int j = 0; j < wrafio_nCitKey; j++)
		ai[j] = wrafio_citKey[j];
	
	int k = 0;
	for(int l = 1; l < wrafio_nCitKey; l++)
	{
		ai[l]--;
		for(int i1 = 0; i1 < 3; i1++)
			af[ai[l]][i1] = wrafio_citValue[k++];
		
		for(int j1 = 0; j1 < 4; j1++)
			af1[ai[l]][j1] = wrafio_citValue[k++];

		
		for(int k1 = 0; k1 < 3; k1++)
			af2[ai[l]][k1] = wrafio_citValue[k++];
		k+=8;
		
	}
	int df = ai[0];
	free(ai);
	return (df);
}

void wraFileIO::readInterpolator(Node *node,int **key,int *nKey,float **value,int *nValue)
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
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i=0;i<150;i++)
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
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i = 0;i<4;i++)
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
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i = 0;i<4;i++)
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



// helper function for AFM; produces all modifier data from a set of VRMLModels which are considered as morph targets
void wraFileIO::initAnimData(VRMLModel *model0,VRMLModel **models, int *fv,AFMDeformer **deformers, int *nDeformers, float _coordSensitivity)
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

int wraFileIO::readFapCoords(float **af, VRMLModel *m)
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
			af[i][j] = currentModel->coordList[i][j] - currentModel->coordList[i][j];
			if(testDiff(af[i][j],currentModel->coordList[i][j],currentModel->coordList[i][j]))
				diff = 1;
		}
	}
	return(diff);
}

int wraFileIO::readFapTransform(float **af, float **af1, float **af2,VRMLModel *m)
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
			af[i][j] = currentModel->transfList[i][k] - currentModel->transfList[i][k];
			if(testDiff(af[i][j],currentModel->transfList[i][k],currentModel->transfList[i][k]))
				diff = 1;
			k++;
		}
		int diffr = 0;
		for(j=0;j<4;j++)
		{
			af1[i][j] = currentModel->transfList[i][k] - currentModel->transfList[i][k];
			if(testDiff(af1[i][j],currentModel->transfList[i][k],currentModel->transfList[i][k]))
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
			af2[i][j] = currentModel->transfList[i][k] - currentModel->transfList[i][k];
			if(testDiff(af2[i][j],currentModel->transfList[i][k],currentModel->transfList[i][k]))
				diff = 1;
			k++;
		}
	}
	return(diff);
}

// modify q so that q == newq * q0
// newq =  q * inv(q0)
// i.e. new quaternion is the difference between old one and q0
void wraFileIO::quatDiff(float *q, float *q0)
{
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i = 0;i<4;i++)
		if(q[i] != 0.0f)
			break;
		if(i==4)
			return;
		q0[0] = -q0[0];
		q0[1] = -q0[1];
		q0[2] = -q0[2];
		multQuat(q,q,q0);
}


void wraFileIO::arrangeQuat(float *q, float *q0)
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


int wraFileIO::testDiff(float d,float f1,float f2)
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

}
