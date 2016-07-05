#ifndef _NODE_MATERIAL_H
#define _NODE_MATERIAL_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

/**
* Material node.
* \ingroup VRMLParser
*/
class NodeMaterial : public Node {
public:
		    NodeMaterial(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_MATERIAL; }
    virtual Node   *copy() const { return new NodeMaterial(*this); }
    virtual int	    getNodeClass() const { return NODE_MATERIAL; }
	/*
    virtual void    bind();
    virtual void    unbind();
	*/
};

class ProtoMaterial : public Proto {
public:
		    ProtoMaterial(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeMaterial(scene, this); }
};

#endif // _NODE_MATERIAL_H
