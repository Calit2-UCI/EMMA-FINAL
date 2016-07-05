#ifndef _NODE_IMAGETEXTURE_H
#define _NODE_IMAGETEXTURE_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

//#ifndef __gl_h

//#endif

/**
* ImageTexture node.
* \ingroup VRMLParser
*/
class NodeImageTexture : public Node {
public:
		    NodeImageTexture(Scene *scene, Proto *proto);
		    NodeImageTexture(const NodeImageTexture &node);
protected:
    virtual	   ~NodeImageTexture();

public:
    virtual int	    getType() const { return NODE_IMAGE_TEXTURE; }
    virtual Node   *copy() const { return new NodeImageTexture(*this); }
    virtual int	    getNodeClass() const { return NODE_TEXTURE; }
    virtual void    setField(int field, FieldValue *value);
	/*
    void	    load();
    int		    isLoaded();

    virtual void    bind();
    virtual void    unbind();

protected:
    unsigned char  *_image;
    int		    _imageStatus;
    int		    _textureWidth;
    int		    _textureHeight;
    int		    _components;
    //GLuint	    _textureName;
	int	    _textureName;
	*/
};

class ProtoImageTexture : public Proto {
public:
		    ProtoImageTexture(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeImageTexture(scene, this); }
};

#endif // _NODE_IMAGETEXTURE_H
