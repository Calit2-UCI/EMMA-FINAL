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


#ifndef __dotScenePlatformUtils_H__
#define __dotScenePlatformUtils_H__

#include "dotScenePrerequisites.h"
#include <tchar.h>


namespace Ogre
{
	namespace dsi {

	/// \brief Platform specific utility functions.
	///
	/// @warning Since Ogre is not unicode compatible, this class won't compile when unicode strings are used (so the either or
	/// both the UNICODE, and/or the _UNICODE preprocessor directives are set), however it is only up to Ogre, because this class
	/// WAS written in a way to support both ANSI and UNICODE strings.
	class _DotSceneInterfaceExport PlatformUtils
	{
	public:

		#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			/// \brief Only valid under Windows operating systems. Calls the WinAPI GetLastError() function and converts the
			/// returned error code to string.
			static String	GetLastError_AsString();
		#endif

		/// \brief Returns the absolute path of the current working directory. The returned path is standardized as in
		/// StringUtil::standardisePath().
		///
		/// @remarks Future versions will be platform independent, this version only supports Win32 operating systems.
		static String	getCurrentDirectory();

		/// \brief Returns the absolute path of the executable file of the current process.
		///
		/// @note Under Windows operating systems only paths up to the size of MAX_PATH are supported. See Windows.h for more
		/// information.
		/// @remarks Future versions will be platform independent, this version only supports Win32 operating systems.
		static String	getCurrentExecutableFilePath();
	};

	}
}

#endif
