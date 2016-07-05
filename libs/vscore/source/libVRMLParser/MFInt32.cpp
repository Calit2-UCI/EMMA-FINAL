#include "stdafx.h"
#include "MFInt32.h"
#include "SFInt32.h"

MFInt32::MFInt32()
{
}

MFInt32::MFInt32(const int *values, int len)
{
	for (int i = 0; i < len; i++) _value.append(values[i]);
}

MFInt32::MFInt32(const MFInt32 &value)
{
	int length = value.getSize();

	for (int i = 0; i < length; i++) _value.append(value._value[i]);
}

void MFInt32::write(FILE *f, int indent) const
{
	fprintf(f, "[");

	int size = getSize();

	if (size != 0)
	{
		for (int i = 0; i < size; i++)
		{
			if ((i % 20) == 0)
			{
				fprintf(f, "\n");
				indentf(f, indent + 1);
			}

			if (i == (size - 1))
				fprintf(f, "%d", _value[i]);
			else
				fprintf(f, "%d, ", _value[i]);
		}

		fprintf(f, "\n");
		indentf(f, indent);
	}

	fprintf(f, "]\n");
}

bool MFInt32::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	MFInt32 *v = (MFInt32 *) value;
	if (getSize() != v->getSize()) return false;

	int size = getSize();
	for (int i = 0; i < size; i++)
		if (_value[i] != v->_value[i]) return false;

	return true;
}

FieldValue * MFInt32::getSFValue(int index) const
{
	return new SFInt32(_value[index]);
}

void MFInt32::setSFValue(int index, FieldValue *value)
{
	if (value->getType() != SFINT32) return;
	SFInt32 *v = (SFInt32 *) value;
	_value[index] = v->getValue();
}
