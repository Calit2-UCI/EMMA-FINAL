
#ifndef _NODE_SEGMENT_H
#define _NODE_SEGMENT_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif
#ifndef _MATRIX_H
#include "Matrix.h"
#endif

class NodeSegment : public Node {
public:
		    NodeSegment(Scene *scene, Proto *proto);

protected:
		   ~NodeSegment();

public:
    virtual int	    getType() const { return NODE_SEGMENT; }
    virtual Node   *copy() const { return new NodeSegment(*this); }
    virtual void    setField(int index, FieldValue *value);
  
protected:
    
private:
    
};

class ProtoSegment : public Proto {
public:
		    ProtoSegment(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeSegment(scene, this); }
};

#endif // _NODE_SEGMENT_H
