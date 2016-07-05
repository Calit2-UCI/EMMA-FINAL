// FaceModelData.h: interface for the FaceModelData class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FaceModelData_h__
#define __FaceModelData_h__

#define NB_SD 88

#include <cassert>
#include <cmath>
#include <float.h>
#include <string.h>

#include "AFM.h"
#include "FDP.h"

namespace VisageSDK
{

/**
* @class FaceModelData
* @brief Processing class that analyses a model, gathers information
* about it and prepares it for cloning.
*
* @see SimpleCloner
*
*/
class FaceModelData
{

public:

	/**
	* Constructor. Create an uninitialized FaceModelData instance.
	*/
	FaceModelData();

	/**
	* Constructor. Intended to be used for source models.
	* @param model Source model that is to be processed.
	* @param fdp Feature point definitions of the model.
	*/
	FaceModelData( VRMLModel* model, FDP* fdp );

	/**
	* Derive a FaceModelData instance for target model from 
	* the current FaceModelData instance. The created instance
	* processes the target model and prepares it for cloning.
	* @param model Target model.
	* @param sourceModel Source model.
	* @return Target model's FaceModelData instance.
	*/
	FaceModelData* deriveFaceModelDataForTargetModel( VRMLModel* model, VRMLModel* sourceModel );

	/**
	* Process the given model. Intended to be used for source models.
	* If the current FaceModelData has previously been initialized
	* with another model, it is reset first.
	* @param model Source model that is to be processed.
	* @param fdp Feature point definitions of the model.
	* @return true if the model was successfully processed, false if
	* critical errors occured.
	*/
	virtual bool processModel( VRMLModel* model, FDP* fdp );

private:

	/// Process the given model. Used for target models. Invoked by
	/// processModel( VRMLModel* model, FDP* fdp ).
	bool processModel( VRMLModel* model, VRMLModel* sourceModel, int faceMesh, int* lEyeMesh, int nLEyeMesh,
		int* rEyeMesh, int nREyeMesh, int uTeethMesh, int* lTeethMesh, int nLTeethMesh,
		int* tongueMesh, int nTongueMesh, FeaturePoint* globalRotationFP );

public:

	/**
	* Get a log of errors with the model. This log is created when
	* the model is processed.
	* Errors can be critical (causing the processing to fail) or 
	* non-critical. Possible errors are missing feature points 
	* (critical), missing meshes (non-critical) 
	* and mising transform nodes (non-critical).
	* @return Errors log string.
	*/
	string getErrorsLog(){ return errors; }

	/**
	* Get index of the face mesh.
	*/
	int getFaceMesh() { return faceMesh; }

	/**
	* Get index of a left eye mesh.
	* @param i Since eyes can consist of multiple meshes,
	* this index specifies which mesh index to retrieve.
	*/
	int getLEyeMesh( int i ) { assert( i >= 0 && i < nLEyeMesh ); return lEyeMesh[i]; }

	/**
	* Get number of left eye meshes.
	*/
	int getLEyeMeshesNo() { return nLEyeMesh; }

	/**
	* Get index of a right eye mesh.
	* @param i Since eyes often consist of multiple meshes,
	* this index specifies which mesh index to retrieve.
	*/
	int getREyeMesh( int i ) { assert( i >= 0 && i < nREyeMesh ); return rEyeMesh[i]; }

	/**
	* Get number of right eye meshes.
	*/
	int getREyeMeshesNo() { return nREyeMesh; }

	/**
	* Get index of the upper teeth mesh.
	*/
	int getUTeethMesh() { return uTeethMesh; }

	/**
	* Get index of a lower teeth mesh.
	* @param i Since lower teeth often consist of multiple meshes,
	* this index specifies which mesh index to retrieve.
	*/
	int getLTeethMesh( int i ) { assert( i >= 0 && i < nLTeethMesh ); return lTeethMesh[i]; }

	/**
	* Get number of lower teeth meshes.
	*/
	int getLTeethMeshesNo() { return nLTeethMesh; }

	/**
	* Get index of a tongue mesh.
	* @param i Since tongue often consists of multiple meshes,
	* this index specifies which mesh index to retrieve.
	*/
	int getTongueMesh( int i ) { assert( i >= 0 && i < nTongueMesh ); return tongueMesh[i]; }

	/**
	* Get number of tongue meshes.
	*/
	int getTongueMeshesNo() { return nTongueMesh; }

	/**
	* Get global transform node.
	*/
	int getGlobalTransf() { return globalTransf; }

	/**
	* Get left eye transform node.
	*/
	int getLEyeTransf() { return lEyeTransf; }

	/**
	* Get right eye transform node.
	*/
	int getREyeTransf() { return rEyeTransf; }

	/**
	* Get upper teeth transform node.
	*/
	int getUTeethTransf() { return uTeethTransf; }

	/**
	* Get lower teeth transform node.
	*/
	int getLTeethTransf() { return lTeethTransf; }

	/**
	* Get tongue transform node.
	*/
	int getTongueTransf() { return tongueTransf; }

	/**
	* Get axis-aligned bounding box. The cloner needs it 
	* to compute face scaling factors.
	*/
	void getAABB( float& xMin, float& yMin, float& zMin,
		float& xMax, float& yMax, float& zMax )
	{
		xMin = this->xMin;
		yMin = this->yMin;
		zMin = this->zMin;
		xMax = this->xMax;
		yMax = this->yMax;
		zMax = this->zMax;
	}

protected:

	VRMLModel* model, *sourceModel;
	FDP* fdp;
	FeaturePoint* GRFP;
	float xMin, xMax, yMin, yMax, zMin, zMax;
	int animated[50];
	int animated_strict[50];
	int nAnimPoints;
	int faceMesh, lEyeMesh[50], rEyeMesh[50], uTeethMesh, lTeethMesh[50], tongueMesh[50];
	int nLEyeMesh, nREyeMesh, nLTeethMesh, nTongueMesh;
	int globalTransf, lEyeTransf, rEyeTransf, uTeethTransf, lTeethTransf, tongueTransf;
	float globalRotationCenter[3];
	string errors;

	virtual void reset();
	virtual void chooseAnimatedSurfaces();
	virtual void locateMeshes();
	virtual bool surfaceContainsPoint(int s, float *p);
	virtual int findSurfaceContainingPoint(float *p, int s0);
	virtual void findAdjacentSurfaces(int s0, int *s, int *ns, float *fpa);
	virtual void findAdjacentSurfaces(int s0, int *s, int *ns, int nfpa, float **fpa);
	virtual bool findRotationCenters( bool isTarget = false );
	virtual void computeEyeCenter(float *c,int ns,int *s);
	virtual int findTransformContainingSurfaces(int nRSurf,int *rSurf);
	virtual bool transformContainsTransform(int t1,int t2);
	virtual bool transformContainsSurface(int t,int s);
	virtual bool transformContainsSurfaces(int t,int nRSurf,int *rSurf);
	virtual int arrangeRotationCenter(float *ctr,int tran);
	virtual void insertCenterTransform(int t,float c[3]);

	/**
	* Adds a new transform node.
	* NOTE: This method may not be enough to cover all the tasks when adding a new node to a model.
	* It only covers data necessary for its creation and data which doesn't depend on other data.
	* Any children will have to be added to it manually (usually with the AddChild method of the VRMLNode).
	* The user must then make sure the child's parent member is set to the transform, if it's not set already.
	* Furthermore, a transform may or may not have a node of type NT_SHAPE (2, i.e. a mesh) as its child.
	* If it does, then the meshNode and transfInd members will have to be filled appropriately.
	* Finally, this method should only be used for adding nodes to nodes which do not have children of type NT_TRANSFORM.
	* @param name Name of the transform node.
	* @param parentNode Pointer to the parent node.
	* @return Pointer to the created transform node.
	*/
	virtual VRMLNode* addTransform(char* name, VRMLNode* parentNode);
	virtual int findGlobalMotionTransform( bool isTarget = false, bool fullBodyModel = false );
	virtual bool pointsEqual3D( float* p1, float* p2 );
	virtual void findAABB();
	virtual void convertToWC();
	virtual void convertFromWC();
};

}

#endif // __FaceModelData_h__
