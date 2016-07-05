#include "stdafx.h"
#include "MFFloat.h"

MFFloat::MFFloat(int stride)
{
}

MFFloat::MFFloat(const float values[], int len, int stride)
{
	int length = len;

	if (stride != 1) length *= stride;

	for (int i = 0; i < length; i++) _value.append(values[i]);
}

MFFloat::MFFloat(const MFFloat &value)
{
	int length = value.getSize();

	for (int i = 0; i < length; i++) _value.append(value._value[i]);
}

void MFFloat::write(FILE *f, int indent) const
{
	fprintf(f, "[");

	int size = getSize();
	int stride = getStride();

	if (size != 0)
	{
		fprintf(f, "\n");
		indentf(f, indent + 1);

		for (int i = 0; i < size; i++)
		{
			if (stride == 1)
			{
				if ((i % 10) == 0)
				{
					fprintf(f, "\n");
					indentf(f, indent + 1);
				}

				if (i == (size - 1))
					fprintf(f, "%g", _value[i]);
				else
					fprintf(f, "%g, ", _value[i]);
			}
			else
			{
				if (((i % stride) - (stride - 1)) == 0)
				{
					if (i == (size - 1))
					{
						fprintf(f, "%g", _value[i]);
					}
					else
					{
						fprintf(f, "%g,\n", _value[i]);
						indentf(f, indent + 1);
					}
				}
				else
				{
					fprintf(f, "%g ", _value[i]);
				}
			}
		}

		fprintf(f, "\n");
		indentf(f, indent);
	}

	fprintf(f, "]\n");
}

bool MFFloat::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	MFFloat *v = (MFFloat *) value;
	if (getSize() != v->getSize()) return false;

	int size = getSize();
	for (int i = 0; i < size; i++)
		if (_value[i] != v->_value[i]) return false;

	return true;
}

void MFFloat::clamp(float min, float max)
{
	int size = getSize();

	for (int i = 0; i < size; i++)
		_value[i] = CLAMP(_value[i], min, max);
}

void MFFloat::setSFValue(int index, FieldValue *value)
{
	if (value->getType() == SFFLOAT)
	{
		SFFloat *v = (SFFloat *) value;
		_value.set(index, v->getValue());
	}
}

bool MFFloat::equals(const MFFloat *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getSize() != value->getSize()) return false;

	int size = getSize();
	for (int i = 0; i < size; i++)
		if (_value[i] != value->_value[i]) return false;

	return true;
}
