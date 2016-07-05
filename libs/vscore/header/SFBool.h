#ifndef _SFBOOL_H
#define _SFBOOL_H

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif

/**
* SFBool field type.
* \ingroup VRMLParser
*/
class SFBool : public FieldValue {
public:
			SFBool(bool value);

    virtual int         getType() const { return SFBOOL; }
    virtual const char *getTypeName() const { return "SFBool"; }
    virtual void        write(FILE *f, int indent) const;
    virtual bool        equals(const FieldValue *value) const;
    virtual FieldValue *copy() { return new SFBool(*this); }

    bool		getValue() const { return _value; }

private:
    bool		_value;
};

#endif // _SFBOOL_H
