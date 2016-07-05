// Point3d.h: interface for the Point3d class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINT3D_H__AF826441_5150_11D6_B175_009027A47E31__INCLUDED_)
#define AFX_POINT3D_H__AF826441_5150_11D6_B175_009027A47E31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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


#ifdef WIN32
	#include <windows.h>  //necessary for windows gl.h library
	#include <gl\gl.h>
#elif defined(LINUX)
	#include <GL/gl.h>
#elif defined(MAC)
	#include <OpenGL/gl.h>
#elif defined(IPHONE)
	#include <OpenGLES/ES1/gl.h>
#endif



namespace VisageSDK
{

/** Storage class for 3D points.
* Point3D stores the 3D coordinates of a vertex, as well as the index to the surface (indexed face set)
* in the VRMLModel that this point belongs to, and its index within that surface
*
* Implemented in visageVRMLrenderer.lib
*/
class Point3d  
{
		// Easier way to store 3D points.
public:
	
//	Point3d() : x(0), y(0), z(0), surface(-1), vertex(-1), error(2) {};
	
	/** Constructor.
	* Make a new point with coordinates inX, inY, inZ
	*/
	Point3d(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ), surface(-1), vertex(-1), error(2) {};

	/** Constructor.
	* Make a new point with coordinates inX, inY, inZ; it is the vertex number vert in the surface surf.
	*/
	Point3d(GLfloat inX, GLfloat inY, GLfloat inZ, int surf, int vert) : x(inX), y(inY), z(inZ), surface(surf), vertex(vert), error(2) {};

	/**
	 * Operator == overloaded so that find can be used on mObjectSpace vector.
	 * Compares two points and returns TRUE if the distance between them is less than 0.1.
	 */
	bool operator== (const Point3d &compPoint);

	GLfloat x; /**< x coordinate of the point */
	GLfloat y; /**< y coordinate of the point */
	GLfloat z; /**< v coordinate of the point */

	int surface; /**< the index of the surface in the model (VRMLModel::coordList) to which this point belongs */
	int vertex;  /**< the index of the vertex within the surface in the model (VRMLModel::coordList) */

	// If it is a undefined lip-point this is the error of it.
	int error;
};

}


#endif // !defined(AFX_POINT3D_H__AF826441_5150_11D6_B175_009027A47E31__INCLUDED_)
