

#ifndef _NODE_BACKGROUND_H
#define _NODE_BACKGROUND_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeImageTexture;

class NodeBackground : public Node {
public:
		        NodeBackground(Scene *scene, Proto *proto);
protected:
    virtual	       ~NodeBackground();

public:
    virtual int	        getType() const { return NODE_BACKGROUND; }
    virtual Node       *copy() const { return new NodeBackground(*this); }
    virtual void	setField(int field, FieldValue *value);
    virtual void	preDraw();
    void		apply();
private:
    NodeImageTexture   *imageTextures[6];
};

class ProtoBackground : public Proto {
public:
		    ProtoBackground(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeBackground(scene, this); }
};

#endif // _NODE_BACKGROUND_H
