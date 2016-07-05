

#ifndef _NODE_INDEXEDLINESET_H
#define _NODE_INDEXEDLINESET_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeIndexedLineSet : public Node {
public:
		    NodeIndexedLineSet(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_INDEXED_LINE_SET; }
    virtual Node   *copy() const { return new NodeIndexedLineSet(*this); }
    virtual int	    getNodeClass() const { return NODE_GEOMETRY; }
    virtual void    draw();
};

class ProtoIndexedLineSet : public Proto {
public:
		    ProtoIndexedLineSet(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeIndexedLineSet(scene, this); }
};

#endif // _NODE_INDEXEDLINESET_H
