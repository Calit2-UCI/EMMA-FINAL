

#ifndef _NODE_CYLINDER_H
#define _NODE_CYLINDER_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeCylinder : public Node {
public:
		    NodeCylinder(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_CYLINDER; }
    virtual Node   *copy() const { return new NodeCylinder(*this); }
    virtual int	    getNodeClass() const { return NODE_GEOMETRY; }
    virtual void    draw();
};

class ProtoCylinder : public Proto {
public:
		    ProtoCylinder(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeCylinder(scene, this); }
};

#endif // _NODE_CYLINDER_H
