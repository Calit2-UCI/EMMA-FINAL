/*
-----------------------------------------------------------------------------
Author:		Balazs Hajdics (wolverine_@freemail.hu)

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

#ifndef __DotSceneNodeProcessorTreeBuilder_H__
#define __DotSceneNodeProcessorTreeBuilder_H__

#include "XNPPrerequisites.h"
#include "XNPXmlNodeProcessorTreeBuilder.h"

#include "dotScenePrerequisites.h"
#include "dotSceneXmlNodeProcessors.h"

#include "Ogre.h"

#include <vector>
#include <map>
#include <string>

using XmlNodeProcessing::XmlNodeProcessor;
using XmlNodeProcessing::XmlNodeProcessorTreeBuilder;
using XmlNodeProcessing::CommonParamsOfXmlNodeProcessors;

namespace Ogre {
	namespace dsi {

	/// @copydoc XmlNodeProcessorTreeBuilder
	// TODO: FIXTHIS: Currently DotSceneNodeProcessorTreeBuilder doesn't work in the way as it mentioned in the documentation of
	// the XmlNodeProcessorTreeBuilder class.
	/// @remarks
	/// You have to set the following common parameters for the CommonParamsOfXmlNodeProcessors which you are going to use to
	///	parse the scene:
		///		- "SceneManager" => either a pointer to a previously created scene manager or NULL if you want dotSceneLoader to create
		///		  a scene manager for you given the properties in the .scene file.
		///		- "RootSceneNode" => either a pointer to a previously created node, which you want to be the root node of the parsed
		///		  scene (i.e. the whole scene will be placed under this node) or 0 if you want dotSceneLoader to create a root scene node
		///		  for you.
		///		- "ResourceGroup" => either the address of the *static* variable consisting the name of the resource group where
		///		  resource locations for the given .scene file can be found or 0 if you want dotScene loader to parse the resource
		///		  locations and the resource group only from the .scene file.
		///		  If the resource group name is also specified in the .scene file and it differs from the name specified as parameter
		///		  common "ResourceGroup", then it will be ignored.
		///		  If the two names are the same, then dotSceneLoader will parse new resource locations from the .scene file and it will
		///		  add them to the specified resource group.
		///		  IMPORTANT!!! If you specify a valid resource group name for "ResourceGroup" and the dotScene file also consist an
		///		  element "resourceGroup" then the group you specified will be cleared at the beginning of parsing the dotScene file.
		///		  All resources loaded as part of the group you specified will be lost and also all resource declarations made as part
		///		  of the group will be lost too. Resource locations will still exist though. This is required because dotSceneLoader
		///		  have to reinitialise the resource group after adding new resource locations to ensure that scripts in the new locations
		///		  are parsed.
		///		  Set this to ResourceGroupManager::AUTODETECT_RESOURCE_GROUP if you want the resources of the dotScene file to be
		///		  searched for in all groups.
		///		- "RenderWindow" => either pointer to the previously created render window or null if you would like dotSceneLoader to
		///		  create one for you.
		///		- "NamePrefix" => either a pointer to a *static* variable consisting the string to use as a prefix, when creating
		///		  entities, nodes, etc. from the .scene file or null if no prefix should be used at all. This is especially usefull
		///		  in some cases. E.g. when you want to load the same .scene file more then once, e.g. into different nodes of the same
		///		  SceneManager but you don't want to worry about renaming everything to avoid name collisions. Another example would be
		///		  if you want to load two scenes, while one of these scenes is only a slight modification of the other, and now you wish
		///		  to see which works best. In this case it is again possible that some names may collide. You should prefix them.
		///		- "VertexBufferUseShadow" => pointer to a boolean variable. If this common parameter is set, its value will override the
		///		  vertex buffer usage parameter comming from the .scene file.
		///		- "IndexBufferUseShadow" => pointer to a boolean variable. If this common parameter is set, its value will override the
		///		  vertex buffer usage parameter comming from the .scene file.
		///		- "LoadStaticEntities" => pointer to a boolean variable. If set to true static entities will be loaded and added to the
		///		  scene manager. If set to false static entities won't be loaded anyway. If this common parameter is not present, the
		///		  default behaviour is not to load static entities.
		//		  This is especially usefull when you have a scene file with static world geometry stored in an octree. These scene
		///		  files are usually converted using OctreeSceneConverter from a .scene file which consists static entites.
		///		  However OctreeSceneConverter leaves the static entities in the converted xml file intact, so you can upgrade/modify
		///		  them or their attributes later and reconstruct the static world geometry from them again.
		///		  In this case, if you want the scene to be displayed from the static world geometry and the non-static entities, you
		///		  you should set this common parameter to false.
		///		- "LoadStaticOctree" => Whether or not to load the element "octree" of the scene file. When this common parameter is
		///		  set to true, common parameter "LoadStaticEntities" should be set to false. If this common parameter is not present,
		///		  the default behaviour is not to load the static octree. See common parameter "LoadStaticEntities" for more information.
		///		.
		///	
		class _DotSceneInterfaceExport DotSceneNodeProcessorTreeBuilder: public XmlNodeProcessorTreeBuilder
		{

	protected:
	SceneElementProcessor*							mSceneElementProcessor;

		SceneManagerOptionElementProcessor*			mSceneManagerOptionElementProcessor;
		ShadowSettingsElementProcessor*				mShadowSettingsElementProcessor;
			ShadowTextureConfigElementProcessor*	mShadowTextureConfigElementProcessor;

		ResourceGroupElementProcessor*					mResourceGroupElementProcessor;
			ResourceLocationElementProcessor*				mResourceLocationElementProcessor;
			ResourceDeclarationElementProcessor*			mResourceDeclarationElementProcessor;
				NameValuePairListElementProcessor*				mNameValuePairListElementProcessor;
					NameValuePairElementProcessor*					mNameValuePairElementProcessor;

		RenderTargetsElementProcessor*				mRenderTargetsElementProcessor;
		RenderWindowElementProcessor*				mRenderWindowElementProcessor;

		EnvironmentElementProcessor*					mEnvironmentElementProcessor;
			FogElementProcessor*							mFogElementProcessor;
			SkyBoxElementProcessor*							mSkyBoxElementProcessor;
			SkyDomeElementProcessor*						mSkyDomeElementProcessor;
			SkyPlaneElementProcessor*						mSkyPlaneElementProcessor;
			ColourAmbientElementProcessor*					mColourAmbientElementProcessor;
			ColourBackgroundElementProcessor*				mColourBackgroundElementProcessor;

		TrackTargetElementProcessor*					mTrackTargetElementProcessor;
			LocalDirectionVectorElementProcessor*		mLocalDirectionVectorElementProcessor;
			OffsetElementProcessor*						mOffsetElementProcessor;

		LookTargetElementProcessor*						mLookTargetElementProcessor;

		LightElementProcessor*							mLightElementProcessor;
			ColourDiffuseElementProcessor*					mColourDiffuseElementProcessor;
			ColourSpecularElementProcessor*					mColourSpecularElementProcessor;
			SpotLightRangeElementProcessor*					mSpotLightRangeElementProcessor;
			LightAttenuationElementProcessor*				mLightAttenuationElementProcessor;

		CameraElementProcessor*							mCameraElementProcessor;
			ClippingElementProcessor*						mClippingElementProcessor;

		NodesElementProcessor*							mNodesElementProcessor;
			NodeElementProcessor*							mNodeElementProcessor;
				EntityElementProcessor*							mEntityElementProcessor;
					MeshLODBiasElementProcessor*					mMeshLODBiasElementProcessor;
					MaterialLODBiasElementProcessor*				mMaterialLODBiasElementProcessor;
				ParticleSystemElementProcessor*					mParticleSystemElementProcessor;
				BillboardSetElementProcessor*					mBillboardSetElementProcessor;
					BillboardElementProcessor*						mBillboardElementProcessor;
					CommonDirectionElementProcessor*				mCommonDirectionElementProcessor;
					CommonUpVectorElementProcessor*					mCommonUpVectorElementProcessor;
					TextureCoordsElementProcessor*					mTextureCoordsElementProcessor;
						FloatRectElementProcessor*						mFloatRectElementProcessor;
					TextureStacksAndSlicesElementProcessor*			mTextureStacksAndSlicesElementProcessor;

		ViewportsElementProcessor*						mViewportsElementProcessor;
			ViewportElementProcessor*						mViewportElementProcessor;
				CompositorInstancesElementProcessor*				mCompositorInstancesElementProcessor;
					CompositorInstanceElementProcessor*					mCompositorInstanceElementProcessor;
					HDRCompositorInstanceElementProcessor*				mHDRCompositorInstanceElementProcessor;


		AnimationsElementProcessor*						mAnimationsElementProcessor;
			AnimationElementProcessor*						mAnimationElementProcessor;
				NodeAnimationTrackElementProcessor*				mNodeAnimationTrackElementProcessor;
					TransformKeyFrameElementProcessor*				mTransformKeyFrameElementProcessor;
				VertexAnimationTrackElementProcessor*		mVertexAnimationTrackElementProcessor;
					VertexPoseKeyFrameElementProcessor*			mVertexPoseKeyFrameElementProcessor;
						PoseReferenceElementProcessor*				mPoseReferenceElementProcessor;

		AnimationStatesElementProcessor*					mAnimationStatesElementProcessor;
			AnimationStateElementProcessor*						mAnimationStateElementProcessor;


		CommonMovableObjectParamsElementProcessor*	mCommonMovableObjectParamsElementProcessor;

		PositionElementProcessor*					mPositionElementProcessor;
		OrientationElementProcessor*				mOrientationElementProcessor;
			QuaternionElementProcessor*					mQuaternionElementProcessor;
			Angle_AxisElementProcessor*					mAngle_AxisElementProcessor;
			AxisXYZElementProcessor*					mAxisXYZElementProcessor;
			DirectionElementProcessor*					mDirectionElementProcessor;
		ScaleElementProcessor*						mScaleElementProcessor;


#ifdef _USE_DOT_OCTREE
		OctreeElementProcessor*						mOctreeElementProcessor;
#endif

	public:
						DotSceneNodeProcessorTreeBuilder(CommonParamsOfXmlNodeProcessors* commonParams);
		virtual			~DotSceneNodeProcessorTreeBuilder();

		/// @copydoc XmlNodeProcessorTreeBuilder::buildTree()
		virtual		XmlNodeProcessor*	buildTree();
		virtual		void				destroyTree();
	};
	} //namespace Ogre
} // namespace dsi

#endif
