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


#ifndef __DotSceneProcessorImpl_H__
#define __DotSceneProcessorImpl_H__

#include "dotScenePrerequisites.h"
#include "dotSceneInfo.h"

#include "tinyxml.h"

namespace Ogre
{
	//**************************************************************************************************
	namespace dsi
	{
		//**************************************************************************************************
		class _DotSceneInterfaceExport DotSceneProcessorImpl : public progressMaker
		{
		public:
			/// \brief Initializes the loader.
			///
			/// @param ppDotSceneInfo Pointer to a pointer to a DotSceneInfo instance, which will be populated with the information
			/// about the loaded .scene file. If set to null no information will be given about the loaded scene. If
			/// *ppDotSceneInfo is set to null, then a new DotSceneInfo instance will be created for you and it will be populated
			/// with the information about the loaded .scene. IMPORTANT!!! ppDotSceneInfo must point to a valid pointer or null, or
			/// the loader will crash.
			/// @remarks Calling initialize does not actually load anything. Initialization and actual loading
			/// must be separated to allow users of the class taking extra initialization steps (e. g. adding listeners
			/// to the tree builder.
			virtual  void initialize() = 0;

			/// \brief Calling load will do the actual loading.
			///
			/// @remarks Any initialization steps should be taken before calling load, but after calling initialize.
			/// Such steps include e. g. registering listeners to the tree builder.
			/// @remarks Don't forget to call shutdown after load but before destroying the object.
			virtual void load(				
				TiXmlDocument*		pXMLDoc,
				SceneManager*		pSceneManager,
				RenderWindow*		pRWin,
				const String&		groupName,
				const String&		strNamePrefix,
				SceneNode*			pRootNode			= NULL,
				bool				doMaterials			= false,
				bool				forceShadowBuffers	= false,
				DotSceneInfo**		ppDotSceneInfo		= 0) = 0;

			/// \brief
			///
			/// @remarks Call it before destroying the object. The destructor won't call it by itself.
			virtual void shutdown() = 0;

		protected:
			String				mGroupName; // we need this to get the group name for loading
		};
	} // namespace dsi
} // namespace Ogre

#endif // #ifndef __DotSceneProcessorImpl_H__
