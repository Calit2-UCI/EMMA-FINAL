

#ifndef _NODE_COLORINTERPOLATOR_H
#define _NODE_COLORINTERPOLATOR_H

#ifndef _INTERPOLATOR_H
#include "Interpolator.h"
#endif

class NodeColorInterpolator : public Interpolator {
public:
			NodeColorInterpolator(Scene *scene, Proto *proto);
    virtual int		getType() const { return NODE_COLOR_INTERPOLATOR; }
    virtual Node       *copy() const { return new NodeColorInterpolator(*this); }

    virtual int		getNumChannels() const { return 3; }
    virtual FieldValue *createKey(const float *values) const;
    virtual FieldValue *createKeys(const float *values, int numKeys) const;
    virtual void	setKeyValue(int channel, int index, float value);
};

class ProtoColorInterpolator : public ProtoInterpolator {
public:
		    ProtoColorInterpolator(Scene *scene);
    virtual Node   *create(Scene *scene)
		    { return new NodeColorInterpolator(scene, this); }
};

#endif // _NODE_COLORINTERPOLATOR_H
