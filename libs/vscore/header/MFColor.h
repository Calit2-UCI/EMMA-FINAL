#ifndef _MFCOLOR_H
#define _MFCOLOR_H

#ifndef _MFFLOAT_H
#include "MFFloat.h"
#endif

/**
* MFColor field type.
* \ingroup VRMLParser
*/
class MFColor : public MFFloat {
public:
			MFColor();
			MFColor(const float values[], int len);

    virtual int		getType() const { return MFCOLOR; }
    virtual const char *getTypeName() const { return "MFColor"; }
    virtual int		getStride() const { return 3; }
    virtual bool	equals(const FieldValue *value) const;
    virtual FieldValue *copy() { return new MFColor(*this); }

    const float	       *getValue(int index) const { return _value.getData() + index * 3; }
    const float	       *getData() const { return _value.getData(); }

    virtual FieldValue *getSFValue(int index) const;
    virtual void	setSFValue(int index, FieldValue *value);
};

#endif // _MFCOLOR_H
