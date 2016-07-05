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

#ifndef __dotSceneResourceHeader__
#define __dotSceneResourceHeader__

#include "dotScenePrerequisites.h"

namespace Ogre
{
	//**************************************************************************************************
	namespace dsi
	{
		//**************************************************************************************************
		class dotSceneResourceManager; // fwd decl...
		class dotSceneResourcePtr;
		//**************************************************************************************************
		/// implemented just to have access to scenes in Ogre search paths
		class _DotSceneInterfaceExport dotSceneResource : public Ogre::Resource
		{
			friend class dsi::dotSceneResourceManager;
		public:
			/// Overridden from Resource.
			void loadImpl(void);

			/// Unloads the material, frees resources etc.
			/// @see
			/// Resource
			void unloadImpl(void);

			/// @copydoc Resource::calculateSize
			size_t calculateSize(void) const { return 0; } // TODO 

			/// 
			dotSceneResource(ResourceManager *creator, const String &name, ResourceHandle handle, const String &group, bool isManual=false, ManualResourceLoader *loader=0);
			~dotSceneResource();

			/// Get data strean.
			DataStreamPtr getDataStream();
		private:
			dotSceneResource();
			DataStreamPtr mData;
		};

		/// \brief Specialisation of SharedPtr to allow SharedPtr to be assigned to dotSceneResourcePtr.
		///
		/// @note Has to be a subclass since we need operator=.
		/// We could templatise this instead of repeating per Resource subclass, except to do so requires a form VC6 does not
		/// support i.e. ResourceSubclassPtr<T> : public SharedPtr<T>
		class _DotSceneInterfaceExport dotSceneResourcePtr : public SharedPtr<dotSceneResource> 
		{
		public:
			dotSceneResourcePtr() : SharedPtr<dotSceneResource>() {}
			explicit dotSceneResourcePtr(dotSceneResource* rep) : SharedPtr<dotSceneResource>(rep) {}
			dotSceneResourcePtr(const dotSceneResourcePtr& r) : SharedPtr<dotSceneResource>(r) {} 
			dotSceneResourcePtr(const ResourcePtr& r);
			dotSceneResourcePtr& operator=(const ResourcePtr& r);
		};
		//**************************************************************************************************
	}
	//**************************************************************************************************
}

#endif
