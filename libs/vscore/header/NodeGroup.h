#ifndef _NODE_GROUP_H
#define _NODE_GROUP_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

/**
* Group node.
* \ingroup VRMLParser
*/
class NodeGroup : public Node {
public:
		    NodeGroup(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_GROUP; }
    virtual Node   *copy() const { return new NodeGroup(*this); }
	/*
    virtual void    preDraw();
    virtual void    draw();
	*/
};

class ProtoGroup : public Proto {
public:
		    ProtoGroup(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeGroup(scene, this); }
};

#endif // _NODE_GROUP_H
