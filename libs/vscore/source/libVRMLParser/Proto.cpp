#include "stdafx.h"
#include "Proto.h"
#include "ExposedField.h"
#include "EventIn.h"
#include "EventOut.h"
#include "Field.h"
#include "Scene.h"
#include "FieldValue.h"

Proto::Proto(Scene *scene, const String &name)
{
	_scene = scene;
	_name = name;
	_primaryNode = NULL;
	_nodes = NULL;
}

Proto::~Proto()
{
	int size = _eventIns.size();
	EventIn *eventIn;

	for (int i = 0; i < size; i++)
	{
		eventIn = _eventIns.get(0);
		_eventIns.remove(0, 0);
		delete eventIn;
	}

	size = _eventOuts.size();
	EventOut *eventOut;

	for (int i = 0; i < size; i++)
	{
		eventOut = _eventOuts.get(0);
		_eventOuts.remove(0, 0);
		delete eventOut;
	}

	size = _fields.size();
	Field *field;

	for (int i = 0; i < size; i++)
	{
		field = _fields.get(0);
		_fields.remove(0, 0);
		delete field;
	}

	size = _exposedFields.size();
	ExposedField *exposedField;

	for (int i = 0; i < size; i++)
	{
		exposedField = _exposedFields.get(0);
		_exposedFields.remove(0, 0);
		delete exposedField;
	}

	if (_primaryNode != NULL) _primaryNode->unref();

	if (_nodes != NULL)
	{
		int size = _nodes->size();

		for (int i = 0; i < size; i++)
		{
			Node *node = _nodes->get(0);
			_nodes->remove(0, 0);
			node->unref();
		}

		delete _nodes;
	}
}

void	Proto::addElement(Element *element)
{
	switch (element->getElementType())
	{
	case EL_FIELD_DEF:
		{
			Field *field = (Field *) element;
			const char *name = field->getName();

			if (lookupField(name) == INVALID_INDEX)
				_fields.append(field);
			else
				_scene->errorf((const char *) (String(
				"PROTO field already declared - ") += name),
					"\n", NULL);
			break;
		}
	case EL_EVENT_IN:
		{
			EventIn *eventIn = (EventIn *) element;
			const char *name = eventIn->getName();

			if (lookupEventIn(name) == INVALID_INDEX)
				_eventIns.append(eventIn);
			else
				_scene->errorf((const char *) (String(
					"PROTO eventIn already declared - ") += name),
					"\n", NULL);
			break;
		}

	case EL_EVENT_OUT:
		{
			EventOut *eventOut = (EventOut *) element;
			const char *name = eventOut->getName();

			if (lookupEventOut(name) == INVALID_INDEX)
				_eventOuts.append(eventOut);
			else
				_scene->errorf((const char *) (String(
					"PROTO eventOut already declared - ") += name),
					"\n", NULL);
			break;
		}
	case EL_EXPOSED_FIELD:
		{
			ExposedField *exposedField = (ExposedField *) element;
			const char *name = exposedField->getName();

			if (lookupExposedField(name) == INVALID_INDEX)
				addExposedField(exposedField);
			else
				_scene->errorf((const char *) (String(
					"PROTO exposedField already declared - ") +=
					name), "\n", NULL);
			break;
		}
		// EL_UNKNOWN
	default:
		{
			_scene->errorf("Unknown element type.\n", NULL);
		}
	}
}

void	Proto::addExposedField(ExposedField *exposedField)
{
	_exposedFields.append(exposedField);

	exposedField->getValue()->ref();
	// field's flags are set to FF_HIDDEN because this is a hidden field
	Field *field = new Field(exposedField->getType(), exposedField->getName(),
		exposedField->getValue(), NULL,	exposedField->getMin(),
		exposedField->getMax(),	exposedField->getNodeType(),
		exposedField->getFlags() | FF_HIDDEN, exposedField->getStrings());
	exposedField->setField(_fields.size());
	_fields.append(field);

	String *eventInName = new String("set_");
	*eventInName += exposedField->getName();
	if (lookupSimpleEventIn(*eventInName) == INVALID_INDEX)
	{
		exposedField->setEventIn(_eventIns.size());
		addEventIn(exposedField->getType(), *eventInName);
	}
	delete eventInName;

	String *eventOutName = new String(exposedField->getName());
	*eventOutName += "_changed";
	if (lookupSimpleEventOut(*eventOutName) == INVALID_INDEX)
	{
		exposedField->setEventOut(_eventOuts.size());
		addEventOut(exposedField->getType(), *eventOutName);
	}
	delete eventOutName;
}

void	Proto::addEventIn(int fieldType, const String &name)
{
	EventIn *eventIn = new EventIn(fieldType, name);
	_eventIns.append(eventIn);
}

void	Proto::addEventOut(int fieldType, const String &name)
{
	EventOut *eventOut = new EventOut(fieldType, name);
	_eventOuts.append(eventOut);
}

void	Proto::addExposedField(int fieldType, const String &name,
							   FieldValue *defaultValue, float min,
							   float max)
{
	ExposedField *exposedField = new ExposedField(fieldType, name,
		defaultValue, min, max);
	_exposedFields.append(exposedField);
}

void	Proto::addExposedField(int fieldType, const String &name,
							   FieldValue *defaultValue, int nodeType)
{
	ExposedField *exposedField = new ExposedField(fieldType, name,
		defaultValue, -FLT_MAX, FLT_MAX, nodeType);
	_exposedFields.append(exposedField);
}

void	Proto::addExposedField(int fieldType, const String &name,
							   FieldValue *defaultValue, int flags,
							   const char **strings)
{
	ExposedField *exposedField = new ExposedField(fieldType, name,
		defaultValue, -FLT_MAX, FLT_MAX, NODE_ANY, flags, strings);
	_exposedFields.append(exposedField);
}

void	Proto::addField(int fieldType, const String &name,
						FieldValue *defaultValue, float min, float max)
{
	Field *field = new Field(fieldType, name, defaultValue, NULL,
		min, max);
	_fields.append(field);
}

void	Proto::addField(int fieldType, const String &name,
						FieldValue *defaultValue, int nodeType)
{
	Field *field = new Field(fieldType, name, defaultValue, NULL,
		-FLT_MAX, FLT_MAX, nodeType);
	_fields.append(field);
}

void	Proto::addField(int fieldType, const String &name,
						FieldValue *defaultValue, int flags,
						const char **strings)
{
	Field *field = new Field(fieldType, name, defaultValue, NULL,
		-FLT_MAX, FLT_MAX, NODE_ANY, flags, strings);
	_fields.append(field);
}


Node	*Proto::create(Scene *scene)
{
	return new ProtoNode(scene, this);
}

void	Proto::define(Node *primaryNode, NodeList *nodes)
{
	_primaryNode = primaryNode;
	_nodes = nodes;
}

int		Proto::lookupEventIn(const String &name) const
{
	int index = lookupSimpleEventIn(name);

	if (index == INVALID_INDEX)
	{
		index = lookupExposedField(name);

		if (index != INVALID_INDEX)
		{
			index = _exposedFields[index]->getEventIn();
		}
	}
	return index;
}

int		Proto::lookupEventOut(const String &name) const
{
	int index = lookupSimpleEventOut(name);

	if (index == INVALID_INDEX)
	{
		index = lookupExposedField(name);

		if (index != INVALID_INDEX)
		{
			index = _exposedFields[index]->getEventOut();
		}
	}
	return index;
}

int		Proto::lookupField(const String &name) const
{
	int index = INVALID_INDEX;
	int i;

	for (i = 0; i < _fields.size(); i++)
	{
		if (_fields[i]->getName() == name)
		{
			index = i;
			break;
		}
	}
	return index;
}

int		Proto::lookupExposedField(const String &name) const
{
	int index = INVALID_INDEX;
	int i;

	for (i = 0; i < _exposedFields.size(); i++)
	{
		if (_exposedFields[i]->getName() == name)
		{
			index = i;
			break;
		}
	}
	return index;
}

int		Proto::lookupSimpleEventIn(const String &name) const
{
	int index = INVALID_INDEX;
	int i;

	for (i = 0; i < _eventIns.size(); i++)
	{
		if (_eventIns[i]->getName() == name)
		{
			index = i;
			break;
		}
	}
	return index;
}

int		Proto::lookupSimpleEventOut(const String &name) const
{
	int index = INVALID_INDEX;
	int i;

	for (i = 0; i < _eventOuts.size(); i++)
	{
		if (_eventOuts[i]->getName() == name)
		{
			index = i;
			break;
		}
	}
	return index;
}

void	Proto::write(FILE *f, int indent) const
{
//	Array<int> hiddenFields(getNumFields());
//	Array<int> hiddenEventIns(getNumEventIns());
//	Array<int> hiddenEventOuts(getNumEventOuts());
	Array<int> hiddenFields;
	Array<int> hiddenEventIns;
	Array<int> hiddenEventOuts;
	int i;

	// initializing arrays (they will contain flags that show 
	// which fields, eventIns, eventOuts are hidden)
	for (i = 0; i < getNumFields(); i++)
	{
		hiddenFields.append(0);
	}
	for (i = 0; i < getNumEventIns(); i++)
	{
		hiddenEventIns.append(0);
	}
	for (i = 0; i < getNumEventOuts(); i++)
	{
		hiddenEventOuts.append(0);
	}

	// looking for hidden fields, eventIns and eventOuts
	for (i = 0; i < _exposedFields.size(); i++)
	{
		hiddenFields[_exposedFields[i]->getField()] = 1;
		int eventIn = _exposedFields[i]->getEventIn();
		if (eventIn != INVALID_INDEX)
			hiddenEventIns[eventIn] = 1;
		int eventOut = _exposedFields[i]->getEventOut();
		if (eventOut != INVALID_INDEX)
			hiddenEventOuts[eventOut] = 1;
	}

	// writing exposedFields and fields, eventIns, eventOuts
	// that aren't hidden
	int indentation = indent + 2;
	indentf(f, indent);
	fprintf(f, "PROTO %s [\n", (const char *) getName());
	for (i = 0; i < getNumEventIns(); i++)
	{
		if (hiddenEventIns[i] == 0)
			_eventIns[i]->write(f, indentation);
	}
	for (i = 0; i < _exposedFields.size(); i++)
	{
		_exposedFields[i]->write(f, indentation);
	}
	for (i = 0; i < getNumFields(); i++)
	{
		if (hiddenFields[i] == 0)
			_fields[i]->write(f, indentation);
	}
	for (i = 0; i < getNumEventOuts(); i++)
	{
		if (hiddenEventOuts[i] == 0)
			_eventOuts[i]->write(f, indentation);
	}

	// writing nodes
	indentation = indent + 1;
	indentf(f, indent);
	fprintf(f, "]\n");
	indentf(f, indent);
	fprintf(f, "{\n");
	indentf(f, indentation);
	_primaryNode->write(f, indentation);
	for (i = 0; i < _nodes->size(); i++)
	{
		indentf(f, indentation);
		_nodes->get(i)->write(f, indentation);
	}

	// routes should be written here after nodes

	indentf(f, indent);
	fprintf(f, "}\n");
}


ProtoNode::ProtoNode(Scene *scene, Proto *proto):
	Node(scene, proto)
{
}
