#include "stdafx.h"
#include "NodeViewpoint.h"
#include "SFBool.h"
#include "SFFloat.h"
#include "stdafx.h"
#include "SFRotation.h"
#include "SFVec3f.h"
#include "SFString.h"
//#include "SFTime.h"
#include "ExposedField.h"

NodeViewpoint::NodeViewpoint(Scene *scene, Proto *proto):
	Node(scene, proto)
{
}

/*
void NodeViewpoint::preDraw()
{
}

void NodeViewpoint::apply()
{
}

Vec3f NodeViewpoint::getPosition() const
{
	Vec3f v;
	return v;
}

const Quaternion & NodeViewpoint::getOrientation() const
{
	Quaternion *q = new Quaternion();
	return *q;
}

void NodeViewpoint::setPosition(const Vec3f &pos)
{
}

void NodeViewpoint::setOrientation(const Quaternion &orientation)
{
}
*/

ProtoViewpoint::ProtoViewpoint(Scene *scene):
	Proto(scene, "Viewpoint")
{
	addEventIn(SFBOOL, "set_bind");

	ExposedField *exposedField = new ExposedField(SFFLOAT,
		"fieldOfView", new SFFloat(0.785398f), 0, PI);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFBOOL, "jump",
		new SFBool(true));
	addExposedField(exposedField);

	float defaultValue[4] = {0, 0, 1, 0};
	exposedField = new ExposedField(SFROTATION, "orientation",
		new SFRotation(defaultValue));
	addExposedField(exposedField);

	exposedField = new ExposedField(SFVEC3F, "position",
		new SFVec3f(0, 0, 10));
	addExposedField(exposedField);

	addField(SFSTRING, "description", new SFString(""));

	addEventOut(SFTIME, "bindTime");
	addEventOut(SFBOOL, "isBound");
}
