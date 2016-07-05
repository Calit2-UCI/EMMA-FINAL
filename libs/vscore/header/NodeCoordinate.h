#ifndef _NODE_COORDINATE_H
#define _NODE_COORDINATE_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

/**
* Coordinate node.
* \ingroup VRMLParser
*/
class NodeCoordinate : public Node {
public:
		    NodeCoordinate(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_COORDINATE; }
    virtual Node   *copy() const { return new NodeCoordinate(*this); }
    virtual int	    getNodeClass() const { return NODE_COORDINATE; }
};

class ProtoCoordinate : public Proto {
public:
		    ProtoCoordinate(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeCoordinate(scene, this); }
};

#endif // _NODE_COORDINATE_H
