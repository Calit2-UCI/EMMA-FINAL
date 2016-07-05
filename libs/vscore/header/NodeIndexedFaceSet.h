#ifndef _NODE_INDEXEDFACESET_H
#define _NODE_INDEXEDFACESET_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

/*
class MFVec3f;
class MFInt32;
class Vec3f;
*/

/**
* IndexedFaceSet node.
* \ingroup VRMLParser
*/
class NodeIndexedFaceSet : public Node {
public:
		    NodeIndexedFaceSet(Scene *scene, Proto *proto);

protected:
    virtual	   ~NodeIndexedFaceSet();

public:
    virtual int	    getType() const { return NODE_INDEXED_FACE_SET; }
    virtual void    setField(int index, FieldValue *value);
    virtual Node   *copy() const { return new NodeIndexedFaceSet(*this); }
    virtual int	    getNodeClass() const { return NODE_GEOMETRY; }

/*
    void	    getNormals(MFVec3f *coords, MFInt32 *coordIndex);
    void	    generateNormals(MFVec3f *coords, MFInt32 *coordIndex);
    void	    smoothNormals(const MFVec3f *coords,
				  const MFInt32 *coordIndex, 
				  const Vec3f *faceNormals);

    virtual void    draw();
*/

/*
    void	    drawHandles();
    Vec3f	    getHandle(int handle, int *constraint, int *field);
    void	    setHandle(int handle, const Vec3f &v);
*/

/*
protected:
    MFVec3f	   *_normals;
    MFInt32	   *_normalIndex;
    bool	    _normalPerVertex;
    bool	    _normalsDirty;
    float	    _creaseAngle;
*/
};

class ProtoIndexedFaceSet : public Proto {
public:
		    ProtoIndexedFaceSet(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeIndexedFaceSet(scene, this); }
};

#endif // _NODE_INDEXEDFACESET_H
