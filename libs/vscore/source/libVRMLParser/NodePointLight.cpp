#include "stdafx.h"
#include "NodePointLight.h"
#include "SFFloat.h"
#include "SFVec3f.h"
#include "SFColor.h"
#include "SFBool.h"
#include "ExposedField.h"

NodePointLight::NodePointLight(Scene *scene, Proto *proto):
	Node(scene, proto)
{
}

/*
void NodePointLight::preDraw()
{
}

void NodePointLight::drawHandles()
{
}

Vec3f NodePointLight::getHandle(int handle, int *constraint, int *field)
{
	return NULL;
}

void NodePointLight::setHandle(int handle, const Vec3f &v)
{
}
*/

ProtoPointLight::ProtoPointLight(Scene *scene):
	Proto(scene, "PointLight")
{
	ExposedField *exposedField = new ExposedField(SFFLOAT,
		"ambientIntensity",	new SFFloat(0), 0, 1);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFVEC3F, "attenuation",
		new SFVec3f(1, 0, 0), 0);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFCOLOR, "color",
		new SFColor(1, 1, 1), 0, 1);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFFLOAT, "intensity",
		new SFFloat(1), 0, 1);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFVEC3F, "location",
		new SFVec3f(0, 0, 0));
	addExposedField(exposedField);

	exposedField = new ExposedField(SFBOOL, "on",
		new SFBool(true));
	addExposedField(exposedField);

	exposedField = new ExposedField(SFFLOAT, "radius",
		new SFFloat(100), 0);
	addExposedField(exposedField);
}
