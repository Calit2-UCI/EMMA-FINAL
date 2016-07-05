#ifndef _SFINT32_H
#define _SFINT32_H

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif

/**
* SFInt32 field type.
* \ingroup VRMLParser
*/
class SFInt32 : public FieldValue {
public:
			SFInt32(int value);

    virtual int         getType() const { return SFINT32; }
    virtual const char *getTypeName() const { return "SFInt32"; }
    virtual void        write(FILE *f, int indent) const;
    virtual bool	equals(const FieldValue *value) const;
    virtual void	clamp(float min, float max);
    virtual FieldValue *copy() { return new SFInt32(*this); }

    int			getValue() const { return _value; }

private:
    int			_value;
};

#endif // _SFINT32_H
