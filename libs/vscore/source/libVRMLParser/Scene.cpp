#include "stdafx.h"
#include "Scene.h"
#include "Node.h"
#include "Proto.h"
#include "EventIn.h"
#include "EventOut.h"
#include "FieldValue.h"
#include "NodeAppearance.h"
#include "NodeColor.h"
#include "NodeCoordinate.h"
#include "NodeCoordinateInterpolator.h"
#include "NodeDirectionalLight.h"
#include "NodeGroup.h"
#include "NodeImageTexture.h"
#include "NodeIndexedFaceSet.h"
#include "NodeMaterial.h"
#include "NodeNormal.h"
#include "NodePixelTexture.h"
#include "NodePointLight.h"
#include "NodeShape.h"
#include "NodeTextureCoordinate.h"
#include "NodeTextureTransform.h"
#include "NodeTransform.h"
#include "NodeViewpoint.h"
#include "MFNode.h"
#include "Parser.h"
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

Scene::Scene()
{
	// initializing the private map _fieldTypeMap
	_fieldTypeMap["SFBool"] = SFBOOL;
	_fieldTypeMap["SFColor"] = SFCOLOR;
	_fieldTypeMap["SFImage"] = SFIMAGE;
	_fieldTypeMap["SFInt32"] = SFINT32;
	_fieldTypeMap["SFFloat"] = SFFLOAT;
	_fieldTypeMap["SFNode"] = SFNODE;
	_fieldTypeMap["SFRotation"] = SFROTATION;
	_fieldTypeMap["SFString"] = SFSTRING;
	_fieldTypeMap["SFTime"] = SFTIME;
	_fieldTypeMap["SFVec2f"] = SFVEC2F;
	_fieldTypeMap["SFVec3f"] = SFVEC3F;
	_fieldTypeMap["MFColor"] = MFCOLOR;
	_fieldTypeMap["MFInt32"] = MFINT32;
	_fieldTypeMap["MFFloat"] = MFFLOAT;
	_fieldTypeMap["MFNode"] = MFNODE;
	_fieldTypeMap["MFRotation"] = MFROTATION;
	_fieldTypeMap["MFString"] = MFSTRING;
	_fieldTypeMap["MFTime"] = MFTIME;
	_fieldTypeMap["MFVec2f"] = MFVEC2F;
	_fieldTypeMap["MFVec3f"] = MFVEC3F;

	// initializing the private map _protos (not all Protos
	// are created here, just the ones we use)
	_protos["Appearance"] = new ProtoAppearance(this);
	_protos["Color"] = new ProtoColor(this);
	_protos["Coordinate"] = new ProtoCoordinate(this);
	_protos["CoordinateInterpolator"] = new ProtoCoordinateInterpolator(this);
	_protos["DirectionalLight"] = new ProtoDirectionalLight(this);
	_protos["Group"] = new ProtoGroup(this);
	_protos["ImageTexture"] = new ProtoImageTexture(this);
	_protos["IndexedFaceSet"] = new ProtoIndexedFaceSet(this);
	_protos["Material"] = new ProtoMaterial(this);
	_protos["Normal"] = new ProtoNormal(this);
	_protos["PixelTexture"] = new ProtoPixelTexture(this);
	_protos["PointLight"] = new ProtoPointLight(this);
	_protos["Shape"] = new ProtoShape(this);
	_protos["TextureCoordinate"] = new ProtoTextureCoordinate(this);
	_protos["TextureTransform"] = new ProtoTextureTransform(this);
	_protos["Transform"] = new ProtoTransform(this);
	_protos["Viewpoint"] = new ProtoViewpoint(this);

	_root = _protos["Group"]->create(this);
	_defaultViewpoint = (NodeViewpoint *) _protos["Viewpoint"]->create(this);
	_currentViewpoint = NULL;

	srand((unsigned) time(NULL));
}

Scene::~Scene()
{
	delete _defaultViewpoint;
	delete _currentViewpoint;
	for (int i = 0; i < _nodes.size(); i++) _nodes.set(i, NULL);
	_root->unref();

	delete _protos["Appearance"];
	delete _protos["Color"];
	delete _protos["Coordinate"];
	delete _protos["CoordinateInterpolator"];
	delete _protos["DirectionalLight"];
	delete _protos["Group"];
	delete _protos["ImageTexture"];
	delete _protos["IndexedFaceSet"];
	delete _protos["Material"];
	delete _protos["Normal"];
	delete _protos["PixelTexture"];
	delete _protos["PointLight"];
	delete _protos["Shape"];
	delete _protos["TextureCoordinate"];
	delete _protos["TextureTransform"];
	delete _protos["Transform"];
	delete _protos["Viewpoint"];
	for (int i = 0; i <_protoNames.size(); i++)
		delete _protos[_protoNames[i]];
}

void Scene::def(const char *nodeName, Node *value)
{
	_nodeMap[nodeName] = value;
	value->setName(nodeName);

	_nodeNames.append(nodeName);
	_nameFlags[nodeName] = false;
}

void Scene::setIS(const char *protoFieldName, int nodeFieldNameIndex)
{
	_isFieldMap[protoFieldName] = nodeFieldNameIndex;
}

int	Scene::getIS(const char *protoFieldName)
{
	return _isFieldMap[protoFieldName];
}

Node * Scene::use(const char *nodeName)
{
	Node *node = _nodeMap[nodeName];

	// node is being cloned, not copied
	node->ref();

	return node;
}

Node * Scene::getNodeByName(const char *nodeName)
{
	return _nodeMap[nodeName];
}

void Scene::undef(const char *nodeName)
{
	Node *node = _nodeMap[nodeName];

	_nodeMap[nodeName] = NULL;
	node->setName("");

	int index = _nodeNames.find(nodeName);
	_nodeNames.remove(index, index);
}

// this method might not be correct, we do not use it
// because we do not use parser.h (we use our own lexer)
int	Scene::addSymbol(String s)
{
	_symbolList.append(s);
	int id = _symbolList.find(s);
	_symbols[s] = id;
	_numSymbols++;
	return id;
}

// this method might not be correct, we do not use it
// because we do not use parser.h (we use our own lexer)
const String & Scene::getSymbol(int id) const
{
	return _symbolList[id];
}

void Scene::setNodes(NodeList *nodes)
{
	int size = nodes->size();
	for (int i = 0; i < size; i++) addNode(nodes->get(i));

	/*
	int field = _root->getProto()->lookupField("children");
	MFNode *fieldValue = new MFNode(nodes);
	_root->setField(field, (FieldValue *) fieldValue);

	NodeList *nodeList = fieldValue->getValue();

	for (int i = 0; i < nodeList->size(); i++)
		_nodes.append(nodeList->get(i));
	*/
}

// this is implemented differently than the comment above
// the method's declaration states because we do not use parser.h
const char * Scene::parseVRML(FILE *f) 
{
	Parser parser(this, f);
	int flag = parser.parseVRMLFile();

	if (flag == 1)
		fprintf(stderr, "Compile errors:\n%s\n\n",
		(const char *) _compileErrors);
	return NULL;
}

Node * Scene::createNode(const char *nodeType)
{
	if (_protos[nodeType] != NULL)
		return _protos[nodeType]->create(this);
	else
		return NULL;
}

bool Scene::writeVRML(FILE *f)
{
	// initializing _flags for writing non-standard Protos
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for (i = 0; i < _flags.size(); i++)
	{
		_flags[i] = false;
	}

	fprintf(f, "#VRML V2.0 utf8\n\n");

	// writing Protos that are not standard, 
	// i.e. that were defined in VRML file
	for (i = 0; i < _flags.size(); i++)
	{
		if (_flags[i] == false)
		{
			String protoName = _protoNames[i];
			_protos[protoName]->write(f, 0);
			fprintf(f, "\n");
			_flags[i] = true;
		}
	}

	// writing nodes (_root children)
	int index = _root->getProto()->lookupField("children");
	MFNode *fields = (MFNode *) _root->getField(index);
	NodeList *nodes = fields->getValue();

	for (i = 0; i < nodes->size(); i++)
	{
		nodes->get(i)->write(f, 0);
	}

	// writing routes (this should be implemented here if ROUTES are
	// being used)
	// fprintf(f, "\n");
/*
	// writing Protos that are not standard and were left behind, 
	// i.e. that were defined but weren't instatiated (node of that 
	// type wasn't defined)
	for (i = 0; i < _flags.size(); i++)
	{
		if (_flags[i] == false)
		{
			fprintf(f, "\n");
			String protoName = _protoNames[i];
			_protos[protoName]->write(f, 0);
			_flags[i] = true;
		}
	}
*/

	// initializing _nameFlags for writing nodes that have
	// defined names
	for (i = 0; i < _nodeNames.size(); i++)
	{
		String name = _nodeNames[i];
		_nameFlags[name] = false;
	}

	_tempSave = true;
	return _tempSave;
}

void Scene::setFlag(String protoName, bool flag)
{
	int index = _protoNames.find(protoName);

	if (index != -1) _flags[index] = flag;
}

bool Scene::getFlag(String protoName)
{
	int index = _protoNames.find(protoName);

	if (index == -1)
		// this means that there is no that kind of Proto so if
		// true is returned node will not try to write it
		return true;
	else
		return _flags[index];
}

void Scene::addProto(String name, Proto *value)
{
	_protos[name] = value;
	_protoNames.append(name);
	_flags.append(false);
}

Proto * Scene::getProto(String name)
{
	return _protos[name];
}

bool Scene::validRoute(Node *src, int eventOut, Node *dst, int eventIn)
{
	int eventInType = dst->getProto()->getEventIn(eventIn)->getType();
	int eventOutType = src->getProto()->getEventOut(eventOut)->getType();

	if (eventInType == eventOutType) return true;
	else return false;
}

void Scene::addRoute(Node *src, int eventOut, Node *dest, int eventIn)
{
	src->addOutput(eventOut, dest, eventIn);
	dest->addInput(eventIn, src, eventOut);
}

void Scene::deleteRoute(Node *src, int eventOut, Node *dest, int eventIn)
{
	src->removeOutput(eventOut, dest, eventIn);
	dest->removeInput(eventIn, src, eventOut);
}

int	Scene::getFieldType(String name)
{
	return _fieldTypeMap[name];
}

const char * Scene::getFieldTypeName(int type)
{
	switch (type)
	{
		case SFBOOL: return "SFBool";
		case SFCOLOR: return "SFColor";
		case SFIMAGE: return "SFImage";
		case SFINT32: return "SFInt32";
		case SFFLOAT: return "SFFloat";
		case SFNODE: return "SFNode";
		case SFROTATION: return "SFRotation";
		case SFSTRING: return "SFString";
		case SFTIME: return "SFTime";
		case SFVEC2F: return "SFVec2f";
		case SFVEC3F: return "SFVec3f";
		case MFCOLOR: return "MFColor";
		case MFINT32: return "MFInt32";
		case MFFLOAT: return "MFFloat";
		case MFNODE: return	"MFNode";
		case MFROTATION: return "MFRotation";
		case MFSTRING: return "MFString";
		case MFTIME: return "MFTime";
		case MFVEC2F: return "MFVec2f";
		case MFVEC3F: return "MFVec3f";
		default: return NULL;
	}
}

// this is a method with variable number of arguments
// it should have one terminating argument (NULL)
void Scene::errorf(const char *fmt, ...)
{
	va_list list;

	//initialize the va_list i.e char* 
	// variable 'list' by the address
	// of first unknown variable argument
	// by a call of va_start() macro
    va_start(list, fmt);
	_compileErrors += fmt;

    for(;;)
    {
		// in loop, retreive each argument
		// Second argument to va_arg is datatype
		// of expected argument
        const char *argument = va_arg(list, const char *);
		if (argument == NULL) break;
		_compileErrors += argument;
    }

    va_end(list); // cleanup , set 'list' to NULL
}

void Scene::invalidNode(const char *nodeName)
{
	errorf("Invalid node name: ", nodeName, "\n", NULL);
}

void Scene::invalidField(const char *node, const char *field)
{
	errorf("Invalid field ", field, " of the node ", node, "\n", NULL);
}

void Scene::setField(Node *node, int field, FieldValue *value)
{
	node->setField(field, value);
}

void Scene::addNode(Node *node)
{
	if (_nodes.find(node) == -1)
	{
		int field = _root->getProto()->lookupField("children");
		MFNode * children = (MFNode *) _root->getField(field);
		children->getValue()->append(node);
		node->addParent(_root, field);
		_nodes.append(node);
	}
}

void Scene::removeNode(Node *node)
{
	int index = _nodes.find(node);
	if (index != -1)
		_nodes.remove(index, index);

	int field = _root->getProto()->lookupField("children");
	MFNode * children = (MFNode *) _root->getField(field);
	NodeList *list = children->getValue();
	index = list->find(node);
	if (index != -1)
		list->remove(index, index);
}

String Scene::generateUniqueNodeName(Node *node)
{
	String name;
	int randomNumber;
	char digits[9];

	name += node->toString();
	randomNumber = (int) ((float) rand() / (RAND_MAX + 1) * 
		(999999999 - 100000000 + 1) + 100000000);

	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for (i = 0; i < 9; i++)
	{
		digits[i] = (randomNumber % 10) + '0';
		randomNumber /= 10;
	}
	for (i = 8; i >= 0; i--)
	{
		name += digits[i];
	}

	return name;
}


void Scene::setDEFFlag(String nodeName, bool flag)
{
	_nameFlags[nodeName] = flag;
}

bool Scene::getDEFFlag(String nodeName)
{
	return _nameFlags[nodeName];
}
