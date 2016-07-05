

#ifndef _NODE_LOD_H
#define _NODE_LOD_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeLOD : public Node {
public:
		    NodeLOD(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_LOD; }
    virtual Node   *copy() const { return new NodeLOD(*this); }
};

class ProtoLOD : public Proto {
public:
		    ProtoLOD(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeLOD(scene, this); }
};

#endif // _NODE_LOD_H
