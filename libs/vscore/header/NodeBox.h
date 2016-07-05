

#ifndef _NODE_BOX_H
#define _NODE_BOX_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeBox : public Node {
public:
		    NodeBox(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_BOX; }
    virtual Node   *copy() const { return new NodeBox(*this); }
    virtual int	    getNodeClass() const { return NODE_GEOMETRY; }
    virtual void    draw();
    virtual void    drawHandles();
    virtual Vec3f   getHandle(int handle, int *constraint, int *field);
    virtual void    setHandle(int handle, const Vec3f &v);
};

class ProtoBox : public Proto {
public:
		    ProtoBox(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeBox(scene, this); }
};

#endif // _NODE_BOX_H
