

#ifndef _NODE_NORMALINTERPOLATOR_H
#define _NODE_NORMALINTERPOLATOR_H

#ifndef _INTERPOLATOR_H
#include "Interpolator.h"
#endif

class NodeNormalInterpolator : public Interpolator {
public:
			NodeNormalInterpolator(Scene *scene, Proto *proto);

    virtual int		getType() const { return NODE_NORMAL_INTERPOLATOR; }
    virtual Node       *copy() const { return new NodeNormalInterpolator(*this); }

    virtual int		getNumChannels() const;
    virtual FieldValue *createKey(const float *value) const;
    virtual FieldValue *createKeys(const float *value, int numKeys) const;
    virtual void	interpolate(float k, float *values);
};

class ProtoNormalInterpolator : public Proto {
public:
		    ProtoNormalInterpolator(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeNormalInterpolator(scene, this); }
};

#endif // _NODE_NORMALINTERPOLATOR_H
