#ifndef _NODE_POINTLIGHT_H
#define _NODE_POINTLIGHT_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodePointLight : public Node {
public:
		    NodePointLight(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_POINT_LIGHT; }
    virtual Node   *copy() const { return new NodePointLight(*this); }

	/*
    virtual void    preDraw();
    virtual void    drawHandles();

    virtual Vec3f   getHandle(int handle, int *constraint, int *field);
    virtual void    setHandle(int handle, const Vec3f &v);
	*/
protected:
//    GLenum	    _light;
};

class ProtoPointLight : public Proto {
public:
		    ProtoPointLight(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodePointLight(scene, this); }
};

#endif // _NODE_POINTLIGHT_H
