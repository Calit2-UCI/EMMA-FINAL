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


#ifndef __DotSceneProcessor_H__
#define __DotSceneProcessor_H__

#include "dotScenePrerequisites.h"
#include "DotSceneProcessorImpl.h"


namespace Ogre
{
	//***************************************************************************************************
	namespace dsi
	{
		//***************************************************************************************************
		class _DotSceneInterfaceExport DotSceneProcessor: public Singleton<DotSceneProcessor>, public progressMaker
		{
		public:
			DotSceneProcessor();
			virtual			~DotSceneProcessor();
			DotSceneProcessor&	getSingleton(void);

			/// @copydoc DotSceneProcessorImpl::initialize()
			/// @param 
			///		version 
			///			Which version of the underlying DotSceneProcessorImpl implementations to initialize. If left blank all underlying
			///			implementations will be initialized.
			void initialize(const String& version = "");

			/// @copydoc DotSceneProcessorImpl::load()
			void load(		
				const Ogre::String&	fileName, 
				SceneManager*		pSceneManager, 
				RenderWindow*		pRWin, 
				const String&		groupName,
				const String&		strNamePrefix,
				SceneNode*			pRootNode,
				bool				doMaterials,
				bool				forceShadowBuffers,
				DotSceneInfo**		ppDotSceneInfo
				);

			/// @copydoc DotSceneProcessorImpl::shutdown()
			void shutdown();

			/// overide this to add the new registered listener to all implementations
			void addProgressListener(progressListener *pNewPL);

			// TODO: currently serialization is not yet implemented. Implement it.
			//void saveScene(const Ogre::String &fileName, SceneManager *pSceneManager, bool doStatic);

			DotSceneProcessorImpl*	getImplementation(const String& version);

		private:
			typedef std::map<Ogre::String, DotSceneProcessorImpl*>	DotSceneProcessorImplMap;
			
			DotSceneProcessorImplMap	mDotSceneProcessorImplementations;
			DotSceneProcessorImpl*		mpCurrentProcessor; // It holds the processor found by initialize.
			TiXmlDocument*				mpXMLDoc;
		};
		//***************************************************************************************************
	} // namespace dsi
	//***************************************************************************************************
} // namespace Ogre

#endif // #ifndef __DotSceneProcessor_H__

