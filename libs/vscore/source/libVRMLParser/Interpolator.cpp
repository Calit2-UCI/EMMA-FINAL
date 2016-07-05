#include "stdafx.h"
#include "Interpolator.h"
#include "SFFloat.h"
#include "MFFloat.h"
#include "ExposedField.h"

Interpolator::Interpolator(Scene *scene, Proto *proto): Node(scene, proto)
{
}

ProtoInterpolator::ProtoInterpolator(Scene *scene, const char *name,
									 int keyType, int keysType,
									 FieldValue *defaultValue):
	Proto(scene, name)
{
	addEventIn(SFFLOAT, "set_fraction");

	ExposedField *exposedField = new ExposedField(MFFLOAT,
		"key", new MFFloat());
	addExposedField(exposedField);

	exposedField = new ExposedField(keysType, "keyValue",
		defaultValue);
	addExposedField(exposedField);

	addEventOut(keyType, "value_changed");
}
