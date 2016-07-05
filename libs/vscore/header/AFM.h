
#ifndef __AFM_H__
#define __AFM_H__


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

#include "VisageCharModel.h"
#include "VRMLModel.h"
#include "FBADecoder.h"

namespace VisageSDK
{

/**
* Animatable Face and Body Model.
* An Animatable Face and Body Model (AFM) is a full virtual character model with the necessary data for its animation. 
*
* By default, the face animation is
* based on the concept of morph targets; body animation is implemented using the skeleton and skinning. 
* A morph target is a deformed face model, i.e. the face model
* with its vertices and/or transforms moved into new positions. For example, a morph target can represent
* a phoneme, i.e. the mouth position corresponding to a phoneme. In the Visage system, there is one
* morph target for each Face Animation parameter (FAP), so there is a total of 86 morph targets.
* The animation is obtained by interpolating between all those morph targets using the FAPs as weight
* factors for the morph targets.
*
* An AFM contains the face model and all the morph targets necessary for its animation, as well as the full body model, skeleton structure information and bone weights for skinning. An incomplete AFM may contain only some of those.
* This class allows to load and save an AFM, build a new AFM from a set of morph targets, provides access to the model itself (geometry),
* allows to animate the model by passing a set of MPEG-4 Face and Body Animation parameters (FBAPs) to it.
*
* It is possible to fully or partially replace the deformation mechanism that interpretes the
* face and body animation parameters by custom implementations. For more information see the section on <a href="#extensions">visage|SDK extension capabilities.</a>
*
* By default, the AFM (.afm) file format is used for storing the virtual character models. This is an
* open, fully documented file format based on VRML97 standard. For more information, the <a href="AFM File Format Reference.pdf">%AFM File Format Reference</a>.
* For information on supporting other file formats, see the section on <a href="#extensions">visage|SDK extension capabilities.</a>
*
* An AFM also implements VisageCharModel interface, that enables its integration into an engine which
* supports morph target and/or skeletal animation of the virtual character. Saving/loading of the
* character model consists of storing/loading character metadata to/from a VCM file and saving/loading
* character scene graph to/from an AFM file. MPEG-4 animation parameter mappings, morph targets, neutral
* pose transformations and PhotoModel data (vertex and texture coordinates of head meshes) should
* be saved to an AFM file. Also, MPEG-4 animation parameter mappings, morph targets and neutral pose
* transformations should be loaded from an AFM file. In methods that work with morph targets morph
* target identifiers must correspond to the MPEG-4 Face and Body Animation parameter names, because
* AFM defines morph targets as a part of animation parameters and does not assign special names to them.
* AFM generally allows a definition of transform scaling data in animation parameters, but
* VisageCharModel methods disregard this data. This is also the case with the initial facial pose in
* neutral pose data. However, VisageCharModel methods that work with neutral pose data allow a
* specification of the neutral pose positions. This data is ignored in AFM because it supports only
* neutral pose orientations.
*
* Implemented in visagesdkAFM.lib
*
* Demonstrated in <a href="../Samples/AFM/build/msvc8/SimpleAnimation/doc/index.html">SimpleAnimation</a>.
*
*/
class VISAGE_DECLSPEC AFM : public VisageCharModel
{
public:
    
    /**
     * Retuns the number of relevant nodes (Shape, Coordinate, TextureCoordinate and Normal) to monitorize the load
     */
    static void countRelevantNodesToMonitorizeLoadProgress(std::string pathToFile);
    
	/**
	* Constructor.
	* Constructs an empty AFM.
	*/
	AFM();

	/**
	* Constructor.
	* Constructs an AFM by reading the virtual character model from the file. The file
	* name extension will be checked to verify if the file format is supported. By default,
	* only the AFM (.afm) file format is supported. This is an
	* open, fully documented file format based on VRML97 standard. For more information, the <a href="AFM File Format Reference.pdf">%AFM File Format Reference</a>.
	* Other file formats may be supported by attaching custom FileIO implementations to the AFM. For information on supporting other file formats, see the section on <a href="#extensions">visage|SDK extension capabilities.</a>
	* @param fname the name of the file containing the AFM to load.
	*/
	AFM(char *fname);

	/**
	* Constructor.
	* Constructs an AFM from a neutral face model and a set of face models (VRMLModel) representing morph targets.
	* Morph targets must have exactly the same topology as the neutral model, only vertex positions 
	* and transforms may be changed.
	* @param m0 the neutral face model.
	* @param m the morph targets. The first 15 are the visemes according to MPEG-4 specification.
	* The next 7 are the expressions according to MPEG-4. The final 68 are the low level FAPs.
	* Refer to the <a href="../MPEG-4 FBA Overview.pdf">MPEG-4 Face and Body Animation Introduction</a> for parameter details.
	* @param fv the FAP value corresponding to each morph target. For the visemes and expressions it is not used. For the other FAPs it is the value of the FAP represented by the given morph target. So, the model m[N] is the morph target representing the face with applied FAP N with the FAP value fv[N].
	* @param _coordSensitivity the coordinate sensitivity to be used for processing the morph targets, i.e. how much different must a vertex position in a morph target be from the vertex position in the neutral model in order to be registered as a movement. 0.1 is a good value. It actually corresponds to coordinate sensitivity threshold, expressed as percentage of the model size. Higher values can produce smaller %AFM files by sacrifising some finer movement.
	*/
	AFM(VRMLModel *m0, VRMLModel **m, int *fv,float _coordSensitivity);

	/**
	* Destructor.
	* Frees all memory and destroys the AFM.
	*/
	virtual ~AFM();

	/**
	* Read the Animatable Face and Body Model from a file.
	* This method will try to load the Animatable Face and Body Model from the file and return true if
	* the model was succesfuly loaded, or false otherwise. Typically, the file to load
	* is of the AFM type (.afm extension), which is supported by default. Other file types
	* my be supported using the FileIO mechanism.
	*
	* The FileIO objects attached to the AFM are used for loading the file. 
	* Up to 100 instances of file reader-writer classes of type FileIO can be attached to the AFM.
	* Each may support reading and writing of one or more file formats. When AFM needs to read or write
	* to a file, it calls all attached FileIO objects until one of them succesfuly reads the file.
	* By default, a BaseFileIO object is attached to the AFM, allowing to handle AFM (.afm) files.
	* @param fname the name of the file to be loaded.
	*
	* @see addFileIO
	* @see getFileIO
	* @see getNFileIOs
	* @see removeAllFileIOs
	* @param fname the name of the file containing the AFM to load.
	*/
	bool read(char *fname);

	/**
	* Applies a set of Face and Body Animation Parameters (FBAPs) to the face model.
	* Adds the fbaps parameters to the current list of FBAPs in the AFM. The FBAPs are *not* immediately applied to the model 
	* geometry - to do that, the updateGeometry() function must be called. The reason for this
	* is to allow several sets of FBAPs to be applied together by several subsequent calls
	* to applyFBAPs().
	* 
	* In a simple case, you may want to apply a set of FBAPs to the face and see the result immediately.
	* In this case, you call applyFBAPs() followed by updateGeometry().
	*
	* In a slightly more complex case, you may want to apply several sets of FBAPs (for example, when mixing
	* several FBAP streams). In this case, applyFBAPs() is called several times with all FBAPs to apply. The
	* FBAPs are merged together internally and applied to the actual model geometry
	* when the updateGeometry() function is called.
	*
	* This function actually calls AFMDeforme::applyFBAPs() function in all AFMDeformer objects attached
	* the the AFM.
	*
	* @param fbaps the FBAPs to apply.
	* @see updateGeometry
	*/
	void applyFBAPs(const FBAPs *fbaps);

	/**
	* Updates the geometry of the virtual character model using the current FBAPs.
	* The model is deformed corresponding to the current FBAPs. The "current FBAPs" are the internaly
	* stored FBAPs that merge together and accumulate all FBAPs applied to the model using the applyFBAPs() function
	* since the last call to the clearFBAPs() function. A typical animation loop would call clearFBAPs(), then applyFBAPs() one or more times,
	* then updateGeometry(), and finally getVCModel() to get the updated model to render; this is repeated for each frame of animation.
	*
	* This function actually calls AFMDeforme::updateGeometry() function in all AFMDeformer objects attached
	* the the AFM.
	*
	* @see applyFBAPs
	*/
	void updateGeometry();

	/**
	* Returns the model to neutral position.
	* Returns the model geometry to neutral position and clears the current FBAPs.
	*
	* This function actually calls AFMDeforme::clearFBAPs() function in all AFMDeformer objects attached
	* the the AFM.
	*
	* @see applyFBAPs
	* @see updateGeometry
	*/
	void clearFBAPs();

	/**
	* Returns the current virtual character model.
	* Typicaly this is used to obtain the virtual character model for rendering or exporting. 
	* It returns a pointer to the internally used model, not a newly constructed copy of the model. 
	*
	* @return The pointer to the internally used virtual character model.
	* @see VRMLModel
	*/
	VRMLModel *getVCModel();

	/**
	* Get a specific morph target and its FAP value.
	* Note that this function returns a pointer to the internally used model, not a newly constructed copy of the model.
	* If you want to keep several morph targets, you must immediately make a full copy using new VRMLModel(VRMLModel *m).
	* Typical mistake is  to get several morph targets in a loop using this function, and storing the pointers. This
	* does not store the morph targets, because the getMorphTarget function will actually return the same pointer each time.
	*
	* @param ind the index of the morph target to get.
	* - 0 - 14 are MPEG-4 visemes
	* - 15 - 21 are MPEG-4 expressions
	* - 22 - 87 are MPEG-4 low leve FAPs (so 22 is open_jaw)
	* @param fv FAP value will be returned in fv. It is the value of the FAP represented by the given morph target (for visemes it is the blend factor and for expressions the intensity). 
	* @return The VRMLmodel deformed into the requested morph target shape.
	* @see VRMLModel
	*/
	VRMLModel *getMorphTarget(int ind,int *fv);

	/**
	* Add a FileIO file reader/writer class.
	* Up to 100 instances of file reader-writer classes of type FileIO can be attached to the AFM.
	* Each may support reading and writing of one or more file formats. When AFM needs to read or write
	* to a file, it calls all attached FileIO objects until one of them succesfuly reads the file.
	* By default, a BaseFileIO object is attached to the AFM, allowing to handle AFM (.afm) files.
	* @param fio an instance of FileIO to be added.
	*
	* @see addFileIO
	* @see getFileIO
	* @see getNFileIOs
	* @see removeAllFileIOs
	*/
	void addFileIO(FileIO *fio) {FIO[nFIO++] = fio;};
	
	/**
	* Get a file reader/writer attached to the AFM.
	* Get a pointer to one of the FileIO objects attached to the AFM.
	*
	* @param n the number of FileIO to get; they are placed in order they were attached, i.e. for the first attached FileIO, n is 0, for the second one, n is 1 etc.
	*
	* @return if n is non-negative and smaller than the total number of attached FileIOs, return the pointer to the requested FileIO object; otherwise, return NULL.
	*
	* @see addFileIO
	* @see getFileIO
	* @see getNFileIOs
	* @see removeAllFileIOs
	*/
	FileIO *getFileIO(int n) {if(n>=0 && n < nFIO) return(FIO[n]); else return (NULL);};
	
	/**
	* Get the number of file reader/writer objects attached to the AFM.
	*
	* @return number of FileIO objects attached to the AFM.
	*
	* @see addFileIO
	* @see getFileIO
	* @see getNFileIOs
	* @see removeAllFileIOs
	*/
	int getNFileIOs() {return (nFIO);};
	
	/**
	* Remove all FileIO file reader/writer objects.
	*
	* @see addFileIO
	* @see getFileIO
	* @see getNFileIOs
	* @see removeAllFileIOs
	*/
	void removeAllFileIOs() {nFIO = 0;};

	
	/**
	* Add a AFMDeformer object to the AFM.
	* Up to 100 instances of deformers of type AFMDeformer can be attached to the AFM.
	* A deformer applies animation parameters (FBAPs) to the AFM's 3D character model (a VRMLModel object), thus deforming the model. 
	* When AFM needs to apply the FBAPs, it calls the appropriate function (AFMDeformer::clearFBAPs, AFMDeformer::applyFBAPs, AFMDeformer::upsateGeometry) in the attached
	* AFMDeformer objects. All attached deformers are called one by one, in the order they
	* were attached.
	* By default, a BaseAFMDeformer object is attached to the AFM. This deformer performs the animation
	* using morphing and modification of transforms. The AFMDeformer interface allows to implement
	* any other animation method. For even more advanced use, several deformer objects may be combined,
	* each implementing a different animation method. The selective animation enabling mechanism
	* of the AFMDeformer permits to specify, for each deformer, which parameters it implements. Thus,
	* for example, one deformer may be used for all body animation parameters , another for the
	* face animation parameters, and perhaps a specialized deformer for high-level parameters.
	*
	* @param deformer an instance of AFMDeformer to be added.
	*
	* @see AFMDeformer
	* @see addAFMDeformer
	* @see getAFMDeformer
	* @see getNAFMDeformers
	* @see removeAllAFMDeformers
	*/
	void addAFMDeformer(AFMDeformer *deformer) {deformers[nDeformers++] = deformer;};
	
	/**
	* Get a deformer attached to the AFM.
	* Get a pointer to one of the AFMDeformer objects attached to the AFM.
	*
	* @param n the number of AFMDeformer to get; they are placed in order they were attached, i.e. for the first attached AFMDeformer, n is 0, for the second one, n is 1 etc.
	*
	* @return if n is non-negative and smaller than the total number of attached AFMDeformers, return the pointer to the requested AFMDeformer object; otherwise, return NULL.
	*
	* @see addAFMDeformer
	* @see getAFMDeformer
	* @see getNAFMDeformers
	* @see removeAllAFMDeformers
	*/
	AFMDeformer *getAFMDeformer(int n) {if(n>=0 && n < nDeformers) return(deformers[n]); else return (NULL);};
	
	/**
	* Get the number of deformers attached to the AFM.
	*
	* @return number of AFMDeformer objects attached to the AFM.
	*
	* @see addAFMDeformer
	* @see getAFMDeformer
	* @see getNAFMDeformers
	* @see removeAllAFMDeformers
	*/
	int getNAFMDeformers() {return (nDeformers);};
	
	/**
	* Remove all AFMDeformer objects.
	*
	* @see addAFMDeformer
	* @see getAFMDeformer
	* @see getNAFMDeformers
	* @see removeAllAFMDeformers
	*/
	void removeAllAFMDeformers() {nDeformers = 0;};

	/**
	* Interpolates inner lip FAPs from outer or vice versa.
	* Some animation streams use only inner or only outer lip parameters. In such case
	* this function can be called to interpolate the missing FAPs - it actually just copies
	* the corresponding inner lip FAP value to the outer, or vice versa, depending on which one
	* is missing. This is rarely necessary.
	* @param faps the FAPs on which interpolation is applied.
	*/
	void interpolateInnerOuterLips(FAPs *faps);

	/**
	* Writes the AFM into a file.
	* The file
	* name extension will be checked to verify if the file format is supported. By default,
	* only the AFM (.afm) file format is supported. This is an
	* open, fully documented file format based on VRML97 standard. For more information, the <a href="AFM File Format Reference.pdf">%AFM File Format Reference</a>.
	* Other file formats may be supported by attaching custom FileIO implementations to the AFM. For information on supporting other file formats, see the section on <a href="#extensions">visage|SDK extension capabilities.</a>
	* @param fname the name of the file to be written.
	*/
	void write(char *fname);

	/**
	* Sets the coordinate sensitivity.
	* @param cs the coordinate sensitivity to be used for processing the morph targets, i.e. how much different must a vertex position in a morph target be from the vertex position in the neutral model in order to be registered as a movement. 0.1 is a good value. It actually corresponds to coordinate sensitivity threshold, expressed as percentage of the model size. Higher values can produce smaller %AFM files by sacrifising some finer movement.
	*/
	void setCoordSensitivity(float cs);

	/**
	* Sets the creaseAngle parameter of the VRML model.
	* creaseAngle is the VRML parameter determining how normals are interpolated. If the angle between the neighboring
	* triangles is greater than the creaseAngle, the edge between them is interpreted as a sharp crease
	* and the normals are set accordingly. Otherwise, the model is smoothed. Set the creaseAngle to a large value, e.g. 3.14 to
	* obtain a smooth model. This change will take effect when the AFM is written to a file.
	* @param ca the creaseAngle parameter
	*/
	void setCreaseAngle(float ca) {if(faceModel) faceModel->setCreaseAngle(ca);};

	/**
	* Gets an array of identifiers of all nodes in the model.
	*
	* @param nodes An array of identifiers of all nodes in the model.
	*/
	virtual void _getNodes( StringArray& nodes ) const;

	/**
	* Gets an array of identifiers of all nodes in the model.
	*
	* @param nodes An array of identifiers of all nodes in the model.
	* @param nodeId Specifies the node from which to begin the recursive retrieval of
	* node identifiers.
	*/
	virtual void _getNodes( StringArray& nodes, const string& nodeId ) const;

	/**
	* Gets identifier of the character model's root node. It can be used to rotate or
	* translate the entire character model. If the model is a face model,
	* this is the node which controls motion of the entire face. If the model has a skeleton,
	* this node is the skeleton's center-of-mass node.
	*
	* @return the identifier of the character model's root node.
	*/
	virtual string _getModelRoot() const;

	/**
	* Gets an array of identifiers of all meshes in the character model.
	*
	* @param meshes An array of all model meshes' identifiers.
	*/
	virtual void _getMeshes( StringArray& meshes ) const;

	/**
	* Gets list of identifiers of all morph targets on the specified mesh.
	* It is asserted that the mesh exists.
	*
	* @param meshId Identifier of the mesh.
	* @param morphTargets An array of identifiers of all morph targets on the specified mesh.
	*/
	virtual void _getMorphTargets( const string& meshId, StringArray& morphTargets ) const;

	/**
	* Gets an array of identifiers of the specified node's children. It is asserted that
	* the specified node exists.
	*
	* @param id Identifier of the node.
	* @param children An array of identifiers of the specified node's children.
	*/
	virtual void _getNodeChildren( const string& id, StringArray& children ) const;

	/**
	* Gets identifier of the specified node's / bone's parent. If the node doesn't
	* have a parent, empty string is returned. It is asserted that the specified
	* node / bone exists.
	*
	* @param id Identifier of the node / bone.
	* @return identifier of the node's / bone's parent.
	*/
	virtual string _getNodeParent( const string& id ) const;

	/**
	* Gets the initial position of the specified node in the node's local space.
	*/
	Vector3 _getNodeInitialLocalPosition( const string& id ) const;

	/**
	* Gets the initial orientation of the specified node in the node's local space.
	*/
	Quat _getNodeInitialLocalOrientation( const string& id ) const;

	Vector3 _getNodeInitialLocalScale( const string& id ) const { return Vector3( 1, 1, 1 ); }

	/**
	* Gets the initial position of the specified node in the node's world space.
	*/
	Vector3 _getNodeInitialWorldPosition( const string& id ) const;

	/**
	* Gets the initial orientation of the specified node in the node's world space.
	*/
	Quat _getNodeInitialWorldOrientation( const string& id ) const;

	/**
	* Gets the current position of the specified node in the node's local space.
	*/
	Vector3 _getNodeLocalPosition( const string& id ) const;

	/**
	* Sets the current position of the specified node in the node's local space.
	*/
	void _setNodeLocalPosition( const string& id, const Vector3& pos );

	Vector3 _getNodeLocalScale( const string& id ) const { return Vector3( 1, 1, 1 ); }

	/**
	* Gets the current orientation of the specified node in the node's local space.
	*/
	Quat _getNodeLocalOrientation( const string& id ) const;

	/**
	* Sets the current orientation of the specified node in the node's local space.
	*/
	void _setNodeLocalOrientation( const string& id, const Quat& orient );

	/**
	* Gets the current position of the specified node in the node's world space.
	*/
	Vector3 _getNodeWorldPosition( const string& id ) const;

	/**
	* Sets the current position of the specified node in the node's world space.
	*/
	void _setNodeWorldPosition( const string& id, const Vector3& pos );

	/**
	* Gets the current orientation of the specified node in the node's world space.
	*/
	Quat _getNodeWorldOrientation( const string& id ) const;

	/**
	* Sets the current orientation of the specified node in the node's world space.
	*/
	void _setNodeWorldOrientation( const string& id, const Quat& orient );

	/**
	* Determines if the character model has a node with the specified identifier.
	*
	* @param id Identifier of the node.
	* @return true if model has the specified node, otherwise false.
	*/
	virtual bool _hasNode( const string& id ) const;

	/**
	* Determines if the character model has a mesh with the specified identifier.
	*
	* @param id Identifier of the mesh.
	* @return true if model has the specified mesh, otherwise false.
	*/
	virtual bool _hasMesh( const string& id ) const;

	//
	// None of the methods below should even exist. For the love of God, DO NOT USE THEM!
	//

	/**
	* Gets a complete content of the specified mesh' index buffer.
	* It is asserted that the mesh exists.
	*
	* @param meshId Identifier of the mesh.
	* @param faceData The triangle vertex indices of each polygon mesh. Each triplet
	* of indices represents one triangle.
	*/
	void getFacesData( const string& meshId, IntArray& faceData ) const;

	/**
	* Gets number of faces on the specified mesh.
	* It is asserted that the mesh exists.
	*
	* @param meshId Identifier of the mesh.
	* @return number of faces on the specified mesh.
	*/
	int numberOfFaces( const string& meshId ) const;

	/**
	* Gets indices of vertices that comprise the specified face.
	* It is asserted that the mesh and face exist.
	*
	* @param meshId Identifier of the mesh that contains the specified face.
	* @param faceIndex Index of the requested face.
	* @param v1 Index of the first vertex that belongs to the specified face.
	* @param v2 Index of the second vertex that belongs to the specified face.
	* @param v3 Index of the third vertex that belongs to the specified face.
	*/
	virtual void getFaceVertices( const string& meshId, int faceIndex, int& v1,
		int& v2, int& v3 ) const;

	/**
	* Get morph target with the specified identifier on the specified mesh. morphTargetData is
	* filled with non-zero vertex position offsets. It must be asserted that the mesh and morph
	* target exist. Also, it is asserted that morph target identifier corresponds to the MPEG-4
	* Face and Body Animation parameter name.
	*
	* @param meshId Identifier of the mesh.
	* @param morphTargetId Identifier of the morph target. It must correspond to the MPEG-4
	* Face and Body Animation parameter name.
	* @param morphTargetData Vertex position offsets.
	*/
	virtual void getMorphTarget( const string& meshId, const string& morphTargetId,
		VertexPositionsMap& morphTargetData ) const;

	/**
	* Determines if specified mesh has a morph target with the specified identifier.
	* It is asserted that the mesh exists and that morph target identifier corresponds
	* to the MPEG-4 Face and Body Animation parameter name.
	*
	* @param meshId Identifier of the mesh.
	* @param morphTargetId Identifier of the morph target. It must correspond to
	* the MPEG-4 Face and Body Animation parameter name.
	* @return true if specified mesh has specified morph target, otherwise false.
	*/
	virtual bool _hasMorphTarget( const string& meshId,
		const string& morphTargetId ) const;

	/**
	* Remaps an animation parameter on this character model.
	* It is asserted that the specified parameter name is valid
	* according to the MPEG-4 FBA, and that bones used in this parameter exist.
	* Names of all morph targets must be equal to animation parameter name.
	*
	* @param mapping Animation parameter that needs to be mapped on the character
	* model.
	*/
	virtual void setFBAPMapping( const FBAPMapping& mapping );

	/**
	* Gets texture coordinates of the vertex at the specified index in the specified mesh.
	* It is asserted that the mesh exists, that it has a texture at the specified stage
	* and that index >= 0 and index <= total number of vertices.
	*
	* @param meshId Identifier of the mesh.
	* @param index Index of the vertex.
	* @param texStage Texture stage. It should be always equal to 0.
	* @return texture coordinates of the specified vertex in the specified mesh.
	*/
	virtual Vector2 getTexCoords( const string& meshId, int index,
		int texStage = 0 ) const;

	/**
	* Gets texture coordinates of all vertices of the specified mesh.
	* It is asserted that the mesh exists and that it has a texture at the specified
	* stage.
	*
	* @param meshId Identifier of the mesh.
	* @param texCoordData Texture coordinates of all vertices of the specified mesh.
	* @param texStage Texture stage. It should be always equal to 0.
	*/
	virtual void getTexCoordData( const string& meshId, Vector2Array& texCoordData, 
		int texStage = 0 ) const;

	/**
	* Gets filename of a texture map on the specified mesh.
	* It is asserted that the mesh exists and that it has a texture at the
	* specified stage.
	*
	* @param meshId Identifier of the mesh.
	* @param texStage Texture stage. It should be always equal to 0.
	* @return texture filename of the specified mesh.
	*/
	string getTextureFilename( const string& meshId, int texStage = 0 ) const;

	/**
	* Gets complete contents of the specified mesh's vertex buffer.
	* It is asserted that the mesh exists.
	*
	* @param meshId Identifier of the mesh.
	* @param vertexData Vertices' coordinates of the specified mesh.
	*/
	void getVertexData( const string& meshId, Vector3Array& vertexData ) const;

	/**
	* Gets number of vertices in the specified mesh.
	* It is asserted that the mesh exists.
	*
	* @param meshId Identifier of the mesh.
	* @return number of vertices in the specified mesh.
	*/
	int numberOfVertices( const string& meshId ) const;

	/**
	* Gets position of the vertex at the specified index in the specified mesh.
	* It is asserted that the mesh exists and that index >= 0 and index <= total
	* number of vertices.
	*
	* @param meshId Identifier of the mesh.
	* @param index Vertex index in the specified mesh.
	* @return vertex position in the specified mesh.
	*/
	virtual Vector3 getVertexPosition( const string& meshId, int index ) const;

	/**
	* Sets morph target with the specified identifier on the specified mesh. If
	* the morph target doesn't exist it is created. morphTargetData must hold
	* non-zero vertex position offsets.
	* It is asserted that the mesh exists and that morph target identifier
	* corresponds to the MPEG-4 Face and Body Animation parameter name.
	*
	* @param meshId Identifier of the mesh.
	* @param morphTargetId Identifier of the morph target. It must correspond to
	* the MPEG-4 Face and Body Animation parameter name.
	* @param morphTargetData Vertex position offsets.
	*/
	virtual void setMorphTarget( const string& meshId, const string& morphTargetId,
		const VertexPositionsMap& morphTargetData );

	/**
	* Sets position offset of the specified vertex in the specified morph target
	* of the specified mesh. It is asserted that the morph target, mesh and vertex
	* exist. Also, it is asserted that morph target identifier corresponds to the
	* MPEG-4 Face and Body Animation parameter name.
	*
	* @param meshId Identifier of the mesh.
	* @param morphTargetId Identifier of the morph target. It must correspond to
	* the MPEG-4 Face and Body Animation parameter name.
	* @param vertexIndex Index of the mesh vertex.
	* @param vertexOffset Position offset of the specified mesh vertex in the
	* specified morph target.
	*/
	virtual void setMorphTargetVertexOffset( const string& meshId,
		const string& morphTargetId, int vertexIndex, const Vector3& vertexOffset );

	/**
	* Sets texture coordinates of the vertex at the specified index in
	* the specified mesh. It is asserted that the mesh exists, that it
	* has a texture at the specified stage and that index >= 0 and
	* index <= total number of vertices.
	*
	* @param meshId Identifier of the mesh.
	* @param index Index of the vertex.
	* @param texCoord Texture coordinates of the specified vertex in the
	* specified mesh.
	* @param texStage Texture stage. It should be always equal to 0.
	*/
	virtual void setTexCoords( const string& meshId, int index,
		const Vector2& texCoord, int texStage = 0 );

	/**
	* Sets texture coordinates of all vertices of the specified mesh.
	* It is asserted that the mesh exists, that it has a texture at the
	* specified stage and that the number of vertices in the mesh is equal
	* to the number of elements in texCoordData.
	*
	* @param meshId Identifier of the mesh.
	* @param texCoordData Texture coordinates of all vertices of the
	* specified mesh.
	* @param texStage Texture stage. It should be always equal to 0.
	*/
	void setTexCoordData( const string& meshId,
		const Vector2Array& texCoordData, int texStage = 0 );

	/**
	* Sets positions of all vertices of the specified mesh.
	* It is asserted that the mesh exists and that the number of vertices in
	* the mesh is equal to the number of elements in vertexData.
	*
	* @param meshId Identifier of the mesh.
	* @param vertexData Vertices' coordinates of the specified mesh.
	*/
	void setVertexData( const string& meshId,
		const Vector3Array& vertexData );

	/**
	* Sets position of the vertex at the specified index in the specified mesh.
	* It is asserted that the mesh exists and that index >= 0 and index <= total
	* number of vertices.
	*
	* @param id Identifier of the mesh.
	* @param index Vertex index in the specified mesh.
	* @param position Vertex position in the specified mesh.
	*/
	virtual void setVertexPosition( const string& id, int index,
		const Vector3& position );

	/**
	* Deletes the specified morph target from the specified mesh. It is asserted that
	* the mesh and morph target exist. Also it is asserted that morph target identifier
	* corresponds to the MPEG-4 Face and Body Animation parameter name.
	*
	* @param meshId Identifier of the mesh.
	* @param morphTargetId Identifier of the morph target. It must correspond to the MPEG-4
	* Face and Body Animation parameter name.
	*/
	virtual void deleteMorphTarget( const string& meshId, const string& morphTargetId );

	/**
	* Recalculates normals of meshes that comprise the character model.
	* This method is called, for example, by PhotoModel after it has deformed the face meshes to
	* match the face in the 2D photograph.
	*/
	void recalculateNormals();

	/**
	* Gets position offset of the specified vertex in the specified morph target of the specified
	* mesh. It is asserted that the morph target, mesh and vertex exist. Also, it
	* is asserted that morph target identifier corresponds to the MPEG-4 Face and Body Animation
	* parameter name.
	*
	* @param meshId Identifier of the mesh.
	* @param morphTargetId Identifier of the morph target. It must correspond to the MPEG-4
	* Face and Body Animation parameter name.
	* @param vertexIndex Index of the mesh vertex.
	* @return position offset of the specified mesh vertex in the specified morph target.
	*/
	virtual Vector3 getMorphTargetVertexOffset( const string& meshId, const string& morphTargetId,
		int vertexIndex ) const;

	/**
	* Gets neutral pose transformation values for the specified bone. It is asserted that
	* the bone exists. Neutral pose position is always equal to (0, 0, 0), so this method
	* always returns initial bone position. If bone orientation isn't defined, then this
	* method returns model initial pose transformation values.
	*
	* @param boneId Bone id.
	* @param position Initial pose position for the specified bone because neutral pose
	* position is always equal to (0, 0, 0).
	* @param orientation Neutral pose orientation for the bone.
	*/
	virtual void getNeutralPoseTransformations( const string& boneId, Vector3& position,
		Quat& orientation ) const;

	/**
	* Sets neutral pose transformation values for the specified bone or node.
	* It is asserted that the bone or node exists. AFM doesn't support neutral
	* pose positions, it supports only neutral pose orientations. So it is
	* asserted that position vector is equal to (0, 0, 0).
	*
	* @param boneId Identifier of the bone.
	* @param position Bone's neutral pose position. It must be equal to (0, 0, 0).
	* @param orientation Bone's neutral pose orientation.
	*/
	virtual void setNeutralPoseTransformations( const string& boneId,
		const Vector3& position, const Quat& orientation );

	string getNode( int index ) const;
	int getNodeIndex( const string& id ) const;
	string getMesh( int index ) const;
	int getMeshIndex( const string& id ) const;
    
    



protected:

	void _updateGeometry() {}

private:
	bool init();
//	void outputAnimData(FILE *f);
//	void outputTransfData(FILE *f);
	void interpolateInnerOuterLipsFap(FAPs *faps, int ilf, int olf);
//	void readInterpolator(Node *node,int **key,int *nKey,float **value,int *nValue);
	void initAnimData(VRMLModel **m, int *fv);
//	void traverse(Node *_currentRootNode, int traverseType, char *iid);
	void computeSensitivityThreshold();
	VRMLModelModifier *getVRMLModelModifier(const string &name) const;
	FBAPMapping getMapping(const string &name, VRMLModelModifier *modifier) const;
	char *getRootTransformName(VRMLNode *node) const;


	VRMLModel *faceModel;
	float coordSensitivity;
	float sensitivityThreshold;


	int nFIO; // number of FileIO objects attached
	FileIO * FIO[100];

	int nDeformers; // number of AFMDeformer objects attached
	AFMDeformer * deformers[100];
};

}

#endif

