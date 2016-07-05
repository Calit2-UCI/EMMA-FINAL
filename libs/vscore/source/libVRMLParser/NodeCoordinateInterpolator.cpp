#include "stdafx.h"
#include "NodeCoordinateInterpolator.h"
#include "MFVec3f.h"

NodeCoordinateInterpolator::NodeCoordinateInterpolator(Scene *scene,
													   Proto *proto):
	Interpolator(scene, proto)
{
}

/*
int	NodeCoordinateInterpolator::getNumChannels() const
{
	return 0;
}

FieldValue * NodeCoordinateInterpolator::createKey(const float *value) const
{
	return NULL;
}

FieldValue * NodeCoordinateInterpolator::createKeys(const float *values,
													int numKeys) const
{
	return NULL;
}
*/

ProtoCoordinateInterpolator::ProtoCoordinateInterpolator(Scene *scene):
	ProtoInterpolator(scene, "CoordinateInterpolator", MFVEC3F, MFVEC3F,
					  new MFVec3f())
{
}
