#ifndef _SFCOLOR_H
#define _SFCOLOR_H

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif

/**
* SFColor field type.
* \ingroup VRMLParser
*/
class SFColor : public FieldValue {
public:
			SFColor(float r, float g, float b);
			SFColor(const float *value);

    virtual int         getType() const { return SFCOLOR; }
    virtual const char *getTypeName() const { return "SFColor"; }
    virtual void        write(FILE *f, int indent) const;
    virtual bool	equals(const FieldValue *value) const;
    virtual void	clamp(float min, float max);
    virtual FieldValue *copy() { return new SFColor(*this); }

    const float	       *getValue() const { return _value; }
    float		getValue(int index) const { return _value[index]; }
    void		setValue(int index, float value)
			{ _value[index] = value; }

protected:
    bool		equals(const SFColor *value) const;


private:
    float		_value[4];
};

#endif // _SFCOLOR_H
