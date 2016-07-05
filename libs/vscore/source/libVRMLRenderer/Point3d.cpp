// Point3d.cpp: implementation of the Point3d class.
//
//////////////////////////////////////////////////////////////////////
#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include "Point3d.h"
#include <math.h>

namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


bool Point3d::operator== (const Point3d &compPoint)
{
	double dist = (x - compPoint.x) * (x - compPoint.x);
	dist += (y - compPoint.y) * (y - compPoint.y);
	dist += (z - compPoint.z) * (z - compPoint.z);
	dist = sqrt(dist);
	return (dist < 0.1);
}

}
