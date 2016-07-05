/*
-----------------------------------------------------------------------------
Original file:	???
New Author:		Balazs Hajdics (wolverine_@freemail.hu)

Copyright (c) 2007 Balazs Hajdics

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.

-----------------------------------------------------------------------------
*/


#ifndef __dotSceneInfo_H__
#define __dotSceneInfo_H__

#include "dotScenePrerequisites.h"

#include "HDRCompositor.h"

namespace Ogre
{
	/***************************************************************************************************/
	namespace dsi
	{
		/// struct containing extended movable object info
		struct _DotSceneInterfaceExport MovableObjectEx
		{
		public:
			Ogre::MovableObject *pMovable;
			Ogre::Quaternion	rotation;
			Ogre::Vector3		position;
			Ogre::Vector3		scale;
			bool				isStatic;

		public:
			/// default ctor
			MovableObjectEx(Ogre::MovableObject *pMO, Ogre::Vector3 &pos, Ogre::Quaternion &rot, Ogre::Vector3 &sca, bool isStat);

		private:
			/// this ctor should not be used
			MovableObjectEx() {};
		};
		/***************************************************************************************************/
		/// progress listener abstract class
		/***************************************************************************************************/
		/// progress listener class, object that receive progress update from progressMaker classes it's registered with
		class _DotSceneInterfaceExport progressListener
		{
		public:
			virtual void onProgress(const Ogre::String &text) = 0;
		};
		/***************************************************************************************************/
		/// progress maker class
		/***************************************************************************************************/
		/// progress maker class, objects that makes progress can implement this, so can update progress listeners with progress info Strings
		class _DotSceneInterfaceExport progressMaker
		{
		private:
			typedef std::set<progressListener*> tPLSet;
		
		protected:
			tPLSet pls;
		
		public:
			/// add a new progressListener to the stack
			virtual void addProgressListener(progressListener *pNewPL)
			{
				// check if it's already in first
				if (pls.find(pNewPL) == pls.end())
					pls.insert(pNewPL);
				else
					OGRE_EXCEPT(Exception::ERR_DUPLICATE_ITEM, "the specified progressListener is already in the list", "progressListener::addProgressListener");
			}
			/// update all registered progress listeners with the given message
			void updateProgressListeners(const Ogre::String &text)
			{
				tPLSet::iterator endIT = pls.end();
				for (tPLSet::iterator it = pls.begin(); it != endIT; ++it)
					(*it)->onProgress(text);
			}
			/// remove a specific progressListener from the stack
			void removeProgressListener(progressListener *pPL)
			{
				tPLSet::iterator it = pls.find(pPL);
				if (it != pls.end())
					pls.erase(it);
				else
					OGRE_EXCEPT(Exception::ERR_DUPLICATE_ITEM, "the specified progressListener is not in the list", "progressListener::removeProgressListener");
			}
		};

		/// Helper macro for functions of DotSceneInfo class like, reportLoadedEntities(), reportLoadedLights(), etc.
		#define REPORT_LOADED_MOVABLEOBJECT(movableObject, isStatic, ListForAllObjects, NonStaticList, StaticList)\
		{\
			if ( isStatic )\
				StaticList.push_back(movableObject);\
			else\
				NonStaticList.push_back(movableObject);\
\
			ListForAllObjects.push_back(movableObject);\
\
			reportLoadedMovableObject(movableObject, isStatic);\
		}

		class _DotSceneInterfaceExport DotSceneInfo
		{
		public:

			/// Structure type to store data of fog.
			struct FogData
			{
				Ogre::FogMode		mFogMode;
				Ogre::ColourValue	mColour;
				Real				mExpDensity;
				Real				mLinearStart;
				Real				mLinearEnd;
			};

			struct SkyBoxData
			{
				String				mName;
				bool				mEnabled;

				String				mMaterialName;
				Real				mDistance;
				bool				mDrawFirst;

				Quaternion			mOrientation;
			};

			struct SkyDomeData
			{
				String				mName;
				bool				mEnabled;

				String				mMaterialName;

				Real				mCurvature;
				Real				mTiling;

				Real				mDistance;
				bool				mDrawFirst;

				Real				mXSegments;
				Real				mYSegments;
				Real				mYSegmentsKeep;

				Quaternion			mOrientation;
			};

			struct SkyPlaneData
			{
				String				mName;
				bool				mEnabled;

				Ogre::Plane			mPlane;

				String				mMaterialName;

				Real				mScale;
				Real				mTiling;
				bool				mDrawFirst;

				Real				mBow;
				Real				mXSegments;
				Real				mYSegments;
			};

//		protected:
		public:

			/// @name Types for gathering loaded objects.
			/// We are not using sets for storing loaded objects, since loading order may become important at a given stage
			/// of development, however we want to avoid adding the same object to one of the internal lists more then once.
			//@{

			typedef std::vector<SceneNode*>							LoadedSceneNodeList;
			typedef VectorIterator<LoadedSceneNodeList>				LoadedSceneNodeIterator;
			typedef ConstVectorIterator<LoadedSceneNodeList>		ConstLoadedSceneNodeIterator;


			typedef std::map<Ogre::Viewport*, HDRCompositor*>		LoadedHDRCompositorInstancesMap;
			typedef MapIterator<LoadedHDRCompositorInstancesMap>	LoadedHDRCompositorInstancesMapIterator;
			typedef ConstMapIterator<LoadedHDRCompositorInstancesMap>	ConstLoadedHDRCompositorInstancesMapIterator;


			typedef std::vector<FogData>							FogDataList;
			typedef VectorIterator<FogDataList>						FogDataIterator;
			typedef ConstVectorIterator<FogDataList>				ConstFogDataIterator;

			typedef std::map<String, SkyBoxData>					SkyBoxMap;
			typedef MapIterator<SkyBoxMap>							SkyBoxMapIterator;
			typedef ConstMapIterator<SkyBoxMap>						ConstSkyBoxMapIterator;

			typedef std::map<String, SkyDomeData>					SkyDomeMap;
			typedef MapIterator<SkyDomeMap>							SkyDomeMapIterator;
			typedef ConstMapIterator<SkyDomeMap>					ConstSkyDomeMapIterator;

			typedef std::map<String, SkyPlaneData>					SkyPlaneMap;
			typedef MapIterator<SkyPlaneMap>						SkyPlaneMapIterator;
			typedef ConstMapIterator<SkyPlaneMap>					ConstSkyPlaneMapIterator;


			typedef std::vector<MovableObject*>						LoadedMovableObjectList;
			typedef VectorIterator<LoadedMovableObjectList>			LoadedMovableObjectIterator;
			typedef ConstVectorIterator<LoadedMovableObjectList>	ConstLoadedMovableObjectIterator;



			typedef std::vector<Entity*>							LoadedEntityList;
			typedef VectorIterator<LoadedEntityList>				LoadedEntityIterator;
			typedef ConstVectorIterator<LoadedEntityList>			ConstLoadedEntityIterator;

			typedef std::vector<Light*>								LoadedLightList;
			typedef VectorIterator<LoadedLightList>					LoadedLightIterator;
			typedef ConstVectorIterator<LoadedLightList>			ConstLoadedLightIterator;

			typedef std::vector<Camera*>							LoadedCameraList;
			typedef VectorIterator<LoadedCameraList>				LoadedCameraIterator;
			typedef ConstVectorIterator<LoadedCameraList>			ConstLoadedCameraIterator;

			typedef std::vector<ParticleSystem*>					LoadedParticleSystemList;
			typedef VectorIterator<LoadedParticleSystemList>		LoadedParticleSystemIterator;
			typedef ConstVectorIterator<LoadedParticleSystemList>	ConstLoadedParticleSystemIterator;

			typedef std::vector<BillboardSet*>						LoadedBillboardSetList;
			typedef VectorIterator<LoadedBillboardSetList>			LoadedBillboardSetIterator;
			typedef ConstVectorIterator<LoadedBillboardSetList>		ConstLoadedBillboardSetIterator;


			typedef std::vector<AnimationState*>					LoadedAnimationStateList;
			typedef VectorIterator<LoadedAnimationStateList>		LoadedAnimationStateIterator;
			typedef ConstVectorIterator<LoadedAnimationStateList>	ConstLoadedAnimationStateIterator;

			typedef std::multimap<SceneManager*, AnimationState*>			SceneManagerAnimationStatesMultiMap;
			typedef MapIterator<SceneManagerAnimationStatesMultiMap>		SceneManagerAnimationStateIterator;
			typedef ConstMapIterator<SceneManagerAnimationStatesMultiMap>	ConstSceneManagerAnimationStateIterator;

			typedef std::multimap<Entity*, AnimationState*>			EntityAnimationStatesMultiMap;
			typedef MapIterator<EntityAnimationStatesMultiMap>		EntityAnimationStateIterator;
			typedef ConstMapIterator<EntityAnimationStatesMultiMap>	ConstEntityAnimationStateIterator;



			typedef std::vector<Animation*>							LoadedAnimationList;
			typedef VectorIterator<LoadedAnimationList>				LoadedAnimationIterator;
			typedef ConstVectorIterator<LoadedAnimationList>		ConstLoadedAnimationIterator;

			typedef std::multimap<SceneManager*, Animation*>		SceneManagerAnimationsMultiMap;
			typedef MapIterator<SceneManagerAnimationsMultiMap>		SceneManagerAnimationIterator;
			typedef ConstMapIterator<SceneManagerAnimationsMultiMap>	ConstSceneManagerAnimationIterator;


			typedef std::multimap<Entity*, Animation*>				EntityAnimationsMultiMap;
			typedef MapIterator<EntityAnimationsMultiMap>			EntityAnimationIterator;
			typedef ConstMapIterator<EntityAnimationsMultiMap>		ConstEntityAnimationIterator;


			//@}

			typedef std::vector<Ogre::String>						LoadLogVector;
			typedef	Ogre::VectorIterator<LoadLogVector>				LoadLogVectorIterator;
			typedef	Ogre::ConstVectorIterator<LoadLogVector>		ConstLoadLogVectorIterator;

		public:




		protected:
			SceneManager*											mSceneManager;
			SceneNode*												mRootSceneNode;
			LoadedSceneNodeList										mLoadedSceneNodes;

			/// \brief Vector of fog data located in the .scene file.
			///
			/// @remarks If more then one "fog" element is present in the .scene file, all of them will be loaded and reported back
			/// to DotSceneInfo instance, but only the one specified last will affect the scene itself.
			FogDataList												mFogData;

			SkyBoxMap												mSkyBoxes;
			SkyDomeMap												mSkyDomes;
			SkyPlaneMap												mSkyPlanes;

			ColourValue												mAmbientLightColour;
			bool													mHasAmbientLightColourSet;
			ColourValue												mBackgroundColour;
			bool													mHasBackgroundColourSet;


			LoadedHDRCompositorInstancesMap							mLoadedHDRCompositorInstancesMap;



			LoadedMovableObjectList									mLoadedMovableObjects;
			LoadedMovableObjectList									mLoadedStaticMovableObjects;
			LoadedMovableObjectList									mLoadedNonStaticMovableObjects;

			LoadedEntityList										mLoadedEntities;
			LoadedEntityList										mLoadedNonStaticEntities;
			LoadedEntityList										mLoadedStaticEntities;

			LoadedLightList											mLoadedLights;
			LoadedLightList											mLoadedNonStaticLights;
			LoadedLightList											mLoadedStaticLights;

			LoadedCameraList										mLoadedCameras;
			LoadedCameraList										mLoadedNonStaticCameras;
			LoadedCameraList										mLoadedStaticCameras;

			LoadedParticleSystemList								mLoadedParticleSystems;
			LoadedParticleSystemList								mLoadedNonStaticParticleSystems;
			LoadedParticleSystemList								mLoadedStaticParticleSystems;

			LoadedBillboardSetList									mLoadedBillboardSets;
			LoadedBillboardSetList									mLoadedNonStaticBillboardSets;
			LoadedBillboardSetList									mLoadedStaticBillboardSets;

			LoadedAnimationStateList								mLoadedAnimationStates;
			/// Animation states created/retrieved using the scene manager.
			SceneManagerAnimationStatesMultiMap						mLoadedSceneManagerAnimationStates;
			/// Animation states created/retrieved using a given entity.
			EntityAnimationStatesMultiMap							mLoadedEntityAnimationStates;

			LoadedAnimationList										mLoadedAnimations;
			/// Animations created using the scene manager.
			SceneManagerAnimationsMultiMap							mLoadedSceneManagerAnimations;
			/// Animations created using a given entity.
			EntityAnimationsMultiMap								mLoadedEntityAnimations;


			/// Vector consisting all logs which occured during loading.
			LoadLogVector											mLoadLogs;
			/// Vector consisting only informational messages logged during scene loading.
			LoadLogVector											mLoadInfoLogs;
			/// Vector consisting only informational messages logged during scene loading.
			LoadLogVector											mLoadWarningLogs;
			/// Vector consisting only informational messages logged during scene loading.
			LoadLogVector											mLoadErrorLogs;
		public:
			// --------------------------------------------------- Scene manager ---------------------------------------------------

			void	reportSceneManager(SceneManager* sceneManager)
			{
				mSceneManager = sceneManager;
			}

			SceneManager*	getSceneManager()
			{
				return mSceneManager;
			}

			
			// -------------------------------------------------- Root scene node --------------------------------------------------

			void	reportLoadedRootSceneNode(SceneNode* rootSceneNode)
			{
				mRootSceneNode = rootSceneNode;
			}

			SceneNode*	getRootSceneNode()
			{
				return mRootSceneNode;
			}

			// --------------------------------------------- HDR Compositor instances ----------------------------------------------

			void	reportLoadedHDRCompositorInstance(Viewport* viewport, HDRCompositor*	hdrCompositor)
			{
				mLoadedHDRCompositorInstancesMap.insert(LoadedHDRCompositorInstancesMap::value_type(viewport, hdrCompositor));
			}

			bool	hasHDRCompositorInstance()
			{
				return (mLoadedHDRCompositorInstancesMap.size() != 0);
			}

			LoadedHDRCompositorInstancesMapIterator		getHDRCompositorInstanceMapIterator()
			{
				return LoadedHDRCompositorInstancesMapIterator(
					mLoadedHDRCompositorInstancesMap.begin(), 
					mLoadedHDRCompositorInstancesMap.end());
			}

			ConstLoadedHDRCompositorInstancesMapIterator	getHDRCompositorInstanceMapIterator() const
			{
				return ConstLoadedHDRCompositorInstancesMapIterator(
					mLoadedHDRCompositorInstancesMap.begin(), 
					mLoadedHDRCompositorInstancesMap.end());
			}



			// -------------------------------------------------------- Fog --------------------------------------------------------

			void	reportLoadedFogData(FogData	fogData)
			{
				mFogData.push_back(fogData);
			}

			bool	hasFog()
			{
				// This could be written simply as return mFogData.size(), however writting it like that avoids warning C4800.
				return (mFogData.size() != 0);
			}

			FogDataIterator	getFogDataIterator()
			{
				return FogDataIterator(mFogData.begin(), mFogData.end());
			}

			ConstFogDataIterator	getFogDataIterator() const
			{
				return ConstFogDataIterator(mFogData.begin(), mFogData.end());
			}


			// ------------------------------------------------------ Skybox -------------------------------------------------------

			void	reportLoadedSkyBox(SkyBoxData skyBoxData)
			{
				mSkyBoxes[skyBoxData.mName] = skyBoxData;
			}

			bool	hasSkyBox()
			{
				return (mSkyBoxes.size() != 0);
			}

			SkyBoxMapIterator	getSkyBoxMapIterator()
			{
				return SkyBoxMapIterator(mSkyBoxes.begin(), mSkyBoxes.end());
			}

			ConstSkyBoxMapIterator	getSkyBoxMapIterator() const
			{
				return ConstSkyBoxMapIterator(mSkyBoxes.begin(), mSkyBoxes.end());
			}


			// ------------------------------------------------------ Skydome ------------------------------------------------------

			void	reportLoadedSkyDome(SkyDomeData skyDomeData)
			{
				mSkyDomes[skyDomeData.mName] = skyDomeData;
			}

			bool	hasSkyDome()
			{
				return (mSkyDomes.size() != 0);
			}

			SkyDomeMapIterator	getSkyDomeMapIterator()
			{
				return SkyDomeMapIterator(mSkyDomes.begin(), mSkyDomes.end());
			}

			ConstSkyDomeMapIterator	getSkyDomeMapIterator() const
			{
				return ConstSkyDomeMapIterator(mSkyDomes.begin(), mSkyDomes.end());
			}


			// ------------------------------------------------------ Skyplane ------------------------------------------------------

			void	reportLoadedSkyPlane(SkyPlaneData skyPlaneData)
			{
				mSkyPlanes[skyPlaneData.mName] = skyPlaneData;
			}

			bool	hasSkyPlane()
			{
				return (mSkyPlanes.size() != 0);
			}

			SkyPlaneMapIterator	getSkyPlaneMapIterator()
			{
				return SkyPlaneMapIterator(mSkyPlanes.begin(), mSkyPlanes.end());
			}

			ConstSkyPlaneMapIterator	getSkyPlaneMapIterator() const
			{
				return ConstSkyPlaneMapIterator(mSkyPlanes.begin(), mSkyPlanes.end());
			}


			// ------------------------------------------------ Ambient light colour -----------------------------------------------

			void	reportAmbientLightColour(ColourValue ambientLightColour)
			{
				mAmbientLightColour			= ambientLightColour;
				mHasAmbientLightColourSet	= true;
			}
			bool hasAmbientLightColourSet()
			{
				return mHasAmbientLightColourSet;
			}
			ColourValue getAmbientLightColour()
			{
				return mAmbientLightColour;
			}


			// -------------------------------------------------- Background colour ------------------------------------------------

			void	reportBackgroundColour(ColourValue backgroundColour)
			{
				mBackgroundColour = backgroundColour;
			}

			bool hasBackgroundColourSet()
			{
				return mHasBackgroundColourSet;
			}

			ColourValue getBackgroundColour()
			{
				return mBackgroundColour;
			}


			// ----------------------------------------------------- Scene nodes ---------------------------------------------------
			void	reportLoadedSceneNode(SceneNode* sceneNode)
			{
				mLoadedSceneNodes.push_back(sceneNode);
			}
			bool	hasSceneNodes()
			{
				return (mLoadedSceneNodes.size() != 0);
			}

			LoadedSceneNodeIterator	getSceneNodeIterator()
			{
				return LoadedSceneNodeIterator(mLoadedSceneNodes.begin(), mLoadedSceneNodes.end());
			}

			ConstLoadedSceneNodeIterator	getSceneNodeIterator() const
			{
				return ConstLoadedSceneNodeIterator(mLoadedSceneNodes.begin(), mLoadedSceneNodes.end());
			}


			// --------------------------------------------------- Movable objects -------------------------------------------------

			void	reportLoadedMovableObject(MovableObject* movableObject, bool isStatic)
			{
				if ( isStatic )
					mLoadedStaticMovableObjects.push_back(movableObject);
				else
					mLoadedNonStaticMovableObjects.push_back(movableObject);
				
				mLoadedMovableObjects.push_back(movableObject);
			}

			void	reportLoadedEntity(Entity* entity, bool isStatic)
			{
				REPORT_LOADED_MOVABLEOBJECT(
					entity, isStatic, mLoadedEntities, mLoadedNonStaticEntities, mLoadedStaticEntities);
			}

			void	reportLoadedLight(Light* light, bool isStatic)
			{
				REPORT_LOADED_MOVABLEOBJECT(
					light, isStatic, mLoadedLights, mLoadedNonStaticLights, mLoadedStaticLights);
			}

			void	reportLoadedCamera(Camera* camera, bool isStatic)
			{
				REPORT_LOADED_MOVABLEOBJECT(
					camera, isStatic, mLoadedCameras, mLoadedNonStaticCameras, mLoadedStaticCameras);
			}

			void	reportLoadedParticleSystem(ParticleSystem* particleSystem, bool isStatic)
			{
				REPORT_LOADED_MOVABLEOBJECT(
					particleSystem, isStatic, mLoadedParticleSystems, mLoadedNonStaticParticleSystems, mLoadedStaticParticleSystems);
			}

			void	reportLoadedBillboardSet(BillboardSet* billboardSet, bool isStatic)
			{
				REPORT_LOADED_MOVABLEOBJECT(
					billboardSet, isStatic, mLoadedBillboardSets, mLoadedNonStaticBillboardSets, mLoadedStaticBillboardSets);
			}


			// ------------------------------------------------- Animation states --------------------------------------------------

			void	reportLoadedAnimationState(Entity* ownerEntity, AnimationState* animationState)
			{
				mLoadedAnimationStates.push_back(animationState);
				mLoadedEntityAnimationStates.insert(EntityAnimationStatesMultiMap::value_type(ownerEntity, animationState));
			}
			void	reportLoadedAnimationState(SceneManager* ownerSceneManager, AnimationState* animationState)
			{
				mLoadedAnimationStates.push_back(animationState);
				mLoadedSceneManagerAnimationStates.insert(
					SceneManagerAnimationStatesMultiMap::value_type(ownerSceneManager, animationState));
			}


			// ---------------------------------------------------- Animations -----------------------------------------------------

			void	reportLoadedAnimation(Entity* ownerEntity, Animation* animation)
			{
				mLoadedAnimations.push_back(animation);
				mLoadedEntityAnimations.insert(EntityAnimationsMultiMap::value_type(ownerEntity, animation));
			}
			void	reportLoadedAnimation(SceneManager* ownerSceneManager, Animation* animation)
			{
				mLoadedAnimations.push_back(animation);
				mLoadedSceneManagerAnimations.insert(
					SceneManagerAnimationsMultiMap::value_type(ownerSceneManager, animation));
			}


			// ------------------------------------------------------- Logs --------------------------------------------------------

			/// Log a loading error, implemented here so we can have a stack of errors that the client app can analize/show.
			void	logLoadError(const Ogre::String& text);
			/// Log a loading warning, implemented here so we can have a stack of warnings that the client app can analize/show.
			void	logLoadWarning(const Ogre::String& text);
			/// Log a loading info, implemented here so we can have a stack of infos that the client app can analize/show.
			void	logLoadInfo(const Ogre::String& text);

			LoadLogVectorIterator		getSceneLoadLog()
			{
				return LoadLogVectorIterator(mLoadLogs.begin(), mLoadLogs.end());
			}
			ConstLoadLogVectorIterator	getSceneLoadLog() const
			{
				return ConstLoadLogVectorIterator(mLoadLogs.begin(), mLoadLogs.end());
			}

			LoadLogVectorIterator		getSceneLoadInfoLog()
			{
				return LoadLogVectorIterator(mLoadInfoLogs.begin(), mLoadInfoLogs.end());
			}
			ConstLoadLogVectorIterator	getSceneLoadInfoLog() const
			{
				return ConstLoadLogVectorIterator(mLoadInfoLogs.begin(), mLoadInfoLogs.end());
			}

			LoadLogVectorIterator		getSceneLoadWarningLog()
			{
				return LoadLogVectorIterator(mLoadWarningLogs.begin(), mLoadWarningLogs.end());
			}
			ConstLoadLogVectorIterator	getSceneLoadWarningLog() const
			{
				return ConstLoadLogVectorIterator(mLoadWarningLogs.begin(), mLoadWarningLogs.end());
			}

			LoadLogVectorIterator		getSceneLoadErrorLog()
			{
				return LoadLogVectorIterator(mLoadErrorLogs.begin(), mLoadErrorLogs.end());
			}
			ConstLoadLogVectorIterator	getSceneLoadErrorLog() const
			{
				return ConstLoadLogVectorIterator(mLoadErrorLogs.begin(), mLoadErrorLogs.end());
			}



			/// \brief This function does not destroy loaded data, lets Ogre do it implicitly, or the user explicitly by calling
			/// DestroyDotScene(). Just clears internal lists storing references to the previously loaded data (movable objects,
			/// entities, lights, etc.).
			void clear(bool clearRootSceneNodeAndSceneManager = true);

			/// \brief Destroys all entities, billboardsets, particle systems, etc belonging to the scene for which the given
			/// instance of DotSceneInfo holds loading information for.
			///
			/// @remarks
			///		This method is especially useful, when you have loaded many .scenes to the same SceneManager instance using
			///		different nodes as root nodes for all separate .scenes instances and now you would like to unload some of the
			///		previously loaded scenes while you want to keep the others in memory.
			/// @par removeSceneNodes If set to true, the root scene node of the given scene will be destroyed with all of its
			/// children. Any nodes attached to one of the DotSceneLoader loaded nodes after the loading of the scene will be
			/// destroyed too. If set to false, all scene nodes loaded by the DotSceneLoader will left intact. This also includes
			/// their childs whose were attached to them by the application and not by DotSceneLoader.
			void	DestroyDotScene(bool removeSceneNodes = false);

			DotSceneInfo(): 
				mSceneManager(0), mRootSceneNode(0),
				mAmbientLightColour(ColourValue::Black), mHasAmbientLightColourSet(false),
				mBackgroundColour(ColourValue::Black), mHasBackgroundColourSet(false)
			{

			}

			~DotSceneInfo()
			{
				clear();
			}


		private:
			friend class DotSceneLoader;
		public :
			//static bool					isUpToDateWithScene()	{return mIsUpToDateWithScene;}
			/************************************************************
			* using those methods prefixed by _ u fuck'up the sceneInfo	*
			* if u do not use them correctly,						    *
			* ie. first update scene manager, then set those :)		    *
			************************************************************/
			/// set the upToDateFlag
			static void _setUpToDate(bool is)	{mIsUpToDateWithScene = is;}
			/// add a target SceneNode 
			/// (ie. an empty SN wich act as a target for lights/cameras) to the DSI graph
			static void _addTargetSceneNode(Ogre::SceneNode *pSN);
			
		private:
			typedef std::map<Ogre::String,	MovableObjectEx*>	tMapMovableEx;
			typedef std::map<Ogre::String,	Ogre::SceneNode*>	tMapSceneNode;

			static size_t				mNumTris;
			static bool					mIsUpToDateWithScene;

			static Ogre::ColourValue	mAmbLightColor;
			static Ogre::ColourValue	mBgndColor;

			static tMapMovableEx		mMOStatic;
			static tMapMovableEx		mMODynamic;
			static tMapMovableEx		mMOAll;
			static tMapSceneNode		mSNTargets;
		
		public:
			typedef	Ogre::MapIterator<tMapMovableEx>			itMovableEx;
			typedef	Ogre::MapIterator<tMapSceneNode>			itSceneNode;

			static const itMovableEx	getStaticMovablesIterator()		{return itMovableEx(mMOStatic.begin(), mMOStatic.end());}
			static const itMovableEx	getDynamicMovablesIterator()	{return itMovableEx(mMODynamic.begin(), mMODynamic.end());}
			static const itMovableEx	getAllMovablesIterator()		{return itMovableEx(mMOAll.begin(), mMOAll.end());}
			static const itSceneNode	getTargetSceneNodesIterator()	{return itSceneNode(mSNTargets.begin(), mSNTargets.end());}


			static bool isMovableStatic(const Ogre::String &name)		{return mMOStatic.find(name) != mMOStatic.end() ? true : false;}
			static bool isMovableStatic(const Ogre::MovableObject &mo)	{return isMovableStatic(mo.getName());}
			static bool isMovableStatic(const Ogre::MovableObject *pMO)	{assert(pMO); return isMovableStatic(pMO->getName());}

			static bool isMovableDynamic(const Ogre::String &name)		{return mMODynamic.find(name) != mMODynamic.end() ? true : false;}
			static bool isMovableDynamic(const Ogre::MovableObject &mo)	{return isMovableDynamic(mo.getName());}
			static bool isMovableDynamic(const Ogre::MovableObject *pMO){assert(pMO); return isMovableDynamic(pMO->getName());}

			static bool isTarget(const Ogre::String &name)	{return mSNTargets.find(name) != mSNTargets.end() ? true : false;}
			static bool isTarget(const Ogre::SceneNode &sn)	{return isTarget(sn.getName());}
			static bool isTarget(const Ogre::SceneNode *pSN){assert(pSN); return isTarget(pSN->getName());}
		};
	} // namespace dsi
} // namespace Ogre
#endif // #ifndef __dotSceneInfo_H__

