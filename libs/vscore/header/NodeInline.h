

#ifndef _NODE_INLINE_H
#define _NODE_INLINE_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeInline : public Node {
public:
		    NodeInline(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_INLINE; }
    virtual Node   *copy() const { return new NodeInline(*this); }
};

class ProtoInline : public Proto {
public:
		    ProtoInline(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeInline(scene, this); }
};

#endif // _NODE_INLINE_H
