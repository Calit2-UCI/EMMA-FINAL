#ifndef _SCENE_H
#define _SCENE_H

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _ARRAY_H
#include "Array.h"
#endif

#ifndef _MAP_H
#include "Map.h"
#endif

#ifndef _STACK_H
#include "Stack.h"
#endif

#ifndef _NODE_LIST_H
#include "NodeList.h"
#endif

#ifndef KS_STRING_H
#include "ksString.h"
#endif

#ifndef SCENEBASE_H
#include "SceneBase.h"
#endif

#define MAX_PATH_LEN 1024

class FieldValue;
class Node;
class Proto;
class NodeViewpoint;
class SFVec3f;
class Interpolator;

/*HastTables containing pairs of <Key,Data>*/
typedef Map<String, Node *>	NodeMap;
/*string name - node type (used for DEF VRML statements, store the DEF name of a node)*/
typedef Map<String, int>	StringMap;
/*string name - int ID (returned from lexer)*/
typedef Map<String, Proto *>	ProtoMap;
/*string name - Proto type*/

typedef Map<String, int>		IsFieldMap;

/**This is the main manager class that controls all the scene and node aspects, registrations. It is also used as a handle to the current 
 VRML scene. Important data types: <p> Map<String,Node *> NodeMap, Map<String,int> StringMap, Map<String,Proto *> ProtoMap. </p>
 Important variables are:
 <ul>
 <li>NodeMap  _nodeMap - Map containing pairs <String,Node*></li> 
 <li>StringMap  _symbols - Map containing pairs <String-int></li> 
 <li>Array<String>  _symbolList</li>
 <li>ProtoMap  _protos</li>
 <li>NodeList  _nodes - An Array of Nodes*</li> 
 <li>StringMap  _fieldTypeMap</li>
 </ul>
 It also assigns Node _root of the Scene to be a "Group" node and creates a default viewpoint
\ingroup VRMLParser
*/
class Scene : public SceneBase {
public:
	/** Scene class constructor. Initialises the private "StringMap _fieldTypeMap" to contain pairs of "String-Int"
	as "MFVec3f - MFVEC3F" (the int ID MFVEC3F is returned from the scanner). Also initialises the private 
	"ProtoMap _protos" to contain pairs of "String-Proto" as "Anchor - ProtoAnchor". In addition it assigns  
	"Node _root" of the Scene to be a "Group" Node. Also it creates a default viewpoint.*/
						Scene();
						~Scene();
	/**In case of a VRML DEF keyword. Assigns the string "nodename" to the Node.*/
    void				def(const char *nodeName, Node *value);

	/**In case of a VRML IS keyword. Assigns the Node string "fieldname" to the Proto string "fieldname".*/
    void				setIS(const char *protoFieldName, int nodeFieldNameIndex);

	/**In case of a VRML "IS" statement. Returns the String Node FieldName of the IS corresponding String Proto FieldName.*/
	int					getIS(const char *protoFieldName);

	/**In case of a VRML "USE" statement.
	@param nodeName the name of the node. 
	@return the Node that has that DEF nodename in the "String-Node" _nodeMap.*/
    Node				*use(const char *nodeName);

	/**Returns the Node that has the specified DEF name.
	@param nodeName the name of the node. 
	@return the Node that has that DEF nodename in the "String-Node" _nodeMap.*/
    Node				*getNodeByName(const char *nodeName);


	/**Deassociate the Node with name 'nodeName' from that nodeName (in the NodeMap HastTable).*/
    void				undef(const char *nodeName);

	/**Primary used by the lexer. Adds the tokenised String input yytext to the Symbol HashTable and then 
	in the Vector(Array) that contains all the parsed tokens. 
	@param 	s String read by the lexr 
	@return the int id position of the input String in the Scene's Array of input lexer Strings*/
	int					addSymbol(String s);

	/**Returns a String scanned by the lexer. 
    @param id the id(position) of the lexer input String
	@return the String that is contained in the specified id in the Array<String>*/
	const String		&getSymbol(int id) const;

	/**Sets the first primary Node as a root to the scene.*/
    void				setRoot(Node *root)	{ _root = root; }

    /**Gets the first primary Node root of the scene.*/
	Node				*getRoot() const			{ return _root; }

	/**Sets all the root Nodes in the scenegraph. They are set as children of the _root "Group" node. (set in the Scene constructor).*/
    void				setNodes(NodeList *nodes);

	/**Calls the yyparse() method of the parser. Then begins the parsing process.*/
    virtual const char			*parseVRML(FILE *f) ;

	/**Creates a new Node. That Node is taken from the ProtoMap pair string-Proto e,g, "Anchor"-ProtoAnchor initialised in Scene's contructor. */
    Node				*createNode(const char *nodeType);

	/**Outputs to a file all the scene contents(all nodes).*/
	bool				writeVRML(FILE *f);

	/**Adds a named Proto to the Proto HashTable. The Proto is added to the ProtoMap _protos as a pair of key String-Proto values.*/
    void				addProto(String name, Proto *value);

	/**Returns a Proto from the Proto HashTable. From the ProtoMap <String,Proto *>.*/
    Proto				*getProto(String name);

	/**Checks if the Route is valid (from eventOut to eventIn).*/
    static bool			validRoute(Node *src, int eventOut, Node *dst, int eventIn);

	/**In case of a VRML Route statement. Adds an event route from the src Node to dst Node. */
    void				addRoute(Node *src, int eventOut, Node *dest, int eventIn);
    void				deleteRoute(Node *src, int eventOut, Node *dest, int eventIn);

	/**Returns an int from a StringMap HashTable for FieldTypes. That is the int id from the StringMap _fieldTypeMap e.g. key "MFVec3f"-MFVEC3F value*/
    int					getFieldType(String name);

	/**Returns a String from a StringMap HashTable for FieldTypes. From the pairs String-Id returns the opposite of getFieldType i.e. the string name*/
    static const char	*getFieldTypeName(int type);

	/**Outputs error messages to a buffer*/
    void				errorf(const char *fmt, ...);

	/**Outputs error message. In case of an invalid DEF name. */
    void				invalidNode(const char *nodeName);

	/**Outputs error message. In case of an invalid Field.*/
    void				invalidField(const char *node, const char *field);

	/**Sets the field of the current node to the fieldValue*/
    void				setField(Node *node, int field, FieldValue *value);

    //Interpolator		*findUpstreamInterpolator(Node *node, int field) const;

	/**Appends a new node to the Scene. Actually the new Node is added in the Scene's NodeList(Array<Node*>) */
    void				addNode(Node *node);

	/**Removes the Node from the Scene. Actually from Scene._nodes ( a NodeList i.e. an Array<Node*>)*/
    void				removeNode(Node *node);
    
	/**In case of Node::write() and that Node doesn't have a nodeName, generate a new nodeName.*/
    String				generateUniqueNodeName(Node *node);

	/**Returns a reference to all the root Nodes in the Scene contained in it's NodeList.*/
    const NodeList     *getNodes() const { return &_nodes; }
    bool				isTempSave() const { return _tempSave; }

	/**Used to set a flag that shows whether the Proto named protoName has already been written in a file*/
	void setFlag(String protoName, bool flag);
	/**Used to get a flag that shows whether the Proto named protoName has already been written in a file*/
	bool getFlag(String protoName);

	/**Used to set a flag that shows whether a crtain node name has already been written in a file*/
	void setDEFFlag(String nodeName, bool flag);
	/**Used to get a flag that shows whether a crtain node name has already been written in a file*/
	bool getDEFFlag(String nodeName);

private:
    NodeMap				_nodeMap;
    Node				*_root;
	
	/*HastTable containing Strings(Read from lexer)-id(int returned from lexer)*/
    StringMap			_symbols; 
	/*Vector containing all strings(read from lexer) in the id position*/
	Array<String>		_symbolList;
    ProtoMap			_protos;
	
	/*HastTable containing "SFBool"-SFBOOL pairs, lcase-Ucase field types*/
    StringMap			_fieldTypeMap;
	/*All VRML97 nodes that are contained in the scene are added to the vector NodeList*/
    NodeList			_nodes;

	/*number of strings in Array<String> _symbolList[]*/
    int					_numSymbols;
    NodeViewpoint		*_defaultViewpoint;
    NodeViewpoint		*_currentViewpoint;
    String				_compileErrors;

	IsFieldMap			_isFieldMap;
    //int				_numLights;
    //NodeList			_viewpoints;
    //NodeList			_fogs;
    //NodeList			_backgrounds;
    //NodeList			_timeSensors;
    //Stack<Node *>		_fogStack;
    //int				_headlight;
    //const Path		*_selection;
    //int				_selectedHandle;
    //TransformMode		_transformMode;
    //String			_URL;
    //String			_newURL;
    //bool				_running;
    //bool				_recording;
	//Array<FontInfo *>	_fonts;
    //String				_path;
	//List<SceneView *>	_views;
    bool				_tempSave; 
	
	// these two attributes are used for checking whether a 
	// certain Proto (that is not standard) has been written
	// to the file
	Array<String> _protoNames;
	Array<bool> _flags;

	// these two attributes are used for checking whether a 
	// certain node name has already been written to the file
	Map<String, bool> _nameFlags;
	Array<String> _nodeNames;
};

#endif // _SCENE_H
