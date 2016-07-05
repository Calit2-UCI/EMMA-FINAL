// AFM.cpp : Defines the entry point for the DLL application.
//

#include "VRMLModel.h"
#include "VRMLWriteTraverser.h"
#include "BaseFileIO.h"
#include <assert.h>

#include <clocale>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>



#include "Node.h"

#define TT_MORPH 0
#define TT_SKIN 1
#define TT_FIND_TRANSFORM_MESH 2
#define TT_INIT 3
#define TT_REMOVE_NUS 4

// gpVrmlParserLib needs this defined, otherwise link crashes
//#include <iostream.h>
//class ostream_withassign cout;


#ifdef DO_DEBUG_PRINT
//temporary debugging stuff
void DebugPrint(char *s)
{
	FILE *fp = fopen("c:\\Temp\\debugprint.txt","a");
	fprintf(fp,"%s",s);
	fclose(fp);
}

void DebugPrintNum(char *s, float num)
{
	FILE *fp = fopen("c:\\Temp\\debugprint.txt","a");
	fprintf(fp,"%s : %f\n",s,num);
	fclose(fp);
}
#endif


namespace VisageSDK
{

// VRMLTraverser
VRMLTraverser::VRMLTraverser()
{
	
}

VRMLTraverser::~VRMLTraverser()
{
	
}


VRMLMaterial::VRMLMaterial()
{
	// Added by KStankovic
	int i;
	for (i = 0; i < 6; i++)
		cubemapNames[i] = NULL;
	//
}

/**
* Constructor. Constructs a new VRMLMaterial by copying m.
* @param m Material to be copied
*/
VRMLMaterial::VRMLMaterial(VRMLMaterial *m)
{
	for(int i =0;i<3;i++)
	{
		diffuseColor[i] = m->diffuseColor[i];
		specularColor[i] = m->specularColor[i];
		emissiveColor[i] = m->emissiveColor[i];
	}
	ambientIntensity = m->ambientIntensity;
	shininess = m->shininess;
	transparency = m->transparency;
	strcpy(textureName,m->textureName);

	// Added by KStankovic
	for (int i = 0; i < 6; i++)
	{
		if (m->cubemapNames[i] != NULL)
		{
			cubemapNames[i] = new char[strlen(m->cubemapNames[i]) + 1];
			strcpy(cubemapNames[i], m->cubemapNames[i]);
		}
		else
		{
			cubemapNames[i] = NULL;
		}
	}
	//
}

VRMLMaterial::~VRMLMaterial()
{
	// Added by KStankovic
	int i;
	for (i = 0; i < 6; i++)
		if (cubemapNames[i] != NULL) delete [] cubemapNames[i];
	//
}


// VRMLNode

VRMLNode::VRMLNode(int _type, int _ind, int _matInd,Node *_parserNode,char *_name,VRMLNode *_parent)
{
	type = _type;
	ind = _ind;
	matInd = _matInd;
	nChildren = 0;
	parserNode = _parserNode;
	parent = _parent;
	// Added by KStankovic
	name = NULL;
	children = NULL;
	//
	setName(_name);
}

VRMLNode::~VRMLNode()
{
	if(name)
		free(name);

	// Added by KStankovic
	if (children)
	{
		int i;
		for (i = 0; i < nChildren; i++)
			if (children[i] != NULL) delete children[i];

		free(children);
	}
	//
}

void VRMLNode::setName(char *_name)
{
	if(_name)
	{
		// Added by KStankovic
		if (name != NULL) free(name);
		//
		name = (char *) malloc((strlen(_name)+1) * sizeof(char));
		strcpy(name,_name);
	}
	else 
		name = 0;
}

char* VRMLNode::getName()
{
	return name;
}

VRMLNode *VRMLNode::clone()
{
	VRMLNode *n = new VRMLNode(type,ind,matInd,parserNode,name,parent);
	
	for(int i =0;i<nChildren;i++)
	{
		VRMLNode *c = children[i]->clone();
		n->addChild(c);
	}
	return(n);
}

void VRMLNode::addChild(VRMLNode *n)
{
	int nc = nChildren + 1;
	
	if(nc == 1)
		children = (VRMLNode**)malloc(sizeof(VRMLNode*));
	else
		children = (VRMLNode**)realloc(children,sizeof(VRMLNode*)*nc);
	
	children[nChildren] = n;
	nChildren = nc;
}

/*
void VRMLNode::updateToWCMatrix(void *_model, myMatrix _toWC)
{
	VRMLModel *model = (VRMLModel*)_model;
	myMatrix toWC;
	copyMat(toWC,_toWC);
	
	if(type == NT_TRANSFORM)
	{
		myMatrix t;
		identityMat(t);
		model->getTransfMatrix(t,model->transfList[ind]);
		multMat(toWC,t,toWC);
		copyMat(model->toWC[ind],toWC);
		invMat(model->fromWC[ind],toWC);
	}
	for(int i =0;i<nChildren;i++)
	{
		children[i]->updateToWCMatrix(model,toWC);
	}
}
*/

void VRMLNode::updateToWCMatrix(void *_model, myMatrix _toWC)
{
	VRMLModel *model = (VRMLModel*)_model;
	myMatrix toWC;
	copyMat(toWC,_toWC);
	
	updateNodeToWCMatrix(model,toWC);
	
	for(int i =0;i<nChildren;i++)
	{
		children[i]->updateToWCMatrix(model,toWC);
	}
}

void VRMLNode::updateNodeToWCMatrix(VRMLModel *model, myMatrix toWC)
{
	if(type == NT_TRANSFORM)
	{
		myMatrix t;
		identityMat(t);
		model->getTransfMatrix(t,model->transfList[ind]);
		multMat(toWC,t,toWC);
		copyMat(model->toWC[ind],toWC);
		invMat(model->fromWC[ind],toWC);
	}
}

void VRMLNode::getRelativeMatrix(VRMLModel *model,myMatrix r)
{
	myMatrix orig_cur_mat;
	myMatrix orig_par_mat;
	myMatrix cur_mat;
	myMatrix par_mat;
	myMatrix inv_par_mat;

	copyMat(orig_cur_mat,model->toWC[ind]);
	copyMat(orig_par_mat,model->toWC[parent->ind]);

	uniformMatrix(cur_mat,orig_cur_mat);
	uniformMatrix(par_mat,orig_par_mat);

	invMat(inv_par_mat,par_mat);

	multMat(r,cur_mat,inv_par_mat);
}

int VRMLNode::insertTransform(int t)
{
	static int no = (int)&t;
	char name[50];

	for(int i =0;i<nChildren;i++)
	{
		int ins = children[i]->insertTransform(t);
		if(ins)
		{
			sprintf(name,"TRANSF-%d",no++);
			VRMLNode *n = new VRMLNode(NT_TRANSFORM,t,0,NULL,name,this);
			n->addChild(children[i]);
			children[i] = n;
		}
	}
	if(type == NT_TRANSFORM)
	{
		if(ind==t)
		{
			ind++;
			return(1);
		}
		if(ind >= t)
			ind++;
	}
	return(0);
}

// VRMLModel

VRMLModel::VRMLModel()
{
	//setlocale(LC_ALL, "C");
	init();
}


VRMLModel::VRMLModel(char *fn)
{ 
#ifdef DO_DEBUG_PRINT
DebugPrint("\n\n\n\n\n\n\n\n\n\n\nVRMLModel::VRMLModel\n");
DebugPrint(fn);
#endif
	init();
#ifdef DO_DEBUG_PRINT
DebugPrint("\nVRMLModel::VRMLModel MARK1\n");
#endif
	readFromFile(fn);
#ifdef DO_DEBUG_PRINT
DebugPrint("\nVRMLModel::VRMLModel MARK2\n");
#endif
	return; 
}

VRMLModel::VRMLModel(VRMLModel *m)
{ 
	init();
	copy(m);
	return; 
}

VRMLModel::~VRMLModel()
{
	int i;
	for(i=0;i<200;i++)
	{
		if(coordList[i])
			free(coordList[i]);
		if(tCoordList[i])
			free(tCoordList[i]);
		if(normList[i])
			free(normList[i]);
		if(cpvList[i])
			free(cpvList[i]);
		if(triList[i])
			free(triList[i]);
		if(normIndList[i])
			free(normIndList[i]);
		if(cpvIndList[i])
			free(cpvIndList[i]);
		if(texIndList[i])
			free(texIndList[i]);
		// Changed by KStankovic
		if(materials[i])
			delete materials[i];
		//

		// Added by KStankovic
		if (origCoordList[i])
			free(origCoordList[i]);
		if (transfList[i])
			free(transfList[i]);
		//
	}
	/* This should be repaired; for some reason it causes assertion failure
	for(i=0;i<200;i++)
	{
	if(transfList[i])
	free(transfList[i]);
	}
	*/

	// Changed by KStankovic
	delete rootNode;
	//

	// Added by KStankovic
	for (i = 0; i < 100; i++)
	{
		if (FIO[i] != NULL) FIO[i]->unref();
	}
	//
}

void VRMLModel::init()
{ 
	initialized = 0;
	locked = 0;
	nMaterials = 0;
	int i;
	for(i=0;i<200;i++)
	{
		coordList[i]=0;
		origCoordList[i]=0;
		tCoordList[i]=0;
		normList[i]=0;
		cpvList[i]=0;
		triList[i]=0;
		materials[i]=0;
		cpvIndList[i]=0;
		normIndList[i]=0;
		texIndList[i]=0;
		
		coordListSize[i]=0;
		tCoordListSize[i]=0;
		normListSize[i]=0;
		cpvListSize[i]=0;
		triListSize[i]=0;
		cpvIndListSize[i]=0;
		normIndListSize[i]=0;
		texIndListSize[i]=0;

		NPV[i]=0;
		normalsCalculated[i]=0;
		creaseAngle[i]=0;
		solid[i]=0;
		ccw[i]=0;
		convex[i]=0;

		transfList[i]=0;

		// Added by KStankovic
		meshNode[i] = NULL;
		transfNode[i] = NULL;
		//
	}
	rootNode=0;
	filePath[0] = 0;
	globalTranslation[0] = 0.0f;
	globalTranslation[1] = 0.0f;
	globalTranslation[2] = 0.0f;
	globalRotation[0] = 0.0f;
	globalRotation[1] = 0.0f;
	globalRotation[2] = 0.0f;
	globalScale[0] = 1.0f;
	globalScale[1] = 1.0f;
	globalScale[2] = 1.0f;

	nFIO = 1;
	FIO[0] = new BaseFileIO();
	// Added by KStankovic
	for (i = 1; i < 100; i++) FIO[i] = NULL;
	//
}

void VRMLModel::copy(VRMLModel *m)
{ 
	int i,j;
	if(!m)
		return;
	
	nCoordLists = m->nCoordLists;
	nTransfLists = m->nTransfLists;
	nMaterials = m->nMaterials;
	nTransfLists = m->nTransfLists;
	initialized = m->initialized;
	
	for(i=0;i<nCoordLists;i++)
	{
		coordListSize[i] = m->coordListSize[i];
		tCoordListSize[i] = m->tCoordListSize[i];
		triListSize[i] = m->triListSize[i];
		normListSize[i] = m->normListSize[i];
		cpvListSize[i] = m->cpvListSize[i];
		normIndListSize[i] = m->normIndListSize[i];
		texIndListSize[i] = m->texIndListSize[i];
		cpvIndListSize[i] = m->cpvIndListSize[i];
		if(coordListSize[i])
			coordList[i] = (float*)malloc(coordListSize[i]*sizeof(float));
		if(coordListSize[i])
			origCoordList[i] = (float*)malloc(coordListSize[i]*sizeof(float));
		if(tCoordListSize[i])
			tCoordList[i] = (float*)malloc(tCoordListSize[i]*sizeof(float));
		if(normListSize[i])
			normList[i] = (float*)malloc(normListSize[i]*sizeof(float));
		if(cpvListSize[i])
			cpvList[i] = (float*)malloc(cpvListSize[i]*sizeof(float));
		for(j=0;j<coordListSize[i];j++)
			coordList[i][j] = m->coordList[i][j];
		for(j=0;j<coordListSize[i];j++)
			origCoordList[i][j] = m->origCoordList[i][j];
		for(j=0;j<tCoordListSize[i];j++)
			tCoordList[i][j] = m->tCoordList[i][j];
		for(j=0;j<normListSize[i];j++)
			normList[i][j] = m->normList[i][j];
		for(j=0;j<cpvListSize[i];j++)
			cpvList[i][j] = m->cpvList[i][j];
		if(triListSize[i])
			triList[i] = (int*)malloc(triListSize[i]*sizeof(int));
		if(normIndListSize[i])
			normIndList[i] = (int*)malloc(normIndListSize[i]*sizeof(int));
		if(texIndListSize[i])
			texIndList[i] = (int*)malloc(texIndListSize[i]*sizeof(int));
		if(cpvIndListSize[i])
			cpvIndList[i] = (int*)malloc(cpvIndListSize[i]*sizeof(int));
		for(j=0;j<triListSize[i];j++)
			triList[i][j] = m->triList[i][j];
		for(j=0;j<normIndListSize[i];j++)
			normIndList[i][j] = m->normIndList[i][j];
		for(j=0;j<texIndListSize[i];j++)
			texIndList[i][j] = m->texIndList[i][j];
		for(j=0;j<cpvIndListSize[i];j++)
			cpvIndList[i][j] = m->cpvIndList[i][j];
		transfInd[i] = m->transfInd[i];
		CPV[i] = m->CPV[i];
		NPV[i]=m->NPV[i];
		normalsCalculated[i]=m->normalsCalculated[i];
		creaseAngle[i]=m->creaseAngle[i];
		solid[i]=m->solid[i];
		ccw[i]=m->ccw[i];
		convex[i]=m->convex[i];
		if(m->materials[i])
			materials[i] = new VRMLMaterial(m->materials[i]);
	}
	for(i=0;i<nTransfLists;i++)
	{
		transfList[i] = (float*)malloc(17*sizeof(float));
		for(j=0;j<17;j++)
			transfList[i][j] = m->transfList[i][j];
		copyMat(toWC[i],m->toWC[i]);
		copyMat(fromWC[i],m->fromWC[i]);
		transfLevel[i] = m->transfLevel[i];
	}

	nFIO = m->nFIO;
	for(i=0;i<nFIO;i++)
	{
		FIO[i] = m->FIO[i];
		FIO[i]->ref();
	}
	
	rootNode = m->rootNode->clone();

	// this is necessary in order to assign meshNode and transfNode pointers
	traverseType = TT_INIT;
	traverse(rootNode,this);
}


void VRMLModel::finalizeFileReading(char *fileName)
{
	// find the path part in the whole file name (everything up to the slash) and store it; it can be used later to access the foledr, typically for loading the textures
	strcpy(filePath,fileName);
	int i = (int)strlen(filePath);
	while(i>0 && filePath[i] != '/'&& filePath[i] != '\\') i--;
	filePath[i] = 0;		
	int j;

	traverseType = TT_INIT;
	traverse(rootNode,this);

	removeNonUniformScale();


	// calculate the normals if necessary
	for(i=0;i<nCoordLists;i++)
	{
		if(normalsCalculated[i])
		{
			NPV[i] = 1;
			int nc = coordListSize[i];
			int ni = triListSize[i];
			normList[i] = (float *)malloc(sizeof(float)*nc);
			normListSize[i] = nc;
			generateNormals(normList[i],coordList[i],triList[i],nc,ni,ccw[i]);

			normIndListSize[i] = ni;
			normIndList[i] = (int*)malloc(ni*sizeof(int));
			for(j=0;j<ni;j++)
			{
				normIndList[i][j] = triList[i][j];
			}
		}
	}

	// initialize the original coordinates
	for(i = 0;i<nCoordLists;i++)
	{
		float *coord = coordList[i];
		float *origCoord = (float*)malloc(coordListSize[i] * sizeof(float));

		// Added by KStankovic
		if (origCoordList[i] != NULL) free(origCoordList[i]);
		//

		origCoordList[i] = origCoord;
		for(j = 0;j<coordListSize[i];j++)
		{
			origCoord[j] = coord[j];
		}
	}

	initialized = 1;
}

int VRMLModel::readFromFile(char *fn)
{


	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i = 0; i<nFIO; i++)
	{
		if(FIO[i]->read(fn,this) == 1)
			break;
	}

	if(i==nFIO)
	{
		char msg[300];
		sprintf(msg,"Error 163\nCould not read file: >%s< (%s)\n",fn,strerror(NULL) );
//		MessageBox(0,msg,"Error",MB_ICONERROR);
		fprintf(stderr,"%s\n",msg);
		return(0);
	}

	

	return(1);
}


void VRMLModel::getTransformComponents(float *crd,myMatrix tr,myMatrix r,myMatrix s,myMatrix so,myMatrix c,myMatrix negc,myMatrix negso)
{
	int cnt=0;
	identityMat(r);    
	identityMat(tr);    
	identityMat(s);    
	identityMat(so);    
	identityMat(c);    
	identityMat(negso);    
	identityMat(negc);    
	
	//Get the translation
	int j;	//CHANGED BY I.K. declared iterator outside of loop
	for(j=0;j<3;j++)
		tr[3][j] = crd[cnt++];
	
	//Get the rotation
	rotMatFromQuat(r,crd+cnt);
	cnt+=4;
	
	//Get the scale
	for(j=0;j<3;j++)
		s[j][j] *= crd[cnt++];
	
	//Get the scale orientation
	float q[4];
	q[0] = crd[cnt++];
	q[1] = crd[cnt++];
	q[2] = crd[cnt++];
	q[3] = crd[cnt++];
	rotMatFromQuat(so,q);
	q[0] = -q[0];
	q[1] = -q[1];
	q[2] = -q[2];
	rotMatFromQuat(negso,q);
	
	
	//Get the center
	for(j=0;j<3;j++)
		c[3][j] = crd[cnt++];
	for(j=0;j<3;j++)
		negc[3][j] = -c[3][j];
}

void VRMLModel::getTransformComponents(float *crd,float *t,float *r,float *s,float *so,float *c,float *negc,float *negso,float *rq,float *soq,float *negsoq)
{
	int cnt=0;
	int j;
	
	//Get the translation
	for(j=0;j<3;j++)
		t[j] = crd[cnt++];
	
	//Get the rotation
	for(j=0;j<4;j++)
		rq[j] = crd[cnt++];
	r[3] = quat2aa(rq,r);
	
	//Get the scale
	for(j=0;j<3;j++)
		s[j] = crd[cnt++];
	
	//Get the scale orientation
	soq[0] = crd[cnt++];
	soq[1] = crd[cnt++];
	soq[2] = crd[cnt++];
	soq[3] = crd[cnt++];
	so[3] = quat2aa(soq,so);
	negsoq[0] = -soq[0];
	negsoq[1] = -soq[1];
	negsoq[2] = -soq[2];
	negsoq[3] = soq[3];
	negso[3] = quat2aa(negsoq,negso);
	
	
	//Get the center
	for(j=0;j<3;j++)
		c[j] = crd[cnt++];
	for(j=0;j<3;j++)
		negc[j] = -c[j];
}

// get transformation contained in crd by adding to it
// translation contained in tl
void VRMLModel::getTransfMatrix(myMatrix t, float* crd)
{
	myMatrix tr,r,s,so,c,negc,negso;
	getTransformComponents(crd,tr,r,s,so,c,negc,negso);
	
	multMat(t,negc,negso);
	multMat(t,t,s);
	multMat(t,t,so);
	multMat(t,t,r);
	multMat(t,t,c);
	multMat(t,t,tr);
}

void VRMLModel::updateToWCMatrices()
{
	myMatrix idm;
	identityMat(idm);
	
	rootNode->updateToWCMatrix(this,idm);
}



void VRMLModel::generateNormals(float *normals, float *coords,int *ind,int nc,int ni,int _ccw)
{
	int i,j;
	myVector *faceNormals = (myVector *)malloc(sizeof(myVector)*ni/3);
	int *nNeighbors = (int*)malloc(sizeof(int)*nc/3);
	int pc= 0;
	
	for(i=0;i<nc/3 ;i++)
		nNeighbors[i] = 0;
	for(i=0;i<nc ;i++)
		normals[i] = 0.0;
	
	for(i=0;i<ni ;i+=3)
	{
		int i1,i2,i3;
		myVector p1,p2,p3,v1,v2;
		i1 = ind[i];
		i2 = ind[i+1];
		i3 = ind[i+2];
		
		for(j=0;j<3;j++)
		{
			p1[j] = coords[i1*3+j];
			p2[j] = coords[i2*3+j];
			p3[j] = coords[i3*3+j];
			v1[j] = p2[j] - p1[j];
			v2[j] = p3[j] - p1[j];
		}
		cross(faceNormals[pc],v1,v2);
		normalize(faceNormals[pc]);

		if(!_ccw)
		{
			for(j=0;j<3;j++)
				faceNormals[pc][j]=-faceNormals[pc][j];
		}

		for(j=0;j<3;j++)
		{
			normals[i1*3+j] += faceNormals[pc][j];
			normals[i2*3+j] += faceNormals[pc][j];
			normals[i3*3+j] += faceNormals[pc][j];
			nNeighbors[i1]++;
			nNeighbors[i2]++;
			nNeighbors[i3]++;
		}
		pc++;
	}
	for(i=0;i<nc/3 ;i++)
	{
		if(nNeighbors[i] == 0)
		{
			normals[i*3] = 0.0;
			normals[i*3+1] = 1.0;
			normals[i*3+2] = 0.0;
		}
		else
		{
			normals[i*3] /= nNeighbors[i];
			normals[i*3+1] /= nNeighbors[i];
			normals[i*3+2] /= nNeighbors[i];
		}
	}
	
	free(faceNormals);
	free(nNeighbors);
}


int VRMLModel::writeToFile(char *fn)
{
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i = 0; i<nFIO; i++)
	{
		if(FIO[i]->write(fn,this) == 1)
			break;
	}

	if(i==nFIO)
	{
		char msg[300];
		sprintf(msg,"Error 164\nCould not write file: >%s< (%s)\n",fn,strerror(NULL) );
//		MessageBox(0,msg,"Error",MB_ICONERROR);
		fprintf(stderr,"%s\n",msg);
		return(0);
	}

	return(1);
}
void VRMLModel::traverse(VRMLNode *node, VRMLTraverser *traverser)
{

	int i;
	int update;
	switch(node->type)
	{
	case NT_TRANSFORM:
		myMatrix m;
		identityMat(m);
		float t[3],r[4],s[3],so[4],c[3],negc[3],negso[4],rq[4],soq[4],negsoq[4];
		getTransformComponents(transfList[node->ind],t,r,s,so,c,negc,negso,rq,soq,negsoq);
		getTransfMatrix(m,transfList[node->ind]);
		update = traverser->traverseTransform(this,node,node->ind,c,negc,r,rq, s, so,soq, negso, negsoq, t, m);
		if(update)
		{
			transfList[node->ind][0] = t[0];
			transfList[node->ind][1] = t[1];
			transfList[node->ind][2] = t[2];
			transfList[node->ind][3] = rq[0];
			transfList[node->ind][4] = rq[1];
			transfList[node->ind][5] = rq[2];
			transfList[node->ind][6] = rq[3];
			transfList[node->ind][7] = s[0];
			transfList[node->ind][8] = s[1];
			transfList[node->ind][9] = s[2];
		}
		for(i=0;i<node->nChildren;i++)
			traverse(node->children[i],traverser);
		traverser->traverseTransformEnd(this,node,node->ind);
		break;
	case NT_ROOT:
		traverser->traverseRoot(this,node,0);
		for(i=0;i<node->nChildren;i++)
			traverse(node->children[i],traverser);
		traverser->traverseRootEnd(this,node,0);
		break;
	case NT_GROUP:
		traverser->traverseGroup(this,node,0);
		for(i=0;i<node->nChildren;i++)
			traverse(node->children[i],traverser);
		traverser->traverseGroupEnd(this,node,0);
		break;
	case NT_SHAPE:
		traverser->traverseMesh(this,node,node->ind,
			materials[node->matInd],   // material for this shape
			coordList[node->ind],        // vertex coordinates
			origCoordList[node->ind],        // vertex coordinates
			coordListSize[node->ind],     // length of the vertex coordinates array (number of vertices * 3)
			triList[node->ind],       // vertex indices for triangles
			triListSize[node->ind],  // length of coordInd array
			normList[node->ind],         // normals
			normListSize[node->ind],      // length of norm array
			normIndList[node->ind],        // normal indices, per vertex
			normIndListSize[node->ind],   // length of normInd array
			NPV[node->ind],              // Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
			normalsCalculated[node->ind],// If 1, normals are calculated by VRMLModel; if 0, normals were read from the file
			creaseAngle[node->ind],   // VRML creaseAngle (default 0.0; 1.2 is good value to smooth)
			solid[node->ind],           // VRML solid (default 1)
			ccw[node->ind],             // VRML counter-clockwise (default 1)
			convex[node->ind],          // VRML convex (default 1)
			tCoordList[node->ind],       // texture coordinates, if any (else zero)
			tCoordListSize[node->ind], 
			texIndList[node->ind], 
			texIndListSize[node->ind], 
			cpvList[node->ind],          // colors per vertex, if any (else zero)
			cpvListSize[node->ind], 
			cpvIndList[node->ind], 
			cpvIndListSize[node->ind],
			CPV[node->ind],
			&morphTarget[node->ind],
			&boneData[node->ind]);
		
		
		
/*		traverser->traverseShape(this,node,node->ind,
			materials[node->matInd],   // material for this shape
			coordList[node->ind],        // vertex coordinates
			coordListSize[node->ind],     // length of the vertex coordinates array (number of vertices * 3)
			triList[node->ind],       // vertex indices for triangles
			triListSize[node->ind],  // length of coordInd array
			normList[node->ind],         // normals
			normListSize[node->ind],      // length of norm array
			normIndList[node->ind],        // normal indices, per vertex
			normIndListSize[node->ind],   // length of normInd array
			NPV[node->ind],              // Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
			normalsCalculated[node->ind],// If 1, normals are calculated by VRMLModel; if 0, normals were read from the file
			creaseAngle[node->ind],   // VRML creaseAngle (default 0.0; 1.2 is good value to smooth)
			solid[node->ind],           // VRML solid (default 1)
			ccw[node->ind],             // VRML counter-clockwise (default 1)
			convex[node->ind],          // VRML convex (default 1)
			tCoordList[node->ind],       // texture coordinates, if any (else zero)
			tCoordListSize[node->ind], 
			texIndList[node->ind], 
			texIndListSize[node->ind], 
			cpvList[node->ind],          // colors per vertex, if any (else zero)
			cpvListSize[node->ind], 
			cpvIndList[node->ind], 
			cpvIndListSize[node->ind],
			CPV[node->ind]);*/
		break;
	case NT_OTHER:
		traverser->traverseOther(this,node,0);
		break;
	}
}

/*
// remove copies of identical vertices from the vertex list, and arrange triangle list
// accordingly; chanSize is the number of coordinates per vertex: 3 for vertices, 2
// for texture vertices
void VRMLModel::optimiseVRMLSurface(int *_coordListSize,float *_coordList,int *_triListSize,int *_triList, int chanSize,int **_newInd, int **_erased)
{
	int i,j,k;
	int coordListSize1 = 0;
	int newIndCnt = 0;
	float *coordList1 = (float*)malloc((*_coordListSize) * sizeof(float));
	int *triList1 = (int*)malloc((*_triListSize) * sizeof(int));
	int *newInd = (int*)malloc((*_coordListSize) * sizeof(int) / chanSize);
	int *erased = (int*)malloc((*_coordListSize) * sizeof(int) / chanSize);
	
	
	for(i=0;i<(*_coordListSize)/chanSize;i++)
	{
		newInd[i] = -1;
		erased[i] = 0;
	}
	
	for(i=0;i<(*_coordListSize)/chanSize;i++)
	{
		if(newInd[i] >= 0)
			continue;
		for(k=0;k<chanSize;k++)
			coordList1[coordListSize1++] = _coordList[i*chanSize+k];
		newInd[i] = newIndCnt;
		
		for(j=i+1;j<(*_coordListSize)/chanSize;j++)
		{
			for(k=0;k<chanSize;k++)
			{
				if(_coordList[i*chanSize+k] != _coordList[j*chanSize+k]) 
					break;
			}
			if(k<chanSize)
				continue;
			newInd[j] = newIndCnt;
			erased[j] = 1;
		}
		newIndCnt++;
	}
	for(i=0;i<(*_triListSize);i++)
		triList1[i] = newInd[_triList[i]];
	
	(*_coordListSize) = coordListSize1;
	for(i=0;i<(*_coordListSize);i++)
		_coordList[i] = coordList1[i];
	for(i=0;i<(*_triListSize);i++)
		_triList[i] = triList1[i];
	
	free(coordList1);
	free(triList1);
	if(_newInd)
		(*_newInd) = newInd;
	else
		free(newInd);
	if(_erased)
		(*_erased) = erased;
	else
		free(erased);
}
*/

// Commented out by KStankovic
/*
int VRMLModel::optimiseVRML(int **newInd, int **erased)
{
	for(int i = 0;i < nCoordLists;i++)
	{
		if(newInd && erased)
			optimiseVRMLSurface(&coordListSize[i],coordList[i],&triListSize[i],triList[i],3,&newInd[i],&erased[i]);
		else
			optimiseVRMLSurface(&coordListSize[i],coordList[i],&triListSize[i],triList[i],3,0,0);
		if(tCoordListSize[i])
			optimiseVRMLSurface(&tCoordListSize[i],tCoordList[i],&texIndListSize[i],texIndList[i],2,0,0);
		if(!normalsCalculated[i])
			optimiseVRMLSurface(&normListSize[i],normList[i],&normIndListSize[i],normIndList[i],3,0,0);
		if(cpvListSize[i])
			optimiseVRMLSurface(&cpvListSize[i],cpvList[i],&cpvIndListSize[i],cpvIndList[i],3,0,0);
	}
	
	return 1;
}
*/
//

// Commented out by KStankovic
/*
int VRMLModel::optimiseVRML()
{
	return optimiseVRML(0,0);
}
*/
//


//////////////////////////////////////////////////////////////////////
// VRMLModelModifier Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VRMLModelModifier::VRMLModelModifier()
{
	MTData = 0;
	TRotData = 0;
	TScalData = 0;
	TTranData = 0;
	refValue = -1;
}

VRMLModelModifier::~VRMLModelModifier()
{

}

void VRMLModel::traverseMesh(VRMLModel *model,    // pointer to the traversed model
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
							 VRMLMeshBones *bones		// pointer to the list of bones for this mesh
							 )
{
	int j,k;
	VRMLModel *currentModel = (VRMLModel *) model;

	float nativeBoneWeight = 1.0f;
	switch(traverseType)
	{
	case TT_INIT:
		meshNode[index] = thisNode;
		break;
	case TT_MORPH:
		if(!mt->changed)
			return;
		mt->changed = false;
		for(k=0;k<mt->nMTs;k++)
			assert(mt->vertexDisplacement[k]);// just in case...
		for(j = 0;j<coordLength;j++)
		{
//			coord[j] = origCoord[j];
			for(k=0;k<mt->nMTs;k++)
			{
                                coord[j] += mt->vertexDisplacement[k][j] * mt->weight[k];
			}
		}
		break;
	case TT_SKIN:
		float v[3];
		float v1[3];
		if(!bones->changed)
			return;
		bones->changed = false;
		if(bones->nBones == 0)  // if there are no bones, there is no skinning to do
			return;
		int i;
		for(k=0;k<bones->nBones;k++)
			assert(bones->vertexWeight[k]);// just in case...
		for(j = 0;j<coordLength/3;j++) // loop for each vertex
		{
			for(i=0;i<3;i++)
				v[i] = 0.0f;  // we will use v to add up the contributioons of all bones to the final vertex position

			nativeBoneWeight = 1.0f;
			for(k=0;k<bones->nBones;k++)  // loop for each bone
			{
				if(bones->vertexWeight[k][j] == 0.0f) // if the weight of this bone on this vertex is 0, there is no contributioon from this bone
					continue;
				nativeBoneWeight-=bones->vertexWeight[k][j]; // the weight of the native bone on this vertex is 1 - (sum of the weights of all non-native bones)
				multVect3Mat(v1,&bones->bindCoords[k][j*3],currentModel->toWC[bones->boneIndex[k]]); // multiply the bind coordinates by the transformation to the global coordinates; v1 now contains the position that the vertex would have if it influenced only by this bone, expressed in global coordinates
				for(i=0;i<3;i++)
					v[i] += v1[i] * bones->vertexWeight[k][j];  // add the weighted position - this is the contribution of this bone to the final global coordinate position of the vertex
			}

			multVect3Mat(v1,&origCoord[j*3],currentModel->toWC[currentModel->transfInd[index]]); // we do the same for the native bone - v1 now contains the gloabal position that vertex would have if only native bone was influencing it
			for(i=0;i<3;i++)
				v[i] += v1[i] * nativeBoneWeight;  // add the contribution of the native bone; now v contains the final position of the vertex, in global coordinates
			multVect3Mat(&coord[j*3],v,currentModel->fromWC[currentModel->transfInd[index]]);  // we transfer the position back to local coordinates of the current transform, because this transform will be applied by the renderer; this is inefficient and a better mechanism will be implemented to allow global coordinates to be passed to the renderer
		}
		break;
	case TT_FIND_TRANSFORM_MESH:
		if(tmt_foundTransform && tmt_foundMesh == -1)
			tmt_foundMesh = index;
		break;
	default:
		break;
	}
}
	
int VRMLModel::traverseTransform(VRMLModel *model,    // pointer to the traversed model
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
	switch(traverseType)
	{
	case TT_INIT:
		transfNode[index] = thisNode;
		break;
	case TT_REMOVE_NUS:
		if(strstr(thisNode->getName(),"Bip") == thisNode->getName()) // get only the nodes whose name begins with "Bip"
		{ 
			myMatrix cur_mat;
			myMatrix par_mat;
			myMatrix rel_mat;
			myMatrix inv_par_mat;
			if(thisNode->parent->type == NT_TRANSFORM)
				copyMat(par_mat,model->toWC[thisNode->parent->ind]);
			else
				identityMat(par_mat);
			
			copyMat(cur_mat,model->toWC[thisNode->ind]);
			invMat(inv_par_mat,par_mat);
			
			multMat(rel_mat,cur_mat,inv_par_mat);
			
			for(int i =0;i<3;i++)
			{
				if(t[i] != 0.0f)
					model->transfList[index][17+i] = rel_mat[3][i] / t[i];
				t[i] = rel_mat[3][i];
			}
			
			s[0] = s[1] = s[2] = 1;
			
			model->transfList[index][0] = t[0];
			model->transfList[index][1] = t[1];
			model->transfList[index][2] = t[2];
			model->transfList[index][7] = s[0];
			model->transfList[index][8] = s[1];
			model->transfList[index][9] = s[2];
			
			thisNode->updateNodeToWCMatrix(model,par_mat);
		}
		break;
	case TT_FIND_TRANSFORM_MESH:
		if(tmt_transform == index)
			tmt_foundTransform = true;
		break;
	default:
		break;
	}
	return 0;
}
	
void VRMLModel::applyMorphing()
{
	if(!initialized)
		return;

	traverseType = TT_MORPH;
	traverse(rootNode,this);
}

void VRMLModel::applySkinning()
{
	if(!initialized)
		return;

	traverseType = TT_SKIN;
	traverse(rootNode,this);
}

int VRMLModel::findTransformMesh(int ti)
{
	if(!initialized)
		return -1;

	traverseType = TT_FIND_TRANSFORM_MESH;
	tmt_foundMesh = -1;
	tmt_foundTransform = false;
	tmt_transform = ti;
	traverse(rootNode,this);

	return(tmt_foundMesh);
}

int VRMLModel::findNamedMesh(char *name)
{
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i=0;i<nCoordLists;i++)
	{
		if(!strcmp(name,meshNode[i]->getName()))
			break;
	}
	if(i==nCoordLists)
		i=-1;
	return(i);
}

int VRMLModel::findNamedTransform(char *name)
{
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i=0;i<nTransfLists;i++)
	{
		if(!strcmp(name,transfNode[i]->getName()))
			break;
	}
	if(i==nTransfLists)
		i=-1;
	return(i);
}

void VRMLModel::removeNonUniformScale()
{
	updateToWCMatrices();
	traverseType = TT_REMOVE_NUS;
	traverse(rootNode,this);
}

void VRMLModel::recalculateNormals()
{
	for (int i = 0; i < nCoordLists; i++)
	{
		NPV[i] = 1;
		int nc = coordListSize[i];
		int ni = triListSize[i];
		normList[i] = (float *) malloc(nc * sizeof(float));
		normListSize[i] = nc;

		generateNormals(normList[i],coordList[i],triList[i],nc,ni,ccw[i]);

		normIndListSize[i] = ni;
		normIndList[i] = (int *) malloc(ni * sizeof(int));

		for (int j = 0; j < ni; j++)
			normIndList[i][j] = triList[i][j];
	}
}

VRMLMeshBones::~VRMLMeshBones()
{
	for (int i = 0; i < nBones; i++)
	{
		if (vertexWeight[i] != NULL) free(vertexWeight[i]);
		if (bindCoords[i] != NULL) free(bindCoords[i]);
	}
};

}
