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


#ifndef _DotSceneResourceManager_H__
#define _DotSceneResourceManager_H__

#include "dotScenePrerequisites.h"
#include "dotSceneResource.h"

namespace Ogre
{
	//**************************************************************************************************
	namespace dsi
	{
		//**************************************************************************************************
		/// \brief Manages the locating and loading of .scene and .bin files.
		///
		/// @remarks
		/// Like other ResourceManager specialisations it manages the location and loading of a specific type of resource, in this
		/// case files containing .scene XML data data. NB Implemented just to have access to scenes in Ogre search paths.
		class _DotSceneInterfaceExport dotSceneResourceManager : public Ogre::ResourceManager, public Ogre::Singleton<dotSceneResourceManager>
		{
		public:
			OGRE_AUTO_MUTEX dotSceneResourceManager();
			~dotSceneResourceManager();

			/// Creates a DotScene resource - mainly used internally.
			Resource	*createImpl(const String& name, ResourceHandle handle, 
				const String& group, bool isManual, ManualResourceLoader* loader,
				const NameValuePairList* params);

			/// \brief Override standard Singleton retrieval.
			///
			/// @remarks Why do we do this? Well, it's because the Singleton implementation is in a .h file,
			/// which means it gets compiled into anybody who includes it. This is needed for the Singleton
			/// template to work, but we actually only want it compiled into the implementation of the
			/// class based on the Singleton, not all of them. If we don't change this, we get link errors
			/// when trying to use the Singleton-based class from an outside dll.</p>
			/// This method just delegates to the template version anyway, but the implementation stays in this
			/// single compilation unit, preventing link errors.
			static dotSceneResourceManager &getSingleton(void);
		};
		//**************************************************************************************************
	}
	//**************************************************************************************************
}

#endif
