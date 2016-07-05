

#ifndef _NODE_PLANESENSOR_H
#define _NODE_PLANESENSOR_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodePlaneSensor : public Node {
public:
		    NodePlaneSensor(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_PLANE_SENSOR; }
    virtual Node   *copy() const { return new NodePlaneSensor(*this); }
};

class ProtoPlaneSensor : public Proto {
public:
		    ProtoPlaneSensor(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodePlaneSensor(scene, this); }
};

#endif // _NODE_PLANESENSOR_H
