#ifndef _NODE_COORDINATEINTERPOLATOR_H
#define _NODE_COORDINATEINTERPOLATOR_H

#ifndef _INTERPOLATOR_H
#include "Interpolator.h"
#endif

class NodeCoordinateInterpolator : public Interpolator {
public:
			NodeCoordinateInterpolator(Scene *scene, Proto *proto);

    virtual int	        getType() const { return NODE_COORDINATE_INTERPOLATOR; }
    virtual Node       *copy() const { return new NodeCoordinateInterpolator(*this); }

	/*
    virtual int		getNumChannels() const;
    virtual FieldValue *createKey(const float *value) const;
    virtual FieldValue *createKeys(const float *values, int numKeys) const;
	*/
};

class ProtoCoordinateInterpolator : public ProtoInterpolator {
public:
		    ProtoCoordinateInterpolator(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeCoordinateInterpolator(scene, this); }
};

#endif // _NODE_COORDINATEINTERPOLATOR_H
