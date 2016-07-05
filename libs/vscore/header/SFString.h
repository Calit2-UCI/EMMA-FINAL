#ifndef _SFSTRING_H
#define _SFSTRING_H

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif
#ifndef KS_STRING_H
#include "ksString.h"
#endif

/**
* SFString field type.
* \ingroup VRMLParser
*/
class SFString : public FieldValue {
public:
			SFString(const String &value);
			SFString(const char *str);

    const char	       *getValue() const { return _value; }
    virtual int         getType() const { return SFSTRING; }
    virtual const char *getTypeName() const { return "SFString"; }
    virtual void        write(FILE *f, int indent) const;
    virtual bool	equals(const FieldValue *value) const;
    virtual FieldValue *copy() { return new SFString(*this); }

private:
    String	       _value;
};

#endif // _SFSTRING_H
