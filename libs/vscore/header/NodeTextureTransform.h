#ifndef _NODE_TEXTURETRANSFORM_H
#define _NODE_TEXTURETRANSFORM_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeTextureTransform : public Node {
public:
		    NodeTextureTransform(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_TEXTURE_TRANSFORM; }
    virtual Node   *copy() const { return new NodeTextureTransform(*this); }
    virtual int	    getNodeClass() const { return NODE_TEXTURE_TRANSFORM; }

	/*
    virtual void    bind();
    virtual void    unbind();
	*/
};

class ProtoTextureTransform : public Proto {
public:
		    ProtoTextureTransform(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeTextureTransform(scene, this); }
};

#endif // _NODE_TEXTURETRANSFORM_H
