

#ifndef _NODE_DEFBODYTEX_H
#define _NODE_DEFBODYTEX_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif
#ifndef _MATRIX_H
#include "Matrix.h"
#endif

class NodeDefBodyTex : public Node {
public:
		    NodeDefBodyTex(Scene *scene, Proto *proto);

protected:
		   ~NodeDefBodyTex();

public:
    virtual int	    getType() const { return NODE_DEFBODYTEX; }
    virtual Node   *copy() const { return new NodeDefBodyTex(*this); }
    virtual void    setField(int index, FieldValue *value);
  
protected:
    
private:
    
};

class ProtoDefBodyTex : public Proto {
public:
		    ProtoDefBodyTex(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeDefBodyTex(scene, this); }
};

#endif // _NODE_DEFBODYTEX_H
