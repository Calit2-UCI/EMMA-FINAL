#ifndef _MFSTRING_H
#define _MFSTRING_H

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif

class ElementList;

/**
* MFString field type.
* \ingroup VRMLParser
*/
class MFString : public MFieldValue {
public:
			MFString();
			MFString(const MFString &string);
			MFString(StringArray *value);
			MFString(String value);
    virtual	       ~MFString();

    virtual int         getType() const { return MFSTRING; }
    virtual const char *getTypeName() const { return "MFString"; }
    virtual void        write(FILE *f, int indent) const;
    virtual bool	equals(const FieldValue *value) const;
    virtual FieldValue *copy() { return new MFString(*this); }

    const String       &getValue(int index) const { return (*_value)[index]; }
    int			getSize() const { return _value->size(); }

    virtual int		getSFSize() const { return _value->size(); }
    virtual FieldValue *getSFValue(int index) const;
    virtual void	setSFValue(int index, FieldValue *value);

private:
    StringArray	       *_value;
};

#endif // _MFSTRING_H
