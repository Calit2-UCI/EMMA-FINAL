#include "stdafx.h"
#include "SFVec2f.h"

SFVec2f::SFVec2f(float x, float y)
{
	_value[0] = x;
	_value[1] = y;
}

SFVec2f::SFVec2f(const float *values)
{
	for (int i = 0; i < 2; i++) _value[i] = values[i];
}

void SFVec2f::write(FILE *f, int indent) const
{
	fprintf(f, "%g %g\n", _value[0], _value[1]);
}

bool SFVec2f::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	SFVec2f *v = (SFVec2f *) value;
	for (int i = 0; i < 2; i++)
		if (_value[i] != v->getValue(i)) return false;
	return true;
}
