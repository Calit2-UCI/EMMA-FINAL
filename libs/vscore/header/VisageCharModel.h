
#ifndef __VisageCharModel_h__
#define __VisageCharModel_h__

#include "Base.h"
#include "Logger.h"
#include "Math3D.h"
#include "Mesh.h"
#include "MorphTarget.h"
#include "Skeleton.h"
#include "Joint.h"
#include "FBAPs.h"
#include "FDP.h"
#include "FBAPMapping.h"

#ifdef VISAGE_STATIC
	#define VISAGE_DECLSPEC
#else
	#if defined(MAC) || defined(LINUX)
		#define VISAGE_DECLSPEC __attribute__((__visibility__("default")))

	#else
		#ifdef VISAGE_EXPORTS
			#define VISAGE_DECLSPEC __declspec(dllexport)
		#else
			#define VISAGE_DECLSPEC __declspec(dllimport)
		#endif
	#endif

#endif

// C++
#ifdef _DEBUG
#include <assert.h>
#endif
#include <string>

namespace VisageSDK
{

/*! \mainpage  visage|SDK API
* visage|SDK consists of several libraries (.lib), each implementing a specific set of features. The high-level 
* libraries cover all high-level features that most developers are likely to use - lip-sync, speech synthesis-based animation,
* behavior realization etc.
*
* The low-level libraries most often do not need to be used directly. They are provided to enable full access to all details of virtual character 
* animation, deformation, coding and rendering mechanisms and give the developers with more complex requirements
* complete control of the application.
*
* visage|SDK uses some third-party libraries, such as Intel JPEG library and libXML. To see which libraries are needed, refer to the sample projects.
*
* The following sections provide overviews of all libraries, in order of significance, and give links to most important
* classes, as well as to the examples demonstrating the particular functions.
*
*
*
*
* \section vscore The core functions (vscore.dll)
* The core functions include the animation player mechanism, the MPEG-4 Face and Body Animation (FBA) support and renderer integration support.
*
* \subsection visageFAPlayer Animation player
* The FAPlayer and FbaAction classes are central to visage|SDK. They are used to simultaneously
* play multiple animations on a virtual character, where animations can come from various sources (files, lip sync, speech synthesis, procedural animations, custom sources...).
* The animation sources are often provided by other libraries (such as lip sync, visual speech synthesis or facial features tracking).
*
* visage|SDK implements a high-level MPEG-4 face and body animation player functionality through its FAPlayer class. It is provided with a character model instance
* (loaded by the main application) and plays animations on this model. 
* It can read one or more animation tracks
* from MPEG-4 FBA bitstreams, and accept other animation sources through the 
* FbaAction abstract class interface. All animation tracks are played simultaneously and blended.
* Multiple FAPlayers can be used simultaneously to animate multiple virtual characters.
*
* The player can play in real time or off-line. The FbaAction abstract interface provides a simple yet powerful mechanism to play
* all kinds of animation sources. For example, speech synthesis functionality in visage|SDK is an FbaAction and through this
* making the virtual character talk becoms an easy matter of attaching a Visagesapi5tts class to the FAPlayer and calling the Visagesapi5tts::speak() method. 
* Lip sync is implemented in the same manner - the VisageLipSync class is an FbaAction. Other examples
* may include real time motion capture, tracking, procedural animation etc.
*
* Various other utility and helper functions are available, like automatic gaze following (vestibulo-ocular reflex) during
* animation, merging tracks, saving them into file, as well as a utility class for 
* exporting animation into keyframe-based systems or files.
*
* Simple usage of FAPlayer is demonstrated in the SimplePlayer sample. The Speech Synthesis sample is 
* another example that includes speech synthesis and on-the-fly automatic insertion of animation actions. The Lip Sync 
* Face Tracker and Face Tracker 2 samples are also based on the FAPlayer animation mechanism.
*
* <b>Main classes</b>
*
* - FAPlayer: high-level player functionality - playing multiple tracks, real-time and off-line playback, abstract rendering mechanism
* - FbaAction: abstract class for implementing face and body animation sources. Any implementation of FbaAction is an animation source, and it must simply return Face and Body Animation Parameters for a given time. The tracks in the FAPlayer are of type FbaAction.
* - FbaFileAction: an FbaAction that implements file MPEG-4 FBA read/write capabilities for face and body animation, and plays any associated audio tracks.
* - FARenderer: abstract class interface for implementing a synchronous rendering loop within FAPlayer. 
* If an FARenderer is attached to the FAPlayer, the FAPlayer calls its FARenderer::render() function in
* each frame of animation. It can be used for actual on-screen rendering or
* for animation data acces in exporters and plug-ins. Note that the provided examples do not use the FARenderer
* interface for rendering, because they implement their own rendering loop. For most
* real time applications this is probably a better solution, but the FARenderer interface can still
* be useful (for example, the visage|interactive application uses it for updating
* the time slider while playing the animation).
* - FAExporter: the support class for exporting virtual character animation to keyframe-based animation systems. It extends FARenderer.
*
*
** \subsection visageFBAEncoder MPEG-4 FBA support
* visage|SDK contains classes that support coding and decoding of MPEG-4 Face and Body Animation (FBA) files/bitstreams
* and provides storage classes for MPEG-4 Face and Body Animation Parameters (FBAPs) and convenience functions to set
* the various parameters directly using the provided definitions of their names (see @ref FBAPNames).
* The CFBAEncoder class is used to encode a series of MPEG-4 Face and Body Animation Parameters (FBAPs) into an MPEG-4 bitstream.
* The encoded bitstream can be written into a file.
* The CFBADecoder class is used to decode an MPEG-4 FBA bitstream and obtain MPEG-4 Face and Body Animation Parameters (FBAPs) and timestamps.
*
*
*
* MPEG-4 FBA coding and decoding are demonstrated in the FBA Codec sample.
*
* <b>Main classes</b>
*
* - CFBADecoder: the main FBA decoder class, used for decoding an FBA file and obtaining FBAPs and time stamps.
* - CFBAEncoder: the main FBA encoder class, used for encoding FBAPs frame-by-frame, or to encode a whole file from an ASCII FAP/BAP file.
* - FBAPs, BAPs, FAPs, LLFAPs, FAP1, FAP2: storage classes for the full Face and Body Animation parameter set: BAPs, low level FAPs, viseme and expression
* - CodingParameters: storage class for the coding parameters
* - FDP: Storage class for MPEG-4 Facial Definition Points (FDP), the standard feature points in the face.
*
*
** \subsection visageCharModel Renderer interface
* The VisageCharModel class provides an abstract interface for accessing and manipulating the character model.
* In the OpenGL version of visage|SDK, the AFM class is the OpenGL-specific implementation ofa VisageCharModel.
*
* <b>Main classes</b>
*
* - VisageCharModel: Abstract interface to the character model. Allows manual animation of the model with sets of Face and Body Animation Parameter values. Also gives direct access to character's joint transformations and morph target weights.
* - AFM: Animatable Face and Body Model, an implementation of VisageCharModel abstract class. It allows low-level access to model geometry and animation properties.
*
*
*
*
*
* \section visageVision Visage VISION functions (vsvision.dll)
* The vsvision.dll library contains vision-based algorithms for detecting and tracking
* facial features in images/videos. 
*
* \subsection visageVision-d Facial feature detection (vsvision.dll)
* The class VisageFeaturesDetector detects facial features in input images and
* returns their coordinates as result, contained in the FDP object.
*
* The Face Detector sample demonstrates facial feature detection.
*
* <b>Main classes</b>
*
* - VisageFeaturesDetector: Detects facial features in still images.
* - FDP: Used to return the resulting feature points (implemented in vscore.dll).
*
*
* \subsection visageVision-t Facial feature tracking (vsvision.dll)
* Facial feature tracking tracks facial features in video sequences coming
* from AVI files or directly from a camera. The coordinates of facial features 
* are returned in real time for each frame of video.
* Currently, visageVision includes two alternative algorithms for facial features
* tracking, each algorithm having certain advantages over the other and
* may be suitable for different applications. The two algorithms are implemented
* in classes VisageTracker and VisageTracker2.
* VisageTracker must be trained for each face to be tracked. The interactive
* TrainingTool is provided for the purpose of performing the training. Once trained,
* VisageTracker can track the face in the video without any further setup, and it can
* automatically recover from errors, for example when the face leaves the video 
* frame and then comes back.
* VisageTracker2 requires only a very simple semi-automatic setup to be performed before 
* starting to track a new face in a video. If the tracking is lost (e.g. the face has left the video frame)
* it has to be restarted manually.
* Regarding tracked features, VisageTracker2 currently does not track eye opening/closing
* and eye rotations (these features shall be added in future release); VisageTracker
* does not track face rotations, i.e. if the head turns the tracking will fail.
* The Face Tracker demonstrates facial feature detection and the VisageTracker tracking algorithm.
* The Face Tracker 2 sample demonstrates the VisageTracker2 tracking algorithm.
*
* <b>Main classes</b>
*
* - VisageTracker2: Tracks facial features in video. Implements FbaAction, so it can be played in FAPlayer.
* - OpenGLInterface: A virtual class for implementing certain OpenGL functions required for the functioning of VisageTracker2.
* - VisageTrackerObserver: the virtual class serves as an interface for user-defined callback functions triggered and called by the tracker (works with both VisageTracker and VisageTracker2).
* - VisageTracker: Tracks facial features in video. Implements FbaAction, so it can be played in FAPlayer.
* - AAMTrainer: Provides the functionality to train a new person specific model of a user's face.
* - FDP: Used to return the resulting feature points (implemented in vscore.dll).
*
*
*
*
* \section visageLipSync Lip Sync (vslipsync.dll)
* The vslipsync.dll library produces the lips animation synchronised to natural speech in any language by analysing the speech audio signal.
* It can
* directly drive real-time facial animation. The speech
* can be pre-recorded or it can come directly from the microphone. The lip sync can work in real time or off-line. In the
* off-line mode the process takes slightly more time, but the quality of the result is higher.
*
* Lip sync is demonstrated in the Lip Sync sample</a>.
*
* <b>Main classes</b>
*
* - VisageLipSync: the main lip sync class; it is an implementation of FbaAction, so it can be played
* in an FAPlayer, which is the simplest way to apply lip sync to a virtual character, potentially together
* with other animation.
* - LipSyncEventHandler: this virtual class is an interface for receiving lip sync events; an implementation
* of this class can be attached to VisageLipSync, and it will receive notifications of the visemes
* generated in each audio frame. This information may be used to generate additional effects.
*
*
* 
*
* \section visageSAPI5TTS SAPI-5 Visual speech synthesis (vsvtts.dll)
* The vsvtts.dll library  generates speech and corresponding face animation using
* a SAPI-5 compliant Text-to-Speech (TTS) engine, either for on-the-fly rendering or writing into files. In order to implement this functionality,
* it is necessary to install MS SAPI-5 SDK. At runtime, it is necessary to have at least
* one SAPI-5 compatible speech engine installed. The SDK and speech engines are available
* from Microsoft free of charge, and Microsoft allows redistribution of the code developed using
* the SAPI-5 SDK (conditions may change). To download go to http://www.microsoft.com/downloads and search for Speech SDK 5.1; download SpeechSDK51.exe.
*
* Numerous other
* SAPI-compatible speech synthesis engines are available commercially, and they can be used with your visage|SDK based applications
* providing they are fully SAPI compatible and implement all necessary interfaces.
*
* The Speech Synthesis sample demonstrates how to use FAPlayer and Visagesapi5tts for
* real time interactive animation. 
*
* <b>Main classes</b>
*
* - Visagesapi5tts: the main Visual Text-to-Speech class; initialises SAPI, generates speech and 
* animation on-the-fly or encodes them to files; implements FbaAction, so it can be played in FAPlayer.
* - Visagesapi5ttsObserver: the virtual class serves as an interface for user-defined callback functions triggered and called by the TTS when events such as bookmarks and phonemes occur; 
* through this mechanism it is possible to augment TTS-generated animation with facial movements and gestures etc.
*
*
* 
*
* \section visageRealActor (vsrealactor.dll)
* The vsrealactor.dll is a BML realizer that realizes complex, multimodal character behaviors.
* These behaviors can be specified in BML scripts or via the library's BML-compliant API. For example,
* RealActor allows you to specify a multimodal behavior where the character speaks (text-to-speech synthesis is employed)
* and the speech is accompanied by appropriate non-verbal behaviors (e.g. blinking, pointing or gaze).
* Synchronization of speech and non-verbal behaviors is achieved using neural networks.
*
* The Behavior Player sample demonstrates how to use RealActor for
* real-time behavior realizeation.
*
* <b>Main classes</b>
*
* - RealActor: the core behavior realizer class
*
*
*
*
*
*  
*/

/**
* Animatable character model. This class aggregates model data important
* for character animation. This data includes the following:
*
* <ul>
* <li>Character skeleton (see Skeleton), composed of animatable joints (see Joint).
* <li>A list of all meshes that comprise the model, for each mesh a list of morph targets
* defined on the mesh, and for each morph target its current weight.
* <li>Mappings of MPEG-4 face and body animation parameters (FBAPs) to skeletal joints
* and morph targets.
* <li>Standard MPEG-4 neutral pose (see MPEG-4 FBA Overview for more information).
* </ul>
* 
* VisageCharModel also exposes methods needed for animation:
* 
* <ul>
* <li>loadFromFile. Loads metadata needed for animation from the character's VCM
* (Visage Character Model) file. This metadata includes mappings of MPEG-4 FBAPs
* to skeletal joints and morph targets as well as MPEG-4 neutral pose of the character.
* <li>applyFBAPs. Pre-applies a set MPEG-4 face and body animation
* parameter values to the character. Multiple successive calls to this method
* will blend several FBAP value sets together.
* <li>updateGeometry. Applies the current set of blended FBAP values to the model. I.e.
* this method updates the actual geometry that corresponds to your character.
* </ul>
* 
* In general, you will not call these methods manually. Instead, you will
* have an instance of FAPlayer play animations on your model.
* 
* When integrating visage|SDK with a rendering engine (i.e. when you don't want
* to use the default Horde3D renderer), subclass this class and
* implement all the pure virtual methods. These include, in a nutshell:
*
* <ul>
* <li>Scene wrapper methods (_getModelRoot, _hasNode*, _getNode*, _hasMesh* etc.).
* VisageCharModel uses these methods to retrieve information
* about the model's structure and geometry directly from the scene,
* so their implementation is renderer-specific.
* <li>_updateGeometry. This protected method is used to update the scene geometry
* which corresponds to your character.
* </li>
*
* A detailed tutorial on how to integrate visage|SDK with a renderer of your choice
* can be found in visage|SDK documentation.
*
* @see FAPlayer
* @see Skeleton
* @see Joint
* @see Mesh
* @see MorphTarget
* @see Horde3DCharModel
*/
class VISAGE_DECLSPEC VisageCharModel
{

public:

	/**
	* Constructor.
	*/
	VisageCharModel( const string& id );

	/**
	* Destructor.
	*/
	virtual ~VisageCharModel();

	/**
	* Initializes the character model. Creates the skeleton and meshes
	* by using the scene wrapper functions.
	*/
	virtual bool init();

	/**
	* Gets the identifier of the character model.
	*/
	const string& getId() { return id; }

	/**
	* Gets a pointer to the character's skeleton.
	*/
	Skeleton* getSkeleton() const { return skeleton; }

	/**
	* Creates a new mesh on the model.
	*/
	Mesh* createMesh( const string& meshId );
	
	/**
	* Deletes the mesh at the specified index.
	*/
	void deleteMeshByIndex( int meshIndex );

	/**
	* Deletes the specified mesh.
	*/
	void deleteMeshById( const string& meshId );

	/**
	* Returns true if the specified mesh exists, otherwise false.
	*/
	bool hasMesh( const string& meshId ) const;

	/**
	* Returns a pointer to the specified mesh.
	*/
	Mesh* getMeshById( const string& meshId ) const;

	/**
	* Returns a pointer to the mesh at the specified index.
	*/
	Mesh* getMeshByIndex( int meshIndex ) const;

	/**
	* Returns the number of meshes in the model.
	*/
	int numberOfMeshes() const;

	/**
	* Apply a set of face and body animation parameter values.
	* Deformation of the model won't occur until updateGeometry() is called.
	* Typical use: called by FAPlayer once per every animation frame.
	*
	* @see FAPlayer
	*/
	virtual void applyFBAPs( const FBAPs* fbaps );

	/**
	* Clear the currently applied sets of FBAP values.
	* Typical use: called by FAPlayer.
	*
	* @see FAPlayer
	*/
	virtual void clearFBAPs();

	/**
	* Deforms the model based on current sets of face and body animation
	* parameter values. All current sets are deleted after deformation.
	* Typical use: called by FAPlayer once per every animation frame.
	*
	* @see FAPlayer
	*/
	virtual void updateGeometry();

	/**
	* Loads the character model metadata from a VCM file.
	*
	* @param filename VCM filename.
	* @return true if the VCM file was successfully loaded, otherwise false.
	*/
	virtual bool loadFromFile( const string& filename );

	/**
	* Saves the character model animation metadata to a VCM file. Character model must be valid.
	*
	* @param filename VCM filename.
	* @return true if the VCM file was successfully saved, otherwise false.
	*/
	virtual bool saveToFile( const string& filename ) const;

	/**
	* Gets the feature point mappings.
	*/
	virtual FDP* getFeaturePoints();

	/**
	* Determine if specified mesh is part of the left eye.
	*/
	virtual bool isLeftEyeMesh( const string& id ) const;

	/**
	* Add specified mesh to the list of meshes belonging to the left eye.
	*/
	virtual void addLeftEyeMesh( const string& id );

	/**
	* Remove specified mesh from the list of meshes belonging to the left eye.
	*/
	virtual void removeLeftEyeMesh( const string& id );

	/**
	* Get number of left eye meshes.
	*/
	virtual int numberOfLeftEyeMeshes() const;

	/**
	* Get identifier of the left eye mesh at the specified index.
	*/
	virtual const string& getLeftEyeMesh( int i ) const;

	/**
	* Set identifier of the left eye mesh at the specified index.
	*/
	virtual void setLeftEyeMesh( int i, const string& id );

	/**
	* Get identifier of the node or bone which controls motion of the left eye.
	*/
	virtual const string& getLeftEyeNode() const;
	
	/**
	* Set identifier of the node or bone which controls motion of the left eye.
	*/
	virtual void setLeftEyeNode( const string& id );

	/**
	* Determine if specified mesh is part of the right eye.
	*/
	virtual bool isRightEyeMesh( const string& id ) const;

	/**
	* Add specified mesh to the list of meshes belonging to the right eye.
	*/
	virtual void addRightEyeMesh( const string& id );

	/**
	* Remove specified mesh from the list of meshes belonging to the right eye.
	*/
	virtual void removeRightEyeMesh( const string& id );

	/**
	* Get number of right eye meshes.
	*/
	virtual int numberOfRightEyeMeshes() const;

	/**
	* Get identifier of the right eye mesh at the specified index.
	*/
	virtual const string& getRightEyeMesh( int i ) const;

	/**
	* Set identifier of the right eye mesh at the specified index.
	*/
	virtual void setRightEyeMesh( int i, const string& id );

	/**
	* Get identifier of the node or bone which controls motion of the right eye.
	*/
	virtual const string& getRightEyeNode() const;
	
	/**
	* Set identifier of the node or bone which controls motion of the right eye.
	*/
	virtual void setRightEyeNode( const string& id );

	/**
	* Determine if specified mesh is part of the upper teeth.
	*/
	virtual bool isUpperTeethMesh( const string& id ) const;

	/**
	* Add specified mesh to the list of meshes belonging to the upper teeth.
	*/
	virtual void addUpperTeethMesh( const string& id );

	/**
	* Remove specified mesh from the list of meshes belonging to the upper teeth.
	*/
	virtual void removeUpperTeethMesh( const string& id );

	/**
	* Get number of upper teeth meshes.
	*/
	virtual int numberOfUpperTeethMeshes() const;

	/**
	* Get identifier of the upper teeth mesh at the specified index.
	*/
	virtual const string& getUpperTeethMesh( int i ) const;

	/**
	* Set identifier of the upper teeth mesh at the specified index.
	*/
	virtual void setUpperTeethMesh( int i, const string& id );

	/**
	* Get identifier of the node or bone which controls motion of the upper teeth.
	*/
	virtual const string& getUpperTeethNode() const;
	
	/**
	* Set identifier of the node or bone which controls motion of the upper teeth.
	*/
	virtual void setUpperTeethNode( const string& id );

	/**
	* Determine if specified mesh is part of the lower teeth.
	*/
	virtual bool isLowerTeethMesh( const string& id ) const;

	/**
	* Add specified mesh to the list of meshes belonging to the lower teeth.
	*/
	virtual void addLowerTeethMesh( const string& id );

	/**
	* Remove specified mesh from the list of meshes belonging to the lower teeth.
	*/
	virtual void removeLowerTeethMesh( const string& id );

	/**
	* Get number of lower teeth meshes.
	*/
	virtual int numberOfLowerTeethMeshes() const;

	/**
	* Get identifier of the lower teeth mesh at the specified index.
	*/
	virtual const string& getLowerTeethMesh( int i ) const;

	/**
	* Set identifier of the lower teeth mesh at the specified index.
	*/
	virtual void setLowerTeethMesh( int i, const string& id );

	/**
	* Get identifier of the node or bone which controls motion of the lower teeth.
	*/
	virtual const string& getLowerTeethNode() const;
	
	/**
	* Set identifier of the node or bone which controls motion of the lower teeth.
	*/
	virtual void setLowerTeethNode( const string& id );

	/**
	* Determine if specified mesh is part of the tongue.
	*/
	virtual bool isTongueMesh( const string& id ) const;

	/**
	* Add specified mesh to the list of meshes belonging to the tongue.
	*/
	virtual void addTongueMesh( const string& id );

	/**
	* Remove specified mesh from the list of meshes belonging to the tongue.
	*/
	virtual void removeTongueMesh( const string& id );

	/**
	* Get number of tongue meshes.
	*/
	virtual int numberOfTongueMeshes() const;

	/**
	* Get identifier of the tongue mesh at the specified index.
	*/
	virtual const string& getTongueMesh( int i ) const;

	/**
	* Set identifier of the tongue mesh at the specified index.
	*/
	virtual void setTongueMesh( int i, const string& id );

	/**
	* Get identifier of the node or bone which controls motion of the tongue.
	*/
	virtual const string& getTongueNode() const;
	
	/**
	* Set identifier of the node or bone which controls motion of the tongue.
	*/
	virtual void setTongueNode( const string& id );

	/**
	* Determine if specified mesh is part of the face.
	*/
	virtual bool isFaceMesh( const string& id ) const;

	/**
	* Add specified mesh to the list of meshes belonging to the face.
	*/
	virtual void addFaceMesh( const string& id );

	/**
	* Remove specified mesh from the list of meshes belonging to the face.
	*/
	virtual void removeFaceMesh( const string& id );

	/**
	* Get number of face meshes.
	*/
	virtual int numberOfFaceMeshes() const;

	/**
	* Get identifier of the face mesh at the specified index.
	*/
	virtual const string& getFaceMesh( int i ) const;

	/**
	* Set identifier of the face mesh at the specified index.
	*/
	virtual void setFaceMesh( int i, const string& id );

	/**
	* Get identifier of the node or bone which controls motion of the face.
	*/
	virtual const string& getFaceNode() const;
	
	/**
	* Set identifier of the node or bone which controls motion of the face.
	*/
	virtual void setFaceNode( const string& id );

	/**
	* Get identifier of the skeleton center-of-mass node.
	*/
	virtual const string& getSkeletonCOM() const;

	/**
	* Set identifier of the skeleton center-of-mass node.
	*/
	virtual void setSkeletonCOM( const string& id );

	/**
	* Get a facial animation parameter mapping.
	*
	* @param id FAP id. Valid range is 1 to 68, but values 1 and 2 correspond to
	* visemes and expression and will return an empty mapping.
	*/
	virtual const FBAPMapping& getFAPMapping( int id ) const;

	/**
	* Get a viseme parameter mapping.
	*
	* @param no Viseme number. Valid range is 0 to 14.
	*/
	virtual const FBAPMapping& getVisemeMapping( int no ) const;

	/**
	* Get an expression parameter mapping.
	*
	* @param no Expression number. Valid range is 0 to 6.
	*/
	virtual const FBAPMapping& getExpressionMapping( int no ) const;

	/**
	* Get a body animation parameter mapping.
	*
	* @param id BAP id. Valid range is 1 to 186.
	*/
	virtual const FBAPMapping& getBAPMapping( int id ) const;

	/**
	* Get an animation parameter mapping on this character model.
	*
	* @param name Animation parameter name.
	*/
	virtual const FBAPMapping& getFBAPMapping( const string& name ) const;

	/**
	* Remaps an animation parameter on this character model.
	*/
	virtual void setFBAPMapping( const FBAPMapping& mapping );

	/**
	* Gets the identifier of the character model's root node. It can be used to rotate or
	* translate the entire character model. If the model is a face model,
	* this is the node which controls motion of the entire face. If the model has a skeleton,
	* this node is typically the skeleton's center-of-mass node.
	*/
	virtual string _getModelRoot() const = 0;

	/**
	* Determines if the character model has a node with the specified identifier.
	*/
	virtual bool _hasNode( const string& id ) const = 0;

	/**
	* Gets the array of identifiers of all nodes in the character model.
	*/
	virtual void _getNodes( StringArray& nodes ) const = 0;

	/**
	* Gets the identifier of the specified node's / bone's parent. If the node doesn't have a parent,
	* empty string is returned. If the node is model root, empty string should be returned.
	*/
	virtual string _getNodeParent( const string& id ) const = 0;

	/**
	* Gets an array of identifiers of the specified node's children.
	*/
	virtual void _getNodeChildren( const string& id, StringArray& children ) const = 0;

	/**
	* Gets the initial position of the specified node in the node's local space.
	*/
	virtual Vector3 _getNodeInitialLocalPosition( const string& id ) const = 0;

	/**
	* Gets the initial orientation of the specified node in the node's local space.
	*/
	virtual Quat _getNodeInitialLocalOrientation( const string& id ) const = 0;

	/**
	* Gets the initial scale of the specified node in the node's local space.
	*/
	virtual Vector3 _getNodeInitialLocalScale( const string& id ) const = 0;

	/**
	* Gets the initial position of the specified node in the node's world space.
	*/
	virtual Vector3 _getNodeInitialWorldPosition( const string& id ) const;

	/**
	* Gets the initial orientation of the specified node in the node's world space.
	*/
	virtual Quat _getNodeInitialWorldOrientation( const string& id ) const;

	/**
	* Gets the initial scale of the specified node in the node's world space.
	*/
	virtual Vector3 _getNodeInitialWorldScale( const string& id ) const;

	/**
	* Gets the current position of the specified node in the node's local space.
	*/
	virtual Vector3 _getNodeLocalPosition( const string& id ) const = 0;

	/**
	* Gets the current orientation of the specified node in the node's local space.
	*/
	virtual Quat _getNodeLocalOrientation( const string& id ) const = 0;

	/**
	* Gets the current scale of the specified node in the node's local space.
	*/
	virtual Vector3 _getNodeLocalScale( const string& id ) const = 0;

	/**
	* Gets the current position of the specified node in the node's world space.
	*/
	virtual Vector3 _getNodeWorldPosition( const string& id ) const;

	/**
	* Gets the current orientation of the specified node in the node's world space.
	*/
	virtual Quat _getNodeWorldOrientation( const string& id ) const;

	/**
	* Gets the current scale of the specified node in the node's world space.
	*/
	virtual Vector3 _getNodeWorldScale( const string& id ) const;

	/**
	* Determine if the character model has a mesh with the specified identifier.
	*/
	virtual bool _hasMesh( const string& id ) const = 0;

	/**
	* Get an array of identifiers of all meshes in the character model.
	*/
	virtual void _getMeshes( StringArray& meshes ) const = 0;

	/**
	* Determine if specified mesh has a morph target with the specified identifier.
	*/
	virtual bool _hasMorphTarget( const string& meshId, const string& morphTargetId ) const = 0;

	/**
	* Get list of identifiers of all morph targets on the specified mesh.
	*/
	virtual void _getMorphTargets( const string& meshId, StringArray& morphTargets ) const = 0;

protected:

	/**
	* Updates the geometry of the model by copying joint transformations
	* and morph weights onto the model.
	*/
	virtual void _updateGeometry() = 0;

	string id; ///< Character model identifier.

	Skeleton* skeleton; ///< The character's skeleton.
	vector<Mesh*> meshes; ///< Array of meshes that comprise the character.
	map<string, Mesh*> meshesById; /// Map of meshes that comprise the character, indexed by identifiers.
	
	FBAPs curFBAPs; ///< Set of current FBAP values.

	FDP* fdp; ///< Feature point mappings, vertex lip assignments...
	StringArray leftEyeMeshes; ///< Vector containing identifiers of meshes that make up the left eye.
	string leftEyeNode; ///< Identifier of the scene node or bone which controls the motion of the left eye.
	StringArray rightEyeMeshes; ///< Vector containing identifiers of meshes that make up the right eye.
	string rightEyeNode; ///< Identifier of the scene node or bone which controls the motion of the right eye.
	StringArray upperTeethMeshes; ///< Vector containing identifiers of meshes that make up the upper teeth.
	string upperTeethNode; ///< Identifier of the scene node or bone which controls the motion of the upper teeth.
	StringArray lowerTeethMeshes; ///< Vector containing identifiers of meshes that make up the lower teeth.
	string lowerTeethNode; ///< Identifier of the scene node or bone which controls the motion of the lower teeth.
	StringArray tongueMeshes; ///< Vector containing identifiers of meshes that make up the tongue.
	string tongueNode; ///< Identifier of the scene node or bone which controls the motion of the tongue.
	StringArray faceMeshes; ///< Vector containing identifiers of meshes that make up the face.
	string faceNode; ///< Identifier of the scene node or bone which controls the motion of the face.
	string skeletonCOM; ///< Identifier of the center-of-mass node of the character model's skeleton.

	map<int, FBAPMapping> visemeMappings; ///< Map of viseme mappings, indexed by viseme numbers.
	map<int, FBAPMapping> expressionMappings; ///< Map of expression mappings, indexed by expression numbers.
	map<int, FBAPMapping> fapMappings; ///< Map of low-level FAP mappings, indexed by FAP identifiers.
	map<int, FBAPMapping> bapMappings; ///< Map of BAP mappings, indexed by BAP identifiers.

private:

	void _initSkeleton( const string& jointId );
	void _applyFBAPToGeometry( const FBAPMapping& mapping, float t );

};

}

#endif // __VisageCharModel_h__
