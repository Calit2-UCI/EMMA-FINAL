#ifndef _NODE_NORMAL_H
#define _NODE_NORMAL_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

/**
* Normal node.
* \ingroup VRMLParser
*/
class NodeNormal : public Node {
public:
		    NodeNormal(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_NORMAL; }
    virtual Node   *copy() const { return new NodeNormal(*this); }
    virtual int	    getNodeClass() const { return NODE_NORMAL; }
};

class ProtoNormal : public Proto {
public:
		    ProtoNormal(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeNormal(scene, this); }
};

#endif // _NODE_NORMAL_H
