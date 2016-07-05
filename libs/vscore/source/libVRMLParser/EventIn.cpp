#include "stdafx.h"
#include "EventIn.h"
#include "FieldValue.h"
#include "Scene.h"

EventIn::EventIn(int type, String name, ExposedField *exposedField)
{
	_type = type;
	_name = name;
	_exposedField = exposedField;
}

EventIn::~EventIn()
{
}

void EventIn::write(FILE *f, int indent) const
{
	char *types[] = { "SFBool", "SFColor", "SFImage", "SFInt32",
		"SFFloat", "SFNode", "SFRotation", "SFString", "SFTime",
		"SFVec2f", "SFVec3f", "MFColor", "MFInt32", "MFFloat",
		"MFNode", "MFRotation", "MFString", "MFTime", "MFVec2f",
		"MFVec3f" };
	char *type = types[getType()];

	indentf(f, indent);
	fprintf(f, "eventIn      %s", type);
	indentf(f, 11 - (int)strlen(type));	//CHANGED BY I.K. added explicit cast
	fprintf(f, "%s\n", (const char *) getName());
}
