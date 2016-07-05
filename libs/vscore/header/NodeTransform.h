#ifndef _NODE_TRANSFORM_H
#define _NODE_TRANSFORM_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif
/*
#ifndef _MATRIX_H
#include "Matrix.h"
#endif
*/

/**
* Transform node.
* \ingroup VRMLParser
*/
class NodeTransform : public Node {
public:
		    NodeTransform(Scene *scene, Proto *proto);

protected:
		   ~NodeTransform();

public:
    virtual int	    getType() const { return NODE_TRANSFORM; }
    virtual Node   *copy() const { return new NodeTransform(*this); }
    virtual void    setField(int index, FieldValue *value);
	/*
    virtual void    drawHandles();
    virtual void    transform();
    virtual void    transformForHandle(int handle);
    virtual void    draw();
    virtual void    preDraw();
    virtual Vec3f   getHandle(int handle, int *constraint, int *field);
    virtual void    setHandle(int handle, const Vec3f &v);

    //virtual void    receiveEvent(int eventIn, double timestamp, FieldValue *value);

protected:
    static void	    drawCone( float x, float y, float z);
    static void	    drawScaleHandle(float x, float y, float z);
    void	    drawAxisLines();
    void	    drawTranslationHandles();
    void	    drawRotationHandles();
    void	    drawScaleHandles();
private:
    bool	    _matrixDirty;
    Matrix	    _matrix;
	*/
};

class ProtoTransform : public Proto {
public:
		    ProtoTransform(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeTransform(scene, this); }
};

#endif // _NODE_TRANSFORM_H
