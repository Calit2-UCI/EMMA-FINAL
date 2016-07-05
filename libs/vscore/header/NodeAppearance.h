#ifndef _NODE_APPEARANCE_H
#define _NODE_APPEARANCE_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

/**
* Appearance node.
* \ingroup VRMLParser
*/
class NodeAppearance : public Node {
public:
		    NodeAppearance(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_APPEARANCE; }
    virtual Node   *copy() const { return new NodeAppearance(*this); }
    virtual int	    getNodeClass() const { return NODE_APPEARANCE; }

	/*
    void	    bind();
    void	    unbind();
	*/
};

class ProtoAppearance : public Proto {
public:
		    ProtoAppearance(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeAppearance(scene, this); }
};

#endif // _NODE_APPEARANCE_H
