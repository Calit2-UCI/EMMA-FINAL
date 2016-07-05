#include "stdafx.h"
#include "MFColor.h"
#include "SFColor.h"

MFColor::MFColor()
{
}

MFColor::MFColor(const float values[], int len):
	MFFloat(values, len, getStride())
{
}

bool MFColor::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	MFColor *v = (MFColor *) value;
	if (getSize() != v->getSize()) return false;

	int size = getSize();
	for (int i = 0; i < size; i++)
		if (_value[i] != v->_value[i]) return false;

	return true;
}

FieldValue * MFColor::getSFValue(int index) const
{
	int first = index * getStride();
	return new SFColor(_value[first], _value[first + 1], _value[first + 2]);
}

void MFColor::setSFValue(int index, FieldValue *value)
{
	if (value->getType() != SFCOLOR) return;
	SFColor *v = (SFColor *) value;
	int first = index * getStride();

	for (int i = 0; i < 3; i++)
		_value[first + i] = v->getValue(i);
}
