#ifndef _SFVEC2F_H
#define _SFVEC2F_H

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif

/**
* SFVec2f field type.
* \ingroup VRMLParser
*/
class SFVec2f : public FieldValue {
public:
			SFVec2f(float x, float y);
			SFVec2f(const float *values);

    virtual int         getType() const { return SFVEC2F; }
    virtual const char *getTypeName() const { return "SFVec2f"; }
    virtual void        write(FILE *f, int indent) const;
    virtual bool	equals(const FieldValue *value) const;
    virtual FieldValue *copy() { return new SFVec2f(*this); }

    float		getValue(int index) const { return _value[index]; }
    const float	       *getValue() const { return _value; }
    void		setValue(int pos, float value)
			{ _value[pos] = value; }

private:
    float		_value[2];
};

#endif // _SFVEC2F_H
