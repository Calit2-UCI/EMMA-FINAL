#ifndef _NODE_H
#define _NODE_H

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _ARRAY_H
#include "Array.h"
#endif

#ifndef _LIST_H
#include "List.h"
#endif

#ifndef _VEC3F_H
#include "Vec3f.h"
#endif

#ifndef KS_STRING_H
#include "ksString.h"
#endif

#include "NaradaNotificationCenter.hpp"


class FieldValue;
class Node;
class Scene;
class Proto;

class Socket {
public:

	Node *_node;
    int _index;

    Socket() 
	{
        _node = NULL;
        _index = -1;
    }

    Socket(Node *node, int index) 
	{
        _node = node;
        _index = index;
    }

    int operator ==(const Socket &t) { return t._node == _node && t._index == _index; }

    int operator !=(const Socket &t) { return t._node != _node || t._index != _index; }
};

typedef List < Socket > SocketList;

enum flags {
    FNODE_FLAG_SELECTED, NODE_FLAG_TOUCHED, NODE_FLAG_COLLAPSED
};

/**Used to denote the Type of a Node, for the getType().*/
enum {
    NODE_ANCHOR, NODE_APPEARANCE, NODE_AUDIO_CLIP, NODE_BACKGROUND, NODE_BILLBOARD, NODE_BOX, NODE_COLLISION, NODE_COLOR,
       NODE_COLOR_INTERPOLATOR, NODE_CONE, NODE_COORDINATE, NODE_COORDINATE_INTERPOLATOR, NODE_CYLINDER, NODE_CYLINDER_SENSOR,
       NODE_DIRECTIONAL_LIGHT, NODE_ELEVATION_GRID, NODE_EXTRUSION, NODE_FOG, NODE_FONT_STYLE, NODE_GROUP, NODE_IMAGE_TEXTURE,
       NODE_INDEXED_FACE_SET, NODE_INDEXED_LINE_SET, NODE_INLINE, NODE_LOD, NODE_MATERIAL, NODE_MOVIE_TEXTURE,
       NODE_NAVIGATION_INFO, NODE_NORMAL, NODE_NORMAL_INTERPOLATOR, NODE_ORIENTATION_INTERPOLATOR, NODE_PIXEL_TEXTURE,
       NODE_PLANE_SENSOR, NODE_POINT_LIGHT, NODE_POINT_SET, NODE_POSITION_INTERPOLATOR, NODE_PROXIMITY_SENSOR,
       NODE_SCALAR_INTERPOLATOR, NODE_SCRIPT, NODE_SHAPE, NODE_SOUND, NODE_SPHERE, NODE_SPHERE_SENSOR, NODE_SPOT_LIGHT,
       NODE_SWITCH, NODE_TEXT, NODE_TEXTURE_COORDINATE, NODE_TEXTURE_TRANSFORM, NODE_TIME_SENSOR, NODE_TOUCH_SENSOR,
       NODE_TRANSFORM, NODE_VIEWPOINT, NODE_VISIBILITY_SENSOR, NODE_WORLD_INFO,
	   
	   //the following are specific H-Anim version 1.1 hardcoded PROTO nodes
	   NODE_HUMANOID,NODE_JOINT,NODE_SEGMENT,NODE_SITE,NODE_DEFBODYTEX,

       // the following are node "classes", used to validate the scene graph hierarchy
       NODE_ANY, NODE_CHILD, NODE_GEOMETRY, NODE_TEXTURE, NODE_INTERPOLATOR
};




enum Constraint {
    CONSTRAIN_NONE = 0, CONSTRAIN_X, CONSTRAIN_Y, CONSTRAIN_Z, CONSTRAIN_XY, CONSTRAIN_YZ, CONSTRAIN_ZX, CONSTRAIN_SPHERE,
};


/**The base class that initialises all the Nodes. It also adds the current Node to the current Scene
 *and initialises the _fields[i] Fieldvalue array.
 * \ingroup VRMLParser
 */
class Node {
public:
	/**Node constructor for the current Scene.
	@param scene the handle to the current Scene
	@param proto the handle to the current Proto that corrensponds to this Node, e.g. ProtoLOD for NodeLOD etc.
	It also gets from its corresponding Proto, the number of fields, of eventIns, eventOuts.
	*/
    Node(Scene * scene, Proto * proto);

	/**Initialises the Node in case that this is about a new Proto that doesn't pre-exist as a counterpart to a Proto. e.g.
	ProtoTransformgroup - NodeTransformGroup.*/
    Node(const Node & node);
protected:
    virtual ~Node();

public:
	/**Writes in the specified output the contents of the Node in VRML97 format*/
    virtual void write(FILE * f, int indent);
	
	/**Returns a handle to the current Scene*/
    Scene * getScene() const { return _scene; }
	
	/**Returns a FieldValue handle to the specified index Field that is contained in FieldValue * * _field*/
    FieldValue * getField(int index) const;

	/**Sets the Field at that index with the specified FieldValue value. Special care is taken if the field is SF-MFNode.*/
    virtual void setField(int index, FieldValue * value);

	/**Gets the name of the Node*/
    const String & getName() const { return _name; }
	
	/**Returns a string represenation of the Node type*/
	const String toString();

	/**Sets the name of the Node*/
    void setName(const char * name) { _name = name; }

	/**Increments the reference counter of a Node*/
    void ref() { _refs++; }

	/**Decrements the reference counter of a Node*/
	void unref() { if (--_refs == 0) delete this; }

	/**Returns the Type of a Node in the Node subclasses(here pure virtual). E.g. for the NodeGroup it returns NODE_GROUP*/
    virtual int getType() const = 0;
	/**Creates a new instance of that Node in the Node subclasses(here pure virtual).*/
    virtual Node * copy() const = 0;

	/**Finds if the specified child node exists in the current Node. 
	@param child the child Node to look for
	@param field the index in the _fields Fieldvalue* array.
	@return '0' if the child node is found as SFNODE OR
	'i' if the child node is found as MFNODE, where i is its index OR '-1' otherwise.
	*/
    int findChild(Node * child, int field) const;

	/**Looks if the specified Node is an ancestor in the SocketList.*/
	bool hasAncestor(Node * node) const;
	
	/**Calls Proto::lookupEventIn(name)*/
    int lookupEventIn(const String & name) const;

	/**Calls Proto::lookupEventOut(name)*/
    int lookupEventOut(const String & name) const;

	/**Gets a handle to the current Proto.*/
    Proto * getProto() const { return _proto; }

	/**Returns a NODE_CHILD as a node class (used to validate the scene)*/
    virtual int getNodeClass() const { return NODE_CHILD; }

	/**Adds an input to the Node's inputs SocketList.*/
    void addInput(int eventIn, Node * src, int eventOut);

	/**Adds an output to the Nodes' outputs SocketList.*/
    void addOutput(int eventOut, Node * dst, int eventIn);

	/**Finds and removes an input from the Node's inputs SocketList.*/
    void removeInput(int eventIn, Node * src, int eventOut);

	/**Finds and removes an output from the Node's outputs SocketList.*/
    void removeOutput(int eventOut, Node * dst, int eventIn);

    const SocketList & getInput(int i) { return _inputs[i]; }

    const SocketList & getOutput(int i) { return _outputs[i]; }

	/**To be used by Script to update its fields*/
    virtual void update();

	/**Adds a Node parent to the Node's parent SocketList*/
    void addParent(Node * parent, int field);

	/**Removes a Node parent from the Node's parent SocketList*/
    void removeParent(Node * parent, int field);

	/**Gets a Node parent from the Node's parent SocketList*/
    Node * getParent(int index) const { return _parents.get(index)->item()._node; }

	/**Gets a field from the Node's parent SocketList*/
    int getParentField(int index) const { return _parents.get(index)->item()._index; }

    int getNumParents() const { return _parents.size(); }

	int getNumFields() const { return _numFields; }

	/**Used only in the write().*/
    bool needsDEF() const;

    bool getFlag(int flag) const { return (_flags & (1 << flag)) != 0; }
	/**Sets the node flag to either of the enum flags FNODE_FLAG_SELECTED, NODE_LAG_TOUCHED, NODE_FLAG_COLLAPSED.*/
    void setFlag(int flag) { _flags |= (1 << flag); }
    void clearFlag(int flag) { _flags &= ~(1 << flag); } 
    
	/**Finds if the Node is in the current Scene.*/
	//bool isInScene() const;

	/**To be overriden by the subclasses*/
    virtual void preDraw() { }

	/**To be overriden by the subclasses*/
    virtual void transform() { }

	/**To be overriden by the subclasses*/
    virtual void transformForHandle(int /* handle */) { }

	/**To be overriden by the subclasses*/
    virtual void draw() { }

	/**To be overriden by the subclasses*/
    virtual void bind() { }

	/**To be overriden by the subclasses*/
    virtual void unbind() { }

	/**To be overriden by the subclasses*/
    virtual void drawHandles() { }

	/**To be overriden by the subclasses*/
    virtual Vec3f getHandle(int /* handle */, int * /* constraint */, int * /* field */) { return Vec3f(0.0f, 0.0f, 0.0f); }

	/**To be overriden by the subclasses*/
    virtual void setHandle(int /* handle */, const Vec3f & /* v */) { }

	/**To be overriden by the subclasses*/
    void drawHandle(const Vec3f & pos);

	/**Used only from NodeTimeSensor.*/
    //void sendEvent(int eventOut, double timestamp, FieldValue * value);
	/**Used only from NodeTimeSensor.*/
    //virtual void receiveEvent(int eventIn, double timestamp, FieldValue * value);
	//bool isSelected() const { return getFlag(NODE_FLAG_SELECTED); }
	//bool isCollapsed() const { return getFlag(NODE_FLAG_COLLAPSED); }
	/**Calls validChildType() and returns its output.*/
    //bool validChild(int field, Node * child);
	/**Checks if the field is valid.*/
    //bool validChildType(int field, int childType);
	//int findValidField(Node * child);
    //int findValidFieldType(int childType);
    //FieldValue * rewriteField(FieldValue * value, const char * oldBase, const char * newBase);

protected:
	/**Outputs the Fields of the current Node to a file in VRML97 format.*/
    void writeFields(FILE * f, int indent);

	/**Outputs the Routes of the current Node to a file in VRML97 format.*/
    void writeRoutes(FILE * f, int indent) const;

protected:
	/**handle to the current scene*/
    Scene * _scene;

	/**name of the Node*/
    String _name;
    int _refs;
    int _flags;

	/**handle to the current proto*/
    Proto * _proto;
    SocketList _parents;

	/**array of FieldValue ptrs objects*/
    FieldValue * * _fields;
    int _numFields;
    int _numEventIns;
    int _numEventOuts;
    SocketList * _inputs;
    SocketList * _outputs;
	
	
};

#endif  // _NODE_H
