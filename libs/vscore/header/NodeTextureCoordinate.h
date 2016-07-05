#ifndef _NODE_TEXTURECOORDINATE_H
#define _NODE_TEXTURECOORDINATE_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

/**
* TextureCoordinate node.
* \ingroup VRMLParser
*/
class NodeTextureCoordinate : public Node {
public:
		    NodeTextureCoordinate(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_TEXTURE_COORDINATE; }
    virtual Node   *copy() const { return new NodeTextureCoordinate(*this); }
    virtual int	    getNodeClass() const { return NODE_TEXTURE_COORDINATE; }
};

class ProtoTextureCoordinate : public Proto {
public:
		    ProtoTextureCoordinate(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeTextureCoordinate(scene, this); }
};

#endif // _NODE_TEXTURECOORDINATE_H
