#include "stdafx.h"
#include "MFVec3f.h"
#include "SFVec3f.h"
#include "Vec3f.h"

MFVec3f::MFVec3f()
{
}
			
MFVec3f::MFVec3f(const float *values, int len):
	MFFloat(values, len, getStride())
{
}

MFVec3f::MFVec3f(const Vec3f *values, int len)
{
	for (int i = 0; i < len; i++)
	{
		_value.append(values[i].x);
		_value.append(values[i].y);
		_value.append(values[i].z);
	}
}

bool MFVec3f::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	MFVec3f *v = (MFVec3f *) value;
	if (getSize() != v->getSize()) return false;

	int size = getSize();
	for (int i = 0; i < size; i++)
		if (_value[i] != v->_value[i]) return false;

	return true;
}

FieldValue * MFVec3f::getSFValue(int index) const
{
	int first = index * getStride();
	float values[3];

	for (int i = 0; i < 3; i++) values[i] = _value[first + i];

	return new SFVec3f(values);
}

void MFVec3f::setSFValue(int index, FieldValue *value)
{
	if (value->getType() != SFVEC3F) return;
	SFVec3f *v = (SFVec3f *) value;
	const float *vector = v->getValue();
	int first = index * getStride();

	for (int i = 0; i < 3; i++)
		_value[first + i] = vector[i];
}
