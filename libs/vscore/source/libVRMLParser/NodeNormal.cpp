#include "stdafx.h"
#include "NodeNormal.h"
#include "MFVec3f.h"
#include "ExposedField.h"

NodeNormal::NodeNormal(Scene *scene, Proto *proto):
	Node(scene, proto)
{
    NaradaNotificationCenter::postNotification(naradaNotificationLoadProgressReport);
}

ProtoNormal::ProtoNormal(Scene *scene):
	Proto(scene, "Normal")
{
	ExposedField *exposedField = new ExposedField(MFVEC3F,
		"vector", new MFVec3f());
	addExposedField(exposedField);
    
}
