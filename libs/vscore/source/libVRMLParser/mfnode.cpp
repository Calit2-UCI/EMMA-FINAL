#include "stdafx.h"
#include "MFNode.h"
#include "SFNode.h"

MFNode::MFNode()
{
	_value = new NodeList();
}

MFNode::MFNode(NodeList *value)
{
	_value = new NodeList();

	for (int i = 0; i < value->size(); i++)
	{
		_value->append(value->get(i)->copy());
	}
}

MFNode::MFNode(const MFNode &other)
{
	_value = new NodeList();
	for (int i = 0; i < other.getSize(); i++)
	{
		_value->append(other.getValue(i)->copy());
	}
}

MFNode::~MFNode()
{
	int size = _value->size();

	for (int i = 0; i < size; i++)
	{
		Node *node = _value->get(0);
		_value->remove(0, 0);
		node->unref();
	}

	delete _value;
}

void MFNode::write(FILE *f, int indent) const
{
	fprintf(f, "[");

	int size = _value->size();

	if (size != 0)
	{
		fprintf(f, "\n");
		for (int i = 0; i < _value->size(); i++)
		{
			indentf(f, indent + 1);
			getValue(i)->write(f, indent + 1);
		}
		indentf(f, indent);
	}

	fprintf(f, "]\n");
}

bool MFNode::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	MFNode *v = (MFNode *) value;
	if (getSize() != v->getSize()) return false;
	NodeList *list = v->getValue();
	for (int i = 0; i < getSize(); i++)
	{
		Node *node = _value->get(i);
		Node *n = list->get(i);

		if (node->getType() != n->getType()) return false;
		if (node->getNumFields() != n->getNumFields()) return false;

		for (int j = 0; j < node->getNumFields(); j++)
		{
			bool isEqual = 
				node->getField(j)->equals(n->getField(j));
			if (isEqual == false) return false;
		}
	}
	return true;
}

FieldValue * MFNode::getSFValue(int index) const
{
	assert((index >= 0) && (index < _value->size()));
	Node *node = getValue(index);
	return new SFNode(node->copy());
}

void MFNode::setSFValue(int index, FieldValue *value)
{
	if (value->getType() == SFNODE)
	{
		SFNode *v = (SFNode *) value;
		Node *node = v->getValue();

		if (node == NULL) _value->set(index, NULL);
		else _value->set(index, node->copy());
	}
}

FieldValue * MFNode::addNode(Node *node) const
{
	MFNode *newValue = new MFNode(*this);
	newValue->_value->append(node);
	return newValue;
}

FieldValue * MFNode::removeNode(Node *node) const
{
	MFNode *newValue = new MFNode(*this);
	int index = _value->find(node);
	newValue->_value->remove(index, index);
	return newValue;
}
