

#ifndef _NODE_CONE_H
#define _NODE_CONE_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeCone : public Node {
public:
		    NodeCone(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_CONE; }
    virtual Node   *copy() const { return new NodeCone(*this); }
    virtual int	    getNodeClass() const { return NODE_GEOMETRY; }
    virtual void    draw();
};

class ProtoCone : public Proto {
public:
		    ProtoCone(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeCone(scene, this); }
};

#endif // _NODE_CONE_H
