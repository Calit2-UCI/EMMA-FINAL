// BaseAFMDeformer.h: interface for the BaseAFMDeformer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEAFMDEFORMER_H__419E8F27_BB1D_4F53_A3BE_F6DDD7ECA312__INCLUDED_)
#define AFX_BASEAFMDEFORMER_H__419E8F27_BB1D_4F53_A3BE_F6DDD7ECA312__INCLUDED_


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


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AFMDeformer.h"
#include "VRMLModel.h"
#include "string.h"


namespace VisageSDK
{

class VRMLModelModifier;

/**
* The default AFMDeformer.
*
* This deformer implements the animation of the MPEG-4 Face and Body Animation parameters using
* morphing of polygon meshes, and basic modifications of the transforms: translation, rotation
* and scale (these modifications affect the surface using a skinning implementation if bones are defined
* for the 3D model, otherwise they can be rigid segment motions). All parameters are implemented using these basic operations. The exact mapping
* of each parameter to a specific modification is specified by a set of VRMLModelModifier
* objects. Typically, this data is read from an %AFM file. In the default implementation
* it is the BaseFileIO class that reads the %AFM file and constructs the BaseAFMDeformer.
*/
class VISAGE_DECLSPEC BaseAFMDeformer : public AFMDeformer, VRMLTraverser
{
	friend class AFM;

public:
	/* Constructor.
	*
	* Construct a BaseAFMDeformer.
	* FAPModifiers, BAPModifiers, VisemeModifiers and ExpressionModifiers are used describe the mapping of each face and body animation
	* parameter to a specific modification of the VRMLModel, i.e. to specific morph targets
	* for polygon meshes, and translations, rotations and scaling of the transforms in the VRMLModel.
	* The exact way to specify the modification is described in the documentation of @ref VRMLModelModifier.
	*
	* @param model the VRMLModel on which the deformer will act.
	* @param FAPModifiers the array of modifiers for low level FAPs. The array must hold NFAP VRMLModelModifier objects, one for each low-level FAP. Each VRMLModifier must be allocated; and the ones corresponding to animation parameters that are not used are left empty (as constructed by the default constructor - MTData and other fields set to 0, refValue set to -1). For convenience, the constants defined in @ref FAPNames can be used as indices to this array. Note that members 0 and 1 of this array are not used (they would correspond to high-level FAPs).
	* @param ExpressionModifiers the array of modifiers for FAP Expressions. The array must hold 7 VRMLModelModifier objects, one for each low-level FAP. For convenience, the constants defined in @ref ExpressionNames can be used as indices to this array.
	* @param VisemeModifiers the array of modifiers for FAP Visemes. The array must hold 15 VRMLModelModifier objects, one for each low-level FAP. For convenience, the constants defined in @ref VisemeNames can be used as indices to this array.
	* @param BAPModifiers the array of modifiers for BAPs. The array must hold NUM_BAPS VRMLModelModifier objects, one for each BAP. For convenience, the constants defined in @ref BAPNames can be used as indices to this array.
	*
	*
	* @see AFM
	* @see VRMLModel
	* @see VRMLModelModifier
	*/
	BaseAFMDeformer(VRMLModel *model, VRMLModelModifier *FAPModifiers, VRMLModelModifier *ExpressionModifiers, VRMLModelModifier *VisemeModifiers, VRMLModelModifier *BAPModifiers);
	virtual ~BaseAFMDeformer();

	/**
	* Add a set of Face and Body Animation Parameters (FBAPs) to the list of FBAPs to be applied to the 3D model (VRMLModel).
	* The FBAPs are *not* immediately applied to the model 
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
	* @param fbaps the FBAPs to apply.
	* @param model the VRMLmodel on which the FBAPs are applied.
	* @see updateGeometry
	*/	
	void applyFBAPs(VRMLModel *model,const FBAPs *fbaps);

	/**
	* Clear the list of FBAPs to be applied to the model.
	* Clears the current FBAPs. This effectively returns the model geometry to neutral position next time updateGeomtery() is called. 
	* @see applyFBAPs
	* @see updateGeometry
	*/
	void clearFBAPs();

	/**
	* Updates the geometry of the VRMLModel using the current FBAPs.
	* The model is deformed corresponding to the current FBAPs. The "current FBAPs" are the internaly
	* stored FBAPs that have been set using applyFAPs() function. The list of current FBAPs is cleared
	* when clearFBAPs() is called, and all subsequent calls to applyFBAPs() add FBAPs to the current FBAPs list.
	* @see applyFAPs
	*/
	void updateGeometry(VRMLModel *model);

	/**
	* Write the deformer data into file.
	* This method writes out the part of the %AFM file relating to the deformation, and the BaseFileIO
	* class reads this data.
	*
	* @param f the handle of the file into which the data should be written
	* @param format the file format in which the data should be written. The format is indicated by file name extension. The format supported by this implementation is "afm"
	*/
	void write(FILE *f, char *format);


	/**
	* Used to represent the effect of low level MPEG-4 Face Animation Parameters
	* on the VRMLModel, thus providing the mapping between the parameters and the model. One VRMLModelModifier
	* is used for each low level facial animation parameter, so this pointer is allocated to hold NFAP=68 VRMLModelModifiers. The refValue is set to -1
	* for VRMLModelModifiers corresponding to animation parameters that are not implemented.
	*
	* @see VRMLModelModifier
	*/
	VRMLModelModifier *FAPModifiers;


	/**
	* Used to represent the effect of a MPEG-4 facial expressions
	* on the VRMLModel, thus providing the mapping between the parameters and the model. One VRMLModelModifier
	* is used for each expression, so this pointer is allocated to hold 7 VRMLModelModifiers. The refValue is set to -1
	* for VRMLModelModifiers corresponding to expressions that are not implemented.
	*
	* @see VRMLModelModifier
	*/
	VRMLModelModifier *ExpressionModifiers;


	/**
	* Used to represent the effect of a MPEG-4 visemes
	* on the VRMLModel, thus providing the mapping between the parameters and the model. One VRMLModelModifier
	* is used for each viseme, so this pointer is allocated to hold 15 VRMLModelModifiers. The refValue is set to -1
	* for VRMLModelModifiers corresponding to visemes that are not implemented.
	*
	* @see VRMLModelModifier
	*/
	VRMLModelModifier *VisemeModifiers;


	/**
	* Used to represent the effect of a MPEG-4 Body Animation Parameter
	* on the VRMLModel, thus providing the mapping between the parameters and the model. One VRMLModelModifier
	* is used for each animation parameter, so this pointer is allocated to hold NUM_BAPS=296 VRMLModelModifiers. The refValue is set to -1
	* for VRMLModelModifiers corresponding to animation parameters that are not implemented.
	*
	* @see VRMLModelModifier
	*/
	VRMLModelModifier *BAPModifiers;

private:
	void applyModifiers(VRMLModelModifier **mods,float *modWeights,int nMods);
	void allocAnimData();
	void initDoubleFloatPointer(float ***p,int l1);
	void initDoubleFloatPointer(float ***p,int l1,int l2);
	void initTripleFloatPointer(float ****p,int l1,int l2);
	void initTripleFloatPointer(float ****p,int l1,int l2,int l3);
	void freeTripleFloatPointer(float ***p,int l1,int l2);
	void freeDoubleFloatPointer(float **p,int l1);
	void outputAnimDataWra(FILE *of);
	void outputTransfDataWra(FILE *of);
	void outputAnimDataAfm(FILE *of);
	void outputMorphTargetsData(float **mtCoord, FILE *of);
	void outputAnimationParameterData(VRMLModelModifier *mod,char *parameterName, FILE *of);

	
	// traverser functions
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
							 myMatrix m);  // Complete transformation matrix

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
							 VRMLMeshBones *bones);  // pointer to the list of boness for this mesh          

	float *modifierWeights;
	int nModifiers;
	VRMLModelModifier **modifiers;

	float **origPosition;
	float **position;
	float **origOrientation;
	float **orientation;
	float **origScale;
	float **scale;

	int transfCnt;
	int nTransforms;
//	float **origCoord;
//	float **coord;

	int nVisemes;
	int nAllExp;
	int nExp;
	int nFaps;
	int nSurfaces;
	int surfCnt;
//	int *coordLength;
	VRMLModel *currentModel;
	int traversalType;
	int faceNeutral;
};

}

#endif // !defined(AFX_BASEAFMDEFORMER_H__419E8F27_BB1D_4F53_A3BE_F6DDD7ECA312__INCLUDED_)
