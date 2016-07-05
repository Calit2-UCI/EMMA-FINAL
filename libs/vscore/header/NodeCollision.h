

#ifndef _NODE_COLLISION_H
#define _NODE_COLLISION_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeCollision : public Node {
public:
		    NodeCollision(Scene *scene, Proto *proto);
    virtual int	    getType() const { return NODE_COLLISION; }
    virtual Node   *copy() const { return new NodeCollision(*this); }
    
    virtual void    draw();
};

class ProtoCollision : public Proto {
public:
		    ProtoCollision(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeCollision(scene, this); }
};

#endif // _NODE_COLLISION_H
