#include "stdafx.h"
#include "Field.h"
#include "FieldValue.h"

Field::Field(int type, const String &name, FieldValue *value, 
				ExposedField *exposedField, float min,
				float max, int nodeType, int flags, 
				const char **strings)
{
	_type = type;
	_name = name;
	_value = value;
	_exposedField = exposedField;
	_min = min;
	_max = max;
	_nodeType = nodeType;
	_flags = flags;
	_strings = strings;
}

Field::~Field()
{
	_value->unref();
	delete _strings;
}

void Field::write(FILE *f, int indent) const
{
	const char *type = _value->getTypeName(),
		*name = getName();

	indentf(f, indent);
	fprintf(f, "field        %s", type);
	indentf(f, 11 - (int)strlen(type));	//CHANGED BY I.K. added explicit cast
	fprintf(f, "%s", name);
	indentf(f, 18 - (int)strlen(name));
	_value->write(f, indent + 13 + 11);
}
