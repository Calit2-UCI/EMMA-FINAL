
#ifndef _NODE_SCALARINTERPOLATOR_H
#define _NODE_SCALARINTERPOLATOR_H

#ifndef _INTERPOLATOR_H
#include "Interpolator.h"
#endif

class NodeScalarInterpolator : public Interpolator {
public:
			NodeScalarInterpolator(Scene *scene, Proto *proto);

    virtual int		getType() const { return NODE_SCALAR_INTERPOLATOR; }
    virtual Node       *copy() const { return new NodeScalarInterpolator(*this); }
    virtual FieldValue *createKey(const float *value) const;
    virtual FieldValue *createKeys(const float *value, int numKeys) const;

    virtual int		getNumChannels() const { return 1; }
};

class ProtoScalarInterpolator : public ProtoInterpolator {
public:
			ProtoScalarInterpolator(Scene *scene);
    virtual Node       *create(Scene *scene)
			{ return new NodeScalarInterpolator(scene, this); }
};

#endif // _NODE_SCALARINTERPOLATOR_H
