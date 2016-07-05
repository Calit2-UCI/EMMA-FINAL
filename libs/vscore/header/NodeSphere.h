
#ifndef _NODE_SPHERE_H
#define _NODE_SPHERE_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeSphere : public Node {
public:
		    NodeSphere(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_SPHERE; }
    virtual Node   *copy() const { return new NodeSphere(*this); }
    virtual int	    getNodeClass() const { return NODE_GEOMETRY; }

    virtual void    draw();
};

class ProtoSphere : public Proto {
public:
		    ProtoSphere(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeSphere(scene, this); }
};

#endif // _NODE_SPHERE_H
