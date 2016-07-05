#ifndef _NODE_VIEWPOINT_H
#define _NODE_VIEWPOINT_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif
/*
#ifndef _QUATERNION_H
#include "Quaternion.h"
#endif
#ifndef _VEC3F_H
#include "Vec3f.h"
#endif
*/

class NodeViewpoint : public Node {
public:
			NodeViewpoint(Scene *scene, Proto *proto);

    virtual int		getType() const { return NODE_VIEWPOINT; }
    virtual Node       *copy() const { return new NodeViewpoint(*this); }

	/*
    virtual void	preDraw();
    void		apply();

    Vec3f		getPosition() const;
    const Quaternion   &getOrientation() const;

    void		setPosition(const Vec3f &pos);
    void		setOrientation(const Quaternion &orientation);

protected:
    float		_matrix[16];
	*/
};

class ProtoViewpoint : public Proto {
public:
			ProtoViewpoint(Scene *scene);
    virtual Node       *create(Scene *scene)
			{ return new NodeViewpoint(scene, this); }
};

#endif // _NODE_VIEWPOINT_H
