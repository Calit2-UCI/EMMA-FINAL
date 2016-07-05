#include "stdafx.h"
#include "MFString.h"
#include "SFString.h"

MFString::MFString()
{
	_value = new StringArray();
}

MFString::MFString(const MFString &string)
{
	_value = new StringArray();
	int size = string.getSize();

	for (int i = 0; i < size; i++)
	{
		String *s = new String(string._value->get(i));
		_value->append(*s);
		delete s;
	}
}

MFString::MFString(StringArray *value)
{
	_value = new StringArray();
	int size = value->size();

	for (int i = 0; i < size; i++)
	{
		String *s = new String(value->get(i));
		_value->append(*s);
		delete s;
	}
}

MFString::MFString(String value)
{
	_value = new StringArray();
	String *s = new String(value);
	_value->append(*s);
	delete s;
}

MFString::~MFString()
{
	delete _value;
}

void MFString::write(FILE *f, int indent) const
{
	fprintf(f, "[");

	int size = getSize();

	if (size != 0)
	{
		for (int i = 0; i < size; i++)
		{
			if ((i % 3) == 0)
			{
				fprintf(f, "\n");
				indentf(f, indent + 1);
			}

			const String &s = _value->get(i);
			const char *str = (const char *) s;
			int length = s.length();

			fprintf(f, "\"");
			for (int j = 0; j < length; j++)
			{
				if ((str[j] == '\\') || (str[j] == '\"')) fprintf(f, "\\");
				fprintf(f, "%c", str[j]);
			}

			if (i == (size - 1))
				fprintf(f, "\"");
			else
				fprintf(f, "\", ");
		}

		fprintf(f, "\n");
		indentf(f, indent);
	}

	fprintf(f, "]\n");
}

bool MFString::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	MFString *v = (MFString *) value;
	if (getSize() != v->getSize()) return false;

	int size = getSize();
	for (int i = 0; i < size; i++)
		if (!(_value->get(i) == v->_value->get(i))) return false;

	return true;
}

FieldValue * MFString::getSFValue(int index) const
{
	return new SFString(_value->get(index));
}

void MFString::setSFValue(int index, FieldValue *value)
{
	if (value->getType() != SFSTRING) return;
	SFString *v = (SFString *) value;
	String *string = new String(v->getValue());
	_value->set(index, *string);
	delete string;
}
