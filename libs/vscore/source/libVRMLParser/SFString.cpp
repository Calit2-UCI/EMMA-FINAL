//#include "stdafx.h"
#include "SFString.h"

SFString::SFString(const String &value)
{
	_value += (const char *) value;
}

SFString::SFString(const char *str)
{
	_value += str;
}

void SFString::write(FILE *f, int indent) const
{
	const char *str = (const char *) _value;
	int length = _value.length();

	fprintf(f, "\"");
	for (int i = 0; i < length; i++)
	{
		if ((str[i] == '\\') || (str[i] == '\"')) fprintf(f, "\\");
		fprintf(f, "%c", str[i]);
	}
	fprintf(f, "\"\n");
}

bool SFString::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	SFString *v = (SFString *) value;
	if (!(_value == v->_value)) return false;
	return true;
}
