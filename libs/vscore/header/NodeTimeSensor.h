
#ifndef _NODE_TIMESENSOR_H
#define _NODE_TIMESENSOR_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeTimeSensor : public Node {
public:
		    NodeTimeSensor(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_TIME_SENSOR; }
    virtual Node   *copy() const { return new NodeTimeSensor(*this); }
    virtual void    preDraw();
    void	    setTime(double t);
    void	    start(double t);
protected:
    bool	    _active;
    double	    _startTime;
};

class ProtoTimeSensor : public Proto {
public:
		    ProtoTimeSensor(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeTimeSensor(scene, this); }
};

#endif // _NODE_TIMESENSOR_H
