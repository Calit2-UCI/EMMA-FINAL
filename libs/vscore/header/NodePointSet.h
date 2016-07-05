

#ifndef _NODE_POINTSET_H
#define _NODE_POINTSET_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodePointSet : public Node {
public:
		    NodePointSet(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_POINT_SET; }
    virtual Node   *copy() const { return new NodePointSet(*this); }
    virtual int	    getNodeClass() const { return NODE_GEOMETRY; }
};

class ProtoPointSet : public Proto {
public:
		    ProtoPointSet(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodePointSet(scene, this); }
};

#endif // _NODE_POINTSET_H
