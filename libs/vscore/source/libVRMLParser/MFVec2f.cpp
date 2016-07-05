#include "stdafx.h"
#include "MFVec2f.h"
#include "SFVec2f.h"

MFVec2f::MFVec2f()
{
}

MFVec2f::MFVec2f(const float values[], int len):
	MFFloat(values, len, getStride())
{
}
    
bool MFVec2f::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	MFVec2f *v = (MFVec2f *) value;
	if (getSize() != v->getSize()) return false;

	int size = getSize();
	for (int i = 0; i < size; i++)
		if (_value[i] != v->_value[i]) return false;

	return true;
}
    
FieldValue * MFVec2f::getSFValue(int index) const
{
	int first = index * getStride();
	float values[2];

	for (int i = 0; i < 2; i++) values[i] = _value[first + i];

	return new SFVec2f(values);
}

void MFVec2f::setSFValue(int index, FieldValue *value)
{
	if (value->getType() != SFVEC2F) return;
	SFVec2f *v = (SFVec2f *) value;
	const float *vector = v->getValue();
	int first = index * getStride();

	for (int i = 0; i < 2; i++)
		_value[first + i] = vector[i];
}
