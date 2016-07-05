#include "stdafx.h"
#include "MFTime.h"
#include "stdafx.h"

MFTime::MFTime()
{
}

MFTime::MFTime(const MFTime &value)
{
	int length = value._value.size();

	for (int i = 0; i < length; i++) _value.append(value._value[i]);
}

MFTime::MFTime(const double values[], int len)
{
	for (int i = 0; i < len; i++) _value.append(values[i]);
}

MFTime::MFTime(const int values[], int len)
{
	for (int i = 0; i < len; i++) _value.append(values[i]);
}

void MFTime::setSFValue(int index, FieldValue *value)
{
	if (value->getType() == SFTIME)
	{
		SFTime *v = (SFTime *) value;
		_value.set(index, v->getValue());
	}
}

void MFTime::write(FILE *f, int indent) const
{
	fprintf(f, "[");

	int size = _value.size();

	if (size != 0)
	{
		fprintf(f, "\n");
		indentf(f, indent + 1);

		for (int i = 0; i < size; i++)
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

		fprintf(f, "\n");
		indentf(f, indent);
	}

	fprintf(f, "]\n");
}

void MFTime::clamp(float min, float max)
{
	int size = _value.size();

	for (int i = 0; i < size; i++)
		_value[i] = CLAMP(_value[i], min, max);
}

bool MFTime::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	MFTime *v = (MFTime *) value;
	if (_value.size() != v->_value.size()) return false;

	int size = _value.size();
	for (int i = 0; i < size; i++)
		if (_value[i] != v->_value[i]) return false;

	return true;
}

bool MFTime::equals(const MFTime *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (_value.size() != value->_value.size()) return false;

	int size = _value.size();
	for (int i = 0; i < size; i++)
		if (_value[i] != value->_value[i]) return false;

	return true;
}
