#include "stdafx.h"
#include "NodeColor.h"
#include "MFColor.h"
#include "ExposedField.h"

NodeColor::NodeColor(Scene *scene, Proto *proto):
	Node(scene, proto)
{
}

ProtoColor::ProtoColor(Scene *scene):
	Proto(scene, "Color")
{
	ExposedField *exposedField = new ExposedField(MFCOLOR,
		"color", new MFColor(), 0, 1);
	addExposedField(exposedField);
}
