#ifndef _MFINT32_H
#define _MFINT32_H

#ifndef _ARRAY_H
#include "Array.h"
#endif
#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif

/**
* MFInt32 field type.
* \ingroup VRMLParser
*/
class MFInt32 : public MFieldValue {
public:
			MFInt32();
			MFInt32(const int *values, int len);
			MFInt32(const MFInt32 &value);

    virtual int         getType() const { return MFINT32; }
    virtual const char *getTypeName() const { return "MFInt32"; }
    virtual void        write(FILE *f, int indent) const;
    virtual bool	equals(const FieldValue *value) const;
    virtual int		getSFSize() const { return _value.size(); }
    virtual FieldValue *getSFValue(int index) const;
    virtual void	setSFValue(int index, FieldValue *value);
    virtual FieldValue *copy() { return new MFInt32(*this); }

    int			getValue(int index) const { return _value[index]; }
    const int          *getValue() const { return _value.getData(); }
    int			getSize() const { return _value.size(); }

private:
    Array<int>	        _value;
};

#endif // _MFINT32_H
