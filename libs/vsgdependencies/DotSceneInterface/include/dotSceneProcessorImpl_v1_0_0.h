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

#ifndef __DotSceneProcessorImpl_v1_0_0_H__
#define __DotSceneProcessorImpl_v1_0_0_H__

#include "XNPPrerequisites.h"
#include "dotScenePrerequisites.h"
#include "DotSceneProcessorImpl.h"

namespace Ogre
{
	//**************************************************************************************************
	namespace dsi
	{
		//**************************************************************************************************
		class _DotSceneInterfaceExport DotSceneProcessorImpl_v1_0_0 : public DotSceneProcessorImpl
		{
			typedef XmlNodeProcessing::XmlNodeProcessor XmlNodeProcessor;
			typedef XmlNodeProcessing::XmlNodeProcessorTreeBuilder XmlNodeProcessorTreeBuilder;
			typedef XmlNodeProcessing::CommonParamsOfXmlNodeProcessors CommonParamsOfXmlNodeProcessors;

		public:
			DotSceneProcessorImpl_v1_0_0();

			/// @remarks It does call shutdown, however, it is not part of the class specification, so
			/// don't rely on it, or risk writing buggy programs.
			~DotSceneProcessorImpl_v1_0_0();

			/// @copydoc DotSceneProcessorImpl::initialize
			virtual void initialize();

			/// @copydoc DotSceneProcessorImpl::load
			virtual void load(
				TiXmlDocument*		pXMLDoc,
				SceneManager*		pSceneManager,
				RenderWindow*		pRWin,
				const String&		groupName,
				const String&		strNamePrefix,
				SceneNode*			pRootNode			= NULL,
				bool				doMaterials			= false,
				bool				forceShadowBuffers	= false,
				DotSceneInfo**		ppDotSceneInfo		= 0);


			/// @copydoc DotSceneProcessorImpl::shutdown
			virtual void shutdown();

			const CommonParamsOfXmlNodeProcessors*	getCommonParams() const;
			CommonParamsOfXmlNodeProcessors*		getCommonParams();
			const DotSceneNodeProcessorTreeBuilder*	getTreeBuilder() const;
			DotSceneNodeProcessorTreeBuilder*		getTreeBuilder();

		private:
			SceneNode*		mpRoot;
			SceneManager*	mpSceneManager;
			RenderWindow*	mpWin;
			bool			mDoMats;
			bool			mShadow;
			bool			mDoInfo;
			TiXmlDocument*	mpXMLDoc;

			CommonParamsOfXmlNodeProcessors*	mCommonParamsOfXmlNodeProcessors;
			DotSceneNodeProcessorTreeBuilder*	mDotSceneNodeProcessorTreeBuilder; 
			XmlNodeProcessor*					mRootXmlNodeProcessor;
		};
	} // namespace dsi
} // namespace Ogre
#endif // #ifndef __DotSceneProcessorImpl_v1_0_0_H__
