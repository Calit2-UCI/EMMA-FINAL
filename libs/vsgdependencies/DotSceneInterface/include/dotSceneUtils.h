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

//**************************************************************************************************
#ifndef __dotSceneUtils_H__
#define __dotSceneUtils_H__
//**************************************************************************************************
#include "dotScenePrerequisites.h"
#include "tinyxml.h"
#include "Ogre.h"
//**************************************************************************************************
namespace Ogre
{
	//**************************************************************************************************************
	namespace dsi
	{
		//**************************************************************************************************************
		namespace utils
		{
			/// retrieve the file name only component from a file path string
			Ogre::String getFileTitle(const Ogre::String &fileName);
		}// namespace utils
		//**************************************************************************************************************
		namespace xml
		{
			// dotScene format compounds
			Ogre::ColourValue	readColor		(const TiXmlElement *pElem);
			Ogre::Vector3		readVector3		(const TiXmlElement *vecElem);
			Ogre::Vector4		readVector4		(const TiXmlElement *vecElem);

			bool				getAttribute(const TiXmlElement *pElem, const String &attribute, String		&attributeValue);
			bool				getAttribute(const TiXmlElement *pElem, const String &attribute, bool		&attributeValue);
			bool				getAttribute(const TiXmlElement *pElem, const String &attribute, Real		&attributeValue);
			bool				getAttribute(const TiXmlElement *pElem, const String &attribute, int		&attributeValue);
			bool				getAttribute(const TiXmlElement *pElem, const String &attribute, uint32		&attributeValue);

			// named value write methods 
			// (THOSE CREATE NAMED nodes)
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::String &value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, int value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, float value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, double value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, bool value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Matrix3 &value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Matrix4 &value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Vector3 &value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Vector4 &value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::ColourValue &value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Quaternion &value);
		}// namespace xml

	} // namespace dsi

} // namespace Ogre

#endif // #ifndef __dotSceneUtils_H__

