#ifndef _NODE_DIRECTIONALLIGHT_H
#define _NODE_DIRECTIONALLIGHT_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeDirectionalLight : public Node {
public:
		    NodeDirectionalLight(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_DIRECTIONAL_LIGHT; }
    virtual Node   *copy() const { return new NodeDirectionalLight(*this); }
	/*
    virtual void    bind();
    virtual void    unbind();
	*/
protected:
//    GLenum	   _light;	    
};

class ProtoDirectionalLight : public Proto {
public:
		    ProtoDirectionalLight(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeDirectionalLight(scene, this); }
};

#endif // _NODE_DIRECTIONALLIGHT_H
