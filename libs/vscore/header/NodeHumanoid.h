

#ifndef _NODE_HUMANOID_H
#define _NODE_HUMANOID_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif
#ifndef _MATRIX_H
#include "Matrix.h"
#endif

class NodeHumanoid : public Node {
public:
		    NodeHumanoid(Scene *scene, Proto *proto);

protected:
		   ~NodeHumanoid();

public:
    virtual int	    getType() const { return NODE_HUMANOID; }
    virtual Node   *copy() const { return new NodeHumanoid(*this); }
    virtual void    setField(int index, FieldValue *value);
  
protected:
    
private:
    
};

class ProtoHumanoid : public Proto {
public:
		    ProtoHumanoid(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeHumanoid(scene, this); }
};

#endif // _NODE_HUMANOID_H
