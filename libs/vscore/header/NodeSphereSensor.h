
#ifndef _NODE_SPHERESENSOR_H
#define _NODE_SPHERESENSOR_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeSphereSensor : public Node {
public:
		    NodeSphereSensor(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_SPHERE_SENSOR; }
    virtual Node   *copy() const { return new NodeSphereSensor(*this); }
};

class ProtoSphereSensor : public Proto {
public:
		    ProtoSphereSensor(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeSphereSensor(scene, this); }
};

#endif // _NODE_SPHERESENSOR_H
