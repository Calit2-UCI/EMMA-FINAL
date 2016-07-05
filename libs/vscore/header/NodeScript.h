

#ifndef _NODE_SCRIPT_H
#define _NODE_SCRIPT_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeScript : public Node {
public:
		    NodeScript(Scene *scene);

protected:
    virtual	   ~NodeScript();

public:
    virtual int	    getType() const { return NODE_SCRIPT; }
    virtual Node   *copy() const { return new NodeScript(*this); }
    virtual void    write(FILE *f, int indent);

    virtual void    update();
};

class ProtoScript : public Proto {
public:
		    ProtoScript(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeScript(scene); }
};

#endif // _NODE_SCRIPT_H
