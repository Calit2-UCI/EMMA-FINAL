// SimpleCloner.h: interface for the SimpleCloner class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SimpleCloner_h__
#define __SimpleCloner_h__

#include <cassert>

#include "AFM.h"
#include "BaseAFMDeformer.h"
#include "FaceModelData.h"

namespace VisageSDK
{

/**
* @class SimpleCloner
* \brief Copies (clones) facial motion (morph targets) between 3D face models
* with identical topologies.
*
* The morph targets are copied from the source model (an AFM model, i.e. an animatable face model) to the target model (a static model, a VRMLModel).
* A new AFM object is created with the appearance of the target model and the morph targets copied from the source model.
*
* This feature is used when an animatable face model (a source model) is available, and it is designed to copy its set of morph targets to another model (the target model) that is not animatable (does not have morph targets).
* After this operation, the target model becomes animatable. For this to work, the source and target models must have exactly identical scenegraph hierarchy and mesh topology. 
*/
class VISAGE_DECLSPEC SimpleCloner
{

public:

	/**
	* Constructor.
	* Constructs a SimpleCloner object.
	*/
	SimpleCloner();

	/**
	* Destructor.
	* Destroys a SimpleCloner object.
	*/
	virtual ~SimpleCloner()
	{
		delete sourceData;
		delete targetData;
	}

	/**
	* Copies facial motion (morph targets) from one model to another.
	* @param sourceAFM Source model for the cloning process.
	* @param sourceFDP Feature point definitions for the source model.
	* @param targetVRML Target model for the cloning process.
	* @return Target model with a full set of morph targets or NULL
	* if source or target model have errors or are incompatible.
	* The returned model is ready for animation.
	*/
	virtual AFM* clone( AFM* sourceAFM, FDP* sourceFDP, VRMLModel* targetVRML );

	/**
	* Copies facial motion (morph targets) from one model to another.
	* @param sourceAFMFile Path to the source model for the cloning process.
	* @param targetVRMLFile Path to the target model for the cloning process.
	* @return Target model with a full set of morph targets or NULL
	* if source or target model have errors or are incompatible.
	* The returned model is ready for animation.
	*/
	virtual AFM* clone( char* sourceAFMFile, char* targetVRMLFile );	
	/**
	* Retrieves a log of cloning errors. If the cloning process fails,
	* this log contains a list of errors that occurred. 
	* @return Errors log string.
	*/	
	string getErrorsLog() { return errors; }

protected:

	/*
	* Verify that the models have identical topologies.
	* This is done by comparing the coresponding meshes of the source and target model.
	* The meshes of the source model can easily be identified via FDPs, but that is not possible in the target model.
	* That means that a previously-agreed-to identification method has to be used.
	* The method attempts to get the meshes of the target model by name.
	* @param sourceVRML Source model.
	* @param targetVRML Target model.
	* @return -1 if a mesh with the required name can't be found or the number of vertices differs,
	* 0 if the polygon layout differs, 1 otherwise.
	*/
	virtual int verifyTopologiesByName(VRMLModel* sourceVRML, VRMLModel* targetVRML, bool isFaceGen = false);
	/*
	* Finalize the cloning of facial motion.
	* Sets the output model with the facial motion data obtained from the source model.
	* @param sourceAFM Source model.
	* @param targetVRML Target model.
	* @param nSourceMeshes Number of meshes in the source model.
	* @param nTargetMeshes Number of meshes in the target model.
	* @param sourceMeshes[50] List of indexes of the meshes in the source model.
	* @param targetMeshes[50] List of indexes of the meshes in the target model.
	* @param nSourceTransfs Number of transforms in the source model.
	* @param sourceTransfs[6] List of indexes of the transforms in the source model.
	* @param targetTransfs[6] List of indexes of the transforms in the target model.
	* @return Returns the output model with cloned facial motion data.
	*/
	virtual AFM* FinalizeCloning(AFM* sourceAFM, VRMLModel* targetVRML, int nSourceMeshes, int nTargetMeshes, int sourceMeshes[50], int targetMeshes[50], int nSourceTransfs, int sourceTransfs[6], int targetTransfs[6]);
	///source face data
	FaceModelData *sourceData;

	///target face data
	FaceModelData *targetData;

	///Cloning errors log.
	string errors;
};

}

#endif // __SimpleCloner_h__
