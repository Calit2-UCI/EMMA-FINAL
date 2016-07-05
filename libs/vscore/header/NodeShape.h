#ifndef _NODE_SHAPE_H
#define _NODE_SHAPE_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif




/**
* Shape node.
* \ingroup VRMLParser
*/
class NodeShape : public Node {
public:
					NodeShape(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_SHAPE; }
    virtual Node	*copy() const { return new NodeShape(*this); }

	/*
    virtual void    draw();
    bool			isLit() const;
protected:
    void			doSpecularPass(Node *appearance, Node *geometry);
	*/
};

class ProtoShape : public Proto {
public:
					ProtoShape(Scene *scene);
    virtual Node	*create(Scene *scene)
							{ return new NodeShape(scene, this); }
};

#endif // _NODE_SHAPE_H
