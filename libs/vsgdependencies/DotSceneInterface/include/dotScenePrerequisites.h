/*
-----------------------------------------------------------------------------
Author:	Balazs Hajdics

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

#ifndef __dotScenePrerequisites_H__
#define __dotScenePrerequisites_H__

//***************************************************************************************************
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the _DotSceneInterfaceExport
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// _DotSceneInterfaceExport functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
//***************************************************************************************************

#include "Ogre.h"

//----------------------------------------------------------------------------
// Windows Settings
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32

// If we're not including this from a client build, specify that the stuff
// should get exported. Otherwise, import it.
#	if defined( DOTSCENEINTERFACE_BUILDMODE_LIB )
		// Linux compilers don't have symbol import/export directives.
#   	define _DotSceneInterfaceExport
#   	define _DotSceneInterfacePrivate
#   else
#   	if defined( DOTSCENEINTERFACE_BUILDMODE_DLL_EXPORT )
#       	define _DotSceneInterfaceExport __declspec( dllexport )
#   	else
#           if defined( __MINGW32__ )
#               define _DotSceneInterfaceExport
#           else
#       	    define _DotSceneInterfaceExport __declspec( dllimport )
#           endif
#   	endif
#   	define _DotSceneInterfacePrivate
#	endif
#endif // #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32

// ---------------- TinyXML
#define TIXML_USE_STL // if this is not defined, tiXML work so slooooooow :(



	// ---------------------------------------------------- Pre-declare classes ----------------------------------------------------
namespace Ogre {
	namespace dsi {
		class DotSceneProcessor;
		class DotSceneProcessorImpl_v1_0_0;
		class DotSceneNodeProcessorTreeBuilder;

		class DotSceneInfo;
	}
}



#endif // ifndef __dotScenePrerequisites_H__

