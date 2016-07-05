#ifndef _STDAFX_H
#define _STDAFX_H

#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#ifdef WIN32

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <sys/timeb.h>

#define strcasecmp _stricmp
#define snprintf _snprintf




#include <climits>
#include <stdio.h>
#include <assert.h>
#include <float.h>
#include <limits.h>


#endif // WIN32

#ifndef MAX
#define MAX(a, b)   ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b)   ((a) < (b) ? (a) : (b))
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef CLAMP
#define CLAMP(v, min, max)  MIN(MAX(v, min), max)
#endif

#ifndef PI
#define PI 3.141592653589f
#endif

#define EPSILON		    1E-6
#define EQUALF(a, b)	    (fabs((a) - (b)) < EPSILON)

#ifndef RAD2DEG
#define RAD2DEG(a) ((a) * (180.0f / PI))
#define DEG2RAD(a) ((a) * (PI / 180.0f))
#endif

#endif // _STDAFX_H
