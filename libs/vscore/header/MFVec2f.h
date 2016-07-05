#ifndef _MFVEC2F_H
#define _MFVEC2F_H

#ifndef _MFFLOAT_H
#include "MFFloat.h"
#endif

/**
* MFVec2f field type.
* \ingroup VRMLParser
*/
class MFVec2f : public MFFloat {
public:
			MFVec2f();
			MFVec2f(const float values[], int len);

    virtual int		getType() const { return MFVEC2F; }
    virtual const char *getTypeName() const { return "MFVec2f"; }
    virtual int		getStride() const { return 2; }
    virtual bool	equals(const FieldValue *value) const;
    virtual FieldValue *copy() { return new MFVec2f(*this); }

    virtual FieldValue *getSFValue(int index) const;
    virtual void	setSFValue(int index, FieldValue *value);
};

#endif // _MFVEC2F_H

