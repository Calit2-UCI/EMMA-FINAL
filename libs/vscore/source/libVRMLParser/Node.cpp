#include "stdafx.h"
#include "Node.h"
#include "Proto.h"
#include "FieldValue.h"
#include "Field.h"
#include <assert.h>
#include "SFNode.h"
#include "MFNode.h"
#include "EventIn.h"
#include "EventOut.h"
#include "Scene.h"
#include "NodeIndexedFaceSet.h"

#include "MFString.h"
#include "SFString.h"

Node::Node(Scene * scene, Proto * proto)
{
	_scene = scene;
	_proto = proto;
	_numEventIns = _proto->getNumEventIns();
	_numEventOuts = _proto->getNumEventOuts();
	_numFields = _proto->getNumFields();

	_fields = new FieldValue *[_numFields];
	for (int i = 0; i < _numFields; i++)
	{
		FieldValue *value = proto->getField(i)->getDefault()->copy();
		_fields[i] = value;
	}

	_inputs = new SocketList();
	_outputs = new SocketList();
	_refs = 1;
}

Node::Node(const Node & node)
{
	_scene = node._scene;
	_proto = node._proto;
	_numEventIns = node._numEventIns;
	_numEventOuts = node._numEventOuts;
	_numFields = node._numFields;

	_fields = new FieldValue *[_numFields];
	for (int i = 0; i < _numFields; i++)
	{
		FieldValue *value = node._fields[i]->copy();
		_fields[i] = value;
	}

	_name = node._name;
	_inputs = new SocketList(*node._inputs);
	_outputs = new SocketList(*node._outputs);
	SocketList *parents = new SocketList(node._parents);
	_parents.appendList(parents);
	delete parents;
	_flags = node._flags;
	_refs = 1;
}

Node::~Node()
{
	for (int i = 0; i < _numFields; i++) _fields[i]->unref();
	delete [] _fields;

	delete _inputs;
	delete _outputs;
}

FieldValue * Node::getField(int index) const
{
	assert((index >= 0) && (index < _numFields));
	return _fields[index];
}

void Node::setField(int index, FieldValue * value)
{
	assert((index >= 0) && (index < _numFields));
	if (_fields[index] != NULL) _fields[index]->unref();

	if (value->getType() == SFNODE)
	{
		SFNode *v = (SFNode *) value;
		Node *node = v->getValue();
		if (node) node->addParent(this, index);
	}
	if (value->getType() == MFNODE)
	{
		MFNode *v = (MFNode *) value;
		for (int i = 0; i < v->getSize(); i++)
		{
			Node *node = v->getValue(i);
			if (node) node->addParent(this, index);
		}
	}

	_fields[index] = value;
}

const String Node::toString()
{
	return _proto->getName();
}

int Node::findChild(Node * child, int field) const
{
	assert((field >= 0) && (field < _numFields));
	int index = -1;

	if (_fields[field]->getType() == SFNODE)
	{
		SFNode *value = (SFNode *) _fields[field];
		if (value->getValue() == child) index = 0;
	}
	else if (_fields[field]->getType() == MFNODE)
	{
		MFNode *value = (MFNode *) _fields[field];
		NodeList *list = value->getValue();

		for (int i = 0; i < list->size(); i++)
		{
			if (list->get(i) == child)
			{
				index = i;
				break;
			}
		}
	}
	return index;
}

bool Node::hasAncestor(Node * node) const
{
	List<Socket>::Iterator *i;

	for (i = _parents.first(); i != NULL; i = i->next())
	{
		Node *ancestor = i->item()._node;
		if (ancestor == node) return true;
		else
			if (ancestor->hasAncestor(node) == true)
				return true;
	}

	return false;
}

int Node::lookupEventIn(const String & name) const
{
	return _proto->lookupEventIn(name);
}

int Node::lookupEventOut(const String & name) const
{
	return _proto->lookupEventOut(name);
}

// Input contains source node that creates eventOut
// and the corresponding eventIn
void Node::addInput(int eventIn, Node * src, int eventOut)
{
	Socket *input = new Socket(src, eventIn);
	List<Socket>::Iterator *i;
	bool alreadyExists = false;
	// checking whether this input already exists in the
	// _inputs SocketList
	for (i = _inputs->first(); i != NULL; i = i->next())
	{
		if (i->item() == *input)
		{
			alreadyExists = true;
			break;
		}
	}
	
	if (alreadyExists == false)	_inputs->append(*input);
}

// Output contains eventOut and destination node that receives 
// the corresponding eventIn
void Node::addOutput(int eventOut, Node * dst, int eventIn)
{
	Socket *output = new Socket(dst, eventOut);
	List<Socket>::Iterator *i;
	bool alreadyExists = false;
	// checking whether this output already exists in the
	// _outputs SocketList
	for (i = _outputs->first(); i != NULL; i = i->next())
	{
		if (i->item() == *output)
		{
			alreadyExists = true;
			break;
		}
	}
	
	if (alreadyExists == false)	_outputs->append(*output);
}

void Node::removeInput(int eventIn, Node * src, int eventOut)
{
	Socket *input = new Socket(src, eventIn);
	List<Socket>::Iterator *i;

	for (i = _inputs->first(); i != NULL; i = i->next())
	{
		if (i->item() == *input)
		{
			_inputs->remove(i);
			break;
		}
	}
	delete input;
}

void Node::removeOutput(int eventOut, Node * dst, int eventIn)
{
	Socket *output = new Socket(dst, eventOut);
	List<Socket>::Iterator *i;

	for (i = _outputs->first(); i != NULL; i = i->next())
	{
		if (i->item() == *output)
		{
			_outputs->remove(i);
			break;
		}
	}
	delete output;
}

void update()
{
}

void Node::addParent(Node * parent, int field)
{
	Socket *socket = new Socket(parent, field);
	List<Socket>::Iterator *i;
	bool alreadyExists = false;

	// checking whether this socket already exists in the
	// _parents SocketList
	for (i = _parents.first(); i != NULL; i = i->next())
	{
		if (i->item() == *socket)
		{
			alreadyExists = true;
			break;
		}
	}

	if (alreadyExists == false)	_parents.append(*socket);
	delete socket;
}

void Node::removeParent(Node * parent, int field)
{
	Socket *socket = new Socket(parent, field);
	List<Socket>::Iterator *i;

	for (i = _parents.first(); i != NULL; i = i->next())
	{
		if (i->item() == *socket)
		{
			_parents.remove(i);
			break;
		}
	}
	delete socket;
}

bool Node::needsDEF() const
{
	String s;

	if (s == _name) return true;
	else return false;
}

void Node::drawHandle(const Vec3f & pos)
{
}

void Node::update()
{
}

void Node::write(FILE * f, int indent)
{
	bool isDEF = false;

	// writing Proto of this node if it is not a standard
	// node and if this Proto has not already been written
	if ((getType() == -1) && (_scene->getFlag(toString()) == false))
	{
		_proto->write(f, indent);
		fprintf(f, "\n");
		_scene->setFlag(toString(), true);
	}

	if (needsDEF() == true)
	{
		_name = _scene->generateUniqueNodeName(this);
		isDEF = true;
	}
	else
	{
		if (_scene->getDEFFlag(_name) == true)
		{
			fprintf(f, "USE %s \n", (const char *) _name);
		}
		else
		{
			_scene->setDEFFlag(_name, true);
			isDEF = true;
		}
	}

	if (isDEF == true)
	{
		fprintf(f, "DEF %s ", (const char *) _name);
		fprintf(f, "%s {\n", (const char *) toString());
		writeFields(f, indent + 1);
		writeRoutes(f, indent + 1);
		indentf(f, indent);
		fprintf(f, "}\n");
	}
}

void Node::writeFields(FILE * f, int indent)
{
	for (int i = 0; i < _numFields; i++)
	{
		Field *field = _proto->getField(i);
		FieldValue *value = getField(i);

		if (value->equals(field->getDefault()) == false)
		{
			indentf(f, indent);
			fprintf(f, "%s ", (const char *) field->getName());
			value->write(f, indent);
		}
	}
}

// this method maybe isn't correcty implemented,
// we do not use ROUTEs so it is not tested
void Node::writeRoutes(FILE * f, int indent) const
{
	// searching for inputs and corresponding outputs
	// of this node's children
	for (int j = 0; j < _numFields; j++)
	{
		if (_fields[j]->getType() == SFNODE)
		{
			SFNode *node = (SFNode *) _fields[j];
			if (node->getValue() == NULL) continue;
			SocketList *inputs = node->getValue()->_inputs;
			List<Socket>::Iterator *i1;
			for (i1 = _inputs->first(); i1 != NULL; i1 = i1->next())
			{
				fprintf(f, "\n");
				indentf(f, indent);

				Node *dst = node->getValue();
				EventIn *eventIn = dst->_proto->getEventIn(i1->item()._index);
				Node *src = i1->item()._node;
				EventOut *eventOut;
				List<Socket>::Iterator *i2;
				for (i2 = src->_outputs->first(); i2 != NULL; i2 = i2->next())
				{
					if (i2->item()._node == dst)
					{
						eventOut = 
							src->_proto->getEventOut(i2->item()._index);
						if (eventIn->getType() == eventOut->getType()) break;
					}
				}

				fprintf(f, "ROUTE %s.%s TO %s.%s",
					(const char *) src->toString(),
					(const char *) eventOut->getName(),
					(const char *) dst->toString(),
					(const char *) eventIn->getName());
			}
		}
		else if (_fields[j]->getType() == MFNODE)
		{
			MFNode *nodes = (MFNode *) _fields[j];
			NodeList *list = nodes->getValue();
			for (int k = 0; k < list->size(); k++)
			{
				Node *dst = list->get(k);
				SocketList *inputs = dst->_inputs;
				List<Socket>::Iterator *i1;
				for (i1 = _inputs->first(); i1 != NULL; i1 = i1->next())
				{
					fprintf(f, "\n");
					indentf(f, indent);

					EventIn *eventIn =
						dst->_proto->getEventIn(i1->item()._index);
					Node *src = i1->item()._node;
					EventOut *eventOut;
					List<Socket>::Iterator *i2;
					for (i2 = src->_outputs->first(); i2 != NULL;
					i2 = i2->next())
					{
						if (i2->item()._node == dst)
						{
							eventOut = 
								src->_proto->getEventOut(i2->item()._index);
							if (eventIn->getType() == eventOut->getType())
								break;
						}
					}

					fprintf(f, "ROUTE %s.%s TO %s.%s",
						(const char *) src->toString(),
						(const char *) eventOut->getName(),
						(const char *) dst->toString(),
						(const char *) eventIn->getName());
				}
			}
		}
	}
}
