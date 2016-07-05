#include "stdafx.h"
#include "NodeCoordinate.h"
#include "MFVec3f.h"
#include "ExposedField.h"

NodeCoordinate::NodeCoordinate(Scene *scene, Proto *proto):
	Node(scene, proto)
{
    NaradaNotificationCenter::postNotification(naradaNotificationLoadProgressReport);
}

ProtoCoordinate::ProtoCoordinate(Scene * scene):
	Proto(scene, "Coordinate")
{
	ExposedField *exposedField = new ExposedField(MFVEC3F, "point",
		new MFVec3f());
	addExposedField(exposedField);
}
