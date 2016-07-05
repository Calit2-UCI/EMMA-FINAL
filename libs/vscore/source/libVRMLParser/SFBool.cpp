#include "stdafx.h"
#include "SFBool.h"

SFBool::SFBool(bool value)
{
	_value = value;
}

void SFBool::write(FILE *f, int indent) const
{
	if (_value == true) fprintf(f, "TRUE\n");
	else fprintf(f, "FALSE\n");
}

bool SFBool::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	SFBool *v = (SFBool *) value;
	if (v->_value != _value) return false;
	return true;
}
