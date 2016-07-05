#ifndef _NODE_COLOR_H
#define _NODE_COLOR_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

/**
* Color node.
* \ingroup VRMLParser
*/
class NodeColor : public Node {
public:
		    NodeColor(Scene *scene, Proto *proto);
    virtual int	    getType() const { return NODE_COLOR; }
    virtual Node   *copy() const { return new NodeColor(*this); }
    virtual int	    getNodeClass() const { return NODE_COLOR; }
};

class ProtoColor : public Proto {
public:
		    ProtoColor(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeColor(scene, this); }
};

#endif // _NODE_COLOR_H
