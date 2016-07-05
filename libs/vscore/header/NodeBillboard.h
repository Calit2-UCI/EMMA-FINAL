

#ifndef _NODE_BILLBOARD_H
#define _NODE_BILLBOARD_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeBillboard : public Node {
public:
		    NodeBillboard(Scene *scene, Proto *proto);
    virtual int	    getType() const { return NODE_BILLBOARD; }
    virtual Node   *copy() const { return new NodeBillboard(*this); }

    virtual void    transform();
    virtual void    draw();
};

class ProtoBillboard : public Proto {
public:
		    ProtoBillboard(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeBillboard(scene, this); }
};

#endif // _NODE_BILLBOARD_H
