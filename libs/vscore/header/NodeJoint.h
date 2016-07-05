

#ifndef _NODE_JOINT_H
#define _NODE_JOINT_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif
#ifndef _MATRIX_H
#include "Matrix.h"
#endif

class NodeJoint : public Node {
public:
		    NodeJoint(Scene *scene, Proto *proto);

protected:
		   ~NodeJoint();

public:
    virtual int	    getType() const { return NODE_JOINT; }
    virtual Node   *copy() const { return new NodeJoint(*this); }
    virtual void    setField(int index, FieldValue *value);
  
protected:
    
private:
    
};

class ProtoJoint : public Proto {
public:
		    ProtoJoint(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeJoint(scene, this); }
};

#endif // _NODE_JOINT_H
