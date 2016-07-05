

#ifndef _NODE_EXTRUSION_H
#define _NODE_EXTRUSION_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeExtrusion : public Node {
public:
		    NodeExtrusion(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_EXTRUSION; }
    virtual Node   *copy() const { return new NodeExtrusion(*this); }
    virtual int	    getNodeClass() const { return NODE_GEOMETRY; }
};

class ProtoExtrusion : public Proto {
public:
		    ProtoExtrusion(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeExtrusion(scene, this); }
};

#endif // _NODE_EXTRUSION_H
