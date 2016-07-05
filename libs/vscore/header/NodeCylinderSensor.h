

#ifndef _NODE_CYLINDERSENSOR_H
#define _NODE_CYLINDERSENSOR_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeCylinderSensor : public Node {
public:
		    NodeCylinderSensor(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_CYLINDER_SENSOR; }
    virtual Node   *copy() const { return new NodeCylinderSensor(*this); }
};

class ProtoCylinderSensor : public Proto {
public:
		    ProtoCylinderSensor(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeCylinderSensor(scene, this); }
};

#endif // _NODE_CYLINDERSENSOR_H
