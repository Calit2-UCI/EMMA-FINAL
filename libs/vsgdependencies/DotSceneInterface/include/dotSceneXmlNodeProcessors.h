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

#ifndef __dotSceneXmlNodeProcessor_H__
#define __dotSceneXmlNodeProcessor_H__

//#include "dotSceneStableHeaders.h"
//#include "dotScenePrerequisites.h"
#include "dotSceneInterface.h"
#include "XNPXmlNodeProcessor.h"
#include "XNPXmlNodeProcessorTreeBuilder.h"
#include "Ogre.h"

#include "HDRCompositor.h"

using XmlNodeProcessing::XmlNodeProcessor;

namespace Ogre {


	// --------------------------- The following functions should be placed into the class StringConverter ---------------------------

		_DotSceneInterfaceExport		
			HDRCompositor::GLARETYPE	StringGlareTypeToEnumGlareType(const String& strToneMapper);

		_DotSceneInterfaceExport	
			HDRCompositor::TONEMAPPER	StringToneMapperToEnumToneMapper(const String& strToneMapper);

		_DotSceneInterfaceExport
			Ogre::FrameBufferType		StringFrameBufferTypeToEnumFrameBufferType(const String& strFrameBufferType);

		_DotSceneInterfaceExport 
			Ogre::VertexAnimationType	StringVertexAnimationTypeToEnumVertexAnimationType(const String& strVertexAnimationType);
	
		_DotSceneInterfaceExport 
			Ogre::Math::AngleUnit		StringAngleUnitToEnumAngleUnit(const String& strAngleUnit);

		/// \brief Converts a transform space value specified as a string ("local", "parent", "world") to a transform space value
		/// specified as an enum.
		///
		/// @param transformSpace Parameter storing the transform space as a string.
		/// @param elementName Name of the xml element which processor's called this method. Only used if an error occurs.
		/// @param filename Name of the xml file which processor's called this method. Only used if an error occurs.
		/// @returns Parameter transformSpace converted to a Node::TransformSpace enum value.
		_DotSceneInterfaceExport 
			Ogre::Node::TransformSpace StringTransformSpaceToEnumTransformSpace(const String& transformSpace, const String& elementName, const String& filename, dsi::DotSceneInfo* dotSceneInfo);

		_DotSceneInterfaceExport 
			Ogre::PixelFormat StringPixelFormatToEnumPixelFormat(const String& strPixelFormat);

		_DotSceneInterfaceExport 
			Animation::InterpolationMode StringInterpolationModeToEnumInterpolationMode(const String& strInterpolationMode);

		_DotSceneInterfaceExport
			Animation::RotationInterpolationMode StringRInterpolationModeToEnumRInterpolationMode(const String& strRInterpolationMode);
	// --------------------------------- End of functions to be placed into the StringConverter class --------------------------------

	/// \brief Parses the string as a variable of type "type" and returns the address of the parsed data converted to void*.
	///
	/// @warning Space will be allocated for the parsed data dynamically, so the caller function have to ensure this space is freed
	/// when it isn't needed anymore, to avoid memory leaks.
	/// @param value The value to parse as a string. E.g. "3.5", "hello", "23523", "-353", "-3 -5 45", etc.
	/// @param type The type of the data stored in the parameter "value". See dotscene.dtd or the source code for full list of
	/// supported types.
	/// @param elementName Name of the xml element which processor's called this method. Only used if an error occurs.
	/// @param filename Name of the xml file which processor's called this method. Only used if an error occurs.
	/// @param sceneManager Optional pointer to the SceneManager. Needed if the value to parse is a camera, light or entity name.
	/// In this case the instance of the object will be searched using the scene manager supplied.
	/// @param namePrefix Optional name prefix to concatenate before node, camera and movable object names. Use
	/// mTreeBuilder->getCommonParameter("NamePrefix") to get the current name prefix if there is any.
	/// @returns The address of the parsed data converted to void*. Usable if you wan't the result to be passed for methods waiting void*s,
	/// like SceneManager::setOption(const String&, void*).
	_DotSceneInterfaceExport void*	ParseStringBasedOnType(
										const String& value, const String& type,
										const String& elementName, const String& filename,
										SceneManager* sceneManager, const String& namePrefix,
										dsi::DotSceneInfo* dotSceneInfo);

	class _DotSceneInterfaceExport DotSceneXmlNodeProcessor: public XmlNodeProcessor
	{
	protected:
		/// Temporary variable for prequerying element name only once, and then using it in all attribute parser functions.
		String						mElementName;
		/// Temporary variable for prequerying filename only once, and then using it in all attribute parser functions.
		String						mFilename;

		/// Local copy of the pointer of the instance of DotSceneInfo. Used for faster accessing.
		dsi::DotSceneInfo*			mDotSceneInfo;


		virtual void	logAttributeNotFoundError(XmlAttributeDefaultDecl xmlAttributeDefaultDecl, const String& attributeName, const String& elementName, const String& filename, const String& defaultValue);

		/// @name Common attribute loader functions for all dotScene xml elements
		//@{

		/// @param xmlAttributeDefaultDecl If equals with XMLADD_REQUIRED and the attribute specified by attributeName is not
		/// found, an error will be logged.
		/// @param defaultValue This is the value which will be returned if the specified attribute is not found in the given xml
		/// element.
		template<typename AttributeType>
			AttributeType	parseAttribute(const String& attributeName, XmlNodeProcessor* parent, TiXmlElement* currentNode, XmlAttributeDefaultDecl xmlAttributeDefaultDecl, const AttributeType& defaultValue);

		void	parseAttributeAndSaveAsNameValuePair(const String& attributeName, XmlNodeProcessor* parent, TiXmlElement* currentNode, XmlAttributeDefaultDecl xmlAttributeDefaultDecl, NameValuePairList& nameValuePairList, const String& defaultValue="");
		
		/// @param xmlAttributeDefaultDecl If equals with XMLADD_REQUIRED and the attribute specified by attributeName is not
		/// found, an error will be logged.
		/// @param defaultValue This is the value which will be returned if the specified attribute is not found in the given xml
		/// element.
		template<>
			String	parseAttribute(const String& attributeName, XmlNodeProcessor* parent, TiXmlElement* currentNode, XmlAttributeDefaultDecl xmlAttributeDefaultDecl, const String& defaultValue);

		virtual void parseAttribute_id(XmlNodeProcessor* parent, TiXmlElement* currentNode);

		//@}


		virtual String		getNodeNameAutoNameIfNeeded(TiXmlElement *tiXmlElement);
		static unsigned int	msAutoNameNumber;
		virtual String		nextAutoName();

		virtual String		getNodeName_AutoNamedAndPrefixedIfNeeded(TiXmlElement* currentNode);

	public:
		DotSceneXmlNodeProcessor(): XmlNodeProcessor(), mDotSceneInfo(0) {}

		/// For code reusability child node processors do not store pointers to their parents. This is because the same node
		/// processor can be registered to many node processors as a sub-node processor. It is also possible to register the same
		/// node processor (so the same code) for nodes (usually elements) with different names, but with the same contents.
		/// This helper static function is for determining the name as the child node processor was registered in its parent.
		/// This also increases modularity and code-reusability when giving error messages, since you don't have to "hardwire"
		/// the name of the node which you process into the code.
		/// @param currentNode The current TiXmlNode* which is under processing.
		void	queryCurrElementNameAndFilenameAndDotSceneInfo(TiXmlElement* currentNode)
		{
			// Prequery current element name as it present in parent.
			mElementName	= currentNode->Value();
			// Since we know that "scene" is the root element of the document
			mFilename		= currentNode->GetDocument()->Value();

			if (mTreeBuilder)
				mDotSceneInfo	= (dsi::DotSceneInfo*)mTreeBuilder->getCommonParameter("DotSceneInfo");
			else
			{// No tree builder was registered. This is an internal error, throw an exception.
				mDotSceneInfo = 0;

				OGRE_EXCEPT(
					Ogre::Exception::ERR_INTERNAL_ERROR,
					"No tree builder was registered for the \"" + mElementName + "\" element processor! Check your source code!",
					"DotSceneXmlNodeProcessor::queryCurrElementNameAndFilenameAndDotSceneInfo()");
			}
		}
	};

	class ColourElementProcessor: public DotSceneXmlNodeProcessor
	{
	protected:
		ColourValue				mColour;
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

	/// Semi-abstract class for defining interface for element processor which is able to receive a name-value pair list through a
	/// set method.
	class NameValuePairListReceiverElementProcessor: public DotSceneXmlNodeProcessor
	{
	protected:
		Ogre::NameValuePairList				mNameValuePairList;
	public:
		virtual	void  setNameValuePairList(const NameValuePairList& nameValuePairList)
		{
			mNameValuePairList = nameValuePairList;
		}
	};


	class SceneElementProcessor: public DotSceneXmlNodeProcessor
	{
	protected:

		/// @name Attribute loader functions for dotScene xml element named "scene"
		/// These functions are grouped together in doxygen. They are made virtual to make it easy for other developers to
		/// subclass from this class and only overdefine the parsing of the desired attribute.
		//@{
		virtual void parseAttribute_formatVersion	(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		virtual void parseAttribute_minOgreVersion	(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		virtual void parseAttribute_sceneManager	(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		virtual void parseAttribute_author			(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		//@}

	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		virtual void  parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

	class SceneManagerOptionElementProcessor: public NameValuePairListReceiverElementProcessor
	{
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

	class ShadowSettingsElementProcessor: public NameValuePairListReceiverElementProcessor
	{
	protected:
		/// @name Attribute loader functions for dotScene xml element named "shadowSettings"
		/// These functions are grouped together in doxygen. They are made virtual to make it easy for other developers to
		/// subclass from this class and only overdefine the parsing of the desired attribute.
		//@{
		virtual Ogre::ShadowTechnique parseAttribute_shadowTechnique(const String& attributeName, XmlNodeProcessor* parent, TiXmlElement* currentNode);
		//@}
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};
		class ShadowTextureConfigElementProcessor: public NameValuePairListReceiverElementProcessor
		{
		protected:
			Ogre::ShadowTextureConfig mShadowTextureConfig;
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};


	class ResourceGroupElementProcessor: public DotSceneXmlNodeProcessor
	{
	protected:
		String					mResourceGroupName;
	public:
		virtual void*	parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		virtual void	parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

		class ResourceLocationElementProcessor: public DotSceneXmlNodeProcessor
		{
		public:
			enum RelativeTo
			{
				RT_DOT_SCENE_FILE_LOCATION,
				RT_EXE_FILE_LOCATION,
				RT_CURRENT_WORKING_DIRECTORY,
				RT_ABSOLUTE
			};
		protected:
			virtual RelativeTo parseAttribute_relativeTo(const String& attributeName, XmlNodeProcessor* parent, TiXmlElement* currentNode);
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class ResourceDeclarationElementProcessor: public NameValuePairListReceiverElementProcessor
		{
		protected:
			String						mResourceDeclarationName;
			String						mResourceType;
		public:

			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
			virtual void  parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};
			class NameValuePairListElementProcessor: public DotSceneXmlNodeProcessor
			{
			protected:
				NameValuePairList			mNameValuePairList;
			public:
				virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
				virtual void  parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
			};
				class NameValuePairElementProcessor: public DotSceneXmlNodeProcessor
				{
				protected:
					std::pair<String, String> mNameValuePair;
				public:
					virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
				};

	class RenderTargetsElementProcessor: public DotSceneXmlNodeProcessor
	{
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

		class RenderWindowElementProcessor: public DotSceneXmlNodeProcessor
		{
		public:
			struct RenderWindowData
			{
				String				mName;
				unsigned int		mWidth;
				unsigned int		mHeight;

				bool				mFullscreen;

				unsigned int		mPriority;

				bool				mActive;
				bool				mAutoUpdated;
				bool				mPrimary;

				bool				mVisible;




				String				mTitle;
				unsigned int		mColourDepth;
				int					mLeft;
				int					mTop;
				bool				mDepthBuffer;
				unsigned int		mFSAA;
				unsigned int		mDisplayFrequency;
				bool				mVsync;
				
				enum BorderType
				{
					BT_NONE,
					BT_FIXED,
					BT_RESIZE
				};
				BorderType			mBorder;

				bool				mOuterDimensions;
			};

		protected:
			RenderWindowData		mRenderWindowData;
			Ogre::RenderWindow*		mRenderWindow;
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};
			class ViewportsElementProcessor: public DotSceneXmlNodeProcessor
			{
			public:
				virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
			};

			class ViewportElementProcessor: public DotSceneXmlNodeProcessor
			{
			public:
				struct ViewportData
				{
					String				mRenderTargetName;
					RenderTarget*		mRenderTarget;
					String				mCameraName;
					Camera*				mCamera;
					bool				mChangeCameraAspectRatio;
					
					Real				mLeft;
					Real				mTop;
					Real				mWidth;
					Real				mHeight;
					int					mZOrder;
					bool				mClearEveryFrame;
					unsigned int		mClearBuffers;
					String				mMaterialSchemeName;
					bool				mOverlaysEnabled;
					bool				mSkiesEnabled;
					bool				mShadowsEnabled;
					unsigned int		mVisibilityMask;
					String				mRenderQueueInvocationSequenceName;
				};

			protected:
				ViewportData			mViewportData;
				Ogre::Viewport*			mViewport;

			public:
				virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
			};
				class CompositorInstancesElementProcessor: public DotSceneXmlNodeProcessor
				{
				public:
					virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
				};

					class CompositorInstanceElementProcessor: public DotSceneXmlNodeProcessor
					{
					public:
						struct CompositorInstanceData
						{
							String		mCompositorName;
							int			mPosition;
							bool		mEnabled;
						};
					protected:
						CompositorInstanceData		mCompositorInstanceData;
						Ogre::CompositorInstance*	mCompositorInstance;
					public:
						virtual void  parseCompositorData(XmlNodeProcessor* parent, TiXmlElement* currentNode, CompositorInstanceData& compositorInstanceData);
						virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
					};

					class HDRCompositorInstanceElementProcessor: public DotSceneXmlNodeProcessor
					{
					protected:
						HDRCompositor*				mHDRCompositorInstance;
					public:
						virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
					};


		class RenderTextureElementProcessor: public NameValuePairListReceiverElementProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class MultiRenderTargetElementProcessor: public DotSceneXmlNodeProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};


	class EnvironmentElementProcessor: public DotSceneXmlNodeProcessor
	{
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

		class FogElementProcessor: public DotSceneXmlNodeProcessor
		{
		protected:
			dsi::DotSceneInfo::FogData		mFogData;
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class SkyBoxElementProcessor: public DotSceneXmlNodeProcessor
		{
		protected:
			dsi::DotSceneInfo::SkyBoxData	mSkyBoxData;
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);

			// \brief Since the orientation of the sky box is parsed by the optional "orientation" child-element, the reporting
			// of loaded data must be done in the parseElementEndImpl() function.
			virtual void  parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};
		
		class SkyDomeElementProcessor: public DotSceneXmlNodeProcessor
		{
		protected:
			dsi::DotSceneInfo::SkyDomeData	mSkyDomeData;
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);

			// \brief Since the orientation of the sky dome is parsed by the optional "orientation" child-element, the reporting
			// of loaded data must be done in the parseElementEndImpl() function.
			virtual void  parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class SkyPlaneElementProcessor: public DotSceneXmlNodeProcessor
		{
		protected:
			dsi::DotSceneInfo::SkyPlaneData	mSkyPlaneData;
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class ColourAmbientElementProcessor: public ColourElementProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class ColourBackgroundElementProcessor: public ColourElementProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

	class BaseNodeElementProcessor: public DotSceneXmlNodeProcessor
	{
	public:
		virtual void	setVisibility(SceneNode* sceneNode, XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

	class NodesElementProcessor: public BaseNodeElementProcessor
	{
	protected:

		/// @name Attribute loader functions for dotScene xml element named "nodes"
		/// These functions are grouped together in doxygen. They are made virtual to make it easy for other developers to
		/// subclass from this class and only overdefine the parsing of the desired attribute.
		//@{
		//@}

	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

	/// Universal position element processor, can be used on lights, cameras, nodes. Please see notes for function
	/// OrientationElementProcessor::setOrienationOnParent() for more information about the internal working of this class.
	class PositionElementProcessor: public DotSceneXmlNodeProcessor
	{
	protected:
		Vector3						mParsedPosition;

	public:

		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};


	/// Universal orientation element processor, can be used on lights, cameras, nodes. Please see notes for function
	/// OrientationElementProcessor::setOrienationOnParent() for more information about the internal working of this class.
	class OrientationElementProcessor: public DotSceneXmlNodeProcessor
	{
	protected:
		Quaternion						mParsedQuaternion;

	protected:
		// TODO: FIXTHIS: UGLY HACK!!!
		/// Ogre Lights and Cameras have a "special ability": although they are not Node objects and they are not inherited from
		/// the Node class, their positions, orientations, scaling can be manipulated as if they were inherited from the Node class.
		/// They simply reimplement the same functionality. However this causes an issue when you try to generalise
		/// position/orientation/scaling setting. This is why this function is implemented. It type cast the parent's parsed node
		/// to Light, Node or Camera based on the value of currentNode->Parent()->Value(). The orientation/direction will be set
		/// on the given (type casted) object.
		virtual void	setOrientationOnParent(XmlNodeProcessor* parent, TiXmlElement* currentNode, const String& functionNameToReportIfError);

	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

		class QuaternionElementProcessor: public OrientationElementProcessor
		{

		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class AxisXYZElementProcessor: public OrientationElementProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class Angle_AxisElementProcessor: public OrientationElementProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class DirectionElementProcessor: public OrientationElementProcessor
		{
		protected:
			Vector3						mDirection;
			Vector3						mLocalDir;
			SceneNode::TransformSpace	mTransformSpace;
			/// @copydoc OrientationElementProcessor::setOrientationOnParent
			virtual void setDirectionOnParent(XmlNodeProcessor* parent, TiXmlElement* currentNode, const String& functionNameToReportIfError);
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

	class ScaleElementProcessor: public DotSceneXmlNodeProcessor
	{
	protected:
		Vector3						mScale;
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};


	// ---------------------------------- Structures, types for "trackTarget" xml element processing ---------------------------------
	struct TrackTargetData
	{
		String			mTargetSceneNodeName;
		Vector3			mLocalDirectionVector;
		Vector3			mOffset;

		TrackTargetData(): mTargetSceneNodeName(""), mLocalDirectionVector(Vector3::NEGATIVE_UNIT_Z), mOffset(Vector3::ZERO) {}
	};

	/// \brief	First	= the SceneNode to enable autotracking on.
	///			Second	= track target data, such as the name of the scene node to track, local direction vector, offset.
	///
	/// @sa TrackTargetData
	typedef std::map<SceneNode*, TrackTargetData> TrackTargetDataOfSceneNodes;

	/// \brief	First	= the Camera to enable autotracking on.
	///			Second	= track target data, such as the name of the scene node to track, local direction vector, offset.
	///
	/// @sa TrackTargetData
	typedef std::map<Camera*, TrackTargetData> TrackTargetDataOfCameras;
	extern String CommonParName_TrackTargetDataOfSceneNodes;
	extern String CommonParName_TrackTargetDataOfCameras;
	// -------------------------------------------------------------------------------------------------------------------------------

	class TrackTargetElementProcessor: public DotSceneXmlNodeProcessor
	{
	protected:
		TrackTargetData	mTrackTargetData;
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		virtual void  parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};
		class LocalDirectionVectorElementProcessor: public DotSceneXmlNodeProcessor
		{
		protected:
			Vector3	mLocalDirectionVector;
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class OffsetElementProcessor: public DotSceneXmlNodeProcessor
		{
		protected:
			Vector3	mOffset;
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};


	// ---------------------------------- Structures, types for "lookTarget" xml element processing ---------------------------------
	struct LookTargetData
	{
		String						mLookTargetSceneNodeName;
		Vector3						mLookPosition; ///< LookAt position. In case scene node name is also given, then an offset.
		Ogre::Node::TransformSpace	mRelativeTo;
		Vector3						mLocalDirectionVector;

		LookTargetData():
			mLookTargetSceneNodeName(""), mRelativeTo(Ogre::Node::TS_WORLD), mLookPosition(Vector3::ZERO), mLocalDirectionVector(Vector3::NEGATIVE_UNIT_Z) {}
	};

	/// \brief	First	= the SceneNode to enable autotracking on.
	///			Second	= track target data, such as the name of the scene node to track, local direction vector, offset.
	///
	/// @sa TrackTargetData
	typedef std::map<SceneNode*, LookTargetData> LookTargetDataOfSceneNodes;

	/// \brief	First	= the Camera to enable autotracking on.
	///			Second	= track target data, such as the name of the scene node to track, local direction vector, offset.
	///
	/// @sa TrackTargetData
	typedef std::map<Camera*, LookTargetData> LookTargetDataOfCameras;
	extern String CommonParName_LookTargetDataOfSceneNodes;
	extern String CommonParName_LookTargetDataOfCameras;
	// -------------------------------------------------------------------------------------------------------------------------------

	class LookTargetElementProcessor: public DotSceneXmlNodeProcessor
	{
	protected:
		LookTargetData	mLookTargetData;
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		virtual void  parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};



	class NodeElementProcessor: public BaseNodeElementProcessor
	{
	protected:

		/// @name Attribute loader functions for dotScene xml element named "node"
		/// These functions are grouped together in doxygen. They are made virtual to make it easy for other developers to
		/// subclass from this class and only overdefine the parsing of the desired attribute.
		//@{
		//@}

	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

	class CommonMovableObjectParamsElementProcessor: public DotSceneXmlNodeProcessor
	{
	protected:
		virtual void getDefaultQueryAndVisibilityFlagsForMovable(MovableObject* movableObject, uint32& defaultQueryFlags, uint32& defaultVisibilityFlags);
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

	/// This class itself doesn't parse any dotScene xml elements. It was designed to be the base class for all dotScene xml
	/// element processors whose process xml elements relating to subclasses of Ogre::MovableObject (e.g. element "entity",
	/// "light", "camera", etc.).
	/// It implements the setStatic() method, so child element processor CommonMovableObjectParamsElementProcessor can set 
	/// whether or not the given movable object instance was declared as static.
	/// Therefore every xml element processor which process a subclass of Ogre::MovableObject can implement parseElementEndImpl()
	/// to notify DotSceneInfo instance about the created movable object instance. 
	/// Otherwise this would be impossible to be done in an object-oriented, structural, modular way, since DotSceneInfo expects
	/// us to tell whether or not a movable object static or not, and we have no way to decide it during the parsing of an xml
	/// element different from "commonMovableObjectParams". We could peek into child elements of the actual xml element, however
	/// this would break the philosophy of XmlNodeProcessing project.
	class MovableObjectElementProcessor: public DotSceneXmlNodeProcessor
	{
	protected:
		bool					mStatic;

	public:
		MovableObjectElementProcessor(): mStatic(false) {}
		void setStatic(bool isStatic) { mStatic = isStatic; }
	};

	class EntityElementProcessor: public MovableObjectElementProcessor
	{
	protected:

		/// @name Attribute loader functions for dotScene xml element named "entity"
		/// These functions are grouped together in doxygen. They are made virtual to make it easy for other developers to
		/// subclass from this class and only overdefine the parsing of the desired attribute.
		//@{

		/// @params attributeName Must be either "vertexBufferUsage" or "indexBufferUsage".
		virtual HardwareBuffer::Usage parseAttribute_Usage(const String& attributeName, XmlNodeProcessor* parent, TiXmlElement* currentNode);

		//@}
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		virtual void  parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

		/// \brief Base class for lod bias element processing. parseElementImpl() only parses lod bias data, but it does not sets
		/// it neither as a mesh, nor as a material lod bias. parseElementImpl() of subclasses must do this.
		///
		class LODBiasElementProcessor: public DotSceneXmlNodeProcessor
		{
		public:
			struct LODBiasData
			{
				Real			m_rFactor;
				unsigned short	m_usMaxDetailIndex;
				unsigned short	m_usMinDetailIndex;
			};
		protected:
			LODBiasData			m_LODBiasData;
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		/// \brief 
		///
		/// @remarks
		/// If attribute "maxDetailIndex" is greater then Entity::getMesh()::getNumLodLevels(), an error will be logged and it will
		/// be truncated to be equal with Entity::getMesh()::getNumLodLevels().
		class MeshLODBiasElementProcessor: public LODBiasElementProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};
		class MaterialLODBiasElementProcessor: public LODBiasElementProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};


	/// @note Although lights can affect the scene in Ogre without being attached to any scene node, this class will attach
	/// the created light to its parent scene node or to the application supplied root node if it was created at the top level
	/// of the scene.
	/// This is required because it is possible to load many .scene files into different nodes of the same scene manager and
	/// we don't wan't to mess up the scene if the node where the whole .scene file is loaded is moved.
	class LightElementProcessor: public MovableObjectElementProcessor
	{
	protected:

		/// @name Attribute loader functions for dotScene xml element named "light"
		/// These functions are grouped together in doxygen. They are made virtual to make it easy for other developers to
		/// subclass from this class and only overdefine the parsing of the desired attribute.
		//@{

			virtual Light::LightTypes	parseAttribute_type(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		//@}
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		virtual void  parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

		/// Universal colourDiffuse element processor, can be used on lights and billboards.
		class ColourDiffuseElementProcessor: public ColourElementProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class ColourSpecularElementProcessor: public ColourElementProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class SpotLightRangeElementProcessor: public DotSceneXmlNodeProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class LightAttenuationElementProcessor: public DotSceneXmlNodeProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};


	/// @note Although cameras can be used in Ogre without being attached to any scene node, this class will attach
	/// the created camera to its parent scene node or to the application supplied root node if it was created at the top level
	/// of the scene.
	/// This is required because it is possible to load many .scene files into different nodes of the same scene manager and
	/// we don't wan't to mess up the scene if the node where the whole .scene file is loaded is moved.
	class CameraElementProcessor: public MovableObjectElementProcessor
	{
	protected:

		/// @name Attribute loader functions for dotScene xml element named "camera"
		/// These functions are grouped together in doxygen. They are made virtual to make it easy for other developers to
		/// subclass from this class and only overdefine the parsing of the desired attribute.
		//@{
			virtual ProjectionType	parseAttribute_projectionType(XmlNodeProcessor* parent, TiXmlElement* currentNode);
			virtual PolygonMode		parseAttribute_polygonMode(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		//@}
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		virtual void  parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};
		class ClippingElementProcessor: public DotSceneXmlNodeProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};


	class ParticleSystemElementProcessor: public MovableObjectElementProcessor
	{
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		virtual void  parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

	class BillboardSetElementProcessor: public MovableObjectElementProcessor
	{
	protected:

		/// @name Attribute loader functions for dotScene xml element named "billboardSet"
		/// These functions are grouped together in doxygen. They are made virtual to make it easy for other developers to
		/// subclass from this class and only overdefine the parsing of the desired attribute.
		//@{
			virtual BillboardType			parseAttribute_billboardType		(XmlNodeProcessor* parent, TiXmlElement* currentNode);
			virtual BillboardOrigin			parseAttribute_billboardOrigin		(XmlNodeProcessor* parent, TiXmlElement* currentNode);
			virtual BillboardRotationType	parseAttribute_billboardRotationType(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		//@}

	public:
		virtual void*	parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		virtual void	parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

		class CommonDirectionElementProcessor: public DotSceneXmlNodeProcessor
		{
			Vector3		mCommonDirection;
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class CommonUpVectorElementProcessor: public DotSceneXmlNodeProcessor
		{
			Vector3		mCommonUpVector;
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class TextureCoordsElementProcessor: public DotSceneXmlNodeProcessor
		{
		protected:
			typedef std::vector<Ogre::FloatRect> TextureCoordSets;

			TextureCoordSets			mTextureCoords;
		public:
			virtual TextureCoordSets&	getTextureCoords() { return mTextureCoords; }
			virtual void				addTextureCoord(const Ogre::FloatRect& floatRect) { mTextureCoords.push_back(floatRect); }
			virtual void*				parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode) { return &mTextureCoords; }
			virtual void				parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

			class FloatRectElementProcessor: public DotSceneXmlNodeProcessor
			{
			protected:
				Ogre::FloatRect			mFloatRect;
			public:
				virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
			};

		class TextureStacksAndSlicesElementProcessor: public DotSceneXmlNodeProcessor
		{
		protected:
			uchar			mStacks;
			uchar			mSlices;
		public:
			virtual void*				parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

		class BillboardElementProcessor: public DotSceneXmlNodeProcessor
		{
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};


	class AnimationsElementProcessor: public DotSceneXmlNodeProcessor
	{
	public:
		virtual void*				parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};

		class AnimationElementProcessor: public DotSceneXmlNodeProcessor
		{
		protected:
			Animation*			mAnimation;
			Entity*				mParentEntity;
			SkeletonInstance*	mParentSkeletonInstance;
			Mesh*				mParentMesh;

			/// @name Attribute loader functions for dotScene xml element named "nodes"
			/// These functions are grouped together in doxygen. They are made virtual to make it easy for other developers to
			/// subclass from this class and only overdefine the parsing of the desired attribute.
			//@{
			virtual Animation::InterpolationMode
				parseAttribute_interpolationMode(const String& attributeName, XmlNodeProcessor* parent, TiXmlElement* currentNode);
			virtual Animation::RotationInterpolationMode
				parseAttribute_rotationInterpolationMode(const String& attributeName, XmlNodeProcessor* parent, TiXmlElement* currentNode);
			//@}
		public:
			virtual Entity*				getParentEntity()			{ return mParentEntity; }
			virtual SkeletonInstance*	getParentSkeletonInstance() { return mParentSkeletonInstance; }
			virtual Mesh*				getParentMesh()				{ return mParentMesh; }
			virtual void*				parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
			virtual	void				parseElementEndImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};
			class NodeAnimationTrackElementProcessor: public DotSceneXmlNodeProcessor
			{
			protected:
				NodeAnimationTrack*		mNodeTrack;
			public:
				virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
			};
				class TransformKeyFrameElementProcessor: public DotSceneXmlNodeProcessor
				{
				protected:
					TransformKeyFrame*		mTransformKeyFrame;
				public:
					virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
				};

			class VertexAnimationTrackElementProcessor: public DotSceneXmlNodeProcessor
			{
			protected:
				VertexAnimationTrack*		mVertexTrack;
			public:
				virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
			};
				class VertexPoseKeyFrameElementProcessor: public DotSceneXmlNodeProcessor
				{
				protected:
					VertexPoseKeyFrame*		mVertexPoseKeyFrame;
				public:
					virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
				};
					class PoseReferenceElementProcessor: public DotSceneXmlNodeProcessor
					{
					protected:
						Ogre::VertexPoseKeyFrame::PoseRef		mPoseRef;
					public:
						PoseReferenceElementProcessor(): DotSceneXmlNodeProcessor(), mPoseRef(0,0) {}
						virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
					};


	class AnimationStatesElementProcessor: public DotSceneXmlNodeProcessor
	{
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};
		class AnimationStateElementProcessor: public DotSceneXmlNodeProcessor
		{
		protected:
			AnimationState*		mAnimationState;
		public:
			virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
		};

#ifdef _USE_DOT_OCTREE
	class OctreeElementProcessor: public DotSceneXmlNodeProcessor
	{
	public:
		virtual void* parseElementImpl(XmlNodeProcessor* parent, TiXmlElement* currentNode);
	};
#endif

}


#endif
