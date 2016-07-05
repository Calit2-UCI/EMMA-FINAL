
#ifndef _NODE_PROXIMITYSENSOR_H
#define _NODE_PROXIMITYSENSOR_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeProximitySensor : public Node {
public:
		    NodeProximitySensor(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_PROXIMITY_SENSOR; }
    virtual Node   *copy() const { return new NodeProximitySensor(*this); }
};

class ProtoProximitySensor : public Proto {
public:
		    ProtoProximitySensor(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeProximitySensor(scene, this); }
};

#endif // _NODE_PROXIMITYSENSOR_H
