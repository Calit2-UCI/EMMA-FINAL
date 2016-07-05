#include "stdafx.h"
#include "NodeDirectionalLight.h"
#include "SFFloat.h"
#include "SFColor.h"
#include "SFVec3f.h"
#include "SFBool.h"
#include "ExposedField.h"

NodeDirectionalLight::NodeDirectionalLight(Scene *scene, Proto *proto):
	Node(scene, proto)
{
}

/*
void NodeDirectionalLight::bind()
{
}

void NodeDirectionalLight::unbind()
{
}
*/

ProtoDirectionalLight::ProtoDirectionalLight(Scene *scene):
	Proto(scene, "DirectionalLight")
{
	ExposedField *exposedField = new ExposedField(SFFLOAT,
		"ambientIntensity",	new SFFloat(0), 0, 1);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFCOLOR,
		"color", new SFColor(1, 1, 1), 0, 1);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFVEC3F, "direction",
		new SFVec3f(0, 0, -1));
	addExposedField(exposedField);

	exposedField = new ExposedField(SFFLOAT, "intensity",
		new SFFloat(1), 0, 1);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFBOOL, "on",
		new SFBool(true));
	addExposedField(exposedField);
}

