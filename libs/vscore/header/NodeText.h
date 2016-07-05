
#ifndef _NODE_TEXT_H
#define _NODE_TEXT_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeText : public Node {
public:
		    NodeText(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_TEXT; }
    virtual Node   *copy() const { return new NodeText(*this); }
    virtual int	    getNodeClass() const { return NODE_GEOMETRY; }
    virtual void    draw();
};

class ProtoText : public Proto {
public:
		    ProtoText(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeText(scene, this); }
};

#endif // _NODE_TEXT_H
