#include "stdafx.h"
#include "NodePixelTexture.h"
#include "SFImage.h"
#include "SFBool.h"
#include "ExposedField.h"

NodePixelTexture::NodePixelTexture(Scene *scene, Proto *proto):
	Node(scene, proto)
{
}

ProtoPixelTexture::ProtoPixelTexture(Scene *scene):
	Proto(scene, "PixelTexture")
{
	ExposedField *exposedField = new ExposedField(SFIMAGE,
		"image", new SFImage(0, 0, 0, NULL));
	addExposedField(exposedField);

	addField(SFBOOL, "repeatS", new SFBool(true));
	addField(SFBOOL, "repeatT", new SFBool(true));
}
