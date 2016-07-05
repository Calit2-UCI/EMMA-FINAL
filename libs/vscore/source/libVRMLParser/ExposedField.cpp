#include "stdafx.h"
#include "ExposedField.h"
#include "FieldValue.h"
#include "Proto.h"

ExposedField::ExposedField(int type, const String &name,
							FieldValue *value, float min,
							float max, int nodeType,
							int flags, const char **strings)
{
	_type = type;
	_name = name;
	_value = value;
	_min = min;
	_max = max;
	_nodeType = nodeType;
	_flags = flags;
	_strings = strings;
	_field = INVALID_INDEX;
	_eventIn = INVALID_INDEX;
	_eventOut = INVALID_INDEX;
}

ExposedField::~ExposedField()
{
	_value->unref();
	delete _strings;
}

void ExposedField::write(FILE *f, int indent) const
{
	const char *type = _value->getTypeName(),
		*name = getName();

	indentf(f, indent);
	fprintf(f, "exposedField %s", type);
	indentf(f, 11 - (int)strlen(type));	//CHANGED BY I.K. added explicit cast
	fprintf(f, "%s", name);
	indentf(f, 18 - (int)strlen(name));	//CHANGED BY I.K. added explicit cast
	_value->write(f, indent + 13 + 11);
}
