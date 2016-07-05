// FaceGenCloner.h: interface for the FaceGenCloner class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FaceGenCloner_h__
#define __FaceGenCloner_h__

#include "SimpleCloner.h"

namespace VisageSDK
{

/**
* @class FaceGenCloner
* \brief Imports FaceGen models and prepares them for animation in Visage Technologies' software by copying facial motion from provided FaceGen models
* with implemented animation capabilities.
*
* This creates an AFM object from a FaceGen model, thus effectively importing the FaceGen model and making it possible to animate it in all Visage applications.
*/
class VISAGE_DECLSPEC FaceGenCloner : public SimpleCloner
{
public:

	/**
	* Constructor.
	* Constructs a new FaceGenCloner object.
	*/
	FaceGenCloner();

	/**
	* Destructor.
	* Destroys a FaceGenCloner object.
	*/
	~FaceGenCloner()
	{
	}

	/**
	* Makes an animatable face model (AFM) from a FaceGen model.\n Note - the FaceGen model must be created according to the following guidelines:\n
	* A. The most important issue is to select meshes that are supported by Visage Technologies. 
	* Click the "Change Polys" button in the lower left section of the main window (below the viewport).
	* You will need to include the following meshes:
	*
	* - Hires Eye Left
	* - Hires Eye Right
	* - one of the three supported face meshes (Lores Skin, Medres Skin, or Hires Skin)
	* - Sock
	* - Teeth Lower
	* - Teeth Upper
	* - Tongue
	*
	* Additionally, you may include one of the hair meshes and the glasses.
	*
	* B. Make sure not to change the model's expression from neutral. In other words, do not use the Morph tab of FaceGen.
	* Should you accidentally change the values within, you can reset the model to the neutral expression by clicking the "Set All to Zero" button within the Morph tab.
	*
	* C. Make sure that you save textures as JPG, not BMP.
	*
	* @param faceGenModel Target model for the cloning process.
	* @return The FaceGen model with a full set of morph targets or NULL
	* if source or target models have errors or are incompatible. 
	* The returned model is ready for animation.
	*/
	AFM* clone( VRMLModel* faceGenModel );

	/**
	* Makes an animatable face model (AFM) from a FaceGen model.\n Note - the FaceGen model must be created according to the following guidelines:\n
	* A. The most important issue is to select meshes that are supported by Visage Technologies. 
	* Click the "Change Polys" button in the lower left section of the main window (below the viewport).
	* You will need to include the following meshes:
	*
	* - Hires Eye Left
	* - Hires Eye Right
	* - one of the three supported face meshes (Lores Skin, Medres Skin, or Hires Skin)
	* - Sock
	* - Teeth Lower
	* - Teeth Upper
	* - Tongue
	*
	* Additionally, you may include one of the hair meshes and the glasses.
	*
	* B. Make sure not to change the model's expression from neutral. In other words, do not use the Morph tab of FaceGen.
	* Should you accidentally change the values within, you can reset the model to the neutral expression by clicking the "Set All to Zero" button within the Morph tab.
	*
	* C. Make sure that you save textures as JPG, not BMP.
	*
	* @param faceGenModelFile Path to the target model for the cloning process.
	* @return The FaceGen model with a full set of morph targets or NULL
	* if source or target models have errors or are incompatible.
	* The returned model is ready for animation.
	*/
	AFM* clone( char* faceGenModelFile );
};

}

#endif // __FaceGenCloner_h__
