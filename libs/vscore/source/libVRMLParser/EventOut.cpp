#include "stdafx.h"
#include "EventOut.h"
#include "FieldValue.h"

EventOut::EventOut(int type, const String &name, ExposedField *exposedField)
{
	_type = type;
	_name = name;
	_exposedField = exposedField;
}

EventOut::~EventOut()
{
}

void EventOut::write(FILE *f, int indent) const
{
	char *types[] = { "SFBool", "SFColor", "SFImage", "SFInt32",
		"SFFloat", "SFNode", "SFRotation", "SFString", "SFTime",
		"SFVec2f", "SFVec3f", "MFColor", "MFInt32", "MFFloat",
		"MFNode", "MFRotation", "MFString", "MFTime", "MFVec2f",
		"MFVec3f" };
	char *type = types[getType()];

	indentf(f, indent);
	fprintf(f, "eventOut     %s", type);
	indentf(f, 11 - (int)strlen(type));	//CHANGED BY I.K. added explicit cast
	fprintf(f, "%s\n", (const char *) getName());
}
