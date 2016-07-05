#include "stdafx.h"
#include "SFTime.h"

SFTime::SFTime(double value)
{
	_value = value;
}

void SFTime::write(FILE *f, int indent) const
{
	fprintf(f, "%g\n", _value);
}

bool SFTime::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	SFTime *v = (SFTime *) value;
	if (_value != v->_value) return false;
	return true;
}

