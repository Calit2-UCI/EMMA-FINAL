
#ifndef _NODE_NAVIGATIONINFO_H
#define _NODE_NAVIGATIONINFO_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeNavigationInfo : public Node {
public:
		    NodeNavigationInfo(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_NAVIGATION_INFO; }
    virtual Node   *copy() const { return new NodeNavigationInfo(*this); }

    virtual void    preDraw();
};

class ProtoNavigationInfo : public Proto {
public:
		    ProtoNavigationInfo(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeNavigationInfo(scene, this); }
};

#endif // _NODE_NAVIGATIONINFO_H
