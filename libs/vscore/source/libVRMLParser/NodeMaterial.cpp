#include "stdafx.h"
#include "NodeMaterial.h"
#include "SFFloat.h"
#include "SFColor.h"
#include "ExposedField.h"

NodeMaterial::NodeMaterial(Scene *scene, Proto *proto):
	Node(scene, proto)
{
}

/*
void NodeMaterial::bind()
{
}

void NodeMaterial::unbind()
{
}
*/

ProtoMaterial::ProtoMaterial(Scene *scene):
	Proto(scene, "Material")
{
	ExposedField *exposedField = new ExposedField(SFFLOAT,
		"ambientIntensity", new SFFloat(0.2f), 0, 1);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFCOLOR, "diffuseColor",
		new SFColor(0.8f, 0.8f, 0.8f), 0, 1);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFCOLOR, "emissiveColor",
		new SFColor(0, 0, 0), 0, 1);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFFLOAT, "shininess",
		new SFFloat(0.2f), 0, 1);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFCOLOR, "specularColor",
		new SFColor(0, 0, 0), 0, 1);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFFLOAT, "transparency",
		new SFFloat(0), 0, 1);
	addExposedField(exposedField);
}
