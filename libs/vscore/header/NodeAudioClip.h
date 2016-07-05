

#ifndef _NODE_AUDIOCLIP_H
#define _NODE_AUDIOCLIP_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeAudioClip : public Node {
public:
		    NodeAudioClip(Scene *scene, Proto *proto);

    virtual Node   *copy() const { return new NodeAudioClip(*this); }
    virtual int	    getNodeClass() const { return NODE_AUDIO_CLIP; }
    virtual int	    getType() const { return NODE_AUDIO_CLIP; }
};

class ProtoAudioClip : public Proto {
public:
		    ProtoAudioClip(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeAudioClip(scene, this); }
};

#endif // _NODE_AUDIOCLIP_H
