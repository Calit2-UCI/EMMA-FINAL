#include "stdafx.h"
#include "NodeImageTexture.h"
#include "MFString.h"
#include "SFBool.h"
#include "ExposedField.h"
#include "Field.h"

NodeImageTexture::NodeImageTexture(Scene *scene, Proto *proto):
	Node(scene, proto)
{
}

NodeImageTexture::NodeImageTexture(const NodeImageTexture &node): Node(node)
{
}

NodeImageTexture::~NodeImageTexture()
{
}

void NodeImageTexture::setField(int field, FieldValue *value)
{
	assert((field >= 0) && (field < _numFields));
	if (_fields[field] != NULL) _fields[field]->unref();

	_fields[field] = value;
}

/*
void NodeImageTexture::load()
{
}

int	NodeImageTexture::isLoaded()
{
	return 0;
}

void NodeImageTexture::bind()
{
}

void NodeImageTexture::unbind()
{
}
*/

ProtoImageTexture::ProtoImageTexture(Scene *scene):
	Proto(scene, "ImageTexture")
{
	ExposedField *exposedField = new ExposedField(MFSTRING,
		"url", new MFString(), -FLT_MAX, FLT_MAX, NODE_ANY,
		FF_URL);
	addExposedField(exposedField);

	addField(SFBOOL, "repeatS", new SFBool(true));

	addField(SFBOOL, "repeatT", new SFBool(true));
}
