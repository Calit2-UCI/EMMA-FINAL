#ifndef _PROTO_H
#define _PROTO_H

#ifndef FLT_MAX
#include <limits.h>
#endif
#include <float.h>

#ifndef _ARRAY_H
#include "Array.h"
#endif
#ifndef KS_STRING_H
#include "ksString.h"
#endif
#ifndef _NODE_H
#include "Node.h"
#endif

#define INVALID_INDEX (-1)




class Element;
class EventIn;
class EventOut;
class Field;
class ExposedField;
class FieldValue;
class Scene;
class Node;
class NodeList;
typedef Array<String> StringArray;

/**Initialises the Proto node as every node is considered to be a Proto
 node with a ProtoNode part (VRML Proto definition) and a Node part (VRML ProtoBody).
\ingroup VRMLParser
 */
class Proto {
public:
	/**Proto default constructor.
	@param *scene the current Scene.
	@param &name the name of the Proto.*/
						Proto(Scene *scene, const String &name);
    virtual				~Proto();

	/**Creates a Node (VRML ProtoBody). As the Proto(VRML Proto definition) is initialised in this class. 
	@return a ProtoNode(scene,this) object which inherits from Node. 
	e.g. the ProtoGroup::create will return a NodeGroup, the ProtoLOD::create() a NodeLOD etc.*/
    virtual Node	    *create(Scene *scene);
	
	Node				*getPrimaryNode(){return _primaryNode;};

	/**
	@param index the index in the Array containing EventIn* */
    EventIn				*getEventIn(int index) const { return _eventIns[index]; }
	
	/**
	@param index the index in the Array containing EventOut* */
    EventOut		    *getEventOut(int index) const { return _eventOuts[index]; }
	
	/**Returns the default specified Field
	@param index the index in the Array containing Field* */
    Field				*getField(int index) const { return _fields[index]; }

	/**
	@param index the index in the Array containing Field* */
    ExposedField	    *getExposedField(int index) const { return _exposedFields[index]; }

	/**
	@return the size of the Fields Array*/
    int					getNumFields() const { return _fields.size(); }

	/**
	@return the size of the EventIns Array*/
    int					getNumEventIns() const { return _eventIns.size(); }

	/**
	@return the size of the EventOuts Array*/
    int					getNumEventOuts() const { return _eventOuts.size(); }

	/**In case lookupSimpleEventIn fails, looks for an exposedField. Then looks up the 
	corresponding eventIn.
	@param name the name of the EventIn to find it's index
	@return the index of the found EventIn in the EventIn Array.*/
    int					lookupEventIn(const String &name) const;

	/**In case lookupSimpleEventOut fails, looks for an exposedField. Then looks up the 
	corresponding eventOut.
	@param name the name of the EventOut to find it's index
	@return the index of the found EventOut in the EventOut Array.*/
    int					lookupEventOut(const String &name) const;

	/**Looks if a Field is contained in the Field Array.
	@param name the name of the Field to find it's index
	@return the index of the found Field in the Field Array OR returns INVALID_INDEX.*/
    int					lookupField(const String &name) const;

	/**Looks if a ExposedField is contained in the ExposedField Array.
	@param name the name of the ExposedField to find it's index
	@return the index of the found ExposedField in the ExposedField Array OR returns INVALID_INDEX.*/
    int					lookupExposedField(const String &name) const;

	/**Gets the name of this Proto.*/
    const String	    &getName() const { return _name; }

	/**Outputs the contents of the Proto to a File*/
    void				write(FILE *f, int indent) const;
	
	/**Adds the parsed VRML element to the appropriate Array. Appends to _fields,_eventIns,
	_eventOuts and calls addExposedField(ExposedField *).*/
    void				addElement(Element *element);

	/**Adds an exposedField to the corresponding Array. This is a special case as an exposed fiels
	is both an eventIn and eventOut. So after adding it to the _exposedFields Array, it adds a hiiden 
	Field in _fields,a hidden EventIn and a hidden EventOut.*/
    void				addExposedField(ExposedField *exposedField);

	/**Defines the primary node and the children nodes of the Proto.*/
    void				define(Node *primaryNode, NodeList *nodes);

protected:
	/**Adds the specified field(name) and its value in the Field Array. Field has min,max values*/
    void				addField(int fieldType, const String &name,
						FieldValue *defaultValue=NULL,
							float min=-FLT_MAX,float max=FLT_MAX);

	/**Adds the specified field(name) and its value in the Field Array. Field is a Node*/
    void				addField(int fieldType, const String &name,
							FieldValue *defaultValue,
							int nodeType);
	/**Adds the specified field(name) and its value in the Field Array. Field is String(s)*/
    void				addField(int fieldType, const String &name,
							FieldValue *defaultValue,
							int flags,
							const char **strings);

	/**Adds the the specified(name) eventIn in the EventIn Array.*/
    void				addEventIn(int fieldType, const String &name);

	/**Adds the the specified(name) eventOut in the EventIn Array.*/
    void				addEventOut(int fieldType, const String &name);


    void				addExposedField(int fieldType,
							const String &name,
							FieldValue *defaultValue,
							float min = -FLT_MAX,
							float max = FLT_MAX);


    void				addExposedField(int fieldType,
							const String &name,
							FieldValue *defaultValue,
							int nodeType);


    void				addExposedField(int fieldType,
							const String &name,
							FieldValue *defaultValue,
							int flags,
							const char **strings);

	/**
	@param name the name of the EventIn to find its index in the EventIn Array.
	@return the index of the eventIn in the Array or INVALID_INDEX*/
    int					lookupSimpleEventIn(const String &name) const;
	
	/**
	@param name the name of the EventOut to find its index in the EventOut Array.
	@return the index of the eventOut in the Array or INVALID_INDEX*/
    int					lookupSimpleEventOut(const String &name) const;

protected:
	/**handle to the current Scene.*/
    Scene					*_scene;

	/**the name of the Proto*/
    String					_name;

	/**An Array(Vector) of EventIn object ptrs*/
    Array<EventIn *>		_eventIns;

	/**An Array(Vector) of EventOut object ptrs*/
    Array<EventOut *>		_eventOuts;

	/**An Array(Vector) of Field object ptrs*/
    Array<Field *>			_fields;

	/**An Array(Vector) of ExposedField object ptrs*/
    Array<ExposedField *>	_exposedFields;

    // for PROTO's:
	/**An Array of Nodes*/
    NodeList				*_nodes;
    Node					*_primaryNode;

};

/**The Proto class when calls create(Scence*)  it returns a ProtoNode(Node) object. In the same manner 
a ProtoGroup(Proto) class when calls the overriden create(Scene*) a NodeGroup(Node) is returned.*/
class ProtoNode : public Node
{
public:
	/**ProtoNode empty constructor(to be overriden). The method "Node *Proto::create(Scene *scene)" of the Proto Class returns a ProtoNode 
	(which is also a Node) at the request of the parser to create a new Node. That is the intermediate
	step, between the chain creation of the VRML node in this parser: Proto+(ProtoNode)->Node.*/
						ProtoNode(Scene *scene, Proto *proto);

    virtual int			getType() const { return -1; }
    virtual Node		*copy() const { return new ProtoNode(*this); }
};

#endif // _PROTO_H
