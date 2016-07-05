
#include "Fba_object_plane.h"

namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fba_object_plane::Fba_object_plane()
{
}

void Fba_object_plane::init(int inStr)
{
	stream.init(inStr);
}
	
	
void Fba_object_plane::close()
{
	stream.closeStream();
}

Fba_object_plane::~Fba_object_plane()
{
}

}
