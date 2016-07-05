// FAExporter.h: interface for the FAExporter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FAEXPORTER_H__
#define __FAEXPORTER_H__

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


#include <float.h>
#include <assert.h>
#include "FAPlayer.h"
#include "AFM.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TT_WRITE 0
#define TT_INITIALIZE0 1
#define TT_INITIALIZE1 2
#define TT_INITIALIZE2 3
#define TT_ADDFRAME 4

namespace VisageSDK
{

/** Transform animation data.
* This structure is used by FAExporter to hold the animation data for one frame of animation for a Transform.
* The parts of transform that can be animated are translation, rotation and scale. This structure holds the differences 
* in translation, rotation and scale between the current frame and the previous frame,
* as well as the information whether the current frame is a key frame
* for any of those elements.
*
* Implemented in visagefaplayer.lib
* @see animData
* @see ifsAnimData
*/
typedef struct
{
	float r[4]; /**< Rotation difference. The difference in rotation between the current and previous frame. The rotations are expressed as quaternions. For the first frame in animation, this holds the absolute value. */
	float t[3];  /**< Translation difference. The difference in translation between the current and previous frame. For the first frame in animation, this holds the absolute value. */
	float s[3];  /**< Scale difference. The difference in scale between the current and previous frame. For the first frame in animation, this holds the absolute value. */
	int keyR; /**< Key frame flag. The value of 1 indicates that this frame is a key frame for the rotation of this transform. */
	int keyT; /**< Key frame flag. The value of 1 indicates that this frame is a key frame for the translation of this transform. */
	int keyS; /**< Key frame flag. The value of 1 indicates that this frame is a key frame for the scale of this transform. */
	int split; /**< split flag. Indicates that this transform is split and its other components follow in the ta field of animData. See FAExporter::forceSplitTransform for a discussion on splitting the transforms*/
} transformAnimData;

/** IFS animation data.
* This structure holds the animation data for one frame of animation for an Indexed face Set (IFS).
* An IFS consists of a number of vertices. This structure holds the differences 
* in vertex positions between the current frame and the previous frame,
* as well as the information whether the current frame is a key frame
* for a vertex, and the number of vertices for which the current frame is a key frame.
*
* Implemented in visagefaplayer.lib
* @see animData
* @see transformAnimData
*/
typedef struct
{
	myVector *v; /**< Vertex position differences. The difference in position of each vertex between the current and previous frame. For the first frame in animation, this holds the absolute vertex positions. */
	int nv; /**< Number of vertices. The number of vertices in this IFS */
	int key; /**< Number of keys. The number of vertices for which the current frame is a key frame */
	int*keys;  /**< Index to keys. This array corresponds to the vertex array v. The value of 1 for a vertex indicates that this frame is a key frame for this vertex. */
} ifsAnimData;

/** Animation data.
* This structure is used by FAExporter to hold the animation data for one frame of animation for the whole model.
* The elements that can be animated are transforms and vertices. Therefore the animation
* data consists of the data for Indexed Face Sets (IFS), which contain vertices, and
* the data for transforms. A model consists of several transforms and several IFSs.
*
* This structure is automatically produced by FAExporter. The data is adapted for use in
* keyframe-based animation systems. It contains information about which frames
* are keyframes for each vertex and transform component (translation, rotation, scale) in the 
* animated 3D model. Using this information, it is convenient to export the whole animation,
* represented as an array of animData structures (one animData per frame), to any keyframe based
* animation system or file format.
*
* Implemented in visagefaplayer.lib
* @see ifsAnimData
* @see transformAnimData
*/
typedef struct
{
	ifsAnimData *ifsa; /**<  IFS animation data. The animation data for Indexed Face Sets, one ifsAnimData structure for each IFS in the model. */
	transformAnimData *ta; /**<  Transform animation data. The animation data for Transforms, one transformAnimData structure for each transform in the model. */
	int nifs; /**<  Number of IFSs. Number of Indexed Face Sets in the model */
	int nt; /**<  Number of Transforms. Number of Transforms in the model */
} animData;

/**
* Face and Body Animation Exporter class.
* This is a utility class used to export full multi-track virtual character animation into keyframe 
* based systems or files. The FAExporter loads all frames of animation, then processes
* them in order to extract keyframes for all moving elements - transform translations/rotations
* and vertex movements. The keyframes are extracted assuming linear interpolation and
* optimised in order to minimise the number of keyframes. Keyframes are inserted only
* at the points in animation where the gradient of a movement changes. The obtained keyframes
* are ready to be exported into a keyframe based system or file. As an example, a plugin
* for 3ds max has been implemented in this way, using an FAExporter to set keyframes
* for transforms.  
*
* The operation is typically as follows:
* - an FAExporter is initialised with a VRMLModel and a number of frames of animation
* - for each frame of animation, addFrame() is called and the deformed VRMLModel is passed to it
* - when all frames are filled in this way, processAnimation() is called; it extracts all key frames
* - the model geometry and all keyframes are now ready to export and available in the FAExporter::ad array
*
* Instead of callind addFrame manually for each frame, it is more elegant to initialise and
* FAPlayer with the animation that needs to be exported, and call the play() function
* of the FAPlayer, passing the FAExporter as the renderer. In this way, the FAExporter::render()
* function is called for each frame of animation, and it calls the addFrame() function.
*
* Implemented in visagefaplayer.lib
*
* @see FAExporter(VRMLModel *m, int nfr)
* @see addFrame()
* @see processAnimation()
* @see VRMLModel
* @see FAPlayer
* @see FAPlayer::play()
* @see render()
* @see FAExporter::ad
*/
class VISAGE_DECLSPEC FAExporter: public FARenderer, public VRMLTraverser
{
public:
	FAExporter();
	FAExporter(VRMLModel *m);

	/**
	* Constructor.
	* Initialise the exporter with the VRMLModel m, and for nfr frames. The geometry
	* of the model is loaded into the exporter. The memory is allocated for the frames.
	* The exporter is now ready to receive animation frames using the addFrame() function.
	* @param m the virtual character model whose animation will be exported. It must be in a neutral position (no movement, no facial expression). Typically, this model is obtained using AFM::getVCModel()
	* @param nfr number of frames in the animation that will be exported.
	* @see addFrame()
	*/
	FAExporter(VRMLModel *m, int nfr);
	FAExporter(int forceSplitTransform,VRMLModel *m);

	/**
	* Constructor.
	* This is a convenience constructor. It does the same as FAExporter(VRMLModel *m, int nfr),
	* but at the same time sets the forceSplitTransform flag.
	*
	* Initialise the exporter with the VRMLModel m, and for nfr frames. The geometry
	* of the model is loaded into the exporter. The memory is allocated for the frames.
	* The exporter is now ready to receive animation frames using the addFrame() function.
	*
	* The forceSplitTransform determines the mode of exporting transforms. In VRML, a transform
	* is a combination of translation, rotation and scale, where scale can be oriented in arbitrary
	* direction, and rotation can be done around an arbitrary center. This can always be represented
	* by a chain of transforms, one transform for each component, in the following order:
	* - inverse translation by the center of rotation vector
	* - inverse scale orientation
	* - scale
	* - scale orientation
	* - rotation
	* - translation by the center of rotation vector
	* - translation 
	*
	* Depending on how the target system handles transforms, it may be necessary to split all transforms into
	* these components and export them as a series of transforms. If forceSplitTransform is set to 1
	* all transforms will always be split like this, i.e. each transform will be exported as 7 transforms.
	* If forceSplitTransform is set to 0, the splitting will be done only when it is necessary for animation.
	* If a transform rotates around a non-origin center, it is impossible to represent it suitably
	* by keyframes (interpolation would not work well) unless it is split into components. Also, the
	* exporter does not treat scale orientation, and therefore a split has to be done if
	* non-identity scale orientation is encountered. So, when
	* a non-origin center of rotation or a non-identity scale orientation are detected, a split is always done.
	*
	* The default value for this flag is 1, i.e. transforms are split.
	* 
	* @param m the face model whose animation will be exported. It must be in a neutral position (no movement, no facial expression).
	* @param nfr number of frames in the animation that will be exported.
	* @param forceSplitTransform controls the splitting of transforms.
	*
	* @see addFrame()
	* @see FAExporter(VRMLModel *m, int nfr)
	* @see forceSplitTransform
	*/
	FAExporter(int forceSplitTransform,VRMLModel *m, int nfr);

	/** Destructor.
	*/
	virtual ~FAExporter();

	/**
	* Add a frame of animation.
	* Animation consists of a number of frames. Each frame is represented by a full face model (VRMLModel)
	* deformed into the position/shape corresponding to the current frame of animation. addFrame() is used
	* to add frames to the exporter. All frames, i.e. their face models, are stored in the exporter.
	* When processAnimation() is called, they are processed. The frames must be added in order from first to last.
	* The number of frames is set by the constructor or by the init() function.
	*
	* All models passed by addFrame() to the exporter must be versions of the same model, the 
	* one that the exporter was initialised with. The only difference between the initial model
	* and the models for frames is in the vertex positions and transforms. Passing a model
	* with a different topology (different number/configuration of vertices) would lead to 
	* unpredictable results.
	*
	* @param m the VRMLModel corresponding to the next frame of animation.
	* @see init()
	* @see VRMLModel
	* @see processAnimation()
	* @see render()
	*/
	void addFrame(VRMLModel *m);

	/**
	* Process the animation and build key frames.
	* Animation consists of a number of frames. Each frame is represented by a full virtual character model (VRMLModel)
	* deformed into the position/shape corresponding to the current frame of animation (addFrame()
	* is used to initialise the frames). 
	* When processAnimation() is called, they are processed. 
	*
	* All models are versions of the same model, the 
	* one that the exporter was initialised with. The only difference between the initial model
	* and the models for frames is in the vertex positions and transforms. These differences are analysed
	* by processAnimation().
	*
	* The elements that are considered for animation are
	* - vertex positions
	* - translation component of transforms
	* - rotation component of transforms
	* - scale component of transforms
	*
	* All vertices and transforms in the model are tested to see if they are animated (i.e. if they change).
	* For those that do, key frames are determined. 
	* 
	* Key frames are an efficient way to represent animation. A value that is animated can be stored
	* only for the key frames, and the rest of the frames is obtained by linear interpolation
	* between key frames. In order to obtain key frames, each animation element (vertex position, translation
	* of a transform...) is analysed and key frames are extraced in such a way that linear interpolation
	* between them gives the original data as a results.
	*
	* The whole animation data is stored in the FAExporter::ad array.
	*
	* @see addFrame()
	* @see VRMLModel
	* @see render()
	* @see ad
	* @see animData the animation data structure.
	*/
	void  processAnimation(void);

	/**
	* FARenderer callback function
	* An implementation of a virtual function from FARenderer. Should not be called directly.
	*/
	void render(VRMLModel *faceModel, long timeMs);

	/**
	* FARenderer callback function
	* An implementation of a virtual function from FARenderer. Should not be called directly.
	*/
	void preRender(FBAPs *fbaps, long timeMs);


	void init(VRMLModel *m, int nfr);
	void traverseRoot(void *model, int index);
	void traverseRootEnd(void *model, int index);
	int traverseTransform(
		void *model,    // VRMLModel pointer to the face model
		int index,      // index of the transform within the face model
		float *c,       // Center of rotation expressed as translation (x,y,z)
		float *negc,    // Negative translation of the center of rotation
		float *r,       // Rotation expressed as axis and angle in radians (x,y,z,angle)
		float *rq,      // Rotation expressed as quaternion (x,y,z,w)
		float *s,       // Scale (x,y,z)
		float *so,      // Scale orientation (acis and angle)
		float *soq,     // Scale orientation (quaternion)
		float *negso,   // Inverse of scale orientation (axis and angle)
		float *negsoq,  // Inverse of scale orientation (quaternion)
		float *t,       // Translation (x,y,z)
		myMatrix m);  // Complete transformation matrix
	void traverseTransformEnd(void *model, int index);
	void traverseGroup(void *model, int index);
	void traverseGroupEnd(void *model, int index);
	void traverseShape(
		void *model,         // VRMLModel pointer to the face model
		int index,           // index of the indexed face set within the face model
		VRMLMaterial *mat,   // material for this shape
		float *coord,        // vertex coordinates
		int coordLength,     // length of the vertex coordinates array (number of vertices * 3)
		int *coordInd,       // vertex indices for triangles
		int coordIndLength,  // length of coordInd array
		float *norm,         // normals
		int normLength,      // length of norm array
		int *normInd,        // normal indices, per vertex
		int normIndLength,   // length of normInd array
		float *tcoord,       // texture coordinates, if any (else zero)
		int tcoordLength, 
		int *tcoordInd, 
		int tcoordIndLength, 
		float *cpv,          // colors per vertex, if any (else zero)
		int cpvLength, 
		int *cpvInd, 
		int cpvIndLength,
		int CPV);            // color per vertex (1) or per face (1)
	void traverseOther(void *model, int index,VRMLNode *node);

	int tc; 
	int gc; 
	VRMLModel *faceModel; /**< The face model. The face model that is being animated. */ 
	int traverseType; 
	int nFrames;	/**< Number of animation frames. Maximum number of animation frames that the exporter is initialised for */
	int loadedFrames;	/**< Number of loaded frames.  Number of loaded animation frames. It is always less than or equal to nFrames */
	animData cad; /**< Current animation data.  Holds the current animation data, i.e. current position of vertices and transforms during the building phase of the exporter; an animData structure. */
	animData iad; 
	animData *ad; /**< Animation data.  Holds the animation data; an array of animData structures, one for each frame of animation. This array is filled with data when processAnimation() function is called. Animation is stored as key frames, i.e. for each frame the differences in vertex positions, transform translations, rotations and scales with respect to the previous frame are stored, as well as the information whether the current frame is a key frame for each of the vertices and transform components. Key frames are frames for which values have to be provided to the keyframe based animation system, and the values for other frames can be interpolated from the key frames. Linear interpolation is assumed. @see animData*/

	/** The forceSplitTransform flag.
	* The forceSplitTransform determines the mode of exporting transforms. In VRML, a transform
	* is a combination of translation, rotation and scale, where scale can be oriented in arbitrary
	* direction, and rotation can be done around an arbitrary center. This can always be represented
	* by a chain of transforms, one transform for each component, in the following order:
	* - inverse translation by the center of rotation vector
	* - inverse scale orientation
	* - scale
	* - scale orientation
	* - rotation
	* - translation by the center of rotation vector
	* - translation 
	*
	* Depending on how the target system handles transforms, it may be necessary to split all transforms into
	* these components and export them as a series of transforms. If forceSplitTransform is set to 1
	* all transforms will always be split like this, i.e. each transform will be exported as 7 transforms.
	* If forceSplitTransform is set to 0, the splitting will be done only when it is necessary for animation.
	* If a transform rotates around a non-origin center, it is impossible to represent it suitably
	* by keyframes (interpolation would not work well) unless it is split into components. Also, the
	* exporter does not treat scale orientation, and therefore a split has to be done if
	* non-identity scale orientation is encountered. So, when
	* a non-origin center of rotation or a non-identity scale orientation are detected, a split is always done.
	*
	* The default value for this flag is 1, i.e. transforms are split.
	*/
	int forceSplitTransform;

private:
	void initCurrentAnim(void);
	int initializeTransform(float *t,float *r, float *s, float *so, float *c, float *negso, float *negc, int pass);
	void initializeIndexedFaceSet(int nc, float *coords,int pass);
	int addFrameTransform(float *t,float *r, float *s, float *so, float *c, float *negso, float *negc);
	void addFrameIndexedFaceSet(int nc, float *coords);

};

}

#endif
