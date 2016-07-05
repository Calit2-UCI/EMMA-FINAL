#include "stdafx.h"
#include "SFFloat.h"

SFFloat::SFFloat(float value)
{
	_value = value;
}

void SFFloat::write(FILE *f, int indent) const
{
	fprintf(f, "%g\n", _value);
}

bool SFFloat::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	SFFloat *v = (SFFloat *) value;
	if (_value != v->_value) return false;
	return true;
}

void SFFloat::clamp(float min, float max)
{
	_value = CLAMP(_value, min, max);
}
