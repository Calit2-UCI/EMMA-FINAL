#include "stdafx.h"
#include "NodeTextureCoordinate.h"
#include "MFVec2f.h"
#include "ExposedField.h"


NodeTextureCoordinate::NodeTextureCoordinate(Scene *scene, Proto *proto):
	Node(scene, proto)
{
    NaradaNotificationCenter::postNotification(naradaNotificationLoadProgressReport);
}

ProtoTextureCoordinate::ProtoTextureCoordinate(Scene *scene):
	Proto(scene, "TextureCoordinate")
{
	ExposedField *exposedField = new ExposedField(MFVEC2F,
		"point", new MFVec2f());
	addExposedField(exposedField);
}
