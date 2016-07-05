#ifndef _SFFLOAT_H
#define _SFFLOAT_H

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif

/**
* SFFloat field type.
* \ingroup VRMLParser
*/
class SFFloat : public FieldValue {
public:
			SFFloat(float value);

    virtual int         getType() const { return SFFLOAT; }
    virtual const char *getTypeName() const { return "SFFloat"; }
    virtual void        write(FILE *f, int indent) const;
    virtual FieldValue *copy() { return new SFFloat(*this); }

    float		getValue() const { return _value; }

	/**Checks if the value equals the specified as argument field value*/
    virtual bool	equals(const FieldValue *value) const;
    virtual void	clamp(float min, float max);

private:
    float	       _value;
};

#endif // _SFFLOAT_H
