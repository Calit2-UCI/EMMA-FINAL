

#ifndef _NODE_SPOTLIGHT_H
#define _NODE_SPOTLIGHT_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeSpotLight : public Node {
public:
		    NodeSpotLight(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_SPOT_LIGHT; }
    virtual Node   *copy() const { return new NodeSpotLight(*this); }
    void	    preDraw();
protected:
//    GLenum	   _light;
};

class ProtoSpotLight : public Proto {
public:
		    ProtoSpotLight(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeSpotLight(scene, this); }
};

#endif // _NODE_SPOTLIGHT_H
