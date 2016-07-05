#include "stdafx.h"
#include "FieldValue.h"

void indentf(FILE *f, int indent)
{
	int i;

	for (i = 0; i < indent; i++)
	{
		fprintf(f, " ");
	}
}

FieldValue::FieldValue()
{
	_refs = 1;
}

FieldValue::FieldValue(const FieldValue &value)
{
	_refs = 1;
}

FieldValue	* FieldValue::addNode(Node *node) const
{
	return (FieldValue *)this;
}

FieldValue	* FieldValue::removeNode(Node *node) const
{
	return (FieldValue *)this;
}
