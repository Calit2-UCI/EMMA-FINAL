#ifndef _SFTIME_H
#define _SFTIME_H

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif

/**
* SFTime field type.
* \ingroup VRMLParser
*/
class SFTime : public FieldValue {
public:
			SFTime(double value);

    virtual int         getType() const { return SFTIME; }
    virtual const char *getTypeName() const { return "SFTime"; }
    virtual void        write(FILE *f, int indent) const;
    virtual bool	equals(const FieldValue *value) const;
    virtual FieldValue *copy() { return new SFTime(*this); }

    double		getValue() const { return _value; }
private:
    double	       _value;
};

#endif // _SFTIME_H
