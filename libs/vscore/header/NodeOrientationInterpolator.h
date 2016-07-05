
#ifndef _NODE_ORIENTATIONINTERPOLATOR_H
#define _NODE_ORIENTATIONINTERPOLATOR_H

#ifndef _INTERPOLATOR_H
#include "Interpolator.h"
#endif

class NodeOrientationInterpolator : public Interpolator {
public:
			NodeOrientationInterpolator(Scene *scene, Proto *proto);

    virtual int		getType() const { return NODE_ORIENTATION_INTERPOLATOR; }
    virtual Node       *copy() const { return new NodeOrientationInterpolator(*this); }

    virtual int		getNumChannels() const;
    virtual FieldValue *createKey(const float *value) const;
    virtual FieldValue *createKeys(const float *value, int numKeys) const;
    virtual void	interpolate(float k, float *values);
};

class ProtoOrientationInterpolator : public Proto {
public:
			ProtoOrientationInterpolator(Scene *scene);
    virtual Node       *create(Scene *scene)
			{ return new NodeOrientationInterpolator(scene, this); }
};

#endif // _NODE_ORIENTATIONINTERPOLATOR_H
