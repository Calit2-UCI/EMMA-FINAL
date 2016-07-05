#include "stdafx.h"
#include "NodeTransform.h"
#include "SFVec3f.h"
#include "MFNode.h"
#include "SFRotation.h"
#include "ExposedField.h"
#include "Field.h"

NodeTransform::NodeTransform(Scene *scene, Proto *proto):
	Node(scene, proto)
{
}

NodeTransform::~NodeTransform()
{
}

void NodeTransform::setField(int index, FieldValue *value)
{
	assert((index >= 0) && (index < _numFields));
	if (_fields[index] != NULL) _fields[index]->unref();

	if (value->getType() == MFNODE)
	{
		MFNode *v = (MFNode *) value;
		for (int i = 0; i < v->getSize(); i++)
		{
			Node *node = v->getValue(i);
			if (node) node->addParent(this, index);
		}
	}

	_fields[index] = value;
}

/*
void NodeTransform::drawHandles()
{
}

void NodeTransform::transform()
{
}

void NodeTransform::transformForHandle(int handle)
{
}

void NodeTransform::draw()
{
}

void NodeTransform::preDraw()
{
}

Vec3f NodeTransform::getHandle(int handle, int *constraint, int *field)
{
	Vec3f v;
	return v;
}

void NodeTransform::setHandle(int handle, const Vec3f &v)
{
}

void NodeTransform::drawCone( float x, float y, float z)
{
}

void NodeTransform::drawScaleHandle(float x, float y, float z)
{
}

void NodeTransform::drawAxisLines()
{
}

void NodeTransform::drawTranslationHandles()
{
}

void NodeTransform::drawRotationHandles()
{
}

void NodeTransform::drawScaleHandles()
{
}
*/

ProtoTransform::ProtoTransform(Scene *scene):
	Proto(scene, "Transform")
{
	addEventIn(MFNODE, "addChildren");
	addEventIn(MFNODE, "removeChildren");

	ExposedField *exposedField = new ExposedField(SFVEC3F,
		"center", new SFVec3f(0, 0, 0));
	addExposedField(exposedField);

	addExposedField(MFNODE, "children", new MFNode(), NODE_CHILD);
	exposedField = getExposedField(_exposedFields.size() - 1);
	exposedField->getValue()->ref();
	// field's flags are set to FF_HIDDEN because this is a hidden field
	Field *field = new Field(MFNODE, "children",
		exposedField->getValue(), NULL,	-FLT_MAX, FLT_MAX,
		NODE_CHILD,	exposedField->getFlags() | FF_HIDDEN);
	exposedField->setField(_fields.size());
	_fields.append(field);

	float defaultValue[4] = {0, 0, 1, 0};
	exposedField = new ExposedField(SFROTATION, "rotation",
		new SFRotation(defaultValue));
	addExposedField(exposedField);

	exposedField = new ExposedField(SFVEC3F, "scale",
		new SFVec3f(1, 1, 1), 0);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFROTATION, "scaleOrientation",
		new SFRotation(defaultValue));
	addExposedField(exposedField);

	exposedField = new ExposedField(SFVEC3F, "translation",
		new SFVec3f(0, 0, 0));
	addExposedField(exposedField);
	
	addField(SFVEC3F, "bboxCenter", new SFVec3f(0, 0, 0));
	//  range is (0,infinity) or -1,-1,-1
	addField(SFVEC3F, "bboxSize", new SFVec3f(-1, -1, -1), 0);
}
