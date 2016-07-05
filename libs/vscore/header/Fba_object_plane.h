
#ifndef __Fba_object_plane_h__
#define __Fba_object_plane_h__

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

#include "Fba_in_buffer.h"
#include "Fba_object_plane.h"
#include "Face_object.h"
#include "Body_object.h"

namespace VisageSDK
{

class VISAGE_DECLSPEC Fba_object_plane  
{
public:
	Fba_object_plane();
	void init(int inStr);
	void close();
	virtual ~Fba_object_plane();

	Fba_in_buffer stream;
	Face_object fo;
	Body_object bo;
};

}

#endif // __Fba_object_plane_h__
