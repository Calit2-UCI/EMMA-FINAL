#include "stdafx.h"
#include "SFNode.h"
#include "Node.h"
#include "MFNode.h"
#include "NodeList.h"

SFNode::SFNode(Node *value)
{
	if (value == NULL) _value = NULL;
	else _value = value;
}

SFNode::SFNode(const SFNode &other)
{
	if (other._value == NULL) _value = NULL;
	else _value = other._value->copy();
}

SFNode::~SFNode()
{
	if (_value != NULL) _value->unref();
}

void SFNode::write(FILE *f, int indent) const
{
	if (_value == NULL) fprintf(f, "NULL\n");
	else _value->write(f, indent);
}

bool SFNode::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL)	return false;
	if (getType() != value->getType()) return false;

	SFNode *v = (SFNode *) value;
	Node *n = v->getValue();
	if (_value == NULL)
	{
		if (n == NULL) return true;
	}
	else
	{
		if (n == NULL)
		{
			return false;
		}
		else
		{
			if (_value->getType() != n->getType()) return false;
			if (_value->getNumFields() != n->getNumFields()) return false;

			int j;
			for (j = 0; j < _value->getNumFields(); j++)
			{
				bool isEqual = 
					_value->getField(j)->equals(n->getField(j));
				if (isEqual == false) return false;
			}
		}
	}

	return true;
}

FieldValue * SFNode::addNode(Node *node) const
{
/*	NodeList *value = new NodeList(node->copy());
	MFNode *newValue = new MFNode(value);
	delete value;
	return newValue;
*/
	return new SFNode(node->copy());
}

FieldValue * SFNode::removeNode(Node *node) const
{
	SFNode *newValue = new SFNode(node->copy());
	if (equals(newValue) == true)
	{
		delete newValue;
		return new SFNode(NULL);
	}
	else
	{
		delete newValue;
		return new SFNode(_value->copy());
	}
	/*
	bool isEqual = true;

	if (_value->getType() != node->getType())
		isEqual = false;
	else
		if (_value->getNumFields() != node->getNumFields())
			isEqual = false;
		else
		{
			int j;
			for (j = 0; j < _value->getNumFields(); j++)
			{
				bool isEqualField = 
					_value->getField(j)->equals(node->getField(j));
				if (isEqualField == false)
				{
					isEqual = false;
					break;
				}
			}
		}

	if (isEqual = true)
		return new SFNode(NULL);
	else
		return new SFNode(_value->copy());
	*/
}

