

#ifndef _NODE_ELEVATIONGRID_H
#define _NODE_ELEVATIONGRID_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class MFVec3f;

class NodeElevationGrid : public Node {
public:
		    NodeElevationGrid(Scene *scene, Proto *proto);

protected:
    virtual	   ~NodeElevationGrid();

public:
    virtual int	    getType() const { return NODE_ELEVATION_GRID; }
    virtual Node   *copy() const { return new NodeElevationGrid(*this); }
    virtual int	    getNodeClass() const { return NODE_GEOMETRY; }

    virtual void	setField(int index, FieldValue *value);

    virtual Vec3f   getHandle(int handle, int *constraint, int *field);
    virtual void    setHandle(int handle, const Vec3f &v);

    virtual void    draw();
    virtual void    drawHandles();
protected:
    void	    generateNormals();
    void	    resizeX();
    void	    resizeZ();
   
protected:
    MFVec3f	   *_normals;
    int		    _normalPerVertex;
    int		    _oldX;
    int		    _oldZ;
};

class ProtoElevationGrid : public Proto {
public:
		    ProtoElevationGrid(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeElevationGrid(scene, this); }
};

#endif // _NODE_ELEVATIONGRID_H
