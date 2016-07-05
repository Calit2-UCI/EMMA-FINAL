#ifndef _NODE_PIXELTEXTURE_H
#define _NODE_PIXELTEXTURE_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodePixelTexture : public Node {
public:
		    NodePixelTexture(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_PIXEL_TEXTURE; }
    virtual Node   *copy() const { return new NodePixelTexture(*this); }
    virtual int	    getNodeClass() const { return NODE_TEXTURE; }
};

class ProtoPixelTexture : public Proto {
public:
		    ProtoPixelTexture(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodePixelTexture(scene, this); }
};

#endif // _NODE_PIXELTEXTURE_H
