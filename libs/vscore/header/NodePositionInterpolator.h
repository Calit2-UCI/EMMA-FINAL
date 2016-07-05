
#ifndef _NODE_POSITIONINTERPOLATOR_H
#define _NODE_POSITIONINTERPOLATOR_H

#ifndef _INTERPOLATOR_H
#include "Interpolator.h"
#endif

class NodePositionInterpolator : public Interpolator {
public:
			NodePositionInterpolator(Scene *scene, Proto *proto);

    virtual int		getType() const { return NODE_POSITION_INTERPOLATOR; }
    virtual Node       *copy() const { return new NodePositionInterpolator(*this); }

    virtual int		getNumChannels() const { return 3; }
    virtual FieldValue *createKey(const float *value) const;
    virtual FieldValue *createKeys(const float *values, int numKeys) const;
};

class ProtoPositionInterpolator : public ProtoInterpolator {
public:
		    ProtoPositionInterpolator(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodePositionInterpolator(scene, this); }
};

#endif // _NODE_POSITIONINTERPOLATOR_H
