#include "stdafx.h"
#include "SFVec3f.h"
#include "assert.h"

SFVec3f::SFVec3f(float x, float y, float z)
{
	_value[0] = x;
	_value[1] = y;
	_value[2] = z;
}

SFVec3f::SFVec3f(const Vec3f &value)
{
	_value[0] = value.x;
	_value[1] = value.y;
	_value[2] = value.z;
}

void SFVec3f::write(FILE *f, int indent) const
{
	fprintf(f, "%g %g %g\n", _value[0], _value[1], _value[2]);
}

bool SFVec3f::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	SFVec3f *v = (SFVec3f *) value;
	for (int i = 0; i < 3; i++)
		if (_value[i] != v->getValue(i)) return false;
	return true;
}

void SFVec3f::setValue(const float *values)
{
	for (int i = 0; i < 3; i++)
		_value[i] = *(values + i);
}

void SFVec3f::setValue(int index, float value)
{
	assert(index >= 0 && index <= 2);
	_value[index] = value;
}

void SFVec3f::clamp(float min, float max)
{
	for (int i = 0; i < 3; i++)
		_value[i] = CLAMP(_value[i], min, max);
}
