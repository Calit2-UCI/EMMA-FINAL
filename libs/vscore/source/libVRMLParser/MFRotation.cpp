#include "stdafx.h"
#include "MFRotation.h"
#include "SFRotation.h"

MFRotation::MFRotation()
{
}

MFRotation::MFRotation(const float values[], int len):
	MFFloat(values, len, getStride())
{
}

bool MFRotation::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	MFRotation *v = (MFRotation *) value;
	if (getSize() != v->getSize()) return false;

	int size = getSize();
	for (int i = 0; i < size; i++)
		if (_value[i] != v->_value[i]) return false;

	return true;
}

FieldValue * MFRotation::getSFValue(int index) const
{
	int first = index * getStride();
	float values[4];

	for (int i = 0; i < 4; i++) values[i] = _value[first + i];

	return new SFRotation(values);
}

void MFRotation::setSFValue(int index, FieldValue *value)
{
	if (value->getType() != SFROTATION) return;
	SFRotation *v = (SFRotation *) value;
	const float *rotation = v->getValue();
	int first = index * getStride();

	for (int i = 0; i < 4; i++)
		_value[first + i] = rotation[i];
}
