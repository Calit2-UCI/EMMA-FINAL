
#include "FaceGenCloner.h"


namespace VisageSDK
{

	FaceGenCloner::FaceGenCloner()
	{

		sourceData = new FaceModelData();
		targetData = new FaceModelData();
	}

	AFM* FaceGenCloner::clone( VRMLModel* faceGenModel )
	{
		int i, j, n;

		string sourceAFMFile =
			".\\SCSource\\src_lo.afm";
		string sourceFDPFile =
			".\\SCSource\\src_lo.fdp";
		int sourceMeshes[50], targetMeshes[50], sourceTransfs[6], targetTransfs[6];
		AFM* sourceAFM = NULL;
		int nSourceMeshes, nTargetMeshes, nSourceTransfs, nTargetTransfs;
		VRMLModel* sourceVRML; //source model


		//there are 3 pregenerated FaceGen source models. This function will try to find which one is the correct one.
		for (n=0; n < 3; n++)
		{
			//if the source model is not src_lo.afm, we change the path to point to src_mid.afm
			if (n == 1)
			{
				sourceAFMFile.replace(sourceAFMFile.rfind("src_lo"), 6, "src_med");
				sourceFDPFile.replace(sourceFDPFile.rfind("src_lo"), 6, "src_med");
			}
			//if the source model is not src_mid.afm, we change the path to point to src_hi.afm
			if (n == 2)
			{
				sourceAFMFile.replace(sourceAFMFile.rfind("src_med"), 7, "src_hi");
				sourceFDPFile.replace(sourceFDPFile.rfind("src_med"), 7, "src_hi");
			}
			if( sourceAFM != NULL ) delete sourceAFM;
			sourceAFM = new AFM();
			sourceAFM->read((char*)sourceAFMFile.c_str());
			FDP* sourceFDP = new FDP(sourceAFM);
			nSourceMeshes = 0;
			memset( sourceMeshes, -1, 50*sizeof(int) );

			errors = "";
			//conversion from .afm to .wrl
			sourceVRML = sourceAFM->getVCModel();
			if(!sourceVRML->initialized)
			{		
				errors.append("Source VRML file not initialized.\n");
				delete sourceAFM;
				return NULL;
			}
			if( !sourceFDP->readFromFile((char*)sourceFDPFile.c_str()))
			{
				errors.append("Feature point definitions couldn't be loaded.\n");
				delete sourceAFM;
				return NULL;
			}

			//verify and process source and target models:

			assert( sourceAFM && faceGenModel && sourceFDP );

			//process source model
			errors.append( "Errors processing source model: \n" );
			if( !sourceData->processModel( sourceVRML, sourceFDP ) )
			{
				errors.append( sourceData->getErrorsLog() );
				return NULL;
			}
			errors.append( sourceData->getErrorsLog() );
			//create an array of source meshes
			if( sourceData->getLEyeMeshesNo() )
			{
				for( i = 0; i < sourceData->getLEyeMeshesNo(); ++i )
					sourceMeshes[nSourceMeshes++] = sourceData->getLEyeMesh(i);
			}
			if( sourceData->getREyeMeshesNo() )
			{
				for( i = 0; i < sourceData->getREyeMeshesNo(); ++i )
					sourceMeshes[nSourceMeshes++] = sourceData->getREyeMesh(i);
			}
			if( sourceData->getFaceMesh() > -1)
			{
				sourceMeshes[nSourceMeshes++] = sourceData->getFaceMesh();
			}
			else
			{
				return NULL;
			}
			if( sourceData->getLTeethMeshesNo() )
			{
				for( i = 0; i < sourceData->getLTeethMeshesNo(); ++i )
					sourceMeshes[nSourceMeshes++] = sourceData->getLTeethMesh(i);
			}
			if( sourceData->getUTeethMesh() > -1 )
			{
				sourceMeshes[nSourceMeshes++] = sourceData->getUTeethMesh();
			}
			if( sourceData->getTongueMeshesNo() )
			{
				for( i = 0; i < sourceData->getTongueMeshesNo(); ++i )
					sourceMeshes[nSourceMeshes++] = sourceData->getTongueMesh(i);
			}

			int a, b;
			a = faceGenModel->findNamedMesh("skin_hi");
			if (a == -1)
			{
				a = faceGenModel->findNamedMesh("skin_med");
			}
			if (a == -1)
			{
				a = faceGenModel->findNamedMesh("skin_lo");
			}
			b = faceGenModel->findNamedMesh("eyeL_hi");
			if (b == -1)
			{
				b = faceGenModel->findNamedMesh("eyeL_lo");
			}
			if ((a != -1) && (b != -1))
			{
				if((sourceVRML->coordListSize[sourceData->getFaceMesh()] == faceGenModel->coordListSize[a])
					&& (sourceVRML->coordListSize[sourceData->getLEyeMesh(0)] == faceGenModel->coordListSize[b]))
				{
					break;
				}
			}
			if ((sourceVRML->coordListSize[sourceData->getFaceMesh()] == faceGenModel->coordListSize[sourceData->getFaceMesh()])
				&& (sourceVRML->coordListSize[sourceData->getLEyeMesh(0)] == faceGenModel->coordListSize[sourceData->getLEyeMesh(0)]))
			{
				break;
			}		
		}

		//process target model
		errors.append( "Errors processing target model: \n" );
		if( !( targetData = sourceData->deriveFaceModelDataForTargetModel( faceGenModel, sourceVRML ) ) )
		{
			errors.append( targetData->getErrorsLog() );
			return NULL;
		}
		errors.append( targetData->getErrorsLog() );

		nTargetMeshes = nSourceTransfs = nTargetTransfs = 0;
		memset( targetMeshes, -1, 50*sizeof(int) );
		memset( sourceTransfs, -1, 6*sizeof(int) );
		memset( targetTransfs, -1, 6*sizeof(int) );

		//create an array of target meshes
		if( targetData->getLEyeMeshesNo() )
		{
			for( i = 0; i < targetData->getLEyeMeshesNo(); ++i )
				targetMeshes[nTargetMeshes++] = targetData->getLEyeMesh(i);
		}
		if( targetData->getREyeMeshesNo() )
		{
			for( i = 0; i < targetData->getREyeMeshesNo(); ++i )
				targetMeshes[nTargetMeshes++] = targetData->getREyeMesh(i);
		}
		if( sourceData->getFaceMesh() > -1 && targetData->getFaceMesh() > -1 )
		{
			targetMeshes[nTargetMeshes++] = targetData->getFaceMesh();
		}
		else
		{
			return NULL;
		}
		if( targetData->getLTeethMeshesNo() )
		{
			for( i = 0; i < targetData->getLTeethMeshesNo(); ++i )
				targetMeshes[nTargetMeshes++] = targetData->getLTeethMesh(i);
		}
		if( targetData->getUTeethMesh() > -1 )
		{
			targetMeshes[nTargetMeshes++] = targetData->getUTeethMesh();
		}
		if( targetData->getTongueMeshesNo() )
		{
			for( i = 0; i < targetData->getTongueMeshesNo(); ++i )
				targetMeshes[nTargetMeshes++] = targetData->getTongueMesh(i);
		}	

		/**
		* Verify that the models have identical topologies.
		* This is done by comparing the coresponding meshes of the source and target model.
		* The meshes of the source model can easily be identified via FDPs, but that is not possible in the target model.
		* That means that a previously-agreed-to identification method has to be used.
		* The SimpleCloner uses two methods for that:
		* The first method attempts to get the meshes of the target model by name.
		* The second method assumes the indexes of the coresponding meshes in the source and target model are the same.
		* It is executed if the first one returns -1 or 0.
		*/
		int a = verifyTopologiesByName(sourceVRML, faceGenModel, true);
		if ( (a == -1) || (a == 0) )
		{
			for( i = 0; i < nSourceMeshes; ++i )
			{
				if( (i == sourceData->getFaceMesh()) || (i == sourceData->getLEyeMesh(0)) || (i == sourceData->getREyeMesh(0)))
					continue;
				if( sourceVRML->coordListSize[sourceMeshes[i]] != 
					faceGenModel->coordListSize[targetMeshes[i]] || 
					sourceVRML->triListSize[sourceMeshes[i]] != 
					faceGenModel->triListSize[targetMeshes[i]] )
				{
					if(a == 0)
						errors.append( "Topology mismatch. Source and target model differ in polygon layout." );
					else
						errors.append( "Topology mismatch. Source and target model differ in number of vertices." );
					return NULL;
				}
			}
			//if the first or second source model is not the correct one, we skip the rest of the for loop
			//and go to the next step

			for( i = 0; i < nSourceMeshes; ++i )
			{
				for( j = 0; j < sourceVRML->triListSize[sourceMeshes[i]]; ++j )
				{
					if( sourceVRML->triList[sourceMeshes[i]][j] != 
						faceGenModel->triList[targetMeshes[i]][j] )
					{
						errors.append( "Topology mismatch. Source and target model differ in polygon layout." );
						return NULL;
					}
				}
			}
		}
		//create arrays of transf nodes
		if( sourceData->getGlobalTransf() > -1 && targetData->getGlobalTransf() > -1 )
		{
			sourceTransfs[nSourceTransfs++] = sourceData->getGlobalTransf();
			targetTransfs[nTargetTransfs++] = targetData->getGlobalTransf();
		}
		if( sourceData->getLEyeTransf() > -1 && targetData->getLEyeTransf() > -1 )
		{
			sourceTransfs[nSourceTransfs++] = sourceData->getLEyeTransf();
			targetTransfs[nTargetTransfs++] = targetData->getLEyeTransf();
		}
		if( sourceData->getREyeTransf() > -1 && targetData->getREyeTransf() > -1 )
		{
			sourceTransfs[nSourceTransfs++] = sourceData->getREyeTransf();
			targetTransfs[nTargetTransfs++] = targetData->getREyeTransf();
		}
		if( sourceData->getLTeethTransf() > -1 && targetData->getLTeethTransf() > -1 )
		{
			sourceTransfs[nSourceTransfs++] = sourceData->getLTeethTransf();
			targetTransfs[nTargetTransfs++] = targetData->getLTeethTransf();
		}
		if( sourceData->getUTeethTransf() > -1 && targetData->getUTeethTransf() > -1 )
		{
			sourceTransfs[nSourceTransfs++] = sourceData->getUTeethTransf();
			targetTransfs[nTargetTransfs++] = targetData->getUTeethTransf();
		}
		if( sourceData->getTongueTransf() > -1 && targetData->getTongueTransf() > -1 )
		{
			sourceTransfs[nSourceTransfs++] = sourceData->getTongueTransf();
			targetTransfs[nTargetTransfs++] = targetData->getTongueTransf();
		}

		return FinalizeCloning(sourceAFM, faceGenModel, nSourceMeshes, nTargetMeshes, sourceMeshes, targetMeshes, nSourceTransfs, sourceTransfs, targetTransfs);
	}

	AFM* FaceGenCloner::clone( char* faceGenModelFile )
	{
		VRMLModel* faceGenModel = new VRMLModel(faceGenModelFile);
		return clone(faceGenModel);
	}

}
