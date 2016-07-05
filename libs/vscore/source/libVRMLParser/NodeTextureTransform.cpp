#include "stdafx.h"
#include "NodeTextureTransform.h"
#include "SFVec2f.h"
#include "SFFloat.h"
#include "ExposedField.h"

NodeTextureTransform::NodeTextureTransform(Scene *scene, Proto *proto):
	Node(scene, proto)
{
}

/*
void NodeTextureTransform::bind()
{
}

void NodeTextureTransform::unbind()
{
}
*/

ProtoTextureTransform::ProtoTextureTransform(Scene *scene):
	Proto(scene, "TextureTransform")
{
	ExposedField *exposedField = new ExposedField(SFVEC2F,
		"center", new SFVec2f(0, 0));
	addExposedField(exposedField);

	exposedField = new ExposedField(SFFLOAT, "rotation",
		new SFFloat(0));
	addExposedField(exposedField);

	exposedField = new ExposedField(SFVEC2F, "scale",
		new SFVec2f(1, 1));
	addExposedField(exposedField);

	exposedField = new ExposedField(SFVEC2F, "translation",
		new SFVec2f(0, 0));
	addExposedField(exposedField);
}
