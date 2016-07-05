
#ifndef _NODE_TOUCHSENSOR_H
#define _NODE_TOUCHSENSOR_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeTouchSensor : public Node {
public:
		    NodeTouchSensor(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_TOUCH_SENSOR; }
    virtual Node   *copy() const { return new NodeTouchSensor(*this); }
};

class ProtoTouchSensor : public Proto {
public:
		    ProtoTouchSensor(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeTouchSensor(scene, this); }
};

#endif // _NODE_TOUCHSENSOR_H
