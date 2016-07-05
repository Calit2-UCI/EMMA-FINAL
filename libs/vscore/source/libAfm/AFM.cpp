// AFM.cpp : Defines the entry point for the DLL application.
//

#ifndef VISAGE_STATIC
#ifdef WIN32
#include "windows.h"  //necessary if VISAGE_STATIC=false for DLL declarations
#endif
#endif

#include "stdlib.h"
#include "math.h"
#include "AFM.h"
#include "VCM.h"
#include "Logger.h"

#include "BaseFileIO.h"
#include "BaseAFMDeformer.h"

#include <clocale>

#include <time.h>
#include <algorithm>

#include <cstdio>
#include <iostream>
#include "NaradaNotificationCenter.hpp"

#ifndef VISAGE_STATIC
	#ifdef WIN3255
	BOOL APIENTRY DllMain( HANDLE hModule, 
						   DWORD  ul_reason_for_call, 
						   LPVOID lpReserved
						 )
	{
		switch (ul_reason_for_call)
		{
			case DLL_PROCESS_ATTACH:
			case DLL_THREAD_ATTACH:
			case DLL_THREAD_DETACH:
			case DLL_PROCESS_DETACH:
				break;
		}
		return TRUE;
	}
	#else //Other O.S., standar C++ usage

#ifdef WIN32
	int __stdcall main(int argc, char* argv[])
#else
	int main(int argc, char* argv[])
#endif
	{
		return 1;
	}

	#endif

#endif


#define TT_READ_INTERPOLATORS 0
#define TT_INIT 1
#define TT_UPDATE_GEOM 2


namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//FILE *myLogFile;

AFM::AFM(VRMLModel *m0, VRMLModel **m, int *fv,float _coordSensitivity) : VisageCharModel("")
{
	init();
	faceModel = new VRMLModel(m0);

	setCoordSensitivity(_coordSensitivity);
	initAnimData(m,fv);

	// initialize FBAP mappings - added by Ratty
	int i;
	BaseAFMDeformer *d = NULL;
	for (i = 0; i < nDeformers; i++)
	{
		d = dynamic_cast<BaseAFMDeformer *>(deformers[i]);

		if( d != NULL ) break;
	}
	if( d == NULL ) return;

	for( int i = 0; i < NVISEME; ++i )
	{
		VisageCharModel::setFBAPMapping( getMapping( VisemeNameString[i], &d->VisemeModifiers[i] ) );
	}
	for( int i = 0; i < NEXP; ++i )
	{
		VisageCharModel::setFBAPMapping( getMapping( ExpressionNameString[i], &d->ExpressionModifiers[i] ) );
	}
	for( int i = 0; i < NFAP; ++i )
	{
		VisageCharModel::setFBAPMapping( getMapping( FAPNameString[i], &d->FAPModifiers[i] ) );
	}
	for( int i = 0; i < NBAP; ++i )
	{
		VisageCharModel::setFBAPMapping( getMapping( BAPNameString[i], &d->BAPModifiers[i] ) );
	}

	// added by Ratty
	VisageCharModel::init();
	//
}

AFM::AFM(char *fname) : VisageCharModel("")
{
	init();

	read(fname);

/*
	faceModel = new VRMLModel();

	AFMDeformer *_deformers[100];
	int _nDeformers;

	for(int i = 0; i<nFIO; i++)
	{
		if(FIO[i]->read(fname,getVCModel(), _deformers, &_nDeformers) == 1)
			break;
	}

	if(i==nFIO)
	{
		char msg[300];
		sprintf(msg,"Error 164\nCould not read file: >%s< (%s)\n",fname,strerror(NULL) );
//		MessageBox(0,msg,"Error",MB_ICONERROR);
		fprintf(stderr,"%s\n",msg);
		return;
	}

	for(i=0;i<_nDeformers;i++)
		addAFMDeformer(_deformers[i]);

	setCoordSensitivity(0.1f);
	*/
}


bool AFM::init()
{
	setlocale(LC_NUMERIC, "C");
	FAPs *f = new FAPs(); // this is just to enforce licensing

	memset( &deformers[0], NULL, 100 * sizeof(AFMDeformer*) );
	memset( &FIO[0], NULL, 100 * sizeof(FileIO*) );
	nDeformers = 0;
	nFIO = 1;
	FIO[0] = new BaseFileIO();

	faceModel = NULL;
	delete f;

	srand((unsigned) time(NULL));

	return true;
}

void AFM::computeSensitivityThreshold()
{
	float f = 0.0f;
	int cnt = 0;

	if(!faceModel)
		sensitivityThreshold = 0.0001f;

	for(int i = 0;i < faceModel->nCoordLists;i++)
	{
		for(int j=0;j<faceModel->coordListSize[i];j++)
		{
			f += faceModel->coordList[i][j];
			cnt++;
		}
	}
	f = myfabs(f);
	if(f>0.0f)
	{
		f /= (float)cnt;
		sensitivityThreshold = f * coordSensitivity / 100.0f;
	}
	else
		sensitivityThreshold = (float)0.0001;
}

void AFM::setCoordSensitivity(float cs)
{
	coordSensitivity = cs;
	computeSensitivityThreshold();
}


AFM::~AFM()
{
	// Added by KStankovic
	int i;
	for (i = 0; i < 100; i++)
	{
		if (FIO[i] != NULL) delete FIO[i];
		if (deformers[i] != NULL) delete deformers[i];
	}

	if (faceModel != NULL) delete faceModel;
	//
}


void AFM::write(char *fname)
{
	clearFBAPs();
	updateGeometry();
	
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i = 0; i<nFIO; i++)
	{
		if(FIO[i]->write(fname,getVCModel(), deformers,nDeformers) == 1)
			break;
	}

	if(i==nFIO)
	{
		char msg[300];
		sprintf(msg,"Error 164\nCould not write file: >%s< (%s)\n",fname,strerror(NULL) );
//		MessageBox(0,msg,"Error",MB_ICONERROR);
		fprintf(stderr,"%s\n",msg);
		return;
	}
}

VRMLModel *AFM::getVCModel()
{
	return(faceModel);
}

void AFM::applyFBAPs(const FBAPs *fbaps)
{
	for(int i =0;i<nDeformers;i++)
		deformers[i]->applyFBAPs(faceModel,fbaps);

	// added by Ratty
	VisageCharModel::applyFBAPs(fbaps);
	//
}

void AFM::updateGeometry()
{
	for(int i =0;i<nDeformers;i++)
		deformers[i]->updateGeometry(faceModel);

	// added by Ratty
	VisageCharModel::updateGeometry();
	//
}

void AFM::clearFBAPs()
{
	for(int i =0;i<nDeformers;i++)
		deformers[i]->clearFBAPs();
}

VRMLModel *AFM::getMorphTarget(int ind,int *fv)
{
	FAPs faps;
	FBAPs fbaps;

	if(ind < 15)
	{
			(*fv) = 63;
			faps.groupMask[0] = 1;
			faps.fap1.viseme1 = ind;
			faps.fap1.blend = (*fv);
	}
	else
		if(ind < 22)
		{
			(*fv) = 63;
			faps.groupMask[1] = 1;
			faps.fap2.expression1 = ind - 15;
			faps.fap2.intensity1 = (*fv);
		}
		else
		{
			if(ind == 42 || ind == 43 || ind == 44 || ind == 45 || ind == 66 || ind == 67 || ind == 68 || ind == 69)
			(*fv) = 150000;
				else
			(*fv) = 1000;
			faps.groupMask[ind - 20] = 1;
			faps.llf.value[ind - 20] = (*fv);
		}
		clearFBAPs();
		fbaps.setFAPs(&faps);
		applyFBAPs(&fbaps);
		updateGeometry();
		faceModel->applySkinning();
		faceModel->applyMorphing();
		return(faceModel);
}




void AFM::initAnimData(VRMLModel **m, int *fv)
{
	BaseFileIO bfio;

	AFMDeformer *_deformers[100];
	int _nDeformers;

	bfio.initAnimData(faceModel,m,fv, _deformers, &_nDeformers, coordSensitivity);

	for(int i=0;i<_nDeformers;i++)
		addAFMDeformer(_deformers[i]);
}


void AFM::interpolateInnerOuterLips(FAPs *faps)
{
	int startInnerLips = 3;
	int startOuterLips = 50;
	int nLipFaps = 10;
	int i;
	
	for(i=0;i<nLipFaps;i++)
	{
		interpolateInnerOuterLipsFap(faps,startInnerLips + i, startOuterLips + i);
		interpolateInnerOuterLipsFap(faps,startOuterLips + i, startInnerLips + i);
	}
	
}

void AFM::interpolateInnerOuterLipsFap(FAPs *faps, int ilf, int olf)
{
	//if outer lip fap animation is defined, but no fap is set, than interpolate the outer lip from inner

	int implemented = 0;
	for(int i =0;i<nDeformers;i++)
	{
		if(deformers[i]->getFilter()->faps->groupMask[olf])
			implemented = 1;
	}

	if(implemented && faps->groupMask[olf] == 0)
		if(faps->groupMask[ilf] == 1)
		{
			faps->llf.value[olf] = faps->llf.value[ilf];
			faps->groupMask[olf] = 1;
		}

// old implementation :	if(FAPModifiers[olf].refValue  != -1 && faps->groupMask[olf] == 0)
}
    

void AFM::countRelevantNodesToMonitorizeLoadProgress(std::string pathToFile)
{
    FILE *fp = fopen(pathToFile.c_str(),"r");
    if (!fp) {
        std::cout << "cannot count actions, there's no file at " << pathToFile.c_str() << std::endl;
        return;
    }
    //////////////////
    //get the number of nodes tagged with: Shape, Coordinate (and TextureCoordinate) and Normal, to have an idea of the progress
    char fileReadLine[100];
    int relevantNodesForProgress = 0;
    while (!feof(fp))
    {
        if ( fgets (fileReadLine , 100 , fp) != NULL )
        {
            if(strstr(fileReadLine, "   Shape") || strstr(fileReadLine, "coord Coordinate") || strstr(fileReadLine, "texCoord TextureCoordinate") || strstr(fileReadLine, "normal Normal"))
            {
                relevantNodesForProgress++;
            }
        }
    }
    //    std::cout << "AFM file analyzed to monitorize the progress, number of relevant nodes for progress: " << relevantNodesForProgress << std::endl;
    NaradaNotificationCenter::postNotification(naradaNotificationLoadInitReport, relevantNodesForProgress);
    rewind(fp);
    fclose(fp);
    
    /////////////////
}


AFM::AFM() : VisageCharModel("")
{
	init();
}

bool AFM::read(char *fname)
{
	faceModel = new VRMLModel();

	AFMDeformer *_deformers[100];
	int _nDeformers = 0;

	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for(i = 0; i<nFIO; i++)
	{
		if(FIO[i]->read(fname,getVCModel(), _deformers, &_nDeformers) == 1)
			break;
	}

	if(i==nFIO)
	{
		char msg[300];
		sprintf(msg,"Error 164\nCould not read file: >%s< (%s)\n",fname,strerror(NULL) );
//		MessageBox(0,msg,"Error",MB_ICONERROR);
		fprintf(stderr,"%s\n",msg);
		return false;
	}

	for(i=0;i<_nDeformers;i++)
		addAFMDeformer(_deformers[i]);

	setCoordSensitivity(0.1f);

	// initialize character id
	string fnstr(fname);
	string::size_type fni = fnstr.find_last_of("\\/");
	fni = fni == string::npos ? 0 : fni + 1;
	this->id = fnstr.substr(fni);

	// initialize neutral pose - added by Ratty
	// TODO: implement this

	// initialize FBAP mappings - added by Ratty
	BaseAFMDeformer *d = NULL;
	for (i = 0; i < nDeformers; i++)
	{
		d = dynamic_cast<BaseAFMDeformer *>(deformers[i]);

		if( d != NULL ) break;
	}
	if( d == NULL ) return false;

	for( int i = 0; i < NVISEME; ++i )
	{
		VisageCharModel::setFBAPMapping( getMapping( VisemeNameString[i], &d->VisemeModifiers[i] ) );
	}
	for( int i = 0; i < NEXP; ++i )
	{
		VisageCharModel::setFBAPMapping( getMapping( ExpressionNameString[i], &d->ExpressionModifiers[i] ) );
	}
	for( int i = 0; i < NFAP; ++i )
	{
		VisageCharModel::setFBAPMapping( getMapping( FAPNameString[i], &d->FAPModifiers[i] ) );
	}
	for( int i = 0; i < NBAP; ++i )
	{
		VisageCharModel::setFBAPMapping( getMapping( BAPNameString[i], &d->BAPModifiers[i] ) );
	}

	// added by Ratty
	VisageCharModel::init();
	//

	return true;
}

VRMLModelModifier *AFM::getVRMLModelModifier( const string &name ) const
{
	int i;
	for (i = 0; i < nDeformers; i++)
	{
		BaseAFMDeformer *d = dynamic_cast<BaseAFMDeformer *>(deformers[i]);

		if (d)
		{
			int j;

			// search visemes
			for(j = 0; j < NVISEME; j++)
			{
				if(!strcmp(name.c_str(), VisemeNameString[j])) return &(d->VisemeModifiers[j]);
			}

			// search expressions
			for(j = 0; j < NEXP; j++)
			{
				if(!strcmp(name.c_str(), ExpressionNameString[j])) return &(d->ExpressionModifiers[j]);
			}

			// search FAPs
			for(j = 0; j < NFAP; j++)
			{
				if(!strcmp(name.c_str(), FAPNameString[j])) return &(d->FAPModifiers[j]);
			}

			// search BAPs
			for(j = 0; j < NBAP; j++)
			{
				if(!strcmp(name.c_str(), BAPNameString[j])) return &(d->BAPModifiers[j]);
			}
		}
	}

	return NULL;
}

char *AFM::getRootTransformName(VRMLNode *node) const
{
	if (node->type == NT_TRANSFORM || node->type == NT_ROOT)
	{
		return node->getName();
	}
	else
	{
		for (int i = 0; i < node->nChildren; i++)
			return getRootTransformName(node->children[i]);
	}

	return NULL;
}

string AFM::_getModelRoot() const
{
	if (!skeletonCOM.empty()) return skeletonCOM;

	char *name = getRootTransformName(faceModel->transfNode[0]);
	if( name == NULL ) return "";
	return name;
}

FBAPMapping AFM::getMapping(const string &name, VRMLModelModifier *modifier) const
{
	FBAPMapping mapping(name);
	Quat nq;
	Vector3 nv;

	// getting refValue
	int refValue = (int) modifier->refValue;

	if (refValue != -1)
	{
		// setting refValue
		mapping.setRefValue(refValue);

		int i;

		// setting morph targets
		if (modifier->MTData)
		{
			for (i = 0; i < faceModel->nCoordLists; i++)
			{
				if( modifier->MTData[i] && _hasMesh( faceModel->meshNode[i]->getName() ) )
					mapping.mapToMorphTarget(faceModel->meshNode[i]->getName(), name);
			}
		}

		// setting bone rotations
		if (modifier->TRotData)
		{
			for (i = 0; i < faceModel->nTransfLists; i++)
			{
				//if( !_hasNode(faceModel->transfNode[i]->getName()) )
				//	continue;

				getNeutralPoseTransformations( faceModel->transfNode[i]->getName(), nv, nq );

				if (modifier->TRotData[i])
				{
					Vector3 rv( modifier->TRotData[i][0],
						modifier->TRotData[i][1],
						modifier->TRotData[i][2] );
					Quat rq( rv, modifier->TRotData[i][3] );
					
					mapping.mapToBoneRotation( faceModel->transfNode[i]->getName(), nq ^ rq );
				}
			}
		}

		// setting bone translations
		if (modifier->TTranData)
		{
			for (i = 0; i < faceModel->nTransfLists; i++)
			{
				//if( !_hasNode(faceModel->transfNode[i]->getName()) )
				//	continue;

				getNeutralPoseTransformations( faceModel->transfNode[i]->getName(), nv, nq );

				if (modifier->TTranData[i])
				{
					Vector3 rv(modifier->TTranData[i][0],
						modifier->TTranData[i][1],
						modifier->TTranData[i][2]);
					mapping.mapToBoneTranslation( faceModel->transfNode[i]->getName(), nv + rv );
				}
			}
		}
	}

	return mapping;
}

void AFM::getFacesData( const string& meshId, IntArray& faceData ) const
{
	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	int i;
	faceData.clear();

	for (i = 0; i < faceModel->triListSize[ind]; i++)
		faceData.push_back(faceModel->triList[ind][i]);
}

int AFM::numberOfFaces( const string& meshId ) const
{
	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	return (faceModel->triListSize[ind] / 3);
}

void AFM::getFaceVertices( const string& meshId, int faceIndex, int& v1,
						  int& v2, int& v3 ) const
{
	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert((ind != -1));
	#endif

	// checking if the specified face exists
//	int size = faceModel->triListSize[ind] / 3;
	#ifdef _DEBUG
	assert((faceIndex >= 0) && (faceIndex < size));
	#endif

	v1 = faceModel->triList[ind][faceIndex * 3];
	v2 = faceModel->triList[ind][faceIndex * 3 + 1];
	v3 = faceModel->triList[ind][faceIndex * 3 + 2];
}

void AFM::_getMeshes( StringArray& meshes ) const
{
	int i;

	meshes.clear();

	for(i = 0; i < faceModel->nCoordLists; i++)
		meshes.push_back(faceModel->meshNode[i]->getName());
}

void AFM::getMorphTarget( const string& meshId, const string& morphTargetId,
		VertexPositionsMap& morphTargetData ) const
{
	// checking if morph target id corresponds to a MPEG-4 Face and Body animation
	// parameter name
	VRMLModelModifier *modifier = getVRMLModelModifier(morphTargetId);
	#ifdef _DEBUG
	assert(modifier);
	#endif

	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// checking if morph target exists
	#ifdef _DEBUG
	assert(modifier->MTData && modifier->MTData[ind]);
	#endif

	int i, vInd;
	morphTargetData.clear();
	int vn = faceModel->coordListSize[ind] / 3;

	for (i = 0; i < vn; i++)
	{
		vInd = i * 3;
		Vector3 v(modifier->MTData[ind][vInd],
			modifier->MTData[ind][vInd + 1],
			modifier->MTData[ind][vInd + 2]);
		morphTargetData[i] = v;
	}
}

void AFM::_getMorphTargets( const string& meshId, StringArray& morphTargets ) const
{
	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	int i;

	morphTargets.clear();

	for (i = 0; i < nDeformers; i++)
	{
		BaseAFMDeformer *d = dynamic_cast<BaseAFMDeformer *>(deformers[i]);

		if (d)
		{
			int j;

			// searching visemes
			for (j = 0; j < NVISEME; j++)
			{
				if (d->VisemeModifiers[j].MTData && d->VisemeModifiers[j].MTData[ind])
					morphTargets.push_back(VisemeNameString[j]);
			}

			// searching expressions
			for (j = 0; j < NEXP; j++)
			{
				if (d->ExpressionModifiers[j].MTData && d->ExpressionModifiers[j].MTData[ind])
					morphTargets.push_back(ExpressionNameString[j]);
			}

			// searching FAPs
			for (j = 0; j < NFAP; j++)
			{
				if (d->FAPModifiers[j].MTData && d->FAPModifiers[j].MTData[ind])
					morphTargets.push_back(FAPNameString[j]);
			}

			// searching BAPs
			for (j = 0; j < NBAP; j++)
			{
				if (d->BAPModifiers[j].MTData && d->BAPModifiers[j].MTData[ind])
					morphTargets.push_back(BAPNameString[j]);
			}
		}
	}
}

Vector3 AFM::getMorphTargetVertexOffset( const string& meshId, const string& morphTargetId,
										int vertexIndex ) const
{
	// checking if morph target id corresponds to a MPEG-4 Face and Body animation
	// parameter name
	VRMLModelModifier *modifier = getVRMLModelModifier(morphTargetId);
	#ifdef _DEBUG
	assert(modifier);
	#endif

	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// checking if morph target exists
	#ifdef _DEBUG
	assert(modifier->MTData && modifier->MTData[ind]);
	#endif

	// checking if vertex exists
	#ifdef _DEBUG
	assert((vertexIndex >= 0) && (vertexIndex < numberOfVertices(meshId)));
	#endif

	int vInd = vertexIndex * 3;
	Vector3 v(modifier->MTData[ind][vInd], modifier->MTData[ind][vInd + 1],
		modifier->MTData[ind][vInd + 2]);

	return v;
}

void AFM::getNeutralPoseTransformations( const string& boneId, Vector3& position,
										Quat& orientation ) const
{
	// checking if the specified Transform exists
	int ind = getNodeIndex(boneId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// neutral pose position is always equal to (0, 0, 0) -- WTF? No it isn't. - Ratty
	// getting bone initial pose position
	position = _getNodeLocalPosition(boneId);

	// getting neutral pose orientation
	if (faceModel->neutralPoseData.NPRotData &&
		faceModel->neutralPoseData.NPRotData[ind])
	{
		Vector3 axis(faceModel->neutralPoseData.NPRotData[ind][0],
			faceModel->neutralPoseData.NPRotData[ind][1],
			faceModel->neutralPoseData.NPRotData[ind][2]);
		float angle = faceModel->neutralPoseData.NPRotData[ind][3];

		Quat q(axis, angle);
		orientation = q;
	}
	else
	{
		orientation = _getNodeLocalOrientation(boneId);
	}
}

Vector3 AFM::_getNodeInitialLocalPosition( const string& id ) const
{
	#ifdef _DEBUG
	_hasNode(id);
	#endif

	BaseAFMDeformer *d = NULL;
	for( int i = 0; i < nDeformers; i++ )
	{
		d = dynamic_cast<BaseAFMDeformer *>(deformers[i]);

		if( d != NULL ) break;
	}
	if( d == NULL ) return Vector3();

	int ti = getNodeIndex(id);
	return Vector3( d->origPosition[ti][0], d->origPosition[ti][1], d->origPosition[ti][2] );
}

Quat AFM::_getNodeInitialLocalOrientation( const string& id ) const
{
	#ifdef _DEBUG
	_hasNode(id);
	#endif

	BaseAFMDeformer *d = NULL;
	for( int i = 0; i < nDeformers; i++ )
	{
		d = dynamic_cast<BaseAFMDeformer *>(deformers[i]);

		if( d != NULL ) break;
	}
	if( d == NULL ) return Quat();

	int ti = getNodeIndex(id);
	return Quat( d->origOrientation[ti][3], d->origOrientation[ti][0],
		d->origOrientation[ti][1], d->origOrientation[ti][2] );
}

Vector3 AFM::_getNodeInitialWorldPosition( const string& id ) const
{
	// TODO: implement this method
	return Vector3( 0.f, 0.f, 0.f );
}

Quat AFM::_getNodeInitialWorldOrientation( const string& id ) const
{
	// TODO: implement this method
	return Quat( 0.f, 0.f, 0.f );
}

Vector3 AFM::_getNodeLocalPosition( const string& id ) const
{
	// checking if the node exists
	int ind = getNodeIndex(id);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	Vector3 v(faceModel->transfList[ind][0],
		faceModel->transfList[ind][1],
		faceModel->transfList[ind][2]);

	return v;
}

void AFM::_setNodeLocalPosition( const string& id, const Vector3& pos )
{
	// checking if the node exists
	int ind = getNodeIndex(id);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	faceModel->transfList[ind][0] = pos.x;
	faceModel->transfList[ind][1] = pos.y;
	faceModel->transfList[ind][2] = pos.z;
}

Quat AFM::_getNodeLocalOrientation( const string& id ) const
{
	// checking if the node exists
	int ind = getNodeIndex(id);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	Quat q(faceModel->transfList[ind][6],
		faceModel->transfList[ind][3],
		faceModel->transfList[ind][4],
		faceModel->transfList[ind][5]);

	return q;
}

void AFM::_setNodeLocalOrientation( const string& id, const Quat& orient )
{
	// checking if the node exists
	int ind = getNodeIndex(id);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	faceModel->transfList[ind][6] = orient.w;
	faceModel->transfList[ind][3] = orient.x;
	faceModel->transfList[ind][4] = orient.y;
	faceModel->transfList[ind][5] = orient.z;
}

Vector3 AFM::_getNodeWorldPosition( const string& id ) const
{
	// checking if the node exists
	int ind = getNodeIndex(id);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	float *r = new float[3];

	// getting node's local position
	float *vec = new float[3];
	int i;

	for (i = 0; i < 3; i++)
		vec[i] = faceModel->transfList[ind][i];

	// converting local position to world position
	// it is being done using VRMLModel::toWC matrix
	multVect3Mat(r, vec, faceModel->toWC[ind]);

	Vector3 v(r[0],	r[1], r[2]);

	delete [] r;
	delete [] vec;

	return v;
}

void AFM::_setNodeWorldPosition( const string& id, const Vector3& pos )
{
	// TODO: implement this method
}

Quat AFM::_getNodeWorldOrientation( const string& id ) const
{
	// checking if the node exists
	int ind = getNodeIndex(id);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	myMatrix r, m;
	float *quat = new float[4];
	int i;

	// getting node's local orientation
	for (i = 0; i < 4; i++)	quat[i] = faceModel->transfList[ind][i + 3];

	// converting local orientation from quaternion to matrix
	rotMatFromQuat(m, quat);

	// converting local orientation to world orientation
	// it is being done using VRMLModel::toWC matrix
	multMat(r, m, faceModel->toWC[ind]);

	// converting world orientation from matrix to quaternion
	quatFromRotMat(quat, r);

	Quat q(quat[3], quat[0], quat[1], quat[2]);

	delete [] quat;

	return q;
}

void AFM::_setNodeWorldOrientation( const string& id, const Quat& orient )
{
	// TODO: implement this method
}

Vector2 AFM::getTexCoords( const string& meshId, int index,
						  int texStage ) const
{
	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// checking if mesh has texture
	// texture stage is always 0
	int matInd = faceModel->meshNode[ind]->matInd;
	#ifdef _DEBUG
	assert(!texStage && faceModel->tCoordList[ind] &&
		!faceModel->materials[matInd]->cubemapNames[0]);
	#endif

	// checking if vertex exists
	#ifdef _DEBUG
	assert((index >= 0) && (index < (faceModel->coordListSize[ind] / 3)));
	#endif

	// searching for index of texture coordinates
	int texInd = index * 2;

	Vector2 v(faceModel->tCoordList[ind][texInd],
			faceModel->tCoordList[ind][texInd + 1]);

	return v;
}

void AFM::getTexCoordData( const string& meshId, Vector2Array& texCoordData,
						  int texStage ) const
{
	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// checking if mesh has texture
	// texture stage is always 0
//	int matInd = faceModel->meshNode[ind]->matInd;
	#ifdef _DEBUG
	assert(!texStage && faceModel->tCoordList[ind] &&
		!faceModel->materials[matInd]->cubemapNames[0]);
	#endif

	int i, texInd;
	int vn = faceModel->coordListSize[ind] / 3;

	texCoordData.clear();

	for (i = 0; i < vn; i++)
	{
		texInd = i * 2;

		Vector2 v(faceModel->tCoordList[ind][texInd],
			faceModel->tCoordList[ind][texInd + 1]);

		texCoordData.push_back(v);
	}
}

string AFM::getTextureFilename( const string& meshId, int texStage ) const
{
	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// checking if mesh has texture
	// texture stage is always 0
	int matInd = faceModel->meshNode[ind]->matInd;
	VRMLMaterial *material = faceModel->materials[matInd];
	#ifdef _DEBUG
	assert(!texStage && faceModel->tCoordList[ind] &&
		!material->cubemapNames[0]);
	#endif

	return material->textureName;
}

void AFM::getVertexData( const string& meshId, Vector3Array& vertexData ) const
{
	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	vertexData.clear();
	int vn = faceModel->coordListSize[ind] / 3;
	int i;

	for (i = 0; i < vn; i++)
	{
		int vInd = i * 3;
		Vector3 v(faceModel->coordList[ind][vInd],
			faceModel->coordList[ind][vInd + 1],
			faceModel->coordList[ind][vInd + 2]);

		vertexData.push_back(v);
	}
}

int AFM::numberOfVertices( const string& meshId ) const
{
	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	int vn = faceModel->coordListSize[ind] / 3;

	return vn;
}

Vector3 AFM::getVertexPosition( const string& meshId, int index ) const
{
	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// checking if vertex exists
	#ifdef _DEBUG
	assert((index >= 0) && (index < numberOfVertices(meshId)));
	#endif

	Vector3 v(faceModel->coordList[ind][index * 3],
		faceModel->coordList[ind][index * 3 + 1],
		faceModel->coordList[ind][index * 3 + 2]);

	return v;
}

bool AFM::_hasMesh( const string& id ) const
{
	// checking if mesh with the id exists
	int ind = getMeshIndex(id);
	if (ind == -1) return false;
	else return true;
}

bool AFM::_hasMorphTarget( const string& meshId,
						 const string& morphTargetId ) const
{
	// checking if morph target id corresponds to a MPEG-4 Face and
	// Body animation parameter name
	VRMLModelModifier *modifier = getVRMLModelModifier(morphTargetId);
	#ifdef _DEBUG
	assert(modifier);
	#endif

	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	if (!modifier->MTData || !modifier->MTData[ind]) return false;

	return true;
}

bool AFM::_hasNode( const string& id ) const
{
	// this is a crappy solution,
	// but I don't have time nor will
	// to implement anything that doesn't suck
	static StringArray nodes;
	if( nodes.size() == 0 )
		_getNodes( nodes );

	// checking if the specified Transform exists
	for( size_t i = 0; i < nodes.size(); ++i )
		if( nodes[i] == id )
			return true;

	return false;
}

void AFM::setFBAPMapping( const FBAPMapping& mapping )
{
	// checking if parameter id is valid according to the MPEG-4 FBA
	const string &parName = mapping.getFBAPName();
	VRMLModelModifier *modifier = getVRMLModelModifier(parName);
	#ifdef _DEBUG
	assert(modifier);
	#endif

	// checking if names of morph targets are all equal to the parameter
	// name and that they are already defined
	int no = mapping.numberOfMorphTargets();

	for (int i = 0; i < no; i++)
	{
		string meshId, mtId;

		mapping.getMorphTarget(i, meshId, mtId);

		#ifdef _DEBUG
		assert(mtId == parName);
		assert(_hasMorphTarget(meshId, mtId));
		#endif
	}

	// setting refValue
	modifier->refValue = (float) mapping.getRefValue();

	// setting bone rotations
	no = mapping.numberOfRotBones();

	for (int i = 0; i < no; i++)
	{
		string nodeId;
		Quat q;

		mapping.getRefOrientation(i, nodeId, q);

		// checking if the specified Transform exists
		int ind = getNodeIndex(nodeId);
		#ifdef _DEBUG
		assert(ind != -1);
		#endif

		if (!modifier->TRotData)
			modifier->TRotData = (float **) malloc(faceModel->nTransfLists * sizeof(float *));

		if (!modifier->TRotData[ind])
			modifier->TRotData[ind] = (float *) malloc(4 * sizeof(float));

		Vector3 axis;
		float angle;
		q.getAxisAngle(axis, angle);
		modifier->TRotData[ind][0] = axis.x;
		modifier->TRotData[ind][1] = axis.y;
		modifier->TRotData[ind][2] = axis.z;
		modifier->TRotData[ind][3] = angle;
	}

	// setting bone translations
	no = mapping.numberOfTransBones();

	for (int i = 0; i < no; i++)
	{
		string nodeId;
		Vector3 pos;

		mapping.getRefPosition(i, nodeId, pos);

		// checking if the specified Transform exists
		int ind = getNodeIndex(nodeId);
		#ifdef _DEBUG
		assert(ind != -1);
		#endif

		if (!modifier->TTranData)
			modifier->TTranData = (float **) malloc(faceModel->nTransfLists * sizeof(float *));

		if (!modifier->TTranData[ind])
			modifier->TTranData[ind] = (float *) malloc(3 * sizeof(float));

		modifier->TTranData[ind][0] = pos.x;
		modifier->TTranData[ind][1] = pos.y;
		modifier->TTranData[ind][2] = pos.z;
	}

	// deleting bone scaling values
	if (modifier->TScalData)
	{
		for (int i = 0; i < faceModel->nTransfLists; i++)
			if (modifier->TScalData[i]) free(modifier->TScalData[i]);

		free(modifier->TScalData);
	}

	// update mappings in base class - added by Ratty
	VisageCharModel::setFBAPMapping(mapping);
	//
}

void AFM::setMorphTarget( const string& meshId, const string& morphTargetId,
		const VertexPositionsMap& morphTargetData )
{
	// checking if morph target id corresponds to a MPEG-4 Face and Body
	// animation parameter name
	VRMLModelModifier *modifier = getVRMLModelModifier(morphTargetId);
	#ifdef _DEBUG
	assert(modifier);
	#endif

	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// if the morph target doesn't exist it is created
	if (!modifier->MTData)
	{
		modifier->MTData =
			(float **) malloc(faceModel->nCoordLists * sizeof(float *));
	}

	if (!modifier->MTData[ind])
	{
		modifier->MTData[ind] =
			(float *) malloc(faceModel->coordListSize[ind] * sizeof(float));
	}

	// copying vertex offsets
	int i, vInd;
	int vn = faceModel->coordListSize[ind] / 3;

	for (i = 0; i < vn; i++)
	{
		vInd = i * 3;

		VertexPositionsMap::const_iterator iter = morphTargetData.find(i);
		if (iter != morphTargetData.end())
		{
			modifier->MTData[ind][vInd] = iter->second.x;
			modifier->MTData[ind][vInd + 1] = iter->second.y;
			modifier->MTData[ind][vInd + 2] = iter->second.z;
		}
		else
		{
			modifier->MTData[ind][vInd] = 0;
			modifier->MTData[ind][vInd + 1] = 0;
			modifier->MTData[ind][vInd + 2] = 0;
		}
	}
}

void AFM::setMorphTargetVertexOffset( const string& meshId,
									 const string& morphTargetId, int vertexIndex,
									 const Vector3& vertexOffset )
{
	// checking if morph target id corresponds to a MPEG-4 Face and Body animation
	// parameter name
	VRMLModelModifier *modifier = getVRMLModelModifier(morphTargetId);
	#ifdef _DEBUG
	assert(modifier);
	#endif

	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// checking if morph target exists
	#ifdef _DEBUG
	assert(modifier->MTData && modifier->MTData[ind]);
	#endif

	// checking if vertex exists
	#ifdef _DEBUG
	assert((vertexIndex >= 0) && (vertexIndex < numberOfVertices(meshId)));
	#endif

	int vInd = vertexIndex * 3;
	modifier->MTData[ind][vInd] = vertexOffset.x;
	modifier->MTData[ind][vInd + 1] = vertexOffset.y;
	modifier->MTData[ind][vInd + 2] = vertexOffset.z;
}

void AFM::setNeutralPoseTransformations( const string& boneId,
										const Vector3& position,
										const Quat& orientation )
{
	// checking if bone or node exists
	int ind = getNodeIndex(boneId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// neutral pose position must be equal to (0, 0, 0)
	#ifdef _DEBUG
	assert((position.x == 0) && (position.y == 0) && (position.z == 0));
	#endif

	// setting neutral pose orientation
	// neutral pose position is being ignored
	if (!faceModel->neutralPoseData.NPRotData)
	{
		faceModel->neutralPoseData.NPRotData =
			(float **) malloc(faceModel->nTransfLists * sizeof(float *));
	}

	if (!faceModel->neutralPoseData.NPRotData[ind])
	{
		faceModel->neutralPoseData.NPRotData[ind] =
			(float *) malloc(4 * sizeof(float));
	}

	Vector3 axis;
	float angle;

	orientation.getAxisAngle(axis, angle);

	faceModel->neutralPoseData.NPRotData[ind][0] = axis.x;
	faceModel->neutralPoseData.NPRotData[ind][1] = axis.y;
	faceModel->neutralPoseData.NPRotData[ind][2] = axis.z; 
	faceModel->neutralPoseData.NPRotData[ind][3] = angle;
}

void AFM::setTexCoords( const string& meshId, int index,
					   const Vector2& texCoord, int texStage )
{
	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// checking if mesh has texture
	// texture stage is always 0
//	int matInd = faceModel->meshNode[ind]->matInd;
	#ifdef _DEBUG
	assert(!texStage && faceModel->tCoordList[ind] &&
		!faceModel->materials[matInd]->cubemapNames[0]);
	#endif

	// checking if vertex exists
	#ifdef _DEBUG
	assert((index >= 0) && (index < (faceModel->coordListSize[ind] / 3)));
	#endif

	// searching for index of texture coordinates
	int texInd = index * 2;

	faceModel->tCoordList[ind][texInd] = texCoord.x;
	faceModel->tCoordList[ind][texInd + 1] = texCoord.y;
}

void AFM::setTexCoordData( const string& meshId,
						  const Vector2Array& texCoordData, int texStage )
{
	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// checking if mesh has texture
	// texture stage is always 0
	int matInd = faceModel->meshNode[ind]->matInd;
	#ifdef _DEBUG
	assert(!texStage && faceModel->tCoordList[ind] &&
		!faceModel->materials[matInd]->cubemapNames[0]);
	#endif

	int vn = faceModel->coordListSize[ind] / 3;

	// checking if the number of vertices in the mesh is equal to the
	// number of elements in texCoordData
	#ifdef _DEBUG
	assert(vn == texCoordData.size());
	#endif

	int i, texInd;

	for (i = 0; i < vn; i++)
	{
		texInd = i * 2;
		faceModel->tCoordList[ind][texInd] = texCoordData[i].x;
		faceModel->tCoordList[ind][texInd + 1] = texCoordData[i].y;
	}
}

void AFM::setVertexData( const string& meshId, const Vector3Array& vertexData )
{
	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	int vn = faceModel->coordListSize[ind] / 3;

	// checking if the number of vertices in the mesh is equal to the number of
	// elements in vertexData
	#ifdef _DEBUG
	assert(vn == vertexData.size());
	#endif

	// setting vertex coordinates
	int i, vInd;

	for (i = 0; i < vn; i++)
	{
		vInd = i * 3;
		faceModel->coordList[ind][vInd] = vertexData[i].x;
		faceModel->coordList[ind][vInd + 1] = vertexData[i].y;
		faceModel->coordList[ind][vInd + 2] = vertexData[i].z;

		faceModel->origCoordList[ind][vInd] = vertexData[i].x;
		faceModel->origCoordList[ind][vInd + 1] = vertexData[i].y;
		faceModel->origCoordList[ind][vInd + 2] = vertexData[i].z;
	}
}

void AFM::setVertexPosition( const string& id, int index,
							const Vector3& position )
{
	// checking if mesh with the id exists
	int ind = getMeshIndex(id);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// checking if vertex exists
	#ifdef _DEBUG
	assert((index >= 0) && (index < numberOfVertices(id)));
	#endif

	faceModel->coordList[ind][index * 3] = position.x;
	faceModel->coordList[ind][index * 3 + 1] = position.y;
	faceModel->coordList[ind][index * 3 + 2] = position.z;

	faceModel->origCoordList[ind][index * 3] = position.x;
	faceModel->origCoordList[ind][index * 3 + 1] = position.y;
	faceModel->origCoordList[ind][index * 3 + 2] = position.z;
}

void AFM::deleteMorphTarget( const string& meshId, const string& morphTargetId )
{
	// checking if morph target id corresponds to a MPEG-4 Face and Body animation
	// parameter name
	VRMLModelModifier *modifier = getVRMLModelModifier(morphTargetId);
	#ifdef _DEBUG
	assert(modifier);
	#endif

	// checking if mesh with the meshId exists
	int ind = getMeshIndex(meshId);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	// checking if morph target exists
	#ifdef _DEBUG
	assert(modifier->MTData && modifier->MTData[ind]);
	#endif

	// deleting morph target
	free(modifier->MTData[ind]);
}

string AFM::_getNodeParent( const string& id ) const
{
	// checking if the specified Transform exists
	int ind = getNodeIndex(id);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	string name = faceModel->transfNode[ind]->parent->getName();
	return name;
}

void AFM::_getNodeChildren( const string& id, StringArray& children ) const
{
	// checking if the specified Transform exists
	int ind = getNodeIndex(id);
	#ifdef _DEBUG
	assert(ind != -1);
	#endif

	children.clear();
	int cNo = faceModel->transfNode[ind]->nChildren;

	for (int i = 0; i < cNo; i++)
	{
		string name = faceModel->transfNode[ind]->children[i]->getName();
		if( getNodeIndex(name) != -1 )
			children.push_back(name);
	}
}

void AFM::_getNodes( StringArray& nodes ) const
{
	nodes.clear();

	_getNodes( nodes, _getModelRoot() );
}

void AFM::_getNodes( StringArray& nodes, const string& nodeId ) const
{
	if( getNodeIndex(nodeId) == -1 )
		return;

	nodes.push_back( nodeId );

	StringArray children;
	_getNodeChildren( nodeId, children );

	for( size_t i = 0; i < children.size(); ++i )
		_getNodes( nodes, children[i] );
}

void AFM::recalculateNormals()
{
	faceModel->recalculateNormals();
}

string AFM::getNode( int index ) const
{
	V_ASSERT( index >= 0 && index < ((AFM*)this)->getVCModel()->nTransfLists );

	return ((AFM*)this)->getVCModel()->transfNode[index]->getName();
}

int AFM::getNodeIndex( const string& id ) const
{
	return ((AFM*)this)->getVCModel()->findNamedTransform( (char*)id.c_str() );
}

string AFM::getMesh( int index ) const
{
	V_ASSERT( index >= 0 && index < ((AFM*)this)->getVCModel()->nCoordLists );

	return ((AFM*)this)->getVCModel()->meshNode[index]->getName();
}

int AFM::getMeshIndex( const string& id ) const
{
	return ((AFM*)this)->getVCModel()->findNamedMesh( (char*)id.c_str() );
}

}
