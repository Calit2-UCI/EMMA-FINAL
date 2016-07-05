
#ifndef _NODE_WORLDINFO_H
#define _NODE_WORLDINFO_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeWorldInfo : public Node {
public:
		    NodeWorldInfo(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_WORLD_INFO; }
    virtual Node   *copy() const { return new NodeWorldInfo(*this); }
};

class ProtoWorldInfo : public Proto {
public:
		    ProtoWorldInfo(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeWorldInfo(scene, this); }
};

#endif // _NODE_WORLDINFO_H
