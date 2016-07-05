
#ifndef _NODE_FONTSTYLE_H
#define _NODE_FONTSTYLE_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class NodeFontStyle : public Node {
public:
		    NodeFontStyle(Scene *scene, Proto *proto);

    virtual int	    getType() const { return NODE_FONT_STYLE; }
    virtual Node   *copy() const { return new NodeFontStyle(*this); }
    virtual int	    getNodeClass() const { return NODE_FONT_STYLE; }
};

class ProtoFontStyle : public Proto {
public:
		    ProtoFontStyle(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeFontStyle(scene, this); }
};

#endif // _NODE_FONTSTYLE_H
