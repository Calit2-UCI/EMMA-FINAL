

#ifndef _NODE_SOUND_H
#define _NODE_SOUND_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeSound : public Node {
public:
		    NodeSound(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_SOUND; }
    virtual Node   *copy() const { return new NodeSound(*this); }
};

class ProtoSound : public Proto {
public:
		    ProtoSound(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeSound(scene, this); }
};

#endif // _NODE_SOUND_H
