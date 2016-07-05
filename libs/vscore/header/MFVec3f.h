#ifndef _MFVEC3F_H
#define _MFVEC3F_H

#ifndef _MFFLOAT_H
#include "MFFloat.h"
#endif

class Vec3f;

/**
* MFVec3f field type.
* \ingroup VRMLParser
*/
class MFVec3f : public MFFloat {
public:
			MFVec3f();
			MFVec3f(const float *values, int len);
			MFVec3f(const Vec3f *values, int len);

    virtual int		getType() const { return MFVEC3F; }
    virtual const char *getTypeName() const { return "MFVec3f"; }
    virtual int		getStride() const { return 3; }
    virtual bool	equals(const FieldValue *value) const;
    virtual FieldValue *copy() { return new MFVec3f(*this); }

    virtual FieldValue *getSFValue(int index) const;
    virtual void	setSFValue(int index, FieldValue *value);

    const float	       *getValue(int index) const
			{ return _value.getData() + index * 3; } 
    const float	       *getData() const
			{ return _value.getData(); }
};

#endif // _MFVEC3F_H
