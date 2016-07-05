#ifndef _MFROTATION_H
#define _MFROTATION_H

#ifndef _MFFLOAT_H
#include "MFFloat.h"
#endif

/**
* MFRotation field type.
* \ingroup VRMLParser
*/
class MFRotation : public MFFloat {
public:
			MFRotation();
			MFRotation(const float values[], int len);
    virtual int		getType() const { return MFROTATION; }
    virtual const char *getTypeName() const { return "MFRotation"; }
    virtual int		getStride() const { return 4; }
    virtual bool	equals(const FieldValue *value) const;
    virtual FieldValue *copy() { return new MFRotation(*this); }

    const float	       *getValue(int index) const { return _value.getData() + index * 4; }
    virtual FieldValue *getSFValue(int index) const;
    virtual void	setSFValue(int index, FieldValue *value);
};

#endif // _MFROTATION_H
