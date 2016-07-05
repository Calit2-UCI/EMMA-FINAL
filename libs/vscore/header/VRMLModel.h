#ifndef __VRMLMODEL_H__
#define __VRMLMODEL_H__


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


#include "stdlib.h"
#include "stdio.h"
#include "myMatrix.h"
#include "FileIO.h"

//class Node;

// node types
#define NT_ROOT 0       /**< VRML node type root. @see VRMLNode::type*/
#define NT_GROUP 1      /**< VRML node type group. @see VRMLNode::type */
#define NT_SHAPE 2      /**< VRML node type shape. @see VRMLNode::type */
#define NT_TRANSFORM 3  /**< VRML node type transform. @see VRMLNode::type */
#define NT_OTHER 4      /**< All other VRML node types. @see VRMLNode::type */

class Node;


namespace VisageSDK
{

class FAPs;

// Changed by KStankovic
/**
*  Container class for the VRML Material node. Includes the VRML material properties.
*  When standard texture is being used the textureName attribute contains the URL of the texture
*  image and every element of the array cubemapNames is equal to NULL. In case of using
*  Cubemap texture the textureName attribute is equal to "" (empty string) and the cubemapNames
*  attribute contains 6 URLs of texture images.
*
* Implemented in visagesdkAFM.lib
*/
//
class VISAGE_DECLSPEC VRMLMaterial {
public:
	VRMLMaterial(); /**< Constructor. */
	~VRMLMaterial(); /**< Destructor. */
	
	/**
	* Constructor. Constructs a new material by copying the given one.
	* @param m material to copy.
	*/
	VRMLMaterial(VRMLMaterial *m);  

	float ambientIntensity; /**< Ambient light intensity. */
	float diffuseColor[3]; /**< Diffuse color. */
	float shininess; /**< Shininess. */
	float transparency; /**< Transparency. */
	float specularColor[3]; /**< Specular color. */
	float emissiveColor[3]; /**< Emissive color. */
	char textureName[200]; /**< Texture file name. */
	// Added by KStankovic
	char *cubemapNames[6]; /**< Cubemap texture file names. The order of the image URLs needs to correspond to the following list of the CubeMap texture image targets:
									<OL>
										<LI>GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT</LI>
										<LI>GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT</LI>
										<LI>GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT</LI>
										<LI>GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT</LI>
										<LI>GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT</LI>
										<LI>GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT</LI>
									</OL>
							*/
	//
};

/**
*  Container class for morph target list for one polygon mesh.
*
* VRMLMeshMT can hold up to 100 morph targets for a polygon mesh. Each morph target is expressed
* as a list of vertex coordinate displacements, and the corresponding weight; the number of coordinates in the list must be
* the same as the number of vertex coordinates in the mesh to which the morph targets are applied.
*
* The morph targets are applied to the mesh by multiplying the coordinates of each morph target
* by its weight factor, and then adding the coordinates to the coordinates of the mesh. See the Definitions section in the <a href="AFM File Format Reference.pdf">%AFM File Format Reference</a>
* for a better understanding of the morphing concepts as applied in visage|SDK.
*
* Implemented in visagesdkAFM.lib
*/
class VISAGE_DECLSPEC VRMLMeshMT {
public:
	VRMLMeshMT(){nMTs = 0;changed=0;}; /**< Constructor. */
	~VRMLMeshMT(){}; /**< Destructor. */
	
	int nMTs; /**< The number of morph targets in the list. */
	float *vertexDisplacement[100]; /**< A list of vertex displacements for each morph target. */
	float weight[100]; /**< A list of vertex displacements for each morph target. */
	bool changed; /**< This flag is set to indicate that the morph target data has been changed and needs to be applied to the model. When the morph targets are applied using VRMLModel::applyMorphing(), the flag is set to false. If the flag is false, the applyMorphing() method does nothing. This mechanism allows to call applyMorphing() asynchronously from setting the morph targets. This is important because applyMorphing() is used by the renderer, which may run in a separate thread from the deformer that sets the morph targets. Therefore applyMorphing() may be called several times between two settings of the morph targets, and this mechanism makes sure that the morph target application is performed only once. When morph targets are set, this flag is set to true, so morphing will be applied next time applyMorphing() is called.*/
};

/**
*  Container class for the bones list for one polygon mesh.
*
* VRMLMeshBones can hold up to 100 bones for a polygon mesh. Each bone is expressed as the index to transform
* in the VRMLModel (the bone),
* a list of vertex weights and the bind pose. See the Definitions section in the <a href="AFM File Format Reference.pdf">%AFM File Format Reference</a>
* for a better understanding of the skinning concepts as applied in visage|SDK.
*
*
* Implemented in visagesdkAFM.lib
*/
class VISAGE_DECLSPEC VRMLMeshBones {
public:
	// Changed by KStankovic
	VRMLMeshBones(){nBones = 0; changed = 0; for (int i = 0; i < 100; i++) boneIndex[i] = 0;}; /**< Constructor. */
	~VRMLMeshBones(); /**< Destructor. */
	//
	
	int boneIndex[100]; /**< A list of bones that influence this mesh, each bone specified by the index of the transform node in the VRMLModel. */
	int nBones; /**< The number of bones in the list. */
	float *vertexWeight[100]; /**< A list of vertex weights for each bone. The size of each list is the number of vertices in the mesh (coordListSize/3) */
	float *bindCoords[100]; /**< A list of bind pose vertex coordinates for each bone. The size of each list is the number of vertex coordinates in the mesh (coordListSize). The bind coordinates are the vertex coordinates of the mesh, transformed into the local coordinate system of the bone in the initial position of the body (the binding pose). In other words, if these coordinates are multiplied by the bone's transformation matrix (the one that transforms the bone's local coordinates to world coordinates), and the bone is in the initial position, then the certices will fall exactly into their initial positions in the global coordinates. When the bone is moved, if the same transformation is used, the vertices stay "attached" to this bone. In the skinning process, these global coordinates are then multiplied by the bone's weight, the process is repeated for all bones and all global coordinate are summed up. The result is the averaged influence of all bones on the vertices. It is important to note that the binding pose may be different from the neutral body position. In fact, the typical binding pose is the "T shape" with arms stretched out horizontaly, and the neutral body position (the position with MPEG-4 body animation parameters set to 0) is with arms hanging down parallel to the body.*/
	bool changed; /**< This flag is set to indicate that the skinning-related data has been changed (typically, a bone has moved), so skinning needs to be applied to the model. When skinning is applied using VRMLModel::applySkinning() function, this function sets the flag is set to false after it applies skinning. If the flag is false, the next call to applySkinning() function does nothing. This mechanism allows to call applySkinning() asynchronously from moving the bones. This is important because applySkinning() is used by the renderer, which may run in a separate thread from the deformer that moves the bones. Therefore applySkinning() may be called several times between two settings of the bones, and this mechanism makes sure that the skinning is performed only once, when needed. When the bone position is changed, the flag is set to true, so the skinning will be applied next time applySkinning() is called.*/
};

/**
*  A 3D model scene graph node. Contains main data of a node in a 3D model (VRMLModel). Its main purpose is to build
* the node hierarchy (scene graph) using parent-child relationship, and allow scene traversal.
* it provides indices (ind, matInd members) that can be used to access the actual data of the
* node within the VRMLModel class. 
* 
* However, most of the time it is easier to use VRMLTraverser
* to traverse the whole model and obtain all necessary data.
*
* If the file format from which the 3D model is read is extended with some custom nodes using the PROTO
* definitions, the VRMLNodes of type NT_OTHER will contain (in the parserNode attribute) a pointer to the original node in the VRML parser.
* This can be used to access additional data from such custom nodes.
*
* Implemented in visagesdkAFM.lib
*/
class VISAGE_DECLSPEC VRMLNode {
public:
	/**
	* Constructor. Constructs a new VRMLNode of the given type and with the given indices.
	* If the node is of type NT_OTHER, a pointer to the node in the VRML parser is passed in _parserNode; for other node types, _parserNode is NULL.
	* @param _type type of the node. Copied to type.
	* @param _ind index to the polygon mesh data (coordList, normList, tCoordList etc.) or transform data (transfList) within VRMLModel. Copied to ind.
	* @param _matInd index to the material within VRMLModel. Copied to matInd.
	* @param _parserNode the pointer to the node in the VRML parser.
	* @param _name the name of the node.
	* @param _parent the parent of the node.
	*/
	VRMLNode(int _type, int _ind, int _matInd,Node *_parserNode,char *_name,VRMLNode *_parent);

	/**
	* Destructor.
	*/
	~VRMLNode();

	/**
	* Adds a child to the node.
	* @param n the node to add as a child to this node.
	*/
	void addChild(VRMLNode *n);

	/**
	* Clone a VRMLNode and its children.
	* Clone this node, i.e. make an exact copy of it. This is recursive, so the whole hierarchy under the node is cloned.
	* @return The clone of this node.
	*/
	VRMLNode *clone();

	/**
	* Inserts a transform into the scene graph at the node which corresponds to the transform t in the VRMLModel (index t in transfLists).
	* All indices greater than t are updated (increased by one) so that the node data still corresponds with the VRMLModel.
	* To do this, the whole hierarchy is traversed starting with this node - it is assumed that this function will be called on the root node of the model.
	* @param t index of the transform in the VRMLModel at which a new transform is to be inserted.
	* @return The function returns 1 when called on the node which corresponds to the transform t in the VRMLModel; 0 is returned otherwise.
	*/
	int insertTransform(int t);

	/**
	* Recursively updates the toWC and fromWC matrices (to-world-coordinates and from-world-coordinates) of all nodes below this node.
	* Typically it is called on the root node of a VRML model. It will traverse the whole hierarchy and
	* update the toWC and fromWC matrices of all transform nodes. These matrices are used
	* to transform between local coordinates at the level of a particular transform and world coordinates. fromWC is the inverse of toWC.
	* The toWC matrix is obtained by concatenating (multiplying) all transform matrices
	* from the top of the model hierarchy downwards.
	* @param _model the VRMLModel on which the operation is performed
	* @param _toWC the toWC matrix at the current level of recursion. It is set to an identity matrix when the function is called on the root node of a model.
	*/
	void updateToWCMatrix(void *_model, myMatrix _toWC);

	void updateNodeToWCMatrix(VRMLModel *model, myMatrix toWC);

	// return relative matrix without non-uniform scaling
	void getRelativeMatrix(VRMLModel *model,myMatrix r);

	/**
	* Set node name.
	* @param _name the name to set to the node.
	*/
	void setName(char *_name);

	/**
	* Get node name.
	* @return the name of the node.
	*/
	char* getName();

	int type;               /**< Node type. Type of VRML node. The value can be NT_ROOT (for root node), NT_GROUP (for a group node), NT_SHAPE (for an polygon mesh node), NT_TRANSFORM (for a transform node), NT_OTHER (for all other nodes)*/
	int ind;                /**< Index. Index to the polygon mesh data (coordList, normList, tCoordList etc.) or transform data (transfList) within VRMLModel. This index allows to access the actual data od polygon mesh and transform nodes within a VRML model by accessing the arrays in the VRMLModel class. */
	int matInd;             /**< Material index. Index to the material within VRMLModel. */
	VRMLNode **children;    /**< Children. Pointers to children nodes of this node.*/
	int nChildren;          /**< Number of children. Number of children of this node. */
	VRMLNode *parent;       /**< Parent node. The parent node of this node (NULL for the root node). */
	Node *parserNode;         /**< Original node in VRML parser. It is used only for nodes of type NT_OTHER. */

private:

	char *name;             /**< Name the node. Taken from the DEF statement in the VRML file, if it exists; otherwise it is NULL or (in case of indexed face sets and transforms) set to an automatically asigned unique name. */
};


/**
* Neutral pose data for a character.
* The NeutralPose node is used to set the neutral pose of the character, i.e. the neutral position of the face and body as defined by the MPEG-4 FBA standard.
* The character is usually modelled and stored in the file in a pose different from the standard MPEG-4 neutral pose (due to skinning requirements and possibly other reasons).
* The neutral pose data is used to set the model to the neutral pose, from which it is animated.
*/
class VISAGE_DECLSPEC NeutralPoseData  
{
public:
	NeutralPoseData(){initialFacialPose=NULL; NPRotData = NULL;};
	virtual ~NeutralPoseData(){};

	FAPs *initialFacialPose;          /**<  Initial facial pose of the model, in case it is different from the MPEG-4 neutral face and body pose. It contains the FAPs that put the face and body in the initial pose. If the initial pose corresponds to the MPEG-4 neutral position, this is NULL. For the  description of the standard MPEG-4 neutral pose, see the <a href="../MPEG-4 FBA Overview.pdf">MPEG-4 Face and Body Animation Introduction</a>.*/


	/**
	* Neutral pose rotation data.
	* The data describing the rotation for each transform in the VRMLModel that puts it in the neutral pose.
	*
	* If NPRotData is NULL, it means that the model is already in neutral pose and no transforms need to be changed.
	* Otherwise, the first pointer must be allocated to the number
	* of transforms in the VRMLModel (VRMLModel::nTransfLists). If we assume
	* that the number of transforms is nt, then the allocation may be done as follows:
	*
	* NPRotData = (float**) malloc(nt * sizeof(float*));
	*
	* Each NPRotData[i] is either NULL or a pointer to the rotation data for the transform in the VRMLModel
	* (VRMLModel::transfList[i]). If NPRotData[i] is NULL, it means that this particular transform is already in neutral pose and does not need to be rotated.
	* Otherwise, NPRotData[i] must be allocated to the size of 4 float values.
	* NPRotData[i] is then filled with rotation coordinates, axis and angle, that describe the neutral pose of the transform i. The first three
	* coordinates are the axis, the fourth one is the angle in radians.
	*
	*/
	float **NPRotData;

};


/**
*  A universal 3D model (VRMLModel class) traverser. 
* VRMLTraverser is the standard and convenient way to access the VRMLModel geometry and other internal
* information necessary
* for rendering, deforming, exporting, writing to files and any other manipulation of 3D models.
*
* A sub-class has to be implemented for each specific type of traversal.
* VRMLWriteTraverser is an example of such specific traverser. Other traversers can be built for
* rendering, writing data in other formats, modifying the model etc.
*
* The traversal is initiated by calling the VRMLModel::traverse() method.The model is traversed from the root node downwards. As nodes are
* traversed, specific callback functions are called for each type of node, and data is passed to those callback functions
* The callback functions have to be implemented in a sub-class of
* VRMLTraverser. The implementations in the VRMLTraverser class are empty, i.e. they do nothing. The
* specific implementation needs only to implement those callback functions that it is interested in. For example,
* a VRMLTraverser that only needs to access the vertex coordinates in the polygon meshes of the model would
* only implement the traverseMesh function.
*
* All callback functions are passed the pointer to the VRMLModel and an
* index. The index is type-dependent. For transforms, it is the index
* of the current transform within the model; for polygon meshes, it
* is the index of the current polygon mesh within the model; for other types
* it is currently unused.
*
* Implemented in visagesdkAFM.lib
*
* To illustrate how the VRMLTraverser mechanism works, the full source code of the VRMLWriteTraverser is provided in the src folder.
* @see VRMLModel
* @see VRMLModel::traverse()
* @see VRMLWriteTraverser
*/
class VISAGE_DECLSPEC VRMLTraverser {
public:
	VRMLTraverser();
	~VRMLTraverser();

	/**
	* Root node traverse callback.
	* This function is called by the traverser when the root node is encountered, i.e. before it is traversed.
	* @param model the VRMLModel being traversed
	* @param thisNode the VRMLNode currently being traversed
	* @param index not used.
	*/
	virtual void traverseRoot(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
							 int index){};

	/**
	* Root node end-of-traverse callback.
	* This function is called by the traverser after the root node is traversed.
	* @param model the VRMLModel being traversed
	* @param thisNode the VRMLNode currently being traversed
	* @param index not used.
	*/
	virtual void traverseRootEnd(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
							 int index){};


	/**
	* Transform node traverse callback.
	* This function is called by the traverser when a Transform node is encountered, i.e. before it is traversed.
	* @param model the VRMLModel being traversed
	* @param thisNode the VRMLNode currently being traversed
	* @param index index to the transform node within the VRMLModel (transfList).
	* @param c center of rotation expressed as translation (x,y,z)
	* @param negc negative translation of the center of rotation
	* @param r rotation expressed as axis and angle in radians (x,y,z,angle)
	* @param rq rotation expressed as quaternion (x,y,z,w)
	* @param s scale (x,y,z)
	* @param so scale orientation (axis and angle)
	* @param soq scale orientation (quaternion)
	* @param negso inverse of scale orientation (axis and angle)
	* @param negsoq inverse of scale orientation (quaternion)
	* @param t translation (x,y,z)
	* @param m complete transformation matrix
	*
	* @return If traverseTransform returns 0, no change will be made to the main model. If it returns 1, transform will be updated with values in t,rq, and s
	*/
	virtual int traverseTransform(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
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
							 myMatrix m){return 0;};  // Complete transformation matrix
	
	/**
	* Transform node end-of-traverse callback.
	* This function is called by the traverser after the Transform node is traversed.
	* @param model the VRMLModel being traversed
	* @param thisNode the VRMLNode currently being traversed
	* @param index index to the transform node within the VRMLModel (transfList).
	*/
	virtual void traverseTransformEnd(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
							 int index){};

	/**
	* Group node traverse callback.
	* This function is called by the traverser when a Group node is encountered, i.e. before it is traversed.
	* @param model the VRMLModel being traversed
	* @param thisNode the VRMLNode currently being traversed
	* @param index not used.
	*/
	virtual void traverseGroup(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
							 int index){};

	/**
	* Group node end-of-traverse callback.
	* This function is called by the traverser after the Group node is traversed.
	* @param model the VRMLModel being traversed
	* @param thisNode the VRMLNode currently being traversed
	* @param index not used.
	*/
	virtual void traverseGroupEnd(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
							 int index){};

	/**
	* Shape node (polygon mesh) traverse callback.
	* This function is called by the traverser when a Shape node is encountered, i.e. before it is traversed.
	* @param model the VRMLModel being traversed
	* @param index index to the polygon mesh data (coordList, normList, tCoordList etc.).
	* @param mat material for this shape
	* @param coord vertex coordinates
	* @param coordLength length of the vertex coordinates array (number of vertices * 3)
	* @param coordInd vertex indices for triangles
	* @param coordIndLength length of coordInd array
	* @param norm normals
	* @param normLength length of norm array
	* @param normInd normal indices, per vertex
	* @param normIndLength length of normInd array
	* @param NPV Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
	* @param normalsCalculated if 1, normals are calculated by VRMLModel; if 0, normals were read from the file
	* @param creaseAngle VRML creaseAngle (default 0.0; 1.2 is good value to smooth)
	* @param solid VRML solid (default 1)
	* @param ccw VRML counter-clockwise (default 1)
	* @param convex VRML convex (default 1)
	* @param tcoord texture coordinates, if any (else zero)
	* @param tcoordLength length of the tcoord array  
	* @param tcoordInd texture coordinate indices 
	* @param tcoordIndLength length of the tcoordInd array 
	* @param cpv colors per vertex, if any (else zero)
	* @param cpvLength length of the cpv array 
	* @param cpvInd indices to colors per vertex, if any (else zero) 
	* @param cpvIndLength length of the cpvInd array 
	* @param CPV color per vertex (1) or per face (0)
	virtual void traverseShape(
		void *model,         // VRMLModel pointer to the face model
		int index,           // index of the polygon mesh within the face model
		VRMLMaterial *mat,   // material for this shape
		float *coord,        // vertex coordinates
		int coordLength,     // length of the vertex coordinates array (number of vertices * 3)
		int *coordInd,       // vertex indices for triangles
		int coordIndLength,  // length of coordInd array
		float *norm,         // normals
		int normLength,      // length of norm array
		int *normInd,        // normal indices, per vertex
		int normIndLength,   // length of normInd array
		int NPV,              // Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
		int normalsCalculated,// If 1, normals are calculated by VRMLModel; if 0, normals were read from the file
		float creaseAngle,   // VRML creaseAngle (default 0.0; 1.2 is good value to smooth)
		int solid,           // VRML solid (default 1)
		int ccw,             // VRML counter-clockwise (default 1)
		int convex,          // VRML convex (default 1)
		float *tcoord,       // texture coordinates, if any (else zero)
		int tcoordLength, 
		int *tcoordInd, 
		int tcoordIndLength, 
		float *cpv,          // colors per vertex, if any (else zero)
		int cpvLength, 
		int *cpvInd, 
		int cpvIndLength,
		int CPV){};            // color per vertex (1) or per face (1)
*/
	/**
	* Shape node (polygon mesh) traverse callback.
	* This function is called by the traverser when a Shape node is encountered, i.e. before it is traversed.
	* The Shape node contains a polygon mesh and the data passed to this method reffers to that mesh.
	*
	* The implementation of this function may change the data, i.e. change the vertex coordinates in order to deform
	* the model.
	*
	* 
	* @param model the VRMLModel being traversed
	* @param thisNode the VRMLNode currently being traversed
	* @param index index to the polygon mesh data (coordList, normList, tCoordList etc.).
	* @param mat material for this shape
	* @param coord vertex coordinates
	* @param origCoord original vertex coordinates (these should not be changed; they can be used to reset the model, or as a reference for deformation)
	* @param coordLength length of the vertex coordinates array (number of vertices * 3)
	* @param coordInd vertex indices for triangles
	* @param coordIndLength length of coordInd array
	* @param norm normals
	* @param normLength length of norm array
	* @param normInd normal indices, per vertex
	* @param normIndLength length of normInd array
	* @param NPV Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
	* @param normalsCalculated if 1, normals are calculated by VRMLModel; if 0, normals were read from the file
	* @param creaseAngle VRML creaseAngle (default 0.0; 1.2 is good value to smooth)
	* @param solid VRML solid (default 1)
	* @param ccw VRML counter-clockwise (default 1)
	* @param convex VRML convex (default 1)
	* @param tcoord texture coordinates, if any (else zero)
	* @param tcoordLength length of the tcoord array  
	* @param tcoordInd texture coordinate indices 
	* @param tcoordIndLength length of the tcoordInd array 
	* @param cpv colors per vertex, if any (else zero)
	* @param cpvLength length of the cpv array 
	* @param cpvInd indices to colors per vertex, if any (else zero) 
	* @param cpvIndLength length of the cpvInd array 
	* @param CPV color per vertex (1) or per face (0)
	* @param mt pointer to the list of morph targets for this mesh
	* @param bones pointer to the list of bones for this mesh
	*/
	virtual void traverseMesh(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
							 int index,           // index of the polygon mesh within the face model
							 VRMLMaterial *mat,   // material for this shape
							 float *coord,        // vertex coordinates
							 float *origCoord,    // original vertex coordinates
							 int coordLength,     // length of the vertex coordinates array (number of vertices * 3)
							 int *coordInd,       // vertex indices for triangles
							 int coordIndLength,  // length of coordInd array
							 float *norm,         // normals
							 int normLength,      // length of norm array
							 int *normInd,        // normal indices, per vertex
							 int normIndLength,   // length of normInd array
							 int NPV,              // Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
							 int normalsCalculated,// If 1, normals are calculated by VRMLModel; if 0, normals were read from the file
							 float creaseAngle,   // VRML creaseAngle (default 0.0; 1.2 is good value to smooth)
							 int solid,           // VRML solid (default 1)
							 int ccw,             // VRML counter-clockwise (default 1)
							 int convex,          // VRML convex (default 1)
							 float *tcoord,       // texture coordinates, if any (else zero)
							 int tcoordLength, 
							 int *tcoordInd, 
							 int tcoordIndLength, 
							 float *cpv,          // colors per vertex, if any (else zero)
							 int cpvLength, 
							 int *cpvInd, 
							 int cpvIndLength,
							 int CPV,           // color per vertex (1) or per face (1)
							 VRMLMeshMT *mt,		// pointer to the list of morph targets for this mesh
							 VRMLMeshBones *bones		// pointer to the list of bones for this mesh
							 ){};            

	/**
	* Traverse callback for other node types.
	* This function is called by the traverser when a node is encountered that is of a type not covered by other callback functions.
	*
	* This is convenient to access the data from any custom nodes implemented as an extension to the %AFM file format. The VRMLNode
	* will contain a pointer to such a custom node in the VRML parser, so the application can retrieve the necessary data contained
	* in the node's fields. Node's fields will be ordered in the same way as they were declared in the PROTO that defines the node type. Value of a certain field
	* can be accessed by calling the method \link Node::getField(int index) const Node::getField(int index)\endlink. Index of the certain field can be obtained
	* by calling Node::getProto()->\link Proto::lookupField(const String &name) const lookupField(const String &name)\endlink,
	* where \a name is the name of the field. For example, value of the field refValue in the AnimationParameter node can be accessed by calling
	* <b>node->getField(node->getProto()->lookupField("refValue"))</b>, where \a node is a pointer to the AnimationParameter node, or by calling
	* <b>node->getField(1)</b>, because 1 is its index according to the field order in the PROTO declaration. This method will return a pointer to the FieldValue
	* object, so to get the actual data the following must be done:
	* -# SFFloat *refValue = (SFFloat *) fieldValue;
	* -# float value = refValue->getValue();
	*
	* where \a fieldValue is a pointer to the FieldValue object, and SFFloat is the type of the field \a refValue. For every field type (SFBool, SFFloat, SFInt32,
	* MFFloat, MFString, ...) there is a class defined in the VRML parser.
	*
	* @param model the VRMLModel being traversed
	* @param thisNode the VRMLNode currently being traversed
	* @param index not used.
	*/
	virtual void traverseOther(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
							 int index){};
};

// Changed by KStankovic
/**
* General 3D model with interfaces for file reading/writing, rendering and animation. 
* General 3D model, including scene graph, geometry, materials, as well as morphing and skinning data.
* The data structure is inspired by VRML97 file format, but extended for morphing, skinning and Cubemap textures.
* Although inspired by VRML97, the structure can be mapped to any file format for reading and writing using the
* general @ref FileIO interface. VRML97 file reading and writing is currently supported (@ref BaseFileIO class).
* 
* The data structure is a scene graph populated by Transforms and IndexedFaceSets (polygon meshes) with textures and materials.
* These are the necessary elements to build a virtual character model.
*
* Although it is possible to access all the data of the 3D model directly through the members
* of this class, it is recommended to use the @ref VRMLTraverser interface for more convenient access. The following discussion of the data 
* structure is intended for advanced users, particularly those implementing new FileIO classes for supporting new file
* formats.
*
* The geometry and material data is stored in a number of arrays which are all public attributes and fully documented.
* For example, vertex coordinates for polygon meshes are stored in a double array @ref coordList, containing one float
* array for each polygon mesh. The array @ref coordListSize gives the size of each of these arrays, and
* @ref nCoordLists gives the number of arrays, i.e. the number of polygon meshes in the model. All other geometry
* and material data (normals, texture coordinates, transforms with their translation, rotation, scale values, etc.)
* are stored in arrays in a similar way.
*
* The scene graph hierarchy is given by a tree of nodes of type VRMLNode, starting with @ref rootNode. Each VRMLNode
* provides indices to the data arrays, allowing to access polygon mesh and transform data.
*
* The convenient way to access all data in the VRMLModel is to extend the class @ref VRMLTraverser. It allows
* both reading and modification of the model data. However, for the initial construction of a VRMLModel (which has to be done by
* FileIO::read() function), it is obviously not yet possible to use the VRMLTraverser - before construction there is
* nothing to traverse. So, the implementations of new FileIO classes (custom file format readers and similar) have to
* access the VRMLModel data structures directly. The documentation of each attribute of the VRMLModel specifies
* how the attribute should be allocated and set by the FileIO implementation.
*
* Implemented in visagesdkAFM.lib
*/
//
class VISAGE_DECLSPEC VRMLModel : public VRMLTraverser
{
public:
/**
* Constructor. Constructs an empty VRML model.
*/
	VRMLModel();

/**
* Constructor. Constructs a new VRML model by copying m.
* @param m model to be copied.
*/
	VRMLModel(VRMLModel *m);

/**
* Constructor. Constructs a new VRML model by reading it from a file. Currently only the VRML97 file format
* is supported by default. Other file formats can be supported by implementing custom FileIO classes.
* @param fn name of the file to be read.
*/
	VRMLModel(char *fn);
/**
* Destructor.
*/
	~VRMLModel();

/**
* Copies the 3D model m onto this model.
* @param m model to be copied.
*/
	void copy(VRMLModel *m);

/**
* File reader. Builds the model by reading it from a file.
* @param fn name of the file to be read.
* @return The function returns 0 if file could not be opened, otherwise it returns 1.
*/
	int readFromFile(char *fn);

/**
* Recalculates normals of meshes that comprise the character model.
* This method is called, for example, after PhotoModel has deformed the face meshes to
* match the face in the 2D photograph.
*/
	void recalculateNormals();

/**
* File writer. Writes the model to a file.
* @param fn name of the file to be written. New file is created, or existing file overwritten.
* @return The function returns 0 if file could not be opened, otherwise it returns 1.
*/
	int writeToFile(char *fn);

/**
* Universal model traversal. 
* @param node VRMLNode from which to start the traversal. Typically this would be the model's own rootNode.
* @param traverser a VRMLTraverser that will be used in this traversal. Typically, a subclass of VRMLTraverser is implemented for a particular traversal function. The callback functions of the traverser are called as the model is traversed and particular nodes encountered, and all important model data is passed to those functions. The functionality of the traversal is implemented in those functions.
* @see rootNode
* @see VRMLTraverser
* @see VRMLWriteTraverser
*/
	void traverse(VRMLNode *node, VRMLTraverser *traverser);

/**
* Apply morphing. Apply the morph targets contained in VRMLModel::morphTarget to the polygon meshes of the model.
* If both morphing and skinning are applied, skinning must be applied first.
*
* @see morphTarget, applySkinning()
*/
	void applyMorphing();

/**
* Apply skinning. Apply akinning based on bones data contained in VRMLModel::boneData to the polygon meshes of the model.
* If both morphing and skinning are applied, skinning must be applied first.
*
* @see morphTarget, applyMorphing();
*/
	void applySkinning();

/**
* Find the transform by name. Find the transform by name, as defined by the DEF
* statement in the model file (e.g. DEF some_name Transform...).
*
* @param name the name of the transform to find.
* @return the index of the found transform, or -1 if not found.
*/
	int findNamedTransform(char *name);

/**
* Find a polygon mesh by name. Find the mesh by name, as defined by the DEF
* statement in the model file (e.g. geometry DEF some_name IndexedFaceSet...).
*
* @param name the name of the mesh to find.
* @return the index of the found mesh, or -1 if not found.
*/
	int findNamedMesh(char *name);
/**
* Find the first mesh directly affected by the transform ti.
*/
	int findTransformMesh(int ti);

/**
* Add a FileIO file reader/writer class.
* Up to 100 instances of file reader-writer classes of type FileIO can be attached to the VRMLModel.
* Each may support reading and writing of one or more file formats. When VRMLModel needs to read or write
* to a file, it calls all attached FileIO objects until one of them succesfuly reads or writes the file.
* By default, a BaseFileIO object is attached to the VRMLModel, allowing to handle VRML97 files.
* @param fio an instance of FileIO to be added.
*
* @see addFileIO
* @see getFileIO
* @see getNFileIOs
* @see removeAllFileIOs
*/
	void addFileIO(FileIO *fio) {FIO[nFIO++] = fio;};
	
/**
* Get a file reader/writer attached to the VRMLModel.
* Get a pointer to one of the FileIO objects attached to the VRMLModel.
*
* @param n the number of FileIO to get; they are placed in order they were attached, i.e. for the first attached FileIO, n is 0, for the second one, n is 1 etc.
*
* @return if n is nonnegative and smaller than the total number of attached FileIOs, return the pointer to the requested FileIO object; otherwise, return NULL.
*
* @see addFileIO
* @see getFileIO
* @see getNFileIOs
* @see removeAllFileIOs
*/
	FileIO *getFileIO(int n) {if(n>=0 && n < nFIO) return(FIO[n]); else return (NULL);};
	
/**
* Get the number of file reader/writer objects attached to the VRMLModel.
*
* @return number of FileIO objects attached to the VRMLModel.
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
*
* @see addFileIO
* @see getFileIO
* @see getNFileIOs
* @see removeAllFileIOs
*/
	void removeAllFileIOs() {nFIO = 0;};

/**
* Finalize reading the model from file. 
* This method must be called only by implementations of FileIO::read() functions
* after they finish reading the file and setting all data of the VRMLModel. This method will then perform the
* final data initialization and make the VRMLModel ready for use (for example, automatic normal calculation is performed here).
* 
* @param fileName the file name from which the VRMLModel was read (full path, if available)
*/
	void finalizeFileReading(char *fileName);

/**
* A helper function to get the components of a transform from raw transform data.
* If VRMLTraverser is used to extract the data from a VRML model, this function
* will typically not be needed because the VRMLTraverser extracts the data appropriately.
*
* Transform data is written in the VRMLModel in a raw form (in the transfList array), an array of floats describing all the
* components of a transform in the same order as they are written in the Transform node in
* a VRML file. This function takes such raw parameters and returns all components of a Transform in
* form of matrices.
* @param crd raw transform data
* @param tr returns translation matrix
* @param r returns rotation matrix
* @param s returns scale matrix
* @param so returns scale orientation matrix
* @param negso returns the inverse of the scale orientation matrix
* @param c returns the center of rotation matrix, i.e. the matrix that translates the coordinate system to the center of rotation
* @param negc returns the inverse of the center of rotation matrix
*
* The complete transformation matrix for the transform node is obtained as T = negc * negso * s * so * r * c * tr
* It can also be obtained directly using the getTransfMatrix() function.
* @see VRMLTraverser
* @see getTransfMatrix
*/
	void getTransformComponents(float *crd,myMatrix tr,myMatrix r,myMatrix s,myMatrix so,myMatrix c,myMatrix negc,myMatrix negso);

/**
* A helper function to get the components of a transform from raw transform data.
* If VRMLTraverser is used to extract the data from a VRML model, this function
* will typically not be needed because the VRMLTraverser extracts the data appropriately.
*
* Transform data is written in the VRMLModel in a raw form (in the transfList array), an array of floats describing all the
* components of a transform in the same order as they are written in the Transform node in
* a VRML file. This function takes such raw parameters and returns all components of a Transform in
* form of translation vectors and quaternions or axis-and-angle notation (for rotations).
* The axis-and-angle notation consists of 4 floats; the first three are the axis, the last one is the angle, in radians.
* @param crd raw transform data
* @param t returns the translation vector
* @param r returns rotation in axis-and-angle notation
* @param rq returns rotation quaternion
* @param s returns scale vector
* @param so returns scale orientation in axis-and-angle notation
* @param soq returns scale orientation quaternion
* @param negso returns the inverse of the scale orientation in axis-and-angle notation
* @param negsoq returns the inverse of the scale orientation quaternion
* @param c returns the center of rotation vector
* @param negc returns the negative of the center of rotation vector
*
* @see VRMLTraverser
*/
	void getTransformComponents(float *crd,float *t,float *r,float *s,float *so,float *c,float *negc,float *negso,float *rq,float *soq,float *negsoq);

/**
* A helper function to get the transform matrix from raw transform data.
* If VRMLTraverser is used to extract the data from a VRML model, this function
* will typically not be needed because the VRMLTraverser extracts the data appropriately.
*
* Transform data is written in the VRMLModel in a raw form (in the transfList array), an array of floats describing all the
* components of a transform in the same order as they are written in the Transform node in
* a VRML file. This function takes such raw parameters and returns the transform matrix.
* @param crd raw transform data
* @param t returns the full transform matrix created from the raw transform data
* @see VRMLTraverser
*/
	void getTransfMatrix(myMatrix t, float* crd);

	/**
	* Updates all toWC and fromWC matrices in the model.
	* @see VRMLModel::toWC
	* @see VRMLModel::fromWC
	* @see updateToWCMatrix()
	*/
	void updateToWCMatrices();

	/**
	* Sets the creaseAngle parameter of the VRML model.
	* creaseAngle is the VRML parameter determining how normals are interpolated. If the angle between the neighboring
	* triangles is greater than the creaseAngle, the edge between them is interpreted as a sharp crease
	* and the normals are set accordingly. Otherwise, the model is smoothed. Set the creaseAngle to a large value, e.g. 3.14 to
	* obtain a smooth model. This change will take effect when the model is written to a file.
	* @param ca the creaseAngle parameter
	*/
	void setCreaseAngle(float ca) {for(int i=0;i<nCoordLists;i++) creaseAngle[i] = ca;};

	// Commented out by KStankovic
	/*
	* Optimises the VRML structure of the model.
	* It can happen that the loaded VRML model has a highly unoptimised structure, with multiple
	* repeated vertices (notably some older versions of 3ds max export such files). This function
	* will detect multiple vertices and remove them, i.e. put only one vertex instead to be shared by
	* neighboring poligons.
	* @return Not used.
	*/
	//int VRMLModel::optimiseVRML();
	//

	// Commented out by KStankovic
	/*
	* Optimises the VRML structure of the model.
	* It can happen that the loaded VRML model has a highly unoptimised structure, with multiple
	* repeated vertices (notably some older versions of 3ds max export such files). This function
	* will detect multiple vertices and remove them, i.e. put only one vertex instead to be shared by
	* neighboring poligons.
	*
	* @param newInd returns new vertex indices after optimising
	* @param erased returns indices of erased vertices
	* @return Not used.
	*/
	//int VRMLModel::optimiseVRML(int **newInd, int **erased);
	//


	VRMLNode *rootNode;        /**<  Root node. The root node of the VRML model. Typically it is used to start a traversal. FileIO::read() method implementation must instantiate the rootNode and create the full scene graph by adding more VRMLNode objects using addChild().*/
	int nCoordLists;	       /**<  Number of polygon meshes (indexed face sets). The number of polygon meshes in the model. The name nCoordLists is given because each polygon mesh is described by several coordinate lists. FileIO::read() method implementation must set this value. */
	VRMLNode *meshNode[200];   /**<  For each polygon mesh, a pointer to the VRMLNode corresponding to this mesh in the scene graph. FileIO::read() method implementation does not need to set these pointers. They are initialized automatically by the finalizeFileReading() method. */
	int coordListSize[200];    /**<  Size of coordinate list for each polygon mesh (number of vertices * 3). Size of each coordinate list as a number of floating point numbers storing the coordinates (number of vertices in the polygon mesh x 3). FileIO::read() method implementation must set these values. */
	float *coordList[200];     /**<  Coordinates. The vertex coordinates of each polygon mesh. FileIO::read() method implementation must allocate these arrays and set the values.  */
	float *origCoordList[200]; /**<  Original coordinates. The original vertex coordinates of each polygon mesh. Unlike @ref coordList, which changes during animation, origCoordList always keeps the original values. It is used to reset the model coordinates. FileIO::read() method implementation should not allocate these arrays, it is done after file reading by the @ref finalizeFileReading() method.  */
	// Changed by KStankovic
	int tCoordListSize[200];   /**<  Size of texture coordinate list for each polygon mesh. The size of each texture coordinate list (in case of standard textures it is equal to the number of texture vertices in the polygon mesh x 2, and for Cubemap textures it is equal to the number of texture vertices in the polygon mesh x 3). FileIO::read() method implementation must set these values (set them to 0 where no texture is used).  */
	float *tCoordList[200];    /**<  Texture coordinates. The texture coordinates of each polygon mesh. They can be 2D or 3D, depending on which type of texture is being used (standard or Cubemap texture). FileIO::read() method implementation must allocate the arrays and set the values for all textured polygon meshes; for non-textured ones it must set the array pointer to NULL.  */
	//
	int normListSize[200];     /**<  Size of normals list for each polygon mesh. The size of each normals list (number of normals x 3). If normals are read from file (see @ref generateNormals), FileIO::read() method implementation must set these values.  */
	float *normList[200];      /**<  Normals. The normals of each polygon mesh. If normals are read from file (see @ref generateNormals), FileIO::read() method implementation must allocate these arrays and set the values.  */
	int cpvListSize[200];      /**<  Size of color-per-vertex list for each polygon mesh. The size of each color-per-vertex list (number of vertex colors x 3). FileIO::read() method implementation must set these values (set them to 0 where no colors are used).  */
	float *cpvList[200];       /**<  Colors. Color-per-vertex data of each polygon mesh, as R, G, B values. FileIO::read() method implementation must allocate the arrays and set the values for all polygon meshes where colors are used; for the other ones ones it must set the array pointer to NULL.  */
	int triListSize[200];      /**<  Size of triangle list for each polygon mesh. The size of each triangle list (number of triangles x 3). FileIO::read() method implementation must set these values. */
	int *triList[200];         /**<  Triangle indices. The triangle vertex indices of each polygon mesh. The indices point into the corresponding coordList. Each triplet of indices in the list creates one triangle. FileIO::read() method implementation must allocate these arrays and set the values.  */
	int normIndListSize[200];  /**<  Size of normal index list for each polygon mesh. The size of each normal index list. If normals are read from file (see @ref generateNormals), FileIO::read() method implementation must set these values. */
	int *normIndList[200];     /**<  Normal indices. The normal indices of each polygon mesh. The indices point into the corresponding @ref normList. If the corresponding @ref NPV field is 1, there are three indices (i.e. three normals) for each triangle; if NPV is 0, there is one index (i.e. one normal) in this list for each triangle. If normals are read from file (see @ref generateNormals), FileIO::read() method implementation must allocate these arrays and set the values.  */

	int NPV[200];              /**<  Normal Per Vertex. Normal Per Vertex is 1 if normals are given for each vertex, 0 if normals are given for each face. This may be different for each polygon meshes, so there are as many NPV values as there are polygon meshes (the number is @ref nCoordLists). If normals are read from file (see @ref generateNormals), FileIO::read() method implementation must set these values. */
	int normalsCalculated[200];/**<  Indicates for each polygon mesh whether normals are calculated automatically, or read from the file. If the value is 1, normals are calculated automatically by VRMLModel after reading the file (i.e. after FileIO::read() returns) and in this case FileIO implementation does not need to set the normals or allocate the arrays for them; if it is 0, normals are read from the file, allocated and set */
	float creaseAngle[200];    /**<  VRML creaseAngle parameter for each polygon mesh. creaseAngle is the VRML parameter determining how normals are interpolated when they are calculated automatically. Default value is 0.0 and this gives sharp edges; 1.2 is good value to get a smooth surface. FileIO::read() method implementation may set the values if it is not setting the normals directly, i.e. if it wants VRMLModel to calculate the normals automatically (determined by @ref generateNormals). */
	int solid[200];            /**<  VRML solid parameter for each polygon mesh. The "solid" parameter from VRML (default is 1) for each polygon mesh. This is a hint for the renderer for how to handle backfacing polygons. FileIO::read() method implementation may set these values.*/
	int ccw[200];              /**<  VRML counter-clockwise parameter for each polygon mesh. The "counter-clockwise" parameter from VRML (default is 1) for each polygon mesh. This is a hint for the renderer for in which order to render vertices, which may affect backface culling. This value also affects automatic calculation of the normals. FileIO::read() method implementation may set these values.*/
	int convex[200];           /**<  VRML convex parameter for each polygon mesh. The "convex" parameter from VRML (default is 1) for each polygon mesh. This is a hint for the renderer. FileIO::read() method implementation may set these values.*/

	int texIndListSize[200];   /**<  Size of texture index list for each polygon mesh. The size of each texture index list (number of triangles x 3). FileIO::read() method implementation must set these values (set them to 0 where no texture is used).   */
	int *texIndList[200];      /**<  Texture indices. The texture indices of each polygon mesh. FileIO::read() method implementation must allocate the arrays and set the values for all textured polygon meshes; for non-textured ones it must set the array pointer to NULL.*/

	int cpvIndListSize[200];   /**<  Size of color index list for each polygon mesh. The size of each color index list. FileIO::read() method implementation must set these values (set them to 0 where no colors are used). */
	int *cpvIndList[200];      /**<  Color indices. The color indices of each polygon mesh. If the corresponding @ref CPV field is 1, there are three indices (i.e. three colors) for each triangle; if CPV is 0, there is one index (i.e. one color) in this list for each triangle. FileIO::read() method implementation must allocate the arrays and set the values for all polygon meshes where colors are used; for the other ones ones it must set the array pointer to NULL. */
	int CPV[200];              /**<  Color Per Vertex. Color Per Vertex is 1 if colors are given for each vertex, 0 if colors are given for each triangle. FileIO::read() method implementation must set these values if colors are used. */
	int transfInd[200];        /**<  Transform index for each polygon mesh. The transform index indicates to which transform (in @ref transfList) does this polygon mesh belong. FileIO::read() method implementation must set these values. */



	// Changed by KStankovic
	VRMLMaterial *materials[200];  /**< Materials. The list of VRML materials used in the model. Each material can have URL of the standard texture or six URLs of the Cubemap texture images. The matInd member of @ref VRMLNode points into this array of materials. FileIO::read() method implementation must fill this list. */
	//
	int nMaterials;               /**<  Number of materials. Number of materials in the model. FileIO::read() method implementation must set this number. */
//	VRMLMaterial *currentMaterial;/**<  Current material pointer. Used during the reading of the VRML file.*/


	int nTransfLists;	       /**<  Number of transforms. The number of transform nodes in the model. FileIO::read() method implementation must set this number.*/
	VRMLNode *transfNode[200]; /**<  For each transform, a pointer to the VRMLNode corresponding to this transform in the scene graph. FileIO::read() method implementation does not need to set these pointers. They are initialized automatically by the finalizeFileReading() method. */
	float *transfList[200];    /**<  Transform data. The raw transform data for each transform node. FileIO::read() method implementation must allocate and set these values. Each transform is described by a total of 17 float values: 3 for translation, 4 for rotation expressed as quaternion, 3 for scale, 4 for scale orientation expressed as quaternion, and 3 for the center of the transform node (the point around which rotation is applied). The data for the transform is inspired by the VRML syntax.*/


	myMatrix toWC[200];        /**<  To-world-coordinates matrix for each transform. The matrix to convert everything under transform from local coords to world coordinates. FileIO::read() method implementation must call the @ref updateToWCMatrices() method after the model is completely built in order to update these matrices (or it may update them directly).*/
	myMatrix fromWC[200];      /**<  From-world-coordinates matrix for each transform. The matrix to convert everything under transform from world coordinates to local coords. FileIO::read() method implementation must call the @ref updateToWCMatrices() method after the model is completely built in order to update these matrices (or it may update them directly).*/
	int transfLevel[200];      /**<  Transform level for each transform. The level of the transform node in the scene graph (root = 0, direct child of the root = 1, child of the child of the root = 2 etc.). FileIO::read() method implementation must set these values. */
	int initialized;           /**<  Initialised flag. This flag indicates whether the VRML model is completely initialised and ready to use. FileIO::read() method implementation must not set this flag; it will be set by VRMLModel after the read() function returns succesfully. */

	char filePath[400];		   /**<  Path of the last read file. The path (folder, directory) of the last read file;if only file name was given, this is an empty string. It is important to notice that texture files are often read only at the time of first rendering. Because the location of the texture files may be expressed as a path relative to the loaction of the model file, if we are in a wrong folder when trying to read the textures they may not be read. The filePath can be used to solve this kind of problem. FileIO::read() method implementation should not change this value. */	
	float globalTranslation[3];/**< global translation to be applied for the model. Before rendering, this will be applied using glTranslatef(). Default value is zero translation. FileIO::read() method implementation does not need to set this value. */
	float globalRotation[3];   /**< global rotation around axes x, y, z to be applied for the model. Before rendering, this will be applied using glRotatef(). Default value is zero rotation. FileIO::read() method implementation does not need to set this value. */
	float globalScale[3];      /**< global scale to be applied for the model. Before rendering, this will be applied using glScalef(). Default value is zero scaling. FileIO::read() method implementation does not need to set this value. */

	int locked;                /**<  Lock flag. This flag indicates whether the VRML model is currently locked. A model can be locked when it is being modified. This is used to prevent rendering the model in one thread at the same time when it is being modified by another thread, which would give unwanted artifacts.*/

	VRMLMeshMT morphTarget[200]; /**<  Morph target list for each polygon mesh. Each VRMLMeshMT object holds the morph target list for one polygon mesh.*/
	VRMLMeshBones boneData[200]; /**<  Bone list for each polygon mesh. Each VRMLMeshBones object holds the bones list for one polygon mesh.*/
	NeutralPoseData neutralPoseData; /**<  Neutral pose. Contains the data that can be used to transform a character model from the initial pose in which it has been modeled, into the MPEG-4 neutral pose.*/
private:
	void removeNonUniformScale();
	// Commented out by KStankovic
	//void VRMLModel::optimiseVRMLSurface(int *_coordListSize,float *_coordList,int *_triListSize,int *_triList, int chanSize,int **_newInd, int **_erased);
	//
	void generateNormals(float *normals, float *coords,int *ind,int nc,int ni,int _ccw);
	
/**
* Initialiser. Initialises a VRMLmodel, i.e. makes it an empty model.
*/
	void init();

	int traverseTransform(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
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
							 myMatrix m);
	void traverseMesh(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
							 int index,           // index of the polygon mesh within the face model
							 VRMLMaterial *mat,   // material for this shape
							 float *coord,        // vertex coordinates
							 float *origCoord,    // original vertex coordinates
							 int coordLength,     // length of the vertex coordinates array (number of vertices * 3)
							 int *coordInd,       // vertex indices for triangles
							 int coordIndLength,  // length of coordInd array
							 float *norm,         // normals
							 int normLength,      // length of norm array
							 int *normInd,        // normal indices, per vertex
							 int normIndLength,   // length of normInd array
							 int NPV,              // Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
							 int normalsCalculated,// If 1, normals are calculated by VRMLModel; if 0, normals were read from the file
							 float creaseAngle,   // VRML creaseAngle (default 0.0; 1.2 is good value to smooth)
							 int solid,           // VRML solid (default 1)
							 int ccw,             // VRML counter-clockwise (default 1)
							 int convex,          // VRML convex (default 1)
							 float *tcoord,       // texture coordinates, if any (else zero)
							 int tcoordLength, 
							 int *tcoordInd, 
							 int tcoordIndLength, 
							 float *cpv,          // colors per vertex, if any (else zero)
							 int cpvLength, 
							 int *cpvInd, 
							 int cpvIndLength,
							 int CPV,           // color per vertex (1) or per face (1)
							 VRMLMeshMT *mt,		// pointer to the list of morph targets for this mesh
							 VRMLMeshBones *bones		// pointer to the list of bones for this mesh
							 );    
	
	int nFIO; // number of FileIO objects attached
	FileIO * FIO[100];
	int traverseType;
	int tmt_foundMesh;
	bool tmt_foundTransform;
	int tmt_transform;

};


/**
* Basic modifier for the 3D model.
* VRMLModelModifier stores the necessary data for one basic modification of a VRMLModel. The modification
* can include morphing the polygon meshes of the model and modifying the transforms by translation, rotation and scaling.
* Any combination of these four operations may be applied to any polygon mesh and transform in the VRMLModel.
*
* VRMLModelModifier only holds the data describing the modification, but does not provide functions to implement them.
*
* VRMLModelModifier is used mainly in two ways: 
*
* (1) To represent the effect of a MPEG-4 Face/Body Animation Parameter
* on the VRMLModel, thus providing the mapping between the parameters and the model. One VRMLModelModifier
* is used for each animation parameter. The field refValue is the value of the animation
* parameter corresponding to the expressed modification. In other words, the modifier
* would modify the 3D model in a way that corresponds to the reference value (refValue) of the expressed
* parameter. The modifications to be applied in order to achieve other values of the paramter are obtained
* by linear interpolation. If refValue is set to -1, the modifier is ignored. The refValue must be set to -1
* for VRMLModelModifiers corresponding to animation parameters that are not implemented.
*
* (2) To actually apply modifications to a VRMLModel. In this case, the refValue is not used. A weight
* factor is applied to the modifications. For morph target, translation and scale, the weight is applied
* by multiplying the values of the modifier data. For rotation, the value of the angle is multiplied by the
* weight.
*
*/
class VISAGE_DECLSPEC VRMLModelModifier  
{
public:
	VRMLModelModifier();
	virtual ~VRMLModelModifier();

	/**
	* Morph target data.
	* The data describing the morph target for each polygon mesh.
	*
	* If MTData is NULL, no morphing is applied.
	*
	* If morphing is applied, than the first pointer must be allocated to the number
	* of polygon meshes in the VRMLModel (VRMLModel::nCoordLists). If we assume
	* that the number of polygon meshes is nc, then the allocation may be done as follows:
	*
	* MTData = (float**) malloc(nc * sizeof(float*));
	*
	* Each MTData[i] is a pointer to the morph target data to the polygon mesh i in the VRMLModel
	* (VRMLModel::coordList[i]). If MTData[i] is NULL, then no morphing is applied to polygon mesh i.
	* Otherwise, for each polygon mesh i on which morphing is applied, MTData[i] must be allocated to the size of the vertex list of the polygon mesh i
	* (VRMLModel::coordListSize[i]). MTData[i] is then filled with vertex coordinates of the morph target. By convention
	* in the current system, these are expressed as displacements of each vertex from its neutral position, and they are
	* expressed in the local coordinate system of each polygon mesh.
	*
	* When the modifier is applied, these values are multiplied by the weight factor, and then added to the corresponding 
	* vertex coordinates in the VRMLModel.
	*
	*/
	float **MTData;

	/**
	* Transform translation data.
	* The data describing the translation for each transform in the VRMLModel.
	*
	* If TTranData is NULL, no translation is applied.
	*
	* If translation is applied, than the first pointer must be allocated to the number
	* of transforms in the VRMLModel (VRMLModel::nTransfLists). If we assume
	* that the number of transforms is nt, then the allocation may be done as follows:
	*
	* TTranData = (float**) malloc(nt * sizeof(float*));
	*
	* Each TTranData[i] is a pointer to the translation data to the transform i in the VRMLModel
	* (VRMLModel::transfList[i]). If TTranData[i] is NULL, then no translation is applied to the transform i.
	* Otherwise, for each transform i on which translation is applied, TTranData[i] must be allocated to the size of 3 float values.
	* TTranData[i] is then filled with translation coordinates to be applied to the transform i.
	*
	* When the modifier is applied, these values are multiplied by the weight factor, and then added to the corresponding 
	* translation vector in the VRMLModel.
	*
	*/
	float **TTranData;

	/**
	* Transform rotation data.
	* The data describing the rotation for each transform in the VRMLModel.
	*
	* If TRotData is NULL, no rotation is applied.
	*
	* If rotation is applied, than the first pointer must be allocated to the number
	* of transforms in the VRMLModel (VRMLModel::nTransfLists). If we assume
	* that the number of transforms is nt, then the allocation may be done as follows:
	*
	* TRotData = (float**) malloc(nt * sizeof(float*));
	*
	* Each TRotData[i] is a pointer to the rotation data for the transform i in the VRMLModel
	* (VRMLModel::transfList[i]). If TRotData[i] is NULL, then no rotation is applied to the transform i.
	* Otherwise, for each transform i on which rotation is applied, TRotData[i] must be allocated to the size of 4 float values.
	* TRotData[i] is then filled with rotation coordinates, axis and angle, to be applied to the transform i. The first three
	* coordinates are the axis, the fourth one is the angle in radians.
	*
	* When the modifier is applied, the angle is multiplied by the weight factor, and then the resulting rotation is applied to the corresponding 
	* rotation in the VRMLModel. This is done by first converting the modifier rotation (with weighted angle) into
	* a quaterinon QM, and converting the original model rotation into a quaternion QO. Then the new model rotation is obtained by
	* multiplying quaternions: Qnew = QM * QO.
	*
	*/
	float **TRotData;

	/**
	* Transform scaling data.
	* The data describing the scaling for each transform in the VRMLModel.
	*
	* If TScalData is NULL, no scaling is applied.
	*
	* If scaling is applied, than the first pointer must be allocated to the number
	* of transforms in the VRMLModel (VRMLModel::nTransfLists). If we assume
	* that the number of transforms is nt, then the allocation may be done as follows:
	*
	* TScalData = (float**) malloc(nt * sizeof(float*));
	*
	* Each TScalData[i] is a pointer to the scaling data for the transform i in the VRMLModel
	* (VRMLModel::transfList[i]). If TScalData[i] is NULL, then no scaling is applied to the transform i.
	* Otherwise, for each transform i on which scaling is applied, TScalData[i] must be allocated to the size of 3 float values.
	* TScalData[i] is then filled with scaling factors to be applied to the transform i.
	*
	*
	* When the modifier is applied, these values are multiplied by the weight factor, and then added to the corresponding 
	* scaling vector in the VRMLModel.
	*
	*/
	float **TScalData;

	/**
	* Reference value.
	* Reference value of an animation parameter, i.e. the value of the parameter corresponding to the
	* implemented modification.
	*
	* The value of -1 indicates that the modifier is not used. When VRMLModelModifier is used to represent
	* an animation parameter, and the parameter is not implemented, refValue must be set to -1 to indicate this.
	*/
	float refValue;

};

}

#endif

