

#ifndef _NODE_MOVIETEXTURE_H
#define _NODE_MOVIETEXTURE_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeMovieTexture : public Node {
public:
		    NodeMovieTexture(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_MOVIE_TEXTURE; }
    virtual Node   *copy() const { return new NodeMovieTexture(*this); }
    virtual int	    getNodeClass() const { return NODE_MOVIE_TEXTURE; }
};

class ProtoMovieTexture : public Proto {
public:
		    ProtoMovieTexture(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeMovieTexture(scene, this); }
};

#endif // _NODE_MOVIETEXTURE_H
