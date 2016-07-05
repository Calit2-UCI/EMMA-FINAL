
#include "FaceModelData.h"

#define REQ_FDPS "2.1 2.2 2.3 2.4 2.5 2.6 2.7 2.8 2.9 2.10 2.11 2.12 2.13 2.14 " \
	"3.1 3.2 3.3 3.4 3.7 3.8 3.9 3.10 3.11 3.12 3.13 3.14 " \
	"4.1 4.2 4.3 4.4 4.5 4.6 " \
	"5.1 5.2 5.3 5.4 " \
	"8.1 8.2 8.3 8.4 8.5 8.6 8.7 8.8 " \
	"9.1 9.2 9.3 9.6 9.7 9.12 9.13 9.14 9.15 " \
	"10.7 10.8 10.9 10.10 " \
	"11.1 11.2 11.3 "

namespace VisageSDK
{

FaceModelData::FaceModelData()
{
	reset();
}

FaceModelData::FaceModelData( VRMLModel* model, FDP* fdp )
{
	processModel( model, fdp );
}

FaceModelData* FaceModelData::deriveFaceModelDataForTargetModel(VRMLModel *model, VRMLModel *sourceModel)
{
	FaceModelData* data = new FaceModelData();

	if( data->processModel( model, sourceModel,
		faceMesh, lEyeMesh, nLEyeMesh, rEyeMesh, nREyeMesh,
		uTeethMesh, lTeethMesh, nLTeethMesh,
		tongueMesh, nTongueMesh, const_cast<FeaturePoint*>(&fdp->getFP(7,1)) )
		)
		return data;
	else
		return NULL;
}

bool FaceModelData::processModel( VRMLModel* model, FDP* fdp )
{
	assert( model && fdp );

	reset();

	this->model = model;
	this->fdp = fdp;

	convertToWC();

	//verify that all required feature points are defined
	for( int i = 2; i < 12; i++ )
	{
		for( int j = 1; j <= fdp->grSize[i]; j++ )
		{
			FeaturePoint *fp = const_cast<FeaturePoint*>(&fdp->getFP(i,j));
			char fpstr[7];
			sprintf( fpstr, "%d.%d ", i, j );
			
			if( !fp->defined && strstr( REQ_FDPS, fpstr ) )
			{
				if( errors == "" )
					errors.append( "Missing feature points " );
				else
					errors.append( ", " );

				errors.append( fpstr );
			}
		}
	}

	//a feature point is missing, terminate
	if( errors != "" ) return false;

	if( model->nCoordLists == 0 )
	{
		errors.append( "No meshes in the model.\n" );
		return false;
	}
	chooseAnimatedSurfaces();
	locateMeshes();
	if( model->nTransfLists == 0 )
	{
		errors.append( "No transform nodes in the model.\n" );
		return false;
	}
	findRotationCenters();

	findAABB();

	convertFromWC();

	return true;
}

bool FaceModelData::processModel( VRMLModel* model, VRMLModel *sourceModel, int faceMesh, int* lEyeMesh, int nLEyeMesh,
		int* rEyeMesh, int nREyeMesh, int uTeethMesh, int* lTeethMesh, int nLTeethMesh,
		int* tongueMesh, int nTongueMesh, FeaturePoint* globalRotationFP )
{
	assert( model );
	assert( globalRotationFP && globalRotationFP->defined );

	reset();

	this->model = model;
	this->sourceModel = sourceModel;

	convertToWC();

	this->faceMesh = faceMesh;
	memcpy( this->lEyeMesh, lEyeMesh, nLEyeMesh * sizeof(int) );
	this->nLEyeMesh = nLEyeMesh;
	memcpy( this->rEyeMesh, rEyeMesh, nREyeMesh * sizeof(int) );
	this->nREyeMesh = nREyeMesh;
	this->uTeethMesh = uTeethMesh;
	memcpy( this->lTeethMesh, lTeethMesh, nLTeethMesh * sizeof(int) );
	this->nLTeethMesh = nLTeethMesh;
	memcpy( this->tongueMesh, tongueMesh, nTongueMesh * sizeof(int) );
	this->nTongueMesh = nTongueMesh;
	//changed by LStamac
	//Code for copying the vertex position into the globalRotationCenter, in case the source and target model face mesh dont have the same index.
	int a = model->findNamedMesh("skin_hi");
	int mi; // added by Ratty
	if(a == -1)
	{
		a = model->findNamedMesh("skin_med");
	}
	if(a == -1)
	{
		a = model->findNamedMesh("skin_lo");
	}
	if(a != -1)
	{
		memcpy( globalRotationCenter, 
		&model->coordList[a][3*globalRotationFP->vert],
		3*sizeof(float) );
		GRFP = globalRotationFP;
	}
	//Code for copying the vertex position into the globalRotationCenter, in case both the source and target model face mesh have the same index.
	else if( ( mi = model->findNamedMesh( (char*)globalRotationFP->surf.c_str() ) ) > -1 && 
		globalRotationFP->vert < model->coordListSize[mi] )
	{
		memcpy( globalRotationCenter,
		&model->coordList[mi][3*globalRotationFP->vert],
		3*sizeof(float) );
	}
	else
		memcpy( globalRotationCenter, globalRotationFP->pos, 3*sizeof(float) );
	
	findRotationCenters( true );

	findAABB();

	convertFromWC();

	return true;
}

void FaceModelData::reset()
{
	model = NULL;
	fdp = NULL;
	nAnimPoints = 0;
	memset( animated, 0, 50 * sizeof(int) );
	memset( animated_strict, 0, 50 * sizeof(int) );
	errors = "";
	faceMesh = -1;
	memset( lEyeMesh, -1, 50 * sizeof(int) );
	memset( rEyeMesh, -1, 50 * sizeof(int) );
	uTeethMesh = -1;
	memset( lTeethMesh, -1, 50 * sizeof(int) );
	memset( tongueMesh, -1, 50 * sizeof(int) );
	nLEyeMesh = nREyeMesh = nLTeethMesh = nTongueMesh = -1;
	globalTransf = lEyeTransf = rEyeTransf = uTeethTransf = lTeethTransf = tongueTransf = -1;
	xMin = yMin = zMin = FLT_MAX;
	xMax = yMax = zMax = -FLT_MAX;
}

void FaceModelData::chooseAnimatedSurfaces()
{
	nAnimPoints = 0;
	int nfpa=0;
	float *fpa[100];
	
	fpa[nfpa++] = const_cast<float*>(fdp->getFPPos(3,5));
	fpa[nfpa++] = const_cast<float*>(fdp->getFPPos(3,6));
	fpa[nfpa++] = const_cast<float*>(fdp->getFPPos(9,8));
	fpa[nfpa++] = const_cast<float*>(fdp->getFPPos(9,9));
	fpa[nfpa++] = const_cast<float*>(fdp->getFPPos(9,10));
	fpa[nfpa++] = const_cast<float*>(fdp->getFPPos(9,11));
	fpa[nfpa++] = const_cast<float*>(fdp->getFPPos(6,1));
	
	int i, j;
	for(i = 2;i <12;i++)
	{
		for(j=1;j<=fdp->grSize[i];j++)
		{
			int mi;
			FeaturePoint *fp = const_cast<FeaturePoint*>(&fdp->getFP(i,j));
			if(!fp->defined) continue;
			if(!fp->animated) continue;
			if(fp->surf == "") continue;
			if( ( mi = model->findNamedMesh( (char*)fp->surf.c_str() ) ) <= -1 )
				continue;
			if(animated[mi]) continue;
			animated[mi] = 1;
			animated_strict[mi] = 1;
			int s[300],ns=0;
			findAdjacentSurfaces(mi, s, &ns, nfpa,fpa);
			for(int k=0;k<ns;k++)
				animated[s[k]] = 1;
		}
	}
	for(i=0;i<model->nCoordLists;i++)
	{
		if(!animated[i]) continue;
		nAnimPoints+=model->coordListSize[i]/3;
	}
}

void FaceModelData::locateMeshes()
{
	faceMesh = model->findNamedMesh( (char*)fdp->getFP(2,1).surf.c_str() );
	if( fdp->getFP(9,10).defined ) uTeethMesh = model->findNamedMesh( (char*)fdp->getFP(9,10).surf.c_str() );

	lEyeMesh[0] = model->findNamedMesh( (char*)fdp->getFP(3,5).surf.c_str() );
	nLEyeMesh = 1;
	findAdjacentSurfaces( lEyeMesh[0], lEyeMesh, &nLEyeMesh, NULL );

	rEyeMesh[0] = model->findNamedMesh( (char*)fdp->getFP(3,6).surf.c_str() );
	nREyeMesh = 1;
	findAdjacentSurfaces( rEyeMesh[0], rEyeMesh, &nREyeMesh, NULL );

	lTeethMesh[0] = model->findNamedMesh( (char*)fdp->getFP(9,9).surf.c_str() );
	nLTeethMesh = 1;
	findAdjacentSurfaces( lTeethMesh[0], lTeethMesh, &nLTeethMesh, (float*)&fdp->getFP(9,8).pos[0] );

	tongueMesh[0] = model->findNamedMesh( (char*)fdp->getFP(6,1).surf.c_str() );
	nTongueMesh = 1;
	findAdjacentSurfaces( tongueMesh[0], tongueMesh, &nTongueMesh, (float*)&fdp->getFP(9,9).pos[0] );
}

bool FaceModelData::surfaceContainsPoint(int s, float *p)
{
	for(int j=0;j<model->coordListSize[s];j+=3)
	{
		float* vrt = &model->coordList[s][j];
		if( pointsEqual3D(vrt,p))
			return(1);
	}
	return false;
}

int FaceModelData::findSurfaceContainingPoint(float *p, int s0)
{
	for(int i=0;i<model->nCoordLists;i++)
	{
		if(i==s0) continue;
		if(animated[i]) continue;
		if(surfaceContainsPoint(i,p))
			return(i);
	}
	return(-1);
}

void FaceModelData::findAdjacentSurfaces(int s0, int *s, int *ns, float *fpa)
{
	int i,j;
	int ns0 = (*ns);
	for(j=0;j<model->coordListSize[s0];j+=3)
	{
		float* vrt = &model->coordList[s0][j];
		int s1 = findSurfaceContainingPoint(vrt,s0);

		if(s1 == -1) continue;

		for(i=0;i<(*ns);i++)
			if(s[i] == s1) break;
			
		if(i<(*ns)) continue;

		if(fpa)
			if(surfaceContainsPoint(s1,fpa)) continue;
			
		s[(*ns)] = s1;		
		(*ns)++;
	}
	for(i=ns0;i<(*ns);i++)
		findAdjacentSurfaces(s[i],s,ns,fpa);
}

void FaceModelData::findAdjacentSurfaces(int s0, int *s, int *ns, int nfpa, float **fpa)
{
	int i,j,k;
	int ns0 = (*ns);
	for(j=0;j<model->coordListSize[s0];j+=3)
	{
		float * vrt = &model->coordList[s0][j];
		int s1 = findSurfaceContainingPoint(vrt,s0);
		if(s1 == -1) continue;
		for(i=0;i<(*ns);i++)
		{
			if(s[i] == s1) 
				break;
		}
		if(i<(*ns)) continue;
		for(k=0;k<nfpa;k++)
		{
			if(fpa[k])
				if(surfaceContainsPoint(s1,fpa[k])) 
					break;
		}
		if(k<nfpa) continue;
		s[(*ns)] = s1;
		(*ns)++;
	}
	for(i=ns0;i<(*ns);i++)
		findAdjacentSurfaces(s[i],s,ns,nfpa,fpa);
}

bool FaceModelData::findRotationCenters( bool isTarget )
{
        float ctr[] = {0.0, 0.0, 0.0};
        int t;

        /*check that the model is a target model and is a FaceGen model.
         *FaceGen models are exported to VRML without Transform nodes,
         *thus nTransfLists will be 0
         */
        if(isTarget && model->nTransfLists == 0)
        {
                /*Because FaceGen exports a model to VRML without transform nodes,
                 *we will have to build a hierarchy of transforms.
                 */

                //root transform node of the entire model
                VRMLNode *root = addTransform("FaceGenRoot", model->rootNode);
                VRMLNode *head = addTransform("Head", root);
                VRMLNode *face = addTransform("Face",  head);

                //due to some requirements for the VRML model,
                //each eye needs an additional transform node above it in the hierarchy.
                VRMLNode *transfLE = addTransform("TransfLE", face);
                VRMLNode *eyeL = addTransform("EyeL", transfLE);
                VRMLNode *transfRE = addTransform("TransfRE",  face);
                VRMLNode *eyeR = addTransform("EyeR", transfRE);

                VRMLNode *teethL = addTransform("TeethL", face);
                //transform node of the upper teeth
                VRMLNode *teethU = addTransform("TeethU", face);
                //transform node of the tongue
                VRMLNode *tongue = addTransform("Tongue", face);
                //transform node of the sock
                VRMLNode *sock = addTransform("Sock", face);

                //Now that all the necessary transform nodes have been created and some initial data set,
                //they need to be connected into a proper hierarchy.

                //add the left eye mesh as a child to the EyeL transform
                if(model->findNamedMesh("eyeL_hi") != -1)
                        eyeL->addChild(model->meshNode[model->findNamedMesh("eyeL_hi")]);
                else
                        eyeL->addChild(model->meshNode[model->findNamedMesh("eyeL_lo")]);
                //set the parent of the left eye mesh to the EyeL transform
                eyeL->children[0]->parent = eyeL;
                //add the EyeL transform as a child to the transfLE transform
                transfLE->addChild(eyeL);
                //add the right eye mesh as a child to the EyeR transform
                if(model->findNamedMesh("eyeR_hi") != -1)
                        eyeR->addChild(model->meshNode[model->findNamedMesh("eyeR_hi")]);
                else
                        eyeR->addChild(model->meshNode[model->findNamedMesh("eyeR_lo")]);
                //set the parent of the right eye mesh to the EyeR transform
                eyeR->children[0]->parent = eyeR;
                //add the EyeR transform as a child to the transfRE transform
                transfRE->addChild(eyeR);
                //lower teeth
                teethL->addChild(model->meshNode[model->findNamedMesh("teethLower")]);
                teethL->children[0]->parent = teethL;
                //upper teeth
                teethU->addChild(model->meshNode[model->findNamedMesh("teethUpper")]);
                teethU->children[0]->parent = teethU;
                //tongue
                tongue->addChild(model->meshNode[model->findNamedMesh("tongue")]);
                tongue->children[0]->parent = tongue;
                //sock
                sock->addChild(model->meshNode[model->findNamedMesh("sock")]);
                sock->children[0]->parent = sock;

                //face
                if(model->findNamedMesh("skin_hi") != -1)
                        face->addChild(model->meshNode[model->findNamedMesh("skin_hi")]); //skin
                else if(model->findNamedMesh("skin_med") != -1)
                        face->addChild(model->meshNode[model->findNamedMesh("skin_med")]);
                else
                        face->addChild(model->meshNode[model->findNamedMesh("skin_lo")]);
                face->children[0]->parent = face;
                face->addChild(transfLE);
                face->addChild(transfRE);
                face->addChild(teethL);
                face->addChild(teethU);
                face->addChild(tongue);
                face->addChild(sock);

                //sock
                if (model->findNamedMesh("sock")!= -1)
                {
                        model->meshNode[model->findNamedMesh("sock")] = sock->children[0];
                        model->transfInd[model->findNamedMesh("sock")] = 10; //face
                }
                //hair
                if (model->findNamedMesh("HairGraduatedBob")!= -1)
                {
                        face->addChild(model->meshNode[model->findNamedMesh("HairGraduatedBob")]);
                        face->children[face->nChildren-1]->parent = face;
                        model->meshNode[model->findNamedMesh("HairGraduatedBob")] = face->children[face->nChildren-1];
                        model->transfInd[model->findNamedMesh("HairGraduatedBob")] = 2; //face
                }
                if (model->findNamedMesh("hairShortBlack")!= -1)
                {
                        face->addChild(model->meshNode[model->findNamedMesh("hairShortBlack")]);
                        face->children[face->nChildren-1]->parent = face;
                        model->meshNode[model->findNamedMesh("hairShortBlack")] = face->children[face->nChildren-1];
                        model->transfInd[model->findNamedMesh("hairShortBlack")] = 2; //face
                }

                //eye glasses
                if (model->findNamedMesh("eyeGlasses")!= -1)
                {
                        face->addChild(model->meshNode[model->findNamedMesh("eyeGlasses")]);
                        face->children[face->nChildren-1]->parent = face;
                        model->meshNode[model->findNamedMesh("eyeGlasses")] = face->children[face->nChildren-1];
                        model->transfInd[model->findNamedMesh("eyeGlasses")] = 2; //face
                }
                //head
                head->addChild(face);
                //root
                root->addChild(head);
                model->rootNode->children[0] = root;
               
                //the meshNode member is linked to the appropriate meshes
                if(model->findNamedMesh("eyeL_hi") != -1)
                        model->meshNode[model->findNamedMesh("eyeL_hi")] = eyeL->children[0]; //left eye
                else
                        model->meshNode[model->findNamedMesh("eyeL_lo")] = eyeL->children[0]; //left eye
                if(model->findNamedMesh("eyeR_hi") != -1)
                        model->meshNode[model->findNamedMesh("eyeR_hi")] = eyeR->children[0]; //right eye
                else
                        model->meshNode[model->findNamedMesh("eyeR_lo")] = eyeR->children[0]; //right eye
                if(model->findNamedMesh("skin_hi") != -1)
                        model->meshNode[model->findNamedMesh("skin_hi")] = face->children[0]; //skin
                else if(model->findNamedMesh("skin_med") != -1)
                        model->meshNode[model->findNamedMesh("skin_med")] = face->children[0]; //skin
                else
                        model->meshNode[model->findNamedMesh("skin_lo")] = face->children[0]; //skin
                model->meshNode[model->findNamedMesh("teethLower")] = teethL->children[0]; //lower teeth
                model->meshNode[model->findNamedMesh("teethUpper")] = teethU->children[0]; //upper teeth
                model->meshNode[model->findNamedMesh("tongue")] = tongue->children[0]; //tongue

                //the transfInd member is linked to the appropriate transform indexes
                if(model->findNamedMesh("eyeL_hi") != -1)
                        model->transfInd[model->findNamedMesh("eyeL_hi")] = 4; //left eye
                else
                        model->transfInd[model->findNamedMesh("eyeL_lo")] = 4; //left eye
                if(model->findNamedMesh("eyeR_hi") != -1)
                        model->transfInd[model->findNamedMesh("eyeR_hi")] = 6; //right eye
                else
                        model->transfInd[model->findNamedMesh("eyeR_lo")] = 6; //right eye
                if(model->findNamedMesh("skin_hi") != -1)
                        model->transfInd[model->findNamedMesh("skin_hi")] = 2; //face
                else if(model->findNamedMesh("skin_med") != -1)
                        model->transfInd[model->findNamedMesh("skin_med")] = 2; //face
                else
                        model->transfInd[model->findNamedMesh("skin_lo")] = 2; //face
               
                model->transfInd[model->findNamedMesh("teethLower")] = 7; //lower teeth
                model->transfInd[model->findNamedMesh("teethUpper")] = 8; //upper teeth
                model->transfInd[model->findNamedMesh("tongue")] = 9; //tongue

                //a node of the type NT_GROUP is added above the Root transform (not to be mistaken with the root node)
                VRMLNode *n = new VRMLNode(NT_GROUP,0,0,NULL,"",model->rootNode);
                n->addChild(root);
                n->children[0]->parent = n;
                model->rootNode->children[0] = n;

                /*Due to the model not having transforms prior to the code in this method (function),
                 *the ToWC (and fromWC) matrices are not set.
                 *This causes a problem when the convertToWC method is called in the processModel method,
                 *resulting in all the coordinates of the model being set to 0.0.
                 *Not only will this result in the model not displaying until being saved to a file and loaded from it,
                 *it will also result in the morph targets not being mapped to the model.
                 *This is fixed with the next segment of code.
                 */
                //ToWC and FromWC matrices of the newly created transforms are set
                model->updateToWCMatrices();
                //the coordinates before the conversion to world coordinates are restored
                for(int i=0; i < model->nCoordLists; ++i)
                {
                        for(int j=0; j < model->coordListSize[i]; ++j)
                        {
                                model->coordList[i][j] = model->origCoordList[i][j];
                        }
                }
                //now that the ToWC and FromWC matrices are correct, the conversion to world coordinates is repeated.
                convertToWC();

                /**
                 * This is a fix for a small issue that occurs due to the convertToWC function executed in the processModel function in the target model.

                 * Since the FaceGen model doesn't have any transform nodes at that point,
                 * convertToWC will convert all the coordinates in the model incorrectly, setting them all to 0.0.
                 * Thus, after the transforms are in place and the convertToWC function above is executed (setting the coordinates correctly)

                 * the globalRotationCenter is set with the correct coordinates.
         */
                int a = model->findNamedMesh("skin_hi");
                if(a == -1)
                {
                        a = model->findNamedMesh("skin_med");
                }
                if(a == -1)
                {
                        a = model->findNamedMesh("skin_lo");
                }
                memcpy( globalRotationCenter,
                        &model->coordList[a][3*GRFP->vert],
                        3*sizeof(float) );

                float ctr2[] = {0.0, 0.0, 0.0};
                memcpy( ctr2, &globalRotationCenter[0], 3*sizeof(float) );
                for(int j=0; j<3; j++)
                        model->transfList[model->findNamedTransform("Face")][j+14] = ctr2[j];
                for(int j=0; j<3; j++)
                        model->transfList[model->findNamedTransform("Head")][j+14] = ctr2[j];

                int b = model->findNamedMesh("eyeL_hi");
                if(b == -1)
                {
                        b = model->findNamedMesh("eyeL_lo");
                }
                computeEyeCenter(ctr2,1,&b);
                for(int j=0; j<3; j++)
                        model->transfList[model->findNamedTransform("EyeL")][j+14] = ctr2[j];
                for(int j=0; j<3; j++)
                        model->transfList[model->findNamedTransform("TransfLE")][j+14] = ctr2[j];

                int c = model->findNamedMesh("eyeR_hi");
                if(c == -1)
                {
                        c = model->findNamedMesh("eyeR_lo");
                }
                computeEyeCenter(ctr2,1,&c);
                for(int j=0; j<3; j++)
                        model->transfList[model->findNamedTransform("EyeR")][j+14] = ctr2[j];
                for(int j=0; j<3; j++)
                        model->transfList[model->findNamedTransform("TransfRE")][j+14] = ctr2[j];

                //the teeth meshes are moved backwards a bit,
                //to avoid the problem of them overlapping with the face mesh during animation
                model->transfList[ model->meshNode[model->findNamedMesh( "teethUpper")]->parent->ind][2] -= 5.0;
                model->transfList[ model->meshNode[model->findNamedMesh( "teethLower")]->parent->ind][2] -= 5.0;
                model->transfList[ model->meshNode[model->findNamedMesh( "tongue")]->parent->ind][2] -= 5.0;
                model->transfList[ model->meshNode[model->findNamedMesh( "sock" )]->parent->ind][2] -= 5.0;

               
        }

        /*
         * Although it is assumed in the processModel method that the source and target models have the same indexes for coresponding meshes,

         * this may not be the case. Thus, the correct index needs to be put into the variables below (e.g. this->faceMesh).

         * Since the names of the transforms are already known,
         * the index of their meshes can be retrieved with the findTransformMesh and findNamedTransform methods.
         */

        if(model->findNamedTransform("FaceGenRoot") != -1)
        {
                //ensure that there is only one eye mesh (FaceGen models have only one mesh)
                this->nLEyeMesh = this->nREyeMesh = 1;
                if((model->findTransformMesh(model->findNamedTransform("Face")) != faceMesh) ||
                   (model->findTransformMesh(model->findNamedTransform("EyeL")) != lEyeMesh[0]) ||
                   (model->findTransformMesh(model->findNamedTransform("EyeR")) != rEyeMesh[0]) ||
                   (model->findTransformMesh(model->findNamedTransform("TeethL")) != lTeethMesh[0]) ||
                   (model->findTransformMesh(model->findNamedTransform("TeethU")) != uTeethMesh) ||
                   (model->findTransformMesh(model->findNamedTransform("Tongue")) != tongueMesh[0]))
                {
                        this->faceMesh = model->findTransformMesh(model->findNamedTransform("Face"));
                        this->lEyeMesh[0] = model->findTransformMesh(model->findNamedTransform("EyeL"));
                        this->rEyeMesh[0] = model->findTransformMesh(model->findNamedTransform("EyeR"));
                        this->lTeethMesh[0] = model->findTransformMesh(model->findNamedTransform("TeethL"));
                        this->uTeethMesh = model->findTransformMesh(model->findNamedTransform("TeethU"));
                        this->tongueMesh[0] = model->findTransformMesh(model->findNamedTransform("Tongue"));
                }
                /*
                 * Make sure that the eyes and face transforms have the correct center of rotation.
                 * Due to the structure of the VRML model,
                 * the two transforms controling the face need to be set with the correct center of rotation.
                 * Likewise for the eyes.
                 */
                float *ctr = fdp ? const_cast<float*>(fdp->getFPPos(7,1)) : &globalRotationCenter[0];
                float ctr1[] = {0.0, 0.0, 0.0};
                float ctr2[] = {0.0, 0.0, 0.0};
                computeEyeCenter(ctr1,nLEyeMesh,this->lEyeMesh);
                computeEyeCenter(ctr2,nREyeMesh,this->rEyeMesh);
                for(int j=0; j < 3; ++j)
                {
                        model->transfList[model->findNamedTransform("Head")][j+14] = ctr[j];
                        model->transfList[model->findNamedTransform("Face")][j+14] = ctr[j];
                        model->transfList[model->findNamedTransform("TransfLE")][j+14] = ctr1[j];
                        model->transfList[model->findNamedTransform("EyeL")][j+14] = ctr1[j];
                        model->transfList[model->findNamedTransform("TransfRE")][j+14] = ctr2[j];
                        model->transfList[model->findNamedTransform("EyeR")][j+14] = ctr2[j];
                }
        }

        //find rotation center of the head
        float *ctr1 = fdp ? const_cast<float*>(fdp->getFPPos(7,1)) : &globalRotationCenter[0];
        t = findGlobalMotionTransform( isTarget );
        if( t == -1 )
        {
                //If this is a full-body character modeled with Biped,
                //then we don't need to find a Transform that parents
                //all head meshes and none other.
                //Instead, we will simply use the Transform which parents all
                //meshes *but* the facial mesh (i.e. eyes, tongue etc.).
                //This Transform is none other than Bip01_Head, which already
                //transforms the facial mesh via physique.
                globalTransf = findGlobalMotionTransform( isTarget, true );
        }
        else if(ctr1 && t!=-1)
        {
                for(int i=0; i<3; i++)
                        ctr[i] = ctr1[i];
                this->globalTransf = arrangeRotationCenter(ctr,t);
        }
        else
        {
                errors.append( "Unable to locate global Transform." );
        }
       
        //find rotation centers of the eyes
        if(nLEyeMesh)
        {
                computeEyeCenter(ctr,nLEyeMesh,this->lEyeMesh);
                t = findTransformContainingSurfaces(nLEyeMesh,this->lEyeMesh);
                if(t!=-1)
                {
                        this->lEyeTransf = arrangeRotationCenter(ctr,t);
                }
                else
                {
                        errors.append( "Unable to locate left eye Transform.\n" );
                }
        }
        if(nREyeMesh)
        {
                computeEyeCenter(ctr,nREyeMesh,this->rEyeMesh);
                t = findTransformContainingSurfaces(nREyeMesh,this->rEyeMesh);
                if(t!=-1)
                {
                        this->rEyeTransf = arrangeRotationCenter(ctr,t);
                }
                else
                {
                        errors.append( "Unable to locate right eye Transform.\n" );
                }
        }      

        this->uTeethTransf = model->transfInd[this->uTeethMesh];
        this->lTeethTransf = findTransformContainingSurfaces( nLTeethMesh, this->lTeethMesh );
        this->tongueTransf = findTransformContainingSurfaces( nTongueMesh, this->tongueMesh );
       
        if(this->globalTransf == -1 || this->lEyeTransf == -1 || this->rEyeTransf == -1 || this->lTeethTransf == -1 ||
                this->uTeethTransf == -1 || this->tongueTransf == -1)
                return false;
        return true;
} 

void FaceModelData::computeEyeCenter(float *c,int ns,int *s)
{
	int i,j,k;
	float r;
	float min[3],max[3];
	for(k=0;k<3;k++)
	{
		min[k] = 1e10;
		max[k] = -1e10;
		c[k] = 0.0;
	}
	
	int cnt=0;
	for(i=0;i<ns;i++)
	{
		for(j=0;j<model->coordListSize[s[i]];j+=3)
		{
			float * v = &model->coordList[s[i]][j];
			cnt++;
			for(k=0;k<3;k++)
			{
				if(v[k] < min[k]) min[k] = v[k];
				if(v[k] > max[k]) max[k] = v[k];
				c[k] += v[k];
			}
		}
	}
	for(k=0;k<2;k++)
	{
		c[k] /= (float)cnt;
	}
	c[2]=max[2];
	r = (max[0] - min[0]) / 2;
	c[2]-=r;
}

int FaceModelData::findTransformContainingSurfaces(int nRSurf,int *rSurf)
{
	if(nRSurf<=0) return(-1);
	int t = model->transfInd[rSurf[0]];
	int l = model->transfLevel[t];
	do
	{
		if(transformContainsSurfaces(t,nRSurf,rSurf))
			return(t);
		//go one level up
		l--;
		while(l>0 && t>0)
		{
			if(model->transfLevel[--t] == l) break;
		}		
	}
	while(l>0);

	return -1;
}

bool FaceModelData::transformContainsTransform(int t1,int t2)
{
	if(t1==t2)
		return true;
	
	int l1 = model->transfLevel[t1];
	int l2 = model->transfLevel[t2];
	
	// transform can not contain another transform on same or higher level
	if(l2 <= l1)
		return false;
	
	// find next higher level transform from t2, then repeat the process
	int t=t2;
	while(t>=0)
	{
		if(model->transfLevel[t]<l2)
			break;
		t--;
	}
	if(t<0)
		return false;

	return transformContainsTransform(t1,t);
}

bool FaceModelData::transformContainsSurface(int t,int s)
{
	int st = model->transfInd[s];
	
	return(transformContainsTransform(t,st));
}

bool FaceModelData::transformContainsSurfaces(int t,int nRSurf,int *rSurf)
{
	for(int i=0;i<nRSurf;i++)
	{
		if(!transformContainsSurface(t,rSurf[i]))
			return false;
	}
	return true;
}

int FaceModelData::arrangeRotationCenter(float *ctr,int tran)
{
	//transform ctr from world coords into local coords just above tran
	myMatrix t,negt,twc,twc1,fwc;
	model->getTransfMatrix(t,model->transfList[tran]);
	copyMat(twc,model->toWC[tran]);
	invMat(negt,t);
	multMat(twc1,negt,twc);
	invMat(fwc,twc1);
	multVect3Mat(ctr,fwc);
	
	//Added by LStamac
	if(model->transfNode[tran]->parent->nChildren == 1)
		for(int j=0; j < 3; ++j)
			model->transfList[model->transfNode[tran]->parent->ind][j+14] = ctr[j];
	
	// check higher-level transform
	int level = model->transfLevel[tran];
	int t1 = tran-1;
	if(t1>=0)
	{
		if(model->transfLevel[t1]<level)
		{
			float *ctr1 = model->transfList[t1] + 14;
			
			if(pointsEqual3D(ctr,ctr1))
				return(t1);
		}
	}

	insertCenterTransform(tran,ctr);
	return(tran);
}

void FaceModelData::insertCenterTransform(int t,float c[3])
{
	int i,j;
	if(t==-1)
		return;

	model->transfList[model->nTransfLists] = (float*)malloc(17*sizeof(float));

	// shift all transform data after t
	for(i=model->nTransfLists;i>=t;i--)
	{
		if(i==0)
			continue;
		for(j=0;j<17;j++)
			model->transfList[i][j] = model->transfList[i-1][j];
		copyMat(model->toWC[i],model->toWC[i-1]);
		copyMat(model->fromWC[i],model->fromWC[i-1]);
		if(i!=t)
			model->transfLevel[i] = model->transfLevel[i-1];
	}
	//shift transform levels of nodes directly under inserted node
	i = t+1;
	do
	{
		model->transfLevel[i]++;
		i++;
		if(model->transfLevel[i] <= model->transfLevel[t])
			break;
	}
	while(i < model->nTransfLists);

	//set the transform with the center of rotation
	float r0[3] = {0.0f,0.0f,1.0f};
	float q0[4];
	aa2quat(q0,r0,0.0f);
	//translation
	for(j=0;j<3;j++)
		model->transfList[t][j] = 0.0f;
	// rotation
	for(j=0;j<4;j++)
		model->transfList[t][j+3] = q0[j];
	// scale
	for(j=0;j<3;j++)
		model->transfList[t][j+7] = 1.0f;
	// scale orientation
	for(j=0;j<4;j++)
		model->transfList[t][j+10] = q0[j];
	// center
	for(j=0;j<3;j++)
		model->transfList[t][j+14] = c[j];

	

	model->rootNode->insertTransform(t);

	// shift transform indices if needed
	if(globalTransf >= t)
		globalTransf++;
	if(lEyeTransf >= t)
		lEyeTransf++;
	if(rEyeTransf >= t)
		rEyeTransf++;
	for(i=0;i<model->nCoordLists;i++)
		if(model->transfInd[i] >= t)
			model->transfInd[i]++;

	//Added by LStamac
	for(i= model->nTransfLists - 1;i>=t;i--)
		model->transfNode[i+1] = model->transfNode[i];
	for(i=t-1; i>0; i--)
		if(transformContainsTransform(i,t))
			model->transfNode[t] = model->transfNode[i]->children[0];
	if (model->transfNode[t]->nChildren > 0)
		model->transfNode[t]->children[model->transfNode[t]->nChildren - 1]->parent = model->transfNode[t];
	model->nTransfLists++;
}

VRMLNode* FaceModelData::addTransform(char* name, VRMLNode* parentNode)
{
	int i;
	
	//allocate space for raw transform data of the transform
	model->transfList[model->nTransfLists] = (float*)malloc(17*sizeof(float));	

	//set the transform with the center of rotation
	float r0[3] = {0.0f,0.0f,1.0f};
	float q0[4];
	aa2quat(q0,r0,0.0f);
	//translation
	for(i=0; i<3; ++i)
		model->transfList[model->nTransfLists][i] = 0.0f;
	// rotation
	for(i=0; i<4; ++i)
		model->transfList[model->nTransfLists][i+3] = q0[i];
	// scale
	for(i=0; i<3; ++i)
		model->transfList[model->nTransfLists][i+7] = 1.0f;
	// scale orientation
	for(i=0; i<4; ++i)
		model->transfList[model->nTransfLists][i+10] = q0[i];
	// center
	for(i=0; i<3; ++i)
		model->transfList[model->nTransfLists][i+14] = 0.0f;

	//set the level of the transform

	//If the parent of the parent of the transform is a transform node, 
	//then the level of the current node is that of its parent + 1.
	if (parentNode->type == NT_TRANSFORM)
		model->transfLevel[model->nTransfLists] = model->transfLevel[parentNode->ind] + 1;
	//Otherwise, the level is 0.
	else 
		model->transfLevel[model->nTransfLists] = 0;

	//the node is created
	VRMLNode *n = new VRMLNode(NT_TRANSFORM, model->nTransfLists, 0, NULL, name, parentNode);
	//link the node with transfNode
	model->transfNode[model->nTransfLists] = n;
	model->nTransfLists++;

	return n;
}

int FaceModelData::findGlobalMotionTransform( bool isTarget, bool fullBodyModel )
{
	int i,ns=0,s[200];
	
	//if we're dealing with a full-body model with a skeleton,
	//we just search for a parent of eyes, teeth and tongue
	if( !fullBodyModel )
	{
		for(i=0;i<model->nCoordLists;i++)
		{
			if( !isTarget && animated_strict[i] )
				s[ns++] = i;
		}
	}
	if( isTarget )
		s[ns++] = faceMesh;
	for(i=0;i<nLEyeMesh;i++)
		s[ns++] = lEyeMesh[i];
	for(i=0;i<nREyeMesh;i++)
		s[ns++] = rEyeMesh[i];
	for(i=0;i<nLTeethMesh;i++)
		s[ns++] = lTeethMesh[i];
	for(i=0;i<nTongueMesh;i++)
		s[ns++] = tongueMesh[i];

	return(findTransformContainingSurfaces(ns,s));
}

bool FaceModelData::pointsEqual3D( float* p1, float* p2 )
{
	if( fabs( p1[0] - p2[0] ) <= 0.001f && 
		fabs( p1[1] - p2[1] ) <= 0.001f && 
		fabs( p1[2] - p2[2] ) <= 0.001f )
		return true;

	return false;
}

void FaceModelData::findAABB()
{
	int i,j;

	for( i = 0; i < model->nCoordLists; ++i )
	{
		for( j = 0; j < model->coordListSize[i]; j += 3 )
		{
			xMin = model->coordList[i][j] < xMin ? 
				model->coordList[i][j] : xMin;
			yMin = model->coordList[i][j+1] < yMin ? 
				model->coordList[i][j+1] : yMin;
			zMin = model->coordList[i][j+2] < zMin ? 
				model->coordList[i][j+2] : zMin;

			xMax = model->coordList[i][j] > xMax ? 
				model->coordList[i][j] : xMax;
			yMax = model->coordList[i][j+1] > yMax ? 
				model->coordList[i][j+1] : yMax;
			zMax = model->coordList[i][j+2] > zMax ? 
				model->coordList[i][j+2] : zMax;
		}
	}
}

void FaceModelData::convertToWC()
{
	int i,j;
	
	for(i=0;i<model->nCoordLists;i++)
	{
		for(j=0;j<model->coordListSize[i];j+=3)
		{
			float* vrt = &model->coordList[i][j];

			multVect3Mat(vrt,model->toWC[model->transfInd[i]]);
		}
	}

}

void FaceModelData::convertFromWC()
{
	int i,j;
	
	for(i=0;i<model->nCoordLists;i++)
	{
		for(j=0;j<model->coordListSize[i];j+=3)
		{
			float * vrt = &model->coordList[i][j];
			multVect3Mat(vrt,model->fromWC[model->transfInd[i]]);
		}
	}

}

}
