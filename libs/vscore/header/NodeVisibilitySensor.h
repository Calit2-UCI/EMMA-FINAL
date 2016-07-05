

#ifndef _NODE_VISIBILITYSENSOR_H
#define _NODE_VISIBILITYSENSOR_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeVisibilitySensor : public Node {
public:
		    NodeVisibilitySensor(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_VISIBILITY_SENSOR; }
    virtual Node   *copy() const { return new NodeVisibilitySensor(*this); }
};

class ProtoVisibilitySensor : public Proto {
public:
		    ProtoVisibilitySensor(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeVisibilitySensor(scene, this); }
};

#endif // _NODE_VISIBILITYSENSOR_H
