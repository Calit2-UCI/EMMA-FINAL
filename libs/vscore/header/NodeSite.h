
#ifndef _NODE_SITE_H
#define _NODE_SITE_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif
#ifndef _MATRIX_H
#include "Matrix.h"
#endif

class NodeSite : public Node {
public:
		    NodeSite(Scene *scene, Proto *proto);

protected:
		   ~NodeSite();

public:
    virtual int	    getType() const { return NODE_SITE; }
    virtual Node   *copy() const { return new NodeSite(*this); }
    virtual void    setField(int index, FieldValue *value);
  
protected:
    
private:
    
};

class ProtoSite : public Proto {
public:
		    ProtoSite(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeSite(scene, this); }
};

#endif // _NODE_SITE_H
