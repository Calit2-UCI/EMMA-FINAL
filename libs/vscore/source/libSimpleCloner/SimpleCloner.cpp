
#include "SimpleCloner.h"


namespace VisageSDK
{
	SimpleCloner::SimpleCloner()
	{
		sourceData = new FaceModelData();
		targetData = new FaceModelData();
		sourceData = new FaceModelData();
		targetData = new FaceModelData();
	}

	AFM* SimpleCloner::clone( AFM* sourceAFM, FDP* sourceFDP, VRMLModel* targetVRML )
	{
		int i, j;
		VRMLModel* sourceVRML; //source model
		int source_meshes[50], target_meshes[50], n_source_meshes = 0, n_target_meshes = 0,
			source_transfs[6], target_transfs[6], n_source_transfs = 0, n_target_transfs = 0;
		memset( source_meshes, -1, 50*sizeof(int) );
		memset( target_meshes, -1, 50*sizeof(int) );
		memset( source_transfs, -1, 6*sizeof(int) );
		memset( target_transfs, -1, 6*sizeof(int) );

		errors = "";

		assert( sourceAFM && targetVRML && sourceFDP );

		sourceVRML = sourceAFM->getVCModel();

		//verify and process source and target models:

		//process source model
		errors.append( "Errors processing source model: \n" );
		if( !sourceData->processModel( sourceVRML, sourceFDP ) )
		{
			errors.append( sourceData->getErrorsLog() );
			return NULL;
		}
		errors.append( sourceData->getErrorsLog() );

		//process target model
		errors.append( "Errors processing target model: \n" );
		if( !( targetData = sourceData->deriveFaceModelDataForTargetModel( targetVRML, sourceVRML ) ) )
		{
			errors.append( targetData->getErrorsLog() );
			return NULL;
		}
		errors.append( targetData->getErrorsLog() );

		//create an array of source meshes
		if( sourceData->getLEyeMeshesNo() )
		{
			for( i = 0; i < sourceData->getLEyeMeshesNo(); ++i )
				source_meshes[n_source_meshes++] = sourceData->getLEyeMesh(i);
		}
		if( sourceData->getREyeMeshesNo() )
		{
			for( i = 0; i < sourceData->getREyeMeshesNo(); ++i )
				source_meshes[n_source_meshes++] = sourceData->getREyeMesh(i);
		}
		if( sourceData->getFaceMesh() > -1 && targetData->getFaceMesh() > -1 )
		{
			source_meshes[n_source_meshes++] = sourceData->getFaceMesh();
		}
		else
		{
			return NULL;
		}
		if( sourceData->getLTeethMeshesNo() )
		{
			for( i = 0; i < sourceData->getLTeethMeshesNo(); ++i )
				source_meshes[n_source_meshes++] = sourceData->getLTeethMesh(i);
		}
		if( sourceData->getUTeethMesh() > -1 )
		{
			source_meshes[n_source_meshes++] = sourceData->getUTeethMesh();
		}
		if( sourceData->getTongueMeshesNo() )
		{
			for( i = 0; i < sourceData->getTongueMeshesNo(); ++i )
				source_meshes[n_source_meshes++] = sourceData->getTongueMesh(i);
		}
		//create an array of target meshes
		if( targetData->getLEyeMeshesNo() )
		{
			for( i = 0; i < targetData->getLEyeMeshesNo(); ++i )
				target_meshes[n_target_meshes++] = targetData->getLEyeMesh(i);
		}
		if( targetData->getREyeMeshesNo() )
		{
			for( i = 0; i < targetData->getREyeMeshesNo(); ++i )
				target_meshes[n_target_meshes++] = targetData->getREyeMesh(i);
		}
		if( sourceData->getFaceMesh() > -1 && targetData->getFaceMesh() > -1 )
		{
			target_meshes[n_target_meshes++] = targetData->getFaceMesh();
		}
		else
		{
			return NULL;
		}
		if( targetData->getLTeethMeshesNo() )
		{
			for( i = 0; i < targetData->getLTeethMeshesNo(); ++i )
				target_meshes[n_target_meshes++] = targetData->getLTeethMesh(i);
		}
		if( targetData->getUTeethMesh() > -1 )
		{
			target_meshes[n_target_meshes++] = targetData->getUTeethMesh();
		}
		if( targetData->getTongueMeshesNo() )
		{
			for( i = 0; i < targetData->getTongueMeshesNo(); ++i )
				target_meshes[n_target_meshes++] = targetData->getTongueMesh(i);
		}

		//create arrays of transf nodes
		if( sourceData->getGlobalTransf() > -1 && targetData->getGlobalTransf() > -1 )
		{
			source_transfs[n_source_transfs++] = sourceData->getGlobalTransf();
			target_transfs[n_target_transfs++] = targetData->getGlobalTransf();
		}
		if( sourceData->getLEyeTransf() > -1 && targetData->getLEyeTransf() > -1 )
		{
			source_transfs[n_source_transfs++] = sourceData->getLEyeTransf();
			target_transfs[n_target_transfs++] = targetData->getLEyeTransf();
		}
		if( sourceData->getREyeTransf() > -1 && targetData->getREyeTransf() > -1 )
		{
			source_transfs[n_source_transfs++] = sourceData->getREyeTransf();
			target_transfs[n_target_transfs++] = targetData->getREyeTransf();
		}
		if( sourceData->getLTeethTransf() > -1 && targetData->getLTeethTransf() > -1 )
		{
			source_transfs[n_source_transfs++] = sourceData->getLTeethTransf();
			target_transfs[n_target_transfs++] = targetData->getLTeethTransf();
		}
		if( sourceData->getUTeethTransf() > -1 && targetData->getUTeethTransf() > -1 )
		{
			source_transfs[n_source_transfs++] = sourceData->getUTeethTransf();
			target_transfs[n_target_transfs++] = targetData->getUTeethTransf();
		}
		if( sourceData->getTongueTransf() > -1 && targetData->getTongueTransf() > -1 )
		{
			source_transfs[n_source_transfs++] = sourceData->getTongueTransf();
			target_transfs[n_target_transfs++] = targetData->getTongueTransf();
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
		int a = verifyTopologiesByName(sourceVRML, targetVRML);
		if( (a == -1) || (a == 0))
		{
			for( i = 0; i < n_source_meshes; ++i )
			{
				if( sourceVRML->coordListSize[source_meshes[i]] != 
					targetVRML->coordListSize[target_meshes[i]] || 
					sourceVRML->triListSize[source_meshes[i]] != 
					targetVRML->triListSize[target_meshes[i]] )
				{
					if (a == 0)
						errors.append( "Topology mismatch. Source and target model differ in polygon layout." );
					else
						errors.append( "Topology mismatch. Source and target model differ in number of vertices." );
					return NULL;
				}
			}

			for( i = 0; i < n_source_meshes; ++i )
			{
				for( j = 0; j < sourceVRML->triListSize[source_meshes[i]]; ++j )
				{
					if( sourceVRML->triList[source_meshes[i]][j] != 
						targetVRML->triList[target_meshes[i]][j] )
					{
						errors.append( "Topology mismatch. Source and target model differ in polygon layout." );
						return NULL;
					}
				}
			}
		}

		return FinalizeCloning(sourceAFM, targetVRML, n_source_meshes, n_target_meshes, source_meshes, target_meshes, n_source_transfs, source_transfs, target_transfs);
	}

	AFM* SimpleCloner::clone( char* sourceAFMFile, char* targetVRMLFile )
	{
		VRMLModel* sourceVRML; //source model
		VRMLModel* targetVRML; 
		AFM *sourceAFM, *targetAFM;
		FDP* sourceFDP = new FDP();

		errors = "";

		sourceAFM = new AFM(sourceAFMFile);
		sourceFDP->setCharModel(sourceAFM);
		if( strstr(targetVRMLFile, ".wrl") != NULL)
			targetVRML = new VRMLModel(targetVRMLFile);
		else
		{
			targetAFM = new AFM(targetVRMLFile);
			targetVRML = targetAFM->getVCModel();
		}
		sourceVRML = sourceAFM->getVCModel();
		if(!sourceVRML->initialized)
		{

			errors.append("Source VRML file not initialized.\n");
			delete sourceAFM;
			return NULL;
		}
		strcpy( sourceAFMFile + strlen(sourceAFMFile) - 3, "fdp" );
		if( !sourceFDP->readFromFile(sourceAFMFile))
		{
			errors.append("Feature point definitions couldn't be loaded.\n");
			delete sourceAFM;
			return NULL;
		}

		errors = "";

		assert( sourceAFM && targetVRML && sourceFDP );
		return clone(sourceAFM, sourceFDP, targetVRML);

	}

	int SimpleCloner::verifyTopologiesByName(VRMLModel* sourceVRML, VRMLModel* targetVRML, bool isFaceGen)
	{
		int j;
		//if any of the meshes can't be found, the method failed
		if( ((targetVRML->findNamedMesh("skin_hi") == -1) && (targetVRML->findNamedMesh("skin_med") == -1) && (targetVRML->findNamedMesh("skin_lo") == -1)) || 
			((targetVRML->findNamedMesh("eyeL_hi") == -1) && (targetVRML->findNamedMesh("eyeL_lo") == -1)) || 
			((targetVRML->findNamedMesh("eyeR_hi") == -1) && (targetVRML->findNamedMesh("eyeR_lo") == -1)) ||
			targetVRML->findNamedMesh("teethLower") == -1 ||
			targetVRML->findNamedMesh("teethUpper") == -1 ||
			targetVRML->findNamedMesh("tongue") == -1 )
		{
			errors.append("Cannot find a mesh by name in the target model.\n");
			errors.append("Please ensure that the names of the meshes are correct!\n");
			errors.append("For more information on naming conventions, please consult the documentation!\n");
			return -1;
		}

		int a = -1, b = -1, c = -1;

		//comparison of the topology of the meshes
		if (isFaceGen == false)
		{
			a = targetVRML->findNamedMesh("skin_hi");
			if(a == -1)
			{
				a = targetVRML->findNamedMesh("skin_med");
			}
			if(a == -1)
			{
				a = targetVRML->findNamedMesh("skin_lo");
			}
			if( sourceVRML->coordListSize[sourceData->getFaceMesh()] !=
				targetVRML->coordListSize[a])
				return -1;

			b = targetVRML->findNamedMesh("eyeL_hi");
			if(b == -1)
			{
				b = targetVRML->findNamedMesh("eyeL_lo");
			}
			if( sourceVRML->coordListSize[sourceData->getLEyeMesh(0)] !=
				targetVRML->coordListSize[b])
				return -1;

			c = targetVRML->findNamedMesh("eyeR_hi");
			if(c == -1)
			{
				c = targetVRML->findNamedMesh("eyeR_lo");
			}
			if( sourceVRML->coordListSize[sourceData->getREyeMesh(0)] !=
				targetVRML->coordListSize[c])
				return -1;
		}

		if(a == -1)
		{
			a = targetVRML->findNamedMesh("skin_hi");
		}
		if(a == -1)
		{
			a = targetVRML->findNamedMesh("skin_med");
		}
		if(a == -1)
		{
			a = targetVRML->findNamedMesh("skin_lo");
		}
		if(	sourceVRML->triListSize[sourceData->getFaceMesh()] !=
			targetVRML->triListSize[a])
			return -1;

		if(b == -1)
		{
			b = targetVRML->findNamedMesh("eyeL_hi");
		}
		if(b == -1)
		{
			b = targetVRML->findNamedMesh("eyeL_lo");
		}
		if(	sourceVRML->triListSize[sourceData->getLEyeMesh(0)] !=
			targetVRML->triListSize[b] )
			return -1;

		if(c == -1)
		{
			c = targetVRML->findNamedMesh("eyeR_hi");
		}
		if(c == -1)
		{
			c = targetVRML->findNamedMesh("eyeR_lo");
		}
		if(	sourceVRML->triListSize[sourceData->getREyeMesh(0)] !=
			targetVRML->triListSize[c] )
			return -1;

		if( sourceVRML->coordListSize[sourceData->getLTeethMesh(0)] !=
			targetVRML->coordListSize[targetVRML->findNamedMesh("teethLower")] ||
			sourceVRML->triListSize[sourceData->getLTeethMesh(0)] !=
			targetVRML->triListSize[targetVRML->findNamedMesh("teethLower")] )
			return -1;

		if(sourceData->getUTeethMesh() > -1)
		{
			if( sourceVRML->coordListSize[sourceData->getUTeethMesh()] !=
				targetVRML->coordListSize[targetVRML->findNamedMesh("teethUpper")] ||
				sourceVRML->triListSize[sourceData->getUTeethMesh()] !=
				targetVRML->triListSize[targetVRML->findNamedMesh("teethUpper")] )
				return -1;
		}
		if( sourceVRML->coordListSize[sourceData->getTongueMesh(0)] !=
			targetVRML->coordListSize[targetVRML->findNamedMesh("tongue")] ||
			sourceVRML->triListSize[sourceData->getTongueMesh(0)] !=
			targetVRML->triListSize[targetVRML->findNamedMesh("tongue")] )
			return -1;

		for( j = 0; j < sourceVRML->triListSize[sourceData->getFaceMesh()]; ++j )
		{
			if( sourceVRML->triList[sourceData->getFaceMesh()][j] != 
				targetVRML->triList[a][j] )
				return 0;
		}
		for( j = 0; j < sourceVRML->triListSize[sourceData->getLEyeMesh(0)]; ++j )
		{
			if( sourceVRML->triList[sourceData->getLEyeMesh(0)][j] != 
				targetVRML->triList[b][j] )
				return 0;
		}
		for( j = 0; j < sourceVRML->triListSize[sourceData->getREyeMesh(0)]; ++j )
		{
			if( sourceVRML->triList[sourceData->getREyeMesh(0)][j] != 
				targetVRML->triList[c][j] )
				return 0;
		}
		for( j = 0; j < sourceVRML->triListSize[sourceData->getLTeethMesh(0)]; ++j )
		{
			if( sourceVRML->triList[sourceData->getLTeethMesh(0)][j] != 
				targetVRML->triList[targetVRML->findNamedMesh("teethLower")][j] )
				return 0;
		}
		if(sourceData->getUTeethMesh() > -1)
		{
			for( j = 0; j < sourceVRML->triListSize[sourceData->getUTeethMesh()]; ++j )
			{
				if( sourceVRML->triList[sourceData->getUTeethMesh()][j] != 
					targetVRML->triList[targetVRML->findNamedMesh("teethUpper")][j] )
					return 0;
			}
		}
		for( j = 0; j < sourceVRML->triListSize[sourceData->getTongueMesh(0)]; ++j )
		{
			if( sourceVRML->triList[sourceData->getTongueMesh(0)][j] != 
				targetVRML->triList[targetVRML->findNamedMesh("tongue")][j] )
				return 0;
		}
		return 1;
	}

	AFM* SimpleCloner::FinalizeCloning(AFM* sourceAFM, VRMLModel* targetVRML, int nSourceMeshes, int nTargetMeshes, int sourceMeshes[50], int targetMeshes[50], int nSourceTransfs, int sourceTransfs[6], int targetTransfs[6])
	{

		int i,j,k;
		AFM* outputAFM; //output AFM
		float sx = 1.f, sy = 1.f, sz = 1.f; //scale factors
		//compute scaling factors:
		float xmin_s, xmax_s, ymin_s, ymax_s, zmin_s, zmax_s,
			xmin_t, xmax_t, ymin_t, ymax_t, zmin_t, zmax_t;	

		sourceData->getAABB( xmin_s, ymin_s, zmin_s, xmax_s, ymax_s, zmax_s );
		targetData->getAABB( xmin_t, ymin_t, zmin_t, xmax_t, ymax_t, zmax_t );

		sx = ( xmax_t - xmin_t ) / ( xmax_s - xmin_s );
		sy = ( ymax_t - ymin_t ) / ( ymax_s - ymin_s );
		sz = ( zmax_t - zmin_t ) / ( zmax_s - zmin_s );

		//clone morph targets:

		BaseAFMDeformer *def, *output_def;

		//get source deformer
		for( i = 0; i < sourceAFM->getNAFMDeformers(); ++i )
		{
			if( !( def = dynamic_cast<BaseAFMDeformer*>( sourceAFM->getAFMDeformer(i) ) ) )
				continue;
		}

		//no morph targets?
		if( !def ) return NULL;

		//create empty AFM
		VRMLModel* mt[NB_SD]; //morph targets
		int refVals[NB_SD]; //ref. values
		for( i = 0; i < NB_SD; ++i )
		{
			mt[i]= NULL;
			refVals[i] = -1;
		}
		outputAFM = new AFM( targetVRML, mt, refVals, 0.1f );
		outputAFM->removeAllAFMDeformers();

		//get target deformer
		VRMLModelModifier *vis_mod = new VRMLModelModifier[15],
			*exp_mod = new VRMLModelModifier[7],
			*fap_mod = new VRMLModelModifier[68],
			*bap_mod = new VRMLModelModifier[NUM_BAPS];

		//copy morph targets / bone mappings
		for( i = 0; i < NB_SD; ++i )
		{
			VRMLModelModifier *mod, *output_mod;

			if( i < 15 )
			{
				mod = &def->VisemeModifiers[i];
				output_mod = &vis_mod[i];
			}
			else if( i < 22 )
			{
				mod = &def->ExpressionModifiers[i - 15];
				output_mod = &exp_mod[i - 15];
			}
			else
			{
				mod = &def->FAPModifiers[i - 20];
				output_mod = &fap_mod[i - 20];
			}

			if( !mod || mod->refValue == -1 )
			{
				continue;
			}

			output_mod->refValue = mod->refValue;

			if( mod->MTData )
			{
				output_mod->MTData = new float*[outputAFM->getVCModel()->nCoordLists];
				memset( output_mod->MTData, 0, outputAFM->getVCModel()->nCoordLists * sizeof(float*) );
				for( j = 0; j < nSourceMeshes; ++j )
				{
					if( !mod->MTData[sourceMeshes[j]] )
					{
						continue;
					}

					output_mod->MTData[targetMeshes[j]] = 
						new float[outputAFM->getVCModel()->coordListSize[targetMeshes[j]]];

					for( k = 0; k < outputAFM->getVCModel()->coordListSize[targetMeshes[j]]; k+=3 )
					{
						output_mod->MTData[targetMeshes[j]][k] = mod->MTData[sourceMeshes[j]][k] * sx;
						output_mod->MTData[targetMeshes[j]][k+1] = mod->MTData[sourceMeshes[j]][k+1] * sy;
						output_mod->MTData[targetMeshes[j]][k+2] = mod->MTData[sourceMeshes[j]][k+2] * sz;
					}
				}
			}

			if( mod->TRotData )
			{
				output_mod->TRotData = new float*[outputAFM->getVCModel()->nTransfLists];
				memset( output_mod->TRotData, 0,
					outputAFM->getVCModel()->nTransfLists * sizeof(float*) );

				for( j = 0; j < nSourceTransfs; ++j )
				{
					if( !mod->TRotData[sourceTransfs[j]] )
					{
						continue;
					}

					output_mod->TRotData[targetTransfs[j]] = new float[4];

					memcpy( output_mod->TRotData[targetTransfs[j]],
						mod->TRotData[sourceTransfs[j]], 4*sizeof(float) );
				}
			}

			if( mod->TScalData )
			{
				output_mod->TScalData = new float*[outputAFM->getVCModel()->nTransfLists];
				memset( output_mod->TScalData, 0,
					outputAFM->getVCModel()->nTransfLists * sizeof(float*) );

				for( j = 0; j < nSourceTransfs; ++j )
				{
					if( !mod->TScalData[sourceTransfs[j]] )
					{
						continue;
					}

					output_mod->TScalData[targetTransfs[j]] = new float[3];

					memcpy( output_mod->TScalData[targetTransfs[j]],
						mod->TScalData[sourceTransfs[j]], 3*sizeof(float) );
				}
			}

			if( mod->TTranData )
			{
				output_mod->TTranData = new float*[outputAFM->getVCModel()->nTransfLists];
				memset( output_mod->TTranData, 0,
					outputAFM->getVCModel()->nTransfLists * sizeof(float*) );

				for( j = 0; j < nSourceTransfs; ++j )
				{
					if( !mod->TTranData[sourceTransfs[j]] )
					{
						continue;
					}

					output_mod->TTranData[targetTransfs[j]] = new float[3];

					memcpy( output_mod->TTranData[targetTransfs[j]],
						mod->TTranData[sourceTransfs[j]], 3*sizeof(float) );
				}
			}
		}

		for( i = 0; i < NUM_BAPS; ++i )
			bap_mod[i].refValue = -1;

		output_def = new BaseAFMDeformer( targetVRML, fap_mod, exp_mod, vis_mod, bap_mod );
		outputAFM->addAFMDeformer( output_def );

		return outputAFM;
	}

}
