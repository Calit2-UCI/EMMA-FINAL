// wraFileIO.h: interface for the wraFileIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEFILEIO_H__991ACAEE_43A7_44A4_A724_A28E1E248A67__INCLUDED_)
#define AFX_wraFileIO_H__991ACAEE_43A7_44A4_A724_A28E1E248A67__INCLUDED_




#ifdef VISAGE_STATIC
	#define AFM_API
#else

	#if defined(MAC) || defined(LINUX)
		#define AFM_API __attribute__((__visibility__("default")))
	#else

		#ifdef AFM_EXPORTS
			#define AFM_API __declspec(dllexport)
		#else
			#define AFM_API __declspec(dllimport)
		#endif
	#endif

#endif


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdlib.h"
#include "stdio.h"
#include "FileIO.h"
#include "myMatrix.h"
#include "VRMLModel.h"
#include "FBAPs.h"
#include "basefileio.h"

class Node;


namespace VisageSDK
{

class hAnimBodyJoint
{
private:
	char name[200]; // name of the joint
	int ind;        // index to the Transform node in the VRMLModel that corresponds to this joint
	float r[4];     // rotation of this joint as quaternion
	float t[3];     // translation of this joint
	int _hasTranslation; // most joints don't have translation, so this flag indicates when it happens
	int changed;    // 1 if value has been changed since last getValue was called, 0 otherwise

public:
	~hAnimBodyJoint();
	hAnimBodyJoint(char *_name, int jointTransfInd, float _value);
	void setName(char *_name) {strcpy(name,_name);};
	char *getName() {return name;};
	void reset();
	void addRotation(int axis, float angle); // add rotation around axis (0-x, 1-y, 2-z) by angle in radians
	float *getRotation() {changed = 0; return r;};
	int hasTranslation() {return _hasTranslation;};
	float *getTranslation() {changed = 0; return t;};
	float *peekRotation() {return r;}; // same as getRotation but does not reset the changed status
	int isChanged() {return changed;};
	int getTransfIndex() {return ind;};

};

class hAnimBody
{
private:
	char name[200];
	int nJoints;
	hAnimBodyJoint *joint[150];
	hAnimBodyJoint *BAPJoint[NBAP];
//	class AFM *afm;

public:
	void clearBAPs();
	void applyBAPs(BAPs *baps);
	void initBAPJoints();
	~hAnimBody();
	hAnimBody(char *bodyName); 
	void setName(char *_name) {strcpy(name,_name);};
	char *getName() {return name;};
	int getJointIndex(char *jointName);
	int getNJoints() {return nJoints;};
	hAnimBodyJoint* getJoint(int i) {if(i>=0 && i < nJoints) return joint[i]; else return NULL;};
	void addJoint(char *jointName, int jointTransfInd);
	int getJointTransfIndex(char *jointName);
	void setBAPModifier(int n,VRMLModelModifier *mod,VRMLModel *m);
	void quatDiff(float *q, float *q0);
	void arrangeQuat(float *q, float *q0);

};


/**
* File input/output interface for Animatable Face Models in the old .wra format.
* This class implements reading and writing for the Animatable Face Models in the old .wra format.
* It is provided in order to enable the usage of older files produced in this format.
* It is recommended to convert such files to the new %AFM format (.afm). A converter is
* available for this purpose.
* The old .wra format will not be supported in future releases.
*/
class AFM_API wraFileIO : public FileIO, VRMLTraverser 
{
public:
	/**
	* Constructor.
	*/
	wraFileIO();
	virtual ~wraFileIO();

	/**
	* Read the VRMLModel from a VRML (.wrl) file.
	*
	* The implementation of the member of FileIO abstract class.
	*
	* @param fileName the name of the file to be read (with full path if necessary)
	* @param model the model to be read
	*
	* @return 1 if succesful; 0 if file could not be read (file not found, file corrupt etc.); -1 if the extension of the fileName is not ".wrl".
	*/
	int read(char *fileName, VRMLModel *model);

	/**
	* Write the VRMLModel into a VRML (.wrl) file.
	*
	* The implementation of the member of FileIO abstract class.
	*
	* @param fileName the name of the file to be write (with full path if necessary)
	* @param model the model to be written into file
	*
	* @return 1 if succesful; 0 if file could not be written ; -1 if the extension of the fileName the fileName is not ".wrl".
	*/
	int write(char *fileName, VRMLModel *model);

	/**
	* Read the AFM from an old format AFM (.wra) file.
	*
	* The obligatory implementation of the member of FileIO abstract class.
	*
	* This implementation uses the default
	* deformer class @ref BaseAFMDeformer; it creates one deformer of this type by reading its parameters from the file.
	*
	* @param fileName the name of the file to be read (with full path if necessary)
	* @param model the model to be read
	* @param deformers a list of deformer objects created by the read function; these deformers will be attached the AFM; the list of pointer is allocated to hold up to 100 pointers, and the read function should assign them, e.g. _deformers[0] = new BaseAFMDeformer(...).
	* @param nDeformers returns the number of deformers returned in the deformers list
	*
	* @return 1 if succesful; 0 if file could not be read (file not found, file corrupt etc.); -1 if the extension of the fileName does not correspond to any supported file type.
	*
	* @see VRMLModel
	* @see VRMLModelModifier
	*/
	int read(char *fileName, VRMLModel *model, AFMDeformer **deformers, int *nDeformers);

	/**
	* Write the AFM into an old format (.wra) file.
	*
	* The obligatory implementation of the member of FileIO abstract class..
	*
	* @param fileName the name of the file to be write (with full path if necessary)
	* @param model the model to be written into file
	* @param deformers a list of deformer objects attached to the AFM
	* @param nDeformers the number of deformers in the deformer list
	*
	* @return 1 if succesful; 0 if file could not be read (file not found, file corrupt etc.); -1 if the extension of the fileName does not correspond to any supported file type.
	*
	* @see VRMLModel
	* @see VRMLModelModifier
	*/
	int write(char *fileName, VRMLModel *model, AFMDeformer **deformers, int nDeformers);


	/**
	* VRMLTraverser implementation.
	*/
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
	
	/**
	* VRMLTraverser implementation.
	*/
	void traverseTransformEnd(VRMLModel *model,    // pointer to the traversed model
							 VRMLNode *thisNode,  // the node currently being traversed
							 int index);

	/**
	* VRMLTraverser implementation.
	*/
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
	
	
	void initAnimData(VRMLModel *model0,VRMLModel **models, int *fv, AFMDeformer **deformers, int *nDeformers, float _coordSensitivity);

private:
	void setModifiers();
	VRMLMaterial *currentMaterial; //Current material pointer. Used during the reading of the VRML file.
	void parseMaterial(Node *_currentRootNode,VRMLMaterial *m);
	void parseTransform(Node *node);
	void parseIndexedFaceSet(Node *node);
	void parseNode(Node *_currentRootNode, int traverseType, int TI, int TL, myMatrix toWC1,VRMLNode * parentNode);
	void updateToWCMatrix(myMatrix toWC);
	void initAnimData();
	void allocAnimData();
	void traverse(Node *_currentRootNode, int traverseType,char *iid);
	void initTripleFloatPointer(float ****p,int l1,int l2);
	void initTripleFloatPointer(float ****p,int l1,int l2,int l3);
	void initDoubleFloatPointer(float ***p,int l1);
	void initDoubleFloatPointer(float ***p,int l1,int l2);
	void freeTripleFloatPointer(float ***p,int l1,int l2);
	void freeDoubleFloatPointer(float **p,int l1);
	int readFapCoords(float **af);
	int readFapTransform(float **af, float **af1, float **af2);
	void readInterpolator(Node *node,int **key,int *nKey,float **value,int *nValue);
	void setRotation(float *r);
	int readFapCoords(float **af,VRMLModel *m);
	int readFapTransform(float **af, float **af1, float **af2,VRMLModel *m);
	void quatDiff(float *q, float *q0);
	void arrangeQuat(float *q, float *q0);
	int testDiff(float d,float f1,float f2);
	void addBones(int childTransform, int parentTransform);
	float computeBoneWeight(float *v,float *center, float range);

	Node *root;                /**<  The root node. The root node of the VRML model (Should normally not be used.*/

	VRMLModel *currentModel;


	// from AFM
	float **origPosition;
	float **position;
	float **origOrientation;
	float **orientation;
	float **origScale;
	float **scale;
	float ***fap1Position;
	float ***fap2Position;
	float ***fapPosition;
	float ***fap1Orientation;
	float ***fap2Orientation;
	float ***fapOrientation;
	float ***fap1Scale;
	float ***fap2Scale;
	float ***fapScale;
	int transfCnt;
	int nTransforms;
	float **origCoord;
	float **coord;
	float ***fap1Coord;
	float ***fap2Coord;
	float ***fapCoord;
	int *fap1Value0;
	int *fap2Value0;
	int *fapValue0;
	int nVisemes;
	int nAllExp;
	int nExp;
	int nFaps;
	float ***FapDisplacement;
	float ***FapPositionDisp;
	float ***FapOrientationDisp;
	float ***FapScaleDisp;
	float *FapWeight;
	int nCurrentFaps;
	int nSurfaces;
	int surfCnt;
	int *coordLength;
	int traversalType;
	float coordSensitivity;

	//data for the bone weights setting traverser
	int bwt_boneTransformIndex;
	int bwt_boneRefTransformIndex;
	int bwt_boneParentTransformIndex;
	float bwt_boneCenter[3];
	float bwt_range;
	bool bwt_traversingUnderBone;

	hAnimBody *body;

	VRMLModelModifier *FAPModifiers;
	VRMLModelModifier *ExpressionModifiers;
	VRMLModelModifier *VisemeModifiers;
	VRMLModelModifier *BAPModifiers;
};

}

#endif // !defined(AFX_wraFileIO_H__991ACAEE_43A7_44A4_A724_A28E1E248A67__INCLUDED_)
