

#ifndef _NODE_FOG_H
#define _NODE_FOG_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeFog : public Node {
public:
		    NodeFog(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_FOG; }
    virtual Node   *copy() const { return new NodeFog(*this); }
    virtual void    preDraw();
    virtual void    apply();
protected:
    float	    _matrix[16];
};

class ProtoFog : public Proto {
public:
		    ProtoFog(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeFog(scene, this); }
};

#endif // _NODE_FOG_H
