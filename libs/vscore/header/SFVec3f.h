#ifndef _SFVEC3F_H
#define _SFVEC3F_H

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif
#ifndef _VEC3F_H
#include "Vec3f.h"
#endif

/**
* SFVec3f field type.
* \ingroup VRMLParser
*/
class SFVec3f : public FieldValue {
public:
			SFVec3f(float x, float y, float z);
			SFVec3f(const Vec3f &value);

    virtual int         getType() const { return SFVEC3F; }
    virtual const char	*getTypeName() const { return "SFVec3f"; }
    virtual void        write(FILE *f, int indent) const;
    virtual bool		equals(const FieldValue *value) const;
    virtual FieldValue	*copy() { return new SFVec3f(*this); }
    
    const float			*getValue() const { return _value; }
    float				getValue(int pos) const { return _value[pos]; }
    void				setValue(const float *values);
    void				setValue(int index, float value);
    virtual void		clamp(float min, float max);

private:
    float		_value[3];
};

#endif // _SFVEC3F_H
