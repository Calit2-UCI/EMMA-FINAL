#include "stdafx.h"
#include "SFColor.h"


SFColor::SFColor(float r, float g, float b)
{
	_value[0] = r;
	_value[1] = g;
	_value[2] = b;
	_value[3] = 1;
}

SFColor::SFColor(const float *value)
{
	for (int i = 0; i < 3; i++) _value[i] = value[i];

	_value[3] = 1;
}

void SFColor::write(FILE *f, int indent) const
{
	fprintf(f, "%g %g %g\n", _value[0], _value[1], _value[2]);
}

bool SFColor::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	SFColor *v = (SFColor *) value;
	for (int i = 0; i < 4; i++)
		if (v->_value[i] != _value[i]) return false;
	return true;
}

void SFColor::clamp(float min, float max)
{
	for (int i = 0; i < 4; i++)
	{
		_value[i] = CLAMP(_value[i], min, max);
	}
}

bool SFColor::equals(const SFColor *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	for (int i = 0; i < 4; i++)
		if (value->_value[i] != _value[i]) return false;
	return true;
}
