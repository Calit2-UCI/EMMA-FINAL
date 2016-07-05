// BaseAFMDeformer.cpp: implementation of the BaseAFMDeformer class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>  //necessary for Sleep use in windows
#else
#include <unistd.h>
#endif

#include "BaseAFMDeformer.h"
#include "FBAPs.h"
#include "myMatrix.h"

#define TT_INIT 1
#define TT_UPDATE_GEOM 2


namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BaseAFMDeformer::BaseAFMDeformer(VRMLModel *model,VRMLModelModifier *FAPModifiers, VRMLModelModifier *ExpressionModifiers, VRMLModelModifier *VisemeModifiers, VRMLModelModifier *BAPModifiers)
{
	int i;
	
	this->FAPModifiers=FAPModifiers;
	this->ExpressionModifiers=ExpressionModifiers;
	this->VisemeModifiers=VisemeModifiers;
	this->BAPModifiers=BAPModifiers;
	currentModel = model;
	
	allocAnimData();

	// set the filter
	filter->faps->groupMask[0] = 0;
	for(i = 0;i < nVisemes;i++)
	{
		if(VisemeModifiers[i].refValue != -1)
			filter->faps->groupMask[0] = 1;
	}
	
	filter->faps->groupMask[1] = 0;
	for(i = 0;i < nExp;i++)
	{
		if(ExpressionModifiers[i].refValue != -1)
			filter->faps->groupMask[1] = 1;
	}
	
	for(i = 2;i < NFAP;i++)
	{
		if(FAPModifiers[i].refValue == -1)
			filter->faps->groupMask[i] = 0;
	}
	for(i = 0;i < NUM_BAPS;i++)
	{
		if(BAPModifiers[i].refValue == -1)
			filter->baps->groupMask[i] = 0;
	}
	
	
	traversalType = TT_INIT;
	currentModel->traverse(currentModel->rootNode,this); // this initializes coord, origCoord, position, origPosition etc.

	// Added by KStankovic
	faceNeutral = 1;
	//
}

BaseAFMDeformer::~BaseAFMDeformer()
{
	// Added by KStankovic
	if (modifierWeights != NULL) free(modifierWeights);
	if (modifiers != NULL) free(modifiers);

	int i, j;

	if (BAPModifiers != NULL)
	{
		for (i = 0; i < NUM_BAPS; i++)
		{
			if (BAPModifiers[i].MTData != NULL)
			{
				for (j = 0; j < currentModel->nCoordLists; j++)
				{
					if (BAPModifiers[i].MTData[j] != NULL) free(BAPModifiers[i].MTData[j]);
				}
				free(BAPModifiers[i].MTData);
			}

			if (BAPModifiers[i].TRotData != NULL)
			{
				for (j = 0; j < currentModel->nTransfLists; j++)
				{
					if (BAPModifiers[i].TRotData[j] != NULL) free(BAPModifiers[i].TRotData[j]);
				}
				free(BAPModifiers[i].TRotData);
			}

			if (BAPModifiers[i].TScalData != NULL)
			{
				for (j = 0; j < currentModel->nTransfLists; j++)
				{
					if (BAPModifiers[i].TScalData[j] != NULL) free(BAPModifiers[i].TScalData[j]);
				}
				free(BAPModifiers[i].TScalData);
			}

			if (BAPModifiers[i].TTranData != NULL)
			{
				for (j = 0; j < currentModel->nTransfLists; j++)
				{
					if (BAPModifiers[i].TTranData[j] != NULL) free(BAPModifiers[i].TTranData[j]);
				}
				free(BAPModifiers[i].TTranData);
			}
		}

		delete [] BAPModifiers;
	}

	if (ExpressionModifiers != NULL)
	{
		for (i = 0; i < 7; i++)
		{
			if (ExpressionModifiers[i].MTData != NULL)
			{
				for (j = 0; j < currentModel->nCoordLists; j++)
				{
					if (ExpressionModifiers[i].MTData[j] != NULL) free(ExpressionModifiers[i].MTData[j]);
				}
				free(ExpressionModifiers[i].MTData);
			}

			if (ExpressionModifiers[i].TRotData != NULL)
			{
				for (j = 0; j < currentModel->nTransfLists; j++)
				{
					if (ExpressionModifiers[i].TRotData[j] != NULL) free(ExpressionModifiers[i].TRotData[j]);
				}
				free(ExpressionModifiers[i].TRotData);
			}

			if (ExpressionModifiers[i].TScalData != NULL)
			{
				for (j = 0; j < currentModel->nTransfLists; j++)
				{
					if (ExpressionModifiers[i].TScalData[j] != NULL) free(ExpressionModifiers[i].TScalData[j]);
				}
				free(ExpressionModifiers[i].TScalData);
			}

			if (ExpressionModifiers[i].TTranData != NULL)
			{
				for (j = 0; j < currentModel->nTransfLists; j++)
				{
					if (ExpressionModifiers[i].TTranData[j] != NULL) free(ExpressionModifiers[i].TTranData[j]);
				}
				free(ExpressionModifiers[i].TTranData);
			}
		}

		delete [] ExpressionModifiers;
	}

	if (FAPModifiers != NULL)
	{
		for (i = 0; i < NFAP; i++)
		{
			if (FAPModifiers[i].MTData != NULL)
			{
				for (j = 0; j < currentModel->nCoordLists; j++)
				{
					if (FAPModifiers[i].MTData[j] != NULL) free(FAPModifiers[i].MTData[j]);
				}
				free(FAPModifiers[i].MTData);
			}

			if (FAPModifiers[i].TRotData != NULL)
			{
				for (j = 0; j < currentModel->nTransfLists; j++)
				{
					if (FAPModifiers[i].TRotData[j] != NULL) free(FAPModifiers[i].TRotData[j]);
				}
				free(FAPModifiers[i].TRotData);
			}

			if (FAPModifiers[i].TScalData != NULL)
			{
				for (j = 0; j < currentModel->nTransfLists; j++)
				{
					if (FAPModifiers[i].TScalData[j] != NULL) free(FAPModifiers[i].TScalData[j]);
				}
				free(FAPModifiers[i].TScalData);
			}

			if (FAPModifiers[i].TTranData != NULL)
			{
				for (j = 0; j < currentModel->nTransfLists; j++)
				{
					if (FAPModifiers[i].TTranData[j] != NULL) free(FAPModifiers[i].TTranData[j]);
				}
				free(FAPModifiers[i].TTranData);
			}
		}

		delete [] FAPModifiers;
	}

	if (VisemeModifiers != NULL)
	{
		for (i = 0; i < 15; i++)
		{
			if (VisemeModifiers[i].MTData != NULL)
			{
				for (j = 0; j < currentModel->nCoordLists; j++)
				{
					if (VisemeModifiers[i].MTData[j] != NULL) free(VisemeModifiers[i].MTData[j]);
				}
				free(VisemeModifiers[i].MTData);
			}

			if (VisemeModifiers[i].TRotData != NULL)
			{
				for (j = 0; j < currentModel->nTransfLists; j++)
				{
					if (VisemeModifiers[i].TRotData[j] != NULL) free(VisemeModifiers[i].TRotData[j]);
				}
				free(VisemeModifiers[i].TRotData);
			}

			if (VisemeModifiers[i].TScalData != NULL)
			{
				for (j = 0; j < currentModel->nTransfLists; j++)
				{
					if (VisemeModifiers[i].TScalData[j] != NULL) free(VisemeModifiers[i].TScalData[j]);
				}
				free(VisemeModifiers[i].TScalData);
			}

			if (VisemeModifiers[i].TTranData != NULL)
			{
				for (j = 0; j < currentModel->nTransfLists; j++)
				{
					if (VisemeModifiers[i].TTranData[j] != NULL) free(VisemeModifiers[i].TTranData[j]);
				}
				free(VisemeModifiers[i].TTranData);
			}
		}

		delete [] VisemeModifiers;
	}

	freeDoubleFloatPointer(origPosition, nTransforms);
	freeDoubleFloatPointer(position, nTransforms);
	freeDoubleFloatPointer(origOrientation, nTransforms);
	freeDoubleFloatPointer(orientation, nTransforms);
	freeDoubleFloatPointer(origScale, nTransforms);
	freeDoubleFloatPointer(scale, nTransforms);
	//
}


void BaseAFMDeformer::applyFBAPs(VRMLModel *model,const FBAPs *fbaps)
{
	currentModel = model;
//	float f = 63.0;
//	float f2 = 63.0;
	int initPoseValue;
	FAPs *initPoseFAPs =0 ;
	FBAPs *filteredFBAPs = filterFBAPs(fbaps);
	FAPs *faps = filteredFBAPs->faps;
	BAPs *baps = filteredFBAPs->baps;
	
	if(currentModel->neutralPoseData.initialFacialPose)
	{
		initPoseFAPs = currentModel->neutralPoseData.initialFacialPose;
	}
				
	// apply fAPs
	if(faps || initPoseFAPs)
	{	
		if(faps->fap1.viseme1 != 0 && VisemeModifiers[faps->fap1.viseme1].refValue != -1)
		{
			modifierWeights[nModifiers] = (float)faps->fap1.blend / (float)VisemeModifiers[faps->fap1.viseme1].refValue;
			modifiers[nModifiers] = &VisemeModifiers[faps->fap1.viseme1];
			nModifiers++;
		}
		if(faps->fap1.viseme2 != 0 && VisemeModifiers[faps->fap1.viseme2].refValue != -1)
		{
			modifierWeights[nModifiers] = 1.0F - (float)faps->fap1.blend / (float)VisemeModifiers[faps->fap1.viseme2].refValue;
			modifiers[nModifiers] = &VisemeModifiers[faps->fap1.viseme2];
			nModifiers++;
		}
		if(faps->fap2.expression1 > 0 && ExpressionModifiers[faps->fap2.expression1].refValue != -1)
		{
			modifierWeights[nModifiers] = (float)faps->fap2.intensity1 / (float)ExpressionModifiers[faps->fap2.expression1].refValue;
			modifiers[nModifiers] = &ExpressionModifiers[faps->fap2.expression1];
			nModifiers++;
		}
		if(faps->fap2.expression2 > 0 && ExpressionModifiers[faps->fap2.expression2].refValue != -1)
		{
			modifierWeights[nModifiers] = (float)faps->fap2.intensity2 / (float)ExpressionModifiers[faps->fap2.expression2].refValue;
			modifiers[nModifiers] = &ExpressionModifiers[faps->fap2.expression2];
			nModifiers++;
		}
		for(int i = 2; i < nFaps; i++)
		{
			//		if(faps->groupMask[i] == 1 && fapValue0[i] != -1)
			if(initPoseFAPs)
				initPoseValue = initPoseFAPs->llf.value[i];
			else
				initPoseValue = 0;
			if((faps->llf.value[i] != 0 || initPoseValue != 0) && FAPModifiers[i].refValue != -1)
			{
				modifierWeights[nModifiers] = (float)(faps->llf.value[i] - initPoseValue) / (float)(FAPModifiers[i].refValue);
				modifiers[nModifiers] = &FAPModifiers[i];
				nModifiers++;
			}
		}
	}		

	if(baps)
	{	
		for(int i = 0; i < NUM_BAPS; i++)
		{
			if((baps->value[i] != 0 )&& BAPModifiers[i].refValue != -1)
			{
				modifierWeights[nModifiers] = (float)(baps->value[i]) / (float)(BAPModifiers[i].refValue);
				modifiers[nModifiers] = &BAPModifiers[i];
				nModifiers++;
			}
		}
	}	

	delete filteredFBAPs;
}

void BaseAFMDeformer::clearFBAPs()
{
	int i,j;

/*	for(i=0;i<nSurfaces;i++)
		for(j=0;j<coordLength[i];j++)
			coord[i][j] = origCoord[i][j];
			*/
	for(i=0;i<nTransforms;i++)
		for(j=0;j<3;j++)
			position[i][j] = origPosition[i][j];
	for(i=0;i<nTransforms;i++)
		for(j=0;j<4;j++)
			orientation[i][j] = origOrientation[i][j];
	for(i=0;i<nTransforms;i++)
		for(j=0;j<3;j++)
			scale[i][j] = origScale[i][j];

	nModifiers = 0;
}

void BaseAFMDeformer::updateGeometry(VRMLModel *model)
{
	currentModel = model;
	applyModifiers(modifiers,modifierWeights,nModifiers);

	if (nModifiers == 0)
		faceNeutral = 1;
	else
		faceNeutral = 0;

	traversalType = TT_UPDATE_GEOM;

	while(currentModel->locked)
	{
#ifdef WIN32
		Sleep(5);
#else
		usleep(5000);
#endif
	}
	currentModel->locked = 1;
	currentModel->traverse(currentModel->rootNode,this);
	currentModel->updateToWCMatrices();
	currentModel->locked = 0;
}

void BaseAFMDeformer::write(FILE *f, char *format)
{
	if(!strcmp(format,"afm") || !strcmp(format,"AFM"))
	{
		outputAnimDataAfm(f);
	}
	if(!strcmp(format,"wra") || !strcmp(format,"WRA"))
	{
		outputAnimDataWra(f);
		outputTransfDataWra(f);
	}
}

void BaseAFMDeformer::applyModifiers(VRMLModelModifier **mods,float *modWeights,int nMods)
{
	float *af2;


	// translation
	if(nMods == 0)
	{
		if(!faceNeutral)
		{
			for(int j = 0; j < nTransforms; j++)
			{
				for(int k = 0; k < 3; k++)
					position[j][k] = origPosition[j][k];
			}
			
		}
	}
	else
	{
		af2 = (float*)malloc(nMods*sizeof(float));
		for(int l = 0; l < nTransforms; l++)
		{
			for(int i1 = 0; i1 < 3; i1++)
			{
				float f = origPosition[l][i1];
				for(int j1 = 0; j1 < nMods; j1++)
				{
					af2[j1] = 0.0;
					if(mods[j1]->TTranData)
						if(mods[j1]->TTranData[l])
							af2[j1] = mods[j1]->TTranData[l][i1];
				}
				
				float f2 = 0.0;
				for(int k1 = 0; k1 < nMods; k1++)
				{
					if(af2[k1] != 0.0)
						f2 += af2[k1] * modWeights[k1];
				}
				
				f2 += f;
				position[l][i1] = f2;
			}
		}
		free(af2);
	}


	// rotation
	if(nMods == 0)
	{
		if(!faceNeutral)
		{
			for(int j = 0; j < nTransforms; j++)
			{
				for(int l = 0; l < 4; l++)
					orientation[j][l] = origOrientation[j][l];
			}
			
		}
	}
	else
	{
		int i1;	//CHANGED BY I.K. declared iterator outside of loop
		for(int k = 0; k < nTransforms; k++)
		{
			float orient[4];
			for(i1=0;i1<4;i1++)
				orient[i1]= origOrientation[k][i1];
			for(i1 = 0; i1 < nMods; i1++)
			{
				if(mods[i1]->TRotData != 0)
					if(mods[i1]->TRotData[k] != 0)
					{
						int flag = 0;
						for(int j1 = 0; j1 < 3; j1++)
						{
							if(mods[i1]->TRotData[k][j1] != 0.0)
								flag = 1;
						}
						if(flag)
						{
							float axis[3];
							float q[4];

							axis[0] = mods[i1]->TRotData[k][0];
							axis[1] = mods[i1]->TRotData[k][1];
							axis[2] = mods[i1]->TRotData[k][2];
							float angle = mods[i1]->TRotData[k][3];

							angle *= modWeights[i1];
							aa2quat(q,axis,angle);
							multQuat(orient,q,orient);
						}
					}
			}
			for(i1=0;i1<4;i1++)
				orientation[k][i1] = orient[i1];
		}
		
	}



	// scale
	if(nMods == 0)
	{
		if(!faceNeutral)
		{
			for(int j = 0; j < nTransforms; j++)
			{
				for(int k = 0; k < 3; k++)
					scale[j][k] = origScale[j][k];
			}
			
		}
	}
	else
	{
		float *af2 = (float*)malloc(nMods*sizeof(float));
		for(int l = 0; l < nTransforms; l++)
		{
			for(int i1 = 0; i1 < 3; i1++)
			{
				float f = origScale[l][i1];
				for(int j1 = 0; j1 < nMods; j1++)
				{
					af2[j1] = 0.0;
					if(mods[j1]->TScalData)
						if(mods[j1]->TScalData[l])
							af2[j1] = mods[j1]->TScalData[l][i1];
				}
				
				float f2 = 0.0;
				for(int k1 = 0; k1 < nMods; k1++)
					if(af2[k1] != 0.0)
						f2 += af2[k1] * modWeights[k1];
					
					f2 += f;
					scale[l][i1] = f2;
			}
		}
		free(af2);
	}
}

void BaseAFMDeformer::initDoubleFloatPointer(float ***p,int l1)
{
	(*p) = (float**)malloc(l1 * sizeof(float*));
	for(int i = 0;i<l1;i++)
		(*p)[i] = 0;
}

void BaseAFMDeformer::initDoubleFloatPointer(float ***p,int l1,int l2)
{
	(*p) = (float**)malloc(l1 * sizeof(float*));
	for(int i = 0;i<l1;i++)
	{
		(*p)[i] = (float*)malloc(l2*sizeof(float));
		for(int j=0;j<l2;j++)
			(*p)[i][j] = 0.0;
	}
}


void BaseAFMDeformer::allocAnimData()
{
	nSurfaces = currentModel->nCoordLists;
	nTransforms = currentModel->nTransfLists;
	nVisemes = 15;
	nExp = 7;
	nFaps = 68;
	nAllExp = nVisemes + nExp + nFaps;
//	int len = 10;

	initDoubleFloatPointer(&origPosition,nTransforms,3);
	initDoubleFloatPointer(&position,nTransforms,3);
	initDoubleFloatPointer(&origOrientation,nTransforms,4);
	initDoubleFloatPointer(&orientation,nTransforms,4);
	initDoubleFloatPointer(&origScale,nTransforms,3);
	initDoubleFloatPointer(&scale,nTransforms,3);
	modifierWeights = (float*)malloc(400*sizeof(float));
	nModifiers = 0;
	modifiers =(VRMLModelModifier**)malloc(400*sizeof(VRMLModelModifier*));
}

// traverser functions
int BaseAFMDeformer::traverseTransform(VRMLModel *model,    // pointer to the traversed model
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
	}
	return(0);
}


void BaseAFMDeformer::traverseMesh(VRMLModel *model,    // pointer to the traversed model
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
		break;
	case TT_UPDATE_GEOM:
		VRMLModel *m = (VRMLModel*)model;
		VRMLMeshMT *MTList=&m->morphTarget[index];
		int oldnMTs = MTList->nMTs;
		MTList->nMTs = 0;

		// reset coordinates
		for(i=0;i<coordLength;i++)
			coord[i] = origCoord[i];

		for(i = 0; i < nModifiers; i++)
		{
			if(modifierWeights[i] != 0.0f)
				if(modifiers[i]->MTData)
					if(modifiers[i]->MTData[index])
					{
						MTList->weight[MTList->nMTs] = modifierWeights[i]; 
						MTList->vertexDisplacement[MTList->nMTs] = modifiers[i]->MTData[index]; 
						MTList->nMTs++;
					}
		}
		if(MTList->nMTs>0) MTList->changed = true;
		if(MTList->nMTs==0 && oldnMTs != 0) MTList->changed = true;
		bones->changed = true;
		break;
	}
}

void BaseAFMDeformer::outputMorphTargetsData(float **mtCoord, FILE *of)
{
	float **coordDiff;
	int j,k;
	bool *surfaceHasMT;
	bool needKomma;
	float sensitivityThreshold = 0.0001f;
	int zeroSeg = 0;
	int segLength[6000];
	int nSegs;
	float tc[3];

	surfaceHasMT = new bool[nSurfaces];
	initDoubleFloatPointer(&coordDiff,nSurfaces);
	for(j=0;j<nSurfaces;j++)
	{
		coordDiff[j] = (float*)malloc(currentModel->coordListSize[j]*sizeof(float));
		for(k=0;k<currentModel->coordListSize[j];k++)
			coordDiff[j][k] = 0.0f;
	}

	fprintf(of,"    morphedIFSs [");
	needKomma = false;
	for(j=0;j<nSurfaces;j++)
	{
		surfaceHasMT[j] = false;
		for(k=0;k<currentModel->coordListSize[j];k++)
		{
			coordDiff[j][k] = 0;
			if(mtCoord[j])
				if(myfabs(mtCoord[j][k]) > sensitivityThreshold)
				{
					coordDiff[j][k] = mtCoord[j][k];
					surfaceHasMT[j] = true;
				}
		}
		if(surfaceHasMT[j])
		{
			if(needKomma)
				fprintf(of,",");
			else
				needKomma = true;
			fprintf(of,"\"%s\"",currentModel->meshNode[j]->getName());
		}
	}
	fprintf(of,"]\n");

	fprintf(of,"    morphedVertices [");
	for(j=0;j<nSurfaces;j++)
	{
		if(!surfaceHasMT[j])
			continue;
		zeroSeg = 0;
		nSegs = 0;
		segLength[nSegs] = 0;
		for(k=0;k<currentModel->coordListSize[j];k+=3)
		{
			int diff = 1;
			float *f = &coordDiff[j][k];
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
		nSegs++;
		for(k=0;k<nSegs;k++)
		{
			fprintf(of,"%d,",segLength[k]);
		}

		fprintf(of,"-1");
		if(j < nSurfaces - 1) fprintf(of,",\n");
	}
	fprintf(of," ] \n");

	fprintf(of,"    morphValues [");
	for(j=0;j<nSurfaces;j++)
	{
		if(!surfaceHasMT[j])
			continue;
		for(k=0;k<currentModel->coordListSize[j];k+=3)
		{
			int diff = 1;
			float *f = &coordDiff[j][k];
			if(f[0] == 0.0f && f[1] == 0.0f && f[2] == 0.0f)
				diff = 0;
			tc[0] = currentModel->origCoordList[j][k] + f[0];
			tc[1] = currentModel->origCoordList[j][k+1] + f[1];
			tc[2] = currentModel->origCoordList[j][k+2] + f[2];
			if(diff)
			{
				fprintf(of,"            %f %f %f",tc[0],tc[1],tc[2]);
				if (k+3 < currentModel->coordListSize[j])
					fprintf(of,",\n");
				else
					if (j+1 < nSurfaces)
						fprintf(of,",\n");
			}
		}
	}
	fprintf(of," ] \n");

	delete surfaceHasMT;
	freeDoubleFloatPointer(coordDiff,nSurfaces);
}

void BaseAFMDeformer::outputAnimationParameterData(VRMLModelModifier *mod,char *parameterName, FILE *of)
{
	bool *activeTransform;
	bool activeTransforms;
	float *m;
	bool needKomma;
	int j;
	float axis[3];
	float q[4],q1[4];
	float rotOut[4];
	float angle;
	if(mod->refValue == -1) 
		return;

	activeTransform = new bool[nTransforms];

	if(!mod->MTData && !mod->TRotData && !mod->TScalData && !mod->TTranData)
		return;

	fprintf(of,"\n\nAnimationParameter {\n");
	fprintf(of,"    name \"%s\"\n",parameterName);
	fprintf(of,"    refValue %d\n",(int)mod->refValue);

	if(mod->MTData)
		outputMorphTargetsData(mod->MTData, of);
	
	if(mod->TTranData)
	{
		activeTransforms = false;
		for(j=0;j<nTransforms;j++)
		{
			activeTransform[j] = false;
			m = mod->TTranData[j];
			if(!m)
				continue;
			if(m[0]==0.0f && m[1]==0.0f && m[2]==0.0f)
				continue;
			activeTransform[j] = true;
			activeTransforms = true;
		}
		if(activeTransforms)
		{
			fprintf(of,"    tTransforms [");
			needKomma = false;
			for(j=0;j<nTransforms;j++)
			{
				if(!activeTransform[j])
					continue;
				if(needKomma)
					fprintf(of,",");
				else
					needKomma = true;
				fprintf(of,"\"%s\"",currentModel->transfNode[j]->getName());
			}
			fprintf(of," ]\n");
			
			needKomma = false;
			fprintf(of,"    translations [");
			for(j=0;j<nTransforms;j++)
			{
				if(!activeTransform[j])
					continue;
				m = mod->TTranData[j];
				if(needKomma)
					fprintf(of,",");
				else
					needKomma = true;
				fprintf(of,"\n          %f, %f, %f",m[0]+origPosition[j][0],m[1]+origPosition[j][1],m[2]+origPosition[j][2]);
			}
			fprintf(of," ]\n");
		}
	}
	
	
	if(mod->TRotData)
	{
		activeTransforms = false;
		for(j=0;j<nTransforms;j++)
		{
			activeTransform[j] = false;
			m = mod->TRotData[j];
			if(!m)
				continue;
			if(m[3]==0.0f)
				continue;
			activeTransform[j] = true;
			activeTransforms = true;
		}
		if(activeTransforms)
		{
			fprintf(of,"    rTransforms [");
			needKomma = false;
			for(j=0;j<nTransforms;j++)
			{
				activeTransform[j] = false;
				m = mod->TRotData[j];
				if(!m)
					continue;
				if(m[3]==0.0f)
					continue;
				activeTransform[j] = true;
				if(needKomma)
					fprintf(of,",");
				else
					needKomma = true;
				fprintf(of,"\"%s\"",currentModel->transfNode[j]->getName());
			}
			fprintf(of," ]\n");
			
			needKomma = false;
			fprintf(of,"    rotations [");
			for(j=0;j<nTransforms;j++)
			{
				if(!activeTransform[j])
					continue;
				m = mod->TRotData[j];
				
				axis[0] = m[0];
				axis[1] = m[1];
				axis[2] = m[2];
				angle = m[3];
				
				aa2quat(q,axis,angle);
				multQuat(q1,q,origOrientation[j]);
				angle = quat2aa(q1,axis);
				rotOut[0] = axis[0];
				rotOut[1] = axis[1];
				rotOut[2] = axis[2];
				rotOut[3] = angle;

				if(needKomma)
					fprintf(of,",");
				else
					needKomma = true;
				fprintf(of,"\n          %f, %f, %f, %f",rotOut[0],rotOut[1],rotOut[2],rotOut[3]);
			}
			fprintf(of," ]\n");
		}
	}
	
	
	
	if(mod->TScalData)
	{
		activeTransforms = false;
		for(j=0;j<nTransforms;j++)
		{
			activeTransform[j] = false;
			m = mod->TScalData[j];
			if(!m)
				continue;
			if(m[0]==0.0f && m[1]==0.0f && m[2]==0.0f)
				continue;
			activeTransform[j] = true;
			activeTransforms = true;
		}
		if(activeTransforms)
		{
			fprintf(of,"    sTransforms [");
			needKomma = false;
			for(j=0;j<nTransforms;j++)
			{
				activeTransform[j] = false;
				m = mod->TScalData[j];
				if(!m)
					continue;
				if(m[0]==0.0f && m[1]==0.0f && m[2]==0.0f)
					continue;
				activeTransform[j] = true;
				if(needKomma)
					fprintf(of,",");
				else
					needKomma = true;
				fprintf(of,"\"%s\"",currentModel->transfNode[j]->getName());
			}
			fprintf(of," ]\n");
			
			needKomma = false;
			fprintf(of,"    scales [");
			for(j=0;j<nTransforms;j++)
			{
				if(!activeTransform[j])
					continue;
				m = mod->TScalData[j];
				if(needKomma)
					fprintf(of,",");
				else
					needKomma = true;
				fprintf(of,"\n          %f, %f, %f",m[0]+origScale[j][0],m[1]+origScale[j][1],m[2]+origScale[j][2]);
			}
			fprintf(of," ]\n");
		}
	}	
	fprintf(of,"}\n");
	
	delete activeTransform;
}

void BaseAFMDeformer::outputAnimDataAfm(FILE *of)
{
	int i;

	fprintf(of,"\n\nPROTO AnimationParameter [\n");
	fprintf(of,"    exposedField     SFString     name                \"\"\n");
	fprintf(of,"    exposedField     SFFloat      refValue            0\n");
	fprintf(of,"    exposedField     MFString     morphedIFSs         []\n");
	fprintf(of,"    exposedField     MFInt32      morphedVertices     []\n");
	fprintf(of,"    exposedField     MFVec3f      morphValues         []\n");
	fprintf(of,"    exposedField     MFString     tTransforms         []\n");
	fprintf(of,"    exposedField     MFVec3f      translations        []\n");
	fprintf(of,"    exposedField     MFString     rTransforms         []\n");
	fprintf(of,"    exposedField     MFRotation   rotations           []\n");
	fprintf(of,"    exposedField     MFString     sTransforms         []\n");
	fprintf(of,"    exposedField     MFVec3f      scales              []\n");
	fprintf(of,"]{Group{}}\n");

	for(i = 0;i < nVisemes;i++)
	{
		if(VisemeModifiers)
			outputAnimationParameterData(&VisemeModifiers[i],VisemeNameString[i],of);
	}
	for(i = 0;i < nExp;i++)
	{
		if(ExpressionModifiers)
			outputAnimationParameterData(&ExpressionModifiers[i],ExpressionNameString[i],of);
	}
	for(i = 0;i < NFAP;i++)
	{
		if(FAPModifiers)
			outputAnimationParameterData(&FAPModifiers[i],FAPNameString[i],of);
	}
	for(i = 0;i < NBAP;i++)
	{
		if(BAPModifiers)
			outputAnimationParameterData(&BAPModifiers[i],BAPNameString[i],of);
	}
}

// functions related to output of .wra file

void BaseAFMDeformer::initTripleFloatPointer(float ****p,int l1,int l2,int l3)
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

void BaseAFMDeformer::initTripleFloatPointer(float ****p,int l1,int l2)
{
	(*p) = (float***)malloc(l1 * sizeof(float**));
	for(int i = 0;i<l1;i++)
	{
		(*p)[i] = (float**)malloc(l2*sizeof(float*));
		for(int j=0;j<l2;j++)
			(*p)[i][j] = 0;
	}
}

void BaseAFMDeformer::freeTripleFloatPointer(float ***p,int l1,int l2)
{
	if(p)
	{
		for(int i = 0;i<l1;i++)
			freeDoubleFloatPointer(p[i],l2);
	}
	free(p);
}

void BaseAFMDeformer::freeDoubleFloatPointer(float **p,int l1)
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

#define NB_SD 88

void BaseAFMDeformer::outputAnimDataWra(FILE *of)
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
//			coordDiff[i][j] = (float*)malloc(coordLength[j]*sizeof(float));
			coordDiff[i][j] = (float*)malloc(currentModel->coordListSize[j]*sizeof(float));
			for(k=0;k<currentModel->coordListSize[j];k++)
				coordDiff[i][j][k] = 0.0f;
		}
	}

	for(i=0;i<NB_SD;i++)
	{
		if(i<15)
		{
//			mtCoord = fap1Coord[i];
			mtCoord = VisemeModifiers[i].MTData;
//			fv = fap1Value0[i];
			fv = (int)VisemeModifiers[i].refValue;
		}
		else
		{
			if(i<=21)
			{
//				mtCoord = fap2Coord[i-15];
				mtCoord = ExpressionModifiers[i-15].MTData;
//				fv = fap2Value0[i-15];
				fv = (int)ExpressionModifiers[i-15].refValue;
			}
			else
			{
//				mtCoord = fapCoord[i-20];
				mtCoord = FAPModifiers[i-20].MTData;
//				fv = fapValue0[i-20];
				fv = (int)FAPModifiers[i-20].refValue;
			}
		}

		if(fv == -1)
			continue;
		if(!mtCoord)
			continue;

		for(j=0;j<nSurfaces;j++)
		{
			for(k=0;k<currentModel->coordListSize[j];k++)
			{
				coordDiff[i][j][k] = 0;
				if(mtCoord[j])
					if(myfabs(mtCoord[j][k]) > sensitivityThreshold)
						coordDiff[i][j][k] = mtCoord[j][k];
			}
		}
	}
	
	for(i=0;i<NB_SD;i++)
	{
		nSegs = 0;
		zeroSeg = 1;
		if(i<15)
		{
			fv = (int)VisemeModifiers[i].refValue;
		}
		else
		{
			if(i<=21)
			{
				fv = (int)ExpressionModifiers[i-15].refValue;
			}
			else
			{
				fv = (int)FAPModifiers[i-20].refValue;
			}
		}

		if(fv == -1)
			continue;
		
		segLength[nSegs] = 0;
		for(j=0;j<nSurfaces;j++)
		{
			for(k=0;k<currentModel->coordListSize[j];k+=3)
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
			for(k=0;k<currentModel->coordListSize[j];k+=3)
			{
				int diff = 1;
				float *f = &coordDiff[i][j][k];
				if(f[0] == 0.0f && f[1] == 0.0f && f[2] == 0.0f)
					diff = 0;
				if(diff)
				{
					fprintf(of,"            %f %f %f",f[0],f[1],f[2]);
					if (k+3 < currentModel->coordListSize[j])
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

void BaseAFMDeformer::outputTransfDataWra(FILE *of)
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
	float zeroVector[4];
	float axis[3],angle;

	for(i=0;i<4;i++)
		zeroVector[i]=0.0f;

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
			fv = (int)VisemeModifiers[i].refValue;
//			if(!fap1Position[i])
//			if(!VisemeModifiers[i].TTranData)
//				continue;
		}
		else
		{
			if(i<=21)
			{
				fv = (int)ExpressionModifiers[i-15].refValue;
//				if(!fap2Position[i-15])
//				if(!ExpressionModifiers[i-15].TTranData)
//					continue;
			}
			else
			{
				fv = (int)FAPModifiers[i-20].refValue;
//				if(!fapPosition[i-20])
//				if(!FAPModifiers[i-20].TTranData)
//					continue;
			}
		}
		if(fv == -1)
			continue;

		
		for(j=0;j<nTransforms;j++)
		{
			t = r = s = 0;
			if(i<15)
			{
				//				t = fap1Position[i][j];
				if(VisemeModifiers[i].TTranData)
					t = VisemeModifiers[i].TTranData[j];
				//				r = fap1Orientation[i][j];
				if(VisemeModifiers[i].TRotData)
					r = VisemeModifiers[i].TRotData[j];
				//				s = fap1Scale[i][j];
				if(VisemeModifiers[i].TScalData)
					s = VisemeModifiers[i].TScalData[j];
			}
			else
			{
				if(i<=21)
				{
					//					t = fap2Position[i-15][j];
					if(ExpressionModifiers[i-15].TTranData)
						t = ExpressionModifiers[i-15].TTranData[j];
					//					r = fap2Orientation[i-15][j];
					if(ExpressionModifiers[i-15].TRotData)
						r = ExpressionModifiers[i-15].TRotData[j];
					//					s = fap2Scale[i-15][j];
					if(ExpressionModifiers[i-15].TScalData)
						s = ExpressionModifiers[i-15].TScalData[j];
				}
				else
				{
					//					t = fapPosition[i-20][j];
					if(FAPModifiers[i-20].TTranData)
						t = FAPModifiers[i-20].TTranData[j];
					//					r = fapOrientation[i-20][j];
					if(FAPModifiers[i-20].TRotData)
						r = FAPModifiers[i-20].TRotData[j];
					//					s = fapScale[i-20][j];
					if(FAPModifiers[i-20].TScalData)
						s = FAPModifiers[i-20].TScalData[j];
				}
			}
			if(!t)
				t = zeroVector;
			if(!r)
				r = zeroVector;
			else
			{
				axis[0]=r[0];
				axis[1]=r[1];
				axis[2]=r[2];
				angle = r[3];
				aa2quat(r,axis,angle);
			}
			if(!s)
				s = zeroVector;
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
			fv = (int)VisemeModifiers[i].refValue;
		}
		else
		{
			if(i<=21)
			{
				fv = (int)ExpressionModifiers[i-15].refValue;
			}
			else
			{
				fv = (int)FAPModifiers[i-20].refValue;
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

}




